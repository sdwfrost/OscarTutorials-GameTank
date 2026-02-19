// 4260_CordicCircle — CORDIC algorithm for sine/cosine
//
// GameTank port of OscarTutorials/4260_CosinCordic.
// The original uses the CORDIC algorithm (COordinate Rotation DIgital
// Computer) to compute sine and cosine using only shifts and adds —
// no multiplication or lookup tables needed. Here we use the same
// algorithm to move a box in a circle, just like the SineTable tutorial
// but with on-the-fly computation.

#include "gt.h"

#define BOX_SIZE   6
#define RADIUS     40

#define CX  (GT_SCREEN_W / 2 - BOX_SIZE / 2)
#define CY  (GT_SCREEN_H / 2 - BOX_SIZE / 2)

// CORDIC arctangent table: atan(2^-i) scaled to 16-bit angle units
// where 32768 = PI (full circle = 65536 units)
static const int cordic_atan[8] = {8192, 4836, 2555, 1297, 651, 326, 163, 81};

// Compute sine and cosine using CORDIC algorithm.
// Input:  w = angle in 16-bit units (0..65535 = 0..2*PI)
// Output: *si = sine, *co = cosine, both in range [-40, 40] approx
static void cordic_sincos(int w, signed char *si, signed char *co)
{
	// Start with a pre-scaled unit vector.
	// The CORDIC gain after 8 iterations is K ~= 1.6468.
	// The output magnitude is start * K, then >>8 for the final value.
	// For output range [-40, 40]: start = 40 * 256 / K = 10240 / 1.6468 ~= 6218
	int dx = 6218;
	int dy = 0;

	// If angle is in the second or third quadrant, flip to first/fourth
	if (w > 16384 || w < -16384)
	{
		w ^= (int)0x8000;
		dx = -dx;
	}

	// 8 CORDIC iterations — each rotates by atan(2^-i)
	for (byte i = 0; i < 8; i++)
	{
		int sx = dx >> i;
		int sy = dy >> i;

		if (w > 0)
		{
			// Rotate forward (counter-clockwise)
			dx += sy;
			dy -= sx;
			w -= cordic_atan[i];
		}
		else
		{
			// Rotate backward (clockwise)
			dx -= sy;
			dy += sx;
			w += cordic_atan[i];
		}
	}

	// Return MSBs as approximate sine and cosine
	*si = (signed char)(dy >> 8);
	*co = (signed char)(dx >> 8);
}

int main(void)
{
	gt_init();

	// 8-bit angle — wraps at 256 = full circle
	byte angle = 0;

	for (;;)
	{
		gt_clear(GT_BLUE);

		signed char sx, sy;

		// Compute sine and cosine via CORDIC
		// Shift angle left 8 bits to get 16-bit angle units
		cordic_sincos((int)angle << 8, &sx, &sy);

		// Draw crosshair at center
		gt_draw_box(CX + BOX_SIZE / 2 - 1, CY - 8, 2, 16 + BOX_SIZE, GT_DARK_GRAY);
		gt_draw_box(CX - 8, CY + BOX_SIZE / 2 - 1, 16 + BOX_SIZE, 2, GT_DARK_GRAY);

		// Clamp to screen bounds (CORDIC can slightly overshoot)
		int px = CX + sx;
		int py = CY + sy;
		if (px < 0) px = 0;
		if (px > GT_SCREEN_W - BOX_SIZE) px = GT_SCREEN_W - BOX_SIZE;
		if (py < 0) py = 0;
		if (py > GT_SCREEN_H - BOX_SIZE) py = GT_SCREEN_H - BOX_SIZE;

		// Draw box at computed position
		gt_draw_box((byte)px, (byte)py, BOX_SIZE, BOX_SIZE, GT_WHITE);

		gt_sync();

		angle++;
	}

	return 0;
}
