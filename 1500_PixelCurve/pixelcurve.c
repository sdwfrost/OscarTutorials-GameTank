// 1500_PixelCurve — Parametric curve drawing with pixel-level plotting
//
// GameTank port of OscarTutorials/1500_BitmapPixels.
// The original draws a spirograph-like parametric curve on a C64 hires
// bitmap using floating-point sin/cos. Here we draw individual pixels
// (1x1 boxes) using sine table lookups, building up a Lissajous-like
// pattern as a scrolling trail.
//
// Key concept: plotting individual pixels on the framebuffer using the
// blitter's color fill mode with 1x1 dimensions.

#include "gt.h"

// Reuse the same sine table as 4250_SineTable
// sintab[i] = round(40 * sin(i * 2*PI / 256)), range [-40, 40]
static const signed char sintab[256] = {
	   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   13,   14,
	  15,   16,   17,   18,   19,   20,   21,   21,   22,   23,   24,   25,   25,   26,   27,   28,
	  28,   29,   30,   30,   31,   32,   32,   33,   33,   34,   34,   35,   35,   36,   36,   37,
	  37,   37,   38,   38,   38,   39,   39,   39,   39,   39,   40,   40,   40,   40,   40,   40,
	  40,   40,   40,   40,   40,   40,   40,   39,   39,   39,   39,   39,   38,   38,   38,   37,
	  37,   37,   36,   36,   35,   35,   34,   34,   33,   33,   32,   32,   31,   30,   30,   29,
	  28,   28,   27,   26,   25,   25,   24,   23,   22,   21,   21,   20,   19,   18,   17,   16,
	  15,   14,   13,   13,   12,   11,   10,    9,    8,    7,    6,    5,    4,    3,    2,    1,
	   0,   -1,   -2,   -3,   -4,   -5,   -6,   -7,   -8,   -9,  -10,  -11,  -12,  -13,  -13,  -14,
	 -15,  -16,  -17,  -18,  -19,  -20,  -21,  -21,  -22,  -23,  -24,  -25,  -25,  -26,  -27,  -28,
	 -28,  -29,  -30,  -30,  -31,  -32,  -32,  -33,  -33,  -34,  -34,  -35,  -35,  -36,  -36,  -37,
	 -37,  -37,  -38,  -38,  -38,  -39,  -39,  -39,  -39,  -39,  -40,  -40,  -40,  -40,  -40,  -40,
	 -40,  -40,  -40,  -40,  -40,  -40,  -40,  -39,  -39,  -39,  -39,  -39,  -38,  -38,  -38,  -37,
	 -37,  -37,  -36,  -36,  -35,  -35,  -34,  -34,  -33,  -33,  -32,  -32,  -31,  -30,  -30,  -29,
	 -28,  -28,  -27,  -26,  -25,  -25,  -24,  -23,  -22,  -21,  -21,  -20,  -19,  -18,  -17,  -16,
	 -15,  -14,  -13,  -13,  -12,  -11,  -10,   -9,   -8,   -7,   -6,   -5,   -4,   -3,   -2,   -1
};

#define TRAIL_LEN  128

// Circular buffer for trail positions
static byte trail_x[TRAIL_LEN];
static byte trail_y[TRAIL_LEN];
static byte trail_head;
static byte trail_count;

int main(void)
{
	gt_init();

	trail_head = 0;
	trail_count = 0;

	// Multiple angle accumulators at different frequencies
	// produce the spirograph effect (like the original's
	// cos(w) + cos(w*5) + cos(w*13) superposition)
	byte a1 = 0;   // x component: frequency 1
	byte a2 = 0;   // x component: frequency 5
	byte a3 = 0;   // y component: frequency 3
	byte a4 = 0;   // y component: frequency 7

	for (;;)
	{
		gt_clear(GT_BLACK);

		// Compute new point using superposition of sine waves
		// x = center + sin(a1+90)*30/40 + sin(a2+90)*10/40
		// y = center + sin(a3)*30/40 + sin(a4)*10/40
		int px = 64 + (sintab[(a1 + 64) & 0xFF] * 3 / 4)
		            + (sintab[(a2 + 64) & 0xFF] / 4);
		int py = 64 + (sintab[a3] * 3 / 4)
		            + (sintab[a4] / 4);

		// Clamp to screen
		if (px < 0) px = 0;
		if (px > 126) px = 126;
		if (py < 0) py = 0;
		if (py > 126) py = 126;

		// Add to trail buffer
		trail_x[trail_head] = (byte)px;
		trail_y[trail_head] = (byte)py;
		trail_head = (trail_head + 1) & (TRAIL_LEN - 1);
		if (trail_count < TRAIL_LEN)
			trail_count++;

		// Draw entire trail
		byte idx = (trail_head - trail_count) & (TRAIL_LEN - 1);
		for (byte i = 0; i < trail_count; i++)
		{
			// Fade color based on age: newer = brighter
			byte color = (i > trail_count - 20) ? GT_WHITE : GT_LIGHT_GRAY;
			gt_draw_box(trail_x[idx], trail_y[idx], 2, 2, color);
			idx = (idx + 1) & (TRAIL_LEN - 1);
		}

		// Draw current point larger and in a bright color
		gt_draw_box((byte)px, (byte)py, 3, 3, GT_YELLOW);

		gt_sync();

		// Advance angles at different rates for Lissajous effect
		a1 += 1;
		a2 += 5;
		a3 += 3;
		a4 += 7;
	}

	return 0;
}
