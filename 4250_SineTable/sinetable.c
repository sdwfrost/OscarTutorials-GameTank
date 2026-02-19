// 4250_SineTable — Precomputed sine table for circular motion
//
// GameTank port of OscarTutorials/4250_CosinTable.
// The original precomputes a 256-entry sine table and uses it to move
// a VIC-II sprite in a circle. Here we move a box around a circle
// using table lookups instead of expensive runtime trig.
//
// Key concept: replace sin()/cos() calls with array lookups.
// cos(a) = sin(a + 64) when the table has 256 entries per full circle.

#include "gt.h"

#define BOX_SIZE   6
#define RADIUS     40

#define CX  (GT_SCREEN_W / 2 - BOX_SIZE / 2)
#define CY  (GT_SCREEN_H / 2 - BOX_SIZE / 2)

// Precomputed: sintab[i] = round(40 * sin(i * 2*PI / 256))
// Range: [-40, 40]. 256 entries = one full circle.
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

int main(void)
{
	gt_init();

	// 8-bit angle — wraps naturally at 256 = full circle
	byte angle = 0;

	for (;;)
	{
		gt_clear(GT_BLUE);

		// Look up sine and cosine from the table
		// cos(a) = sin(a + 64) since 64/256 = 1/4 turn = 90 degrees
		int sx = sintab[(angle + 64) & 0xFF];
		int sy = sintab[angle];

		// Draw crosshair at center for reference
		gt_draw_box(CX + BOX_SIZE / 2 - 1, CY - 8, 2, 16 + BOX_SIZE, GT_DARK_GRAY);
		gt_draw_box(CX - 8, CY + BOX_SIZE / 2 - 1, 16 + BOX_SIZE, 2, GT_DARK_GRAY);

		// Draw box at computed position
		gt_draw_box((byte)(CX + sx), (byte)(CY + sy), BOX_SIZE, BOX_SIZE, GT_WHITE);

		gt_sync();

		angle++;
	}

	return 0;
}
