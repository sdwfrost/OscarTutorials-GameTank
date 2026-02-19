// 4010_FixPointCircle — Fixed-point math to draw a circle of boxes
//
// GameTank port of OscarTutorials/4010_FixPointNumbers.
// The original demonstrates fixed-point arithmetic by rotating a 2D vector.
// Here we use the same technique to draw boxes arranged in a circle,
// animating them by rotating the pattern each frame.

#include "gt.h"

// 8-bit fixed-point (8 integer bits, 8 fraction bits)
#define FBITS    8
#define FONE     (1 << FBITS)
#define FHALF    (FONE / 2)

// Fixed-point multiply: (a * b) >> FBITS
#define FMUL(a, b) ((int)((long)(a) * (long)(b) >> FBITS))

// Number of boxes to draw around the circle
#define NUM_POINTS 12

// Box size for each point on the circle
#define BOX_SIZE   6

// Circle radius in pixels
#define RADIUS     40

// Center of screen
#define CX  (GT_SCREEN_W / 2 - BOX_SIZE / 2)
#define CY  (GT_SCREEN_H / 2 - BOX_SIZE / 2)

// Step angle: full circle / NUM_POINTS, in fixed-point radians
// 2*PI ~= 6.283; 6.283 / 12 ~= 0.5236; 0.5236 * 256 ~= 134
#define ANGLE_STEP 134

int main(void)
{
	gt_init();

	// Angle offset for animation (incremented each frame)
	int angle_offset = 0;

	// Pre-stepped unit vectors: start at (1.0, 0.0) in fixed-point
	// We'll recompute positions each frame by rotating step-by-step

	for (;;)
	{
		gt_clear(GT_BLUE);

		// Start vector at angle_offset: rotate (FONE, 0) by angle_offset steps
		int ux = FONE;     // cos(0) = 1.0
		int uy = 0;        // sin(0) = 0.0

		// Pre-rotate by angle_offset small steps to set starting angle
		// Step size for the offset rotation: same as ANGLE_STEP but smaller
		// We use 1/256 of a circle per offset tick
		// ds ~= 2*PI/256 * 256 = ~6.28 -> ds_fixed = 6
		for (int a = 0; a < angle_offset; a++)
		{
			int dx = FMUL(6, -uy);
			int dy = FMUL(6, ux);
			ux += dx;
			uy += dy;
		}

		// Normalize length — after many rotations the vector drifts.
		// Simple approximation: scale back toward unit length.
		// |v|^2 in fixed-point; target is FONE^2 = 1<<16
		// Skip normalization for simplicity — drift is slow enough
		// for a visual demo over ~256 frames before reset.

		// Now draw NUM_POINTS boxes equally spaced around the circle
		for (byte i = 0; i < NUM_POINTS; i++)
		{
			// Convert unit vector to screen coordinates
			int px = CX + FMUL(RADIUS << FBITS, ux) / FONE;
			int py = CY + FMUL(RADIUS << FBITS, uy) / FONE;

			// Clamp to screen bounds
			if (px < 0) px = 0;
			if (px > GT_SCREEN_W - BOX_SIZE - 1) px = GT_SCREEN_W - BOX_SIZE - 1;
			if (py < 0) py = 0;
			if (py > GT_SCREEN_H - BOX_SIZE - 1) py = GT_SCREEN_H - BOX_SIZE - 1;

			// Alternate colors for visual interest
			byte color = (i & 1) ? GT_YELLOW : GT_WHITE;
			gt_draw_box((byte)px, (byte)py, BOX_SIZE, BOX_SIZE, color);

			// Rotate unit vector by one step (2*PI / NUM_POINTS)
			int dx = FMUL(ANGLE_STEP, -uy);
			int dy = FMUL(ANGLE_STEP, ux);
			ux += dx;
			uy += dy;
		}

		gt_sync();

		angle_offset++;
		if (angle_offset >= 128)
			angle_offset = 0;
	}

	return 0;
}
