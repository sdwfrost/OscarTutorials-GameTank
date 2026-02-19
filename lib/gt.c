#include "gt.h"

// Shadow variables for write-only registers
static byte shadow_banking;
static byte shadow_dma_flags;

// ---------------------------------------------------------------------------
// Interrupt Handlers
// ---------------------------------------------------------------------------

// NMI is triggered by GameTank vblank
__asm nmi_handler
{
	rti
}

// IRQ is triggered when blitter finishes a DMA operation
__asm irq_handler
{
	byt 0x9c, 0x06, 0x40   // stz $4006 — clear blitter IRQ
	rti
}

// Reset handler: clear banking register then jump to oscar64 startup
__asm reset_handler
{
	byt 0x9c, 0x05, 0x20   // stz $2005 — zero the banking register
	jmp 0xff80              // jump to oscar64 startup code
}

// ---------------------------------------------------------------------------
// Boot Vector Table ($FFFA-$FFFF)
// ---------------------------------------------------------------------------
#pragma data(boot)

__export struct Vectors
{
	void *nmi, *reset, *irq;
} vectors = {
	nmi_handler,
	reset_handler,
	irq_handler
};

#pragma data(data)

// ---------------------------------------------------------------------------
// ROM Bank Selection via VIA SPI
// ---------------------------------------------------------------------------
static void via_set_rom_bank(byte bank)
{
	gtvia.ddra = 0x07;     // Set low 3 bits of port A as outputs
	gtvia.iora = 0;

	// Bit-bang 8-bit bank number MSB first
	for (signed char i = 7; i >= 0; i--)
	{
		byte data_bit = (bank >> i) & 1 ? VIA_SPI_MOSI : 0;
		gtvia.iora = data_bit;
		gtvia.iora = data_bit | VIA_SPI_CLK;   // Clock rising edge
	}

	gtvia.iora = VIA_SPI_CS;   // Latch
	gtvia.iora = 0;            // Reset
}

// ---------------------------------------------------------------------------
// Wait for blitter to finish (IRQ fires, handler clears it)
// ---------------------------------------------------------------------------
static void wait_for_irq(void)
{
	__asm volatile
	{
		byt 0xcb    // WAI — wait for interrupt (65C02)
	}
}

// ---------------------------------------------------------------------------
// Library Functions
// ---------------------------------------------------------------------------

void gt_init(void)
{
	// Initialize banking register: enable X/Y clipping so blitter
	// doesn't wrap pixels past the screen edges.
	shadow_banking = BANK_CLIP_X | BANK_CLIP_Y;
	gtsys.banking = shadow_banking;

	// Select ROM bank 254 (banked region, required for 2MB carts)
	via_set_rom_bank(254);

	// Clear audio subsystem
	gtsys.audio_reset = 0;
	gtsys.audio_nmi = 0;
	gtsys.audio_rate = 0;

	// Clear any pending blitter IRQ
	gtblitter.start = 0;

	// Set default DMA flags: enable DMA, enable IRQ, opaque mode.
	// DMA_PAGE_OUT is set here so that after the first gt_flip(),
	// the display page (0) and draw page (1) are different.
	// Without this, both pages start in sync and we'd draw on the
	// visible buffer, causing flickering.
	shadow_dma_flags = DMA_ENABLE | DMA_PAGE_OUT | DMA_IRQ | DMA_OPAQUE;
	gtsys.dma_flags = shadow_dma_flags;

	// Flip once: display toggles to page 0, blitter toggles to page 1
	gt_flip();

	// Clear both framebuffer pages to black so there's no garbage border.
	// VRAM is uninitialized at power-on, so both pages must be wiped.
	gt_clear(GT_BLACK);
	gt_flip();
	gt_clear(GT_BLACK);
	gt_flip();
}

void gt_flip(void)
{
	// Toggle which framebuffer page is shown on screen
	shadow_dma_flags ^= DMA_PAGE_OUT;
	gtsys.dma_flags = shadow_dma_flags;

	// Toggle which framebuffer page the blitter draws into
	shadow_banking ^= BANK_VRAM_SELECT;
	gtsys.banking = shadow_banking;
}

void gt_clear(byte color)
{
	// The blitter's width/height fields are 7 bits (bit 7 = flip flag),
	// so a single operation covers at most 127x127 pixels. The framebuffer
	// is 128x128, so we tile it with four 64x64 quadrants.
	gtsys.dma_flags = shadow_dma_flags | DMA_COLORFILL;

	gtblitter.width = 64;
	gtblitter.height = 64;
	gtblitter.color = color;

	gtblitter.vx = 0;
	gtblitter.vy = 0;
	gtblitter.start = 1;
	wait_for_irq();

	gtblitter.vx = 64;
	gtblitter.vy = 0;
	gtblitter.start = 1;
	wait_for_irq();

	gtblitter.vx = 0;
	gtblitter.vy = 64;
	gtblitter.start = 1;
	wait_for_irq();

	gtblitter.vx = 64;
	gtblitter.vy = 64;
	gtblitter.start = 1;
	wait_for_irq();

	gtsys.dma_flags = shadow_dma_flags;
}

void gt_draw_box(byte x, byte y, byte w, byte h, byte color)
{
	// Enable color fill mode
	gtsys.dma_flags = shadow_dma_flags | DMA_COLORFILL;

	gtblitter.vx = x;
	gtblitter.vy = y;
	gtblitter.width = w;
	gtblitter.height = h;
	gtblitter.color = color;
	gtblitter.start = 1;       // Trigger DMA

	wait_for_irq();            // Wait for blitter to finish

	// Restore DMA flags
	gtsys.dma_flags = shadow_dma_flags;
}

void gt_wait_vblank(void)
{
	// Enable NMI (fires on vertical blank)
	shadow_dma_flags |= DMA_NMI;
	gtsys.dma_flags = shadow_dma_flags;

	// Halt CPU until NMI fires
	__asm volatile
	{
		byt 0xcb    // WAI
	}

	// Disable NMI until next frame
	shadow_dma_flags &= ~DMA_NMI;
	gtsys.dma_flags = shadow_dma_flags;
}

void gt_sync(void)
{
	// Enable NMI temporarily (don't modify shadow — it doesn't have NMI)
	gtsys.dma_flags = shadow_dma_flags | DMA_NMI;

	// Halt CPU until vblank NMI fires
	__asm volatile
	{
		byt 0xcb    // WAI
	}

	// NMI just returned — we are inside the vblank window.
	// Flip the display page immediately. Writing shadow_dma_flags
	// (which has no DMA_NMI) also disables NMI in the same write.
	shadow_dma_flags ^= DMA_PAGE_OUT;
	gtsys.dma_flags = shadow_dma_flags;

	shadow_banking ^= BANK_VRAM_SELECT;
	gtsys.banking = shadow_banking;
}

unsigned gt_read_gamepad(void)
{
	// Read gamepad2 to reset the input latch
	volatile byte unused = gtsys.gamepad2;
	(void)unused;

	// Two consecutive reads of gamepad1: low byte, then high byte
	byte lo = gtsys.gamepad1;
	__asm volatile {}          // Compiler barrier between reads
	byte hi = gtsys.gamepad1;

	// Combine into 16-bit value and invert (buttons are active-low)
	return ~((unsigned)hi << 8 | lo);
}
