#ifndef GT_H
#define GT_H

// GameTank Tutorial Helper Library
// Thin wrapper over GameTank hardware registers for educational use.
// Register definitions match <gametank/gametank.h> from oscar64.

typedef unsigned char byte;

// ---------------------------------------------------------------------------
// Hardware Register Structs
// ---------------------------------------------------------------------------

// System Control Registers at $2000
struct GTSystem
{
	volatile byte audio_reset;    // $2000
	volatile byte audio_nmi;      // $2001
	byte          _pad0[3];       // $2002-$2004
	volatile byte banking;        // $2005 - Banking & video control (write-only)
	volatile byte audio_rate;     // $2006
	volatile byte dma_flags;      // $2007 - DMA/Video control (write-only)
	volatile byte gamepad1;       // $2008 - Player 1 input (read-only)
	volatile byte gamepad2;       // $2009 - Player 2 input (read-only)
};

#define gtsys (*((struct GTSystem *)0x2000))

// Blitter Registers at $4000
struct GTBlitter
{
	volatile byte vx;       // $4000 - Dest X in framebuffer
	volatile byte vy;       // $4001 - Dest Y in framebuffer
	volatile byte gx;       // $4002 - Source X in sprite RAM
	volatile byte gy;       // $4003 - Source Y in sprite RAM
	volatile byte width;    // $4004 - Width (bit 7 = H flip)
	volatile byte height;   // $4005 - Height (bit 7 = V flip)
	volatile byte start;    // $4006 - Write to trigger DMA / write 0 to clear IRQ
	volatile byte color;    // $4007 - Fill color (when color fill mode enabled)
};

#define gtblitter (*((struct GTBlitter *)0x4000))

// VIA 6522 Registers at $2800 (used for ROM bank switching)
struct GTVIA
{
	volatile byte iorb;     // $2800
	volatile byte iora;     // $2801 - SPI data/clock for ROM bank select
	volatile byte ddrb;     // $2802
	volatile byte ddra;     // $2803
	volatile byte t1cl;     // $2804
	volatile byte t1ch;     // $2805
	volatile byte t1ll;     // $2806
	volatile byte t1lh;     // $2807
	volatile byte t2cl;     // $2808
	volatile byte t2ch;     // $2809
	volatile byte sr;       // $280A
	volatile byte acr;      // $280B
	volatile byte pcr;      // $280C
	volatile byte ifr;      // $280D
	volatile byte ier;      // $280E
	volatile byte iora_nh;  // $280F
};

#define gtvia (*((struct GTVIA *)0x2800))

// ---------------------------------------------------------------------------
// Banking Register Bits ($2005)
// ---------------------------------------------------------------------------
#define BANK_VRAM_SELECT  0x08
#define BANK_CLIP_X       0x10
#define BANK_CLIP_Y       0x20

// ---------------------------------------------------------------------------
// DMA Flags ($2007)
// ---------------------------------------------------------------------------
#define DMA_ENABLE         0x01
#define DMA_PAGE_OUT       0x02
#define DMA_NMI            0x04
#define DMA_COLORFILL      0x08
#define DMA_GCARRY         0x10
#define DMA_CPU_TO_VRAM    0x20
#define DMA_IRQ            0x40
#define DMA_OPAQUE         0x80

// ---------------------------------------------------------------------------
// VIA SPI Bits (ROM bank select)
// ---------------------------------------------------------------------------
#define VIA_SPI_CLK   0x01
#define VIA_SPI_MOSI  0x02
#define VIA_SPI_CS    0x04

// ---------------------------------------------------------------------------
// Gamepad Button Masks (from two consecutive reads of gamepad1)
// ---------------------------------------------------------------------------
#define INPUT_UP      0x0808
#define INPUT_DOWN    0x0404
#define INPUT_LEFT    0x0200
#define INPUT_RIGHT   0x0100
#define INPUT_A       0x0010
#define INPUT_B       0x1000
#define INPUT_C       0x2000
#define INPUT_START   0x0020

// ---------------------------------------------------------------------------
// Screen Dimensions
// ---------------------------------------------------------------------------
#define GT_SCREEN_W   128
#define GT_SCREEN_H   128

// ---------------------------------------------------------------------------
// Color Helpers
// ---------------------------------------------------------------------------
// The blitter inverts the color register value before writing to VRAM.
// VRAM values index into a hardware palette. GT_COLOR(n) writes palette
// index n to the framebuffer.
#define GT_COLOR(n)  ((byte)~(n))

// Predefined colors (palette indices, pre-inverted for register writes)
#define GT_BLACK      GT_COLOR(0x20)
#define GT_WHITE      GT_COLOR(0xDF)
#define GT_RED        GT_COLOR(0x5E)
#define GT_GREEN      GT_COLOR(0x38)
#define GT_BLUE       GT_COLOR(0xDC)
#define GT_YELLOW     GT_COLOR(0x7E)
#define GT_CYAN       GT_COLOR(0xFC)
#define GT_MAGENTA    GT_COLOR(0x5C)
#define GT_ORANGE     GT_COLOR(0x76)
#define GT_DARK_GRAY  GT_COLOR(0x08)
#define GT_LIGHT_GRAY GT_COLOR(0xB6)

// ---------------------------------------------------------------------------
// Library Functions
// ---------------------------------------------------------------------------

// Initialize hardware: banking, audio, blitter IRQ, DMA flags, framebuffer
void gt_init(void);

// Flip the double-buffered framebuffer (swap display and draw pages)
void gt_flip(void);

// Clear the entire screen with a solid color
void gt_clear(byte color);

// Draw a filled rectangle at (x,y) with dimensions w x h
void gt_draw_box(byte x, byte y, byte w, byte h, byte color);

// Wait for the next vertical blank (frame sync)
void gt_wait_vblank(void);

// Wait for vblank then immediately flip — ensures the page swap happens
// during the blanking interval to prevent tearing. Use this instead of
// separate gt_wait_vblank() + gt_flip() calls.
void gt_sync(void);

// Read gamepad state; returns bitmask (test with INPUT_* constants)
unsigned gt_read_gamepad(void);

#pragma compile("gt.c")

#endif
