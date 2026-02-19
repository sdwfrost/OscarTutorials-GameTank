// 1350_GravityBoxes — Boxes with gravity physics
//
// GameTank port of OscarTutorials/1350_GravitySprite.
// The original applies gravity to VIC-II sprites with fixed-point positions.
// Here we apply gravity to colored boxes that bounce off the floor.
// Uses 4-bit fixed-point math for sub-pixel precision.

#include "gt.h"

#define NUM_BOXES 4
#define BOX_SIZE  8

// 6-bit fixed-point: upper bits = integer, lower 6 bits = fraction
#define FBITS   6
#define FONE    (1 << FBITS)

#define FLOOR_Y   ((GT_SCREEN_H - BOX_SIZE - 1) << FBITS)
#define RIGHT_X   ((GT_SCREEN_W - BOX_SIZE - 1) << FBITS)

struct Box
{
	int sx, sy;     // position (fixed-point)
	int vx, vy;     // velocity (fixed-point)
	byte color;
};

int main(void)
{
	gt_init();

	struct Box boxes[NUM_BOXES];

	// Initialize with varied positions and horizontal velocities
	boxes[0].sx = 10 << FBITS;  boxes[0].sy = 10 << FBITS;
	boxes[0].vx = 5;            boxes[0].vy = 0;
	boxes[0].color = GT_RED;

	boxes[1].sx = 80 << FBITS;  boxes[1].sy = 20 << FBITS;
	boxes[1].vx = -4;           boxes[1].vy = 0;
	boxes[1].color = GT_GREEN;

	boxes[2].sx = 40 << FBITS;  boxes[2].sy = 5 << FBITS;
	boxes[2].vx = 6;            boxes[2].vy = -4;
	boxes[2].color = GT_CYAN;

	boxes[3].sx = 100 << FBITS; boxes[3].sy = 30 << FBITS;
	boxes[3].vx = -3;           boxes[3].vy = 2;
	boxes[3].color = GT_YELLOW;

	for (;;)
	{
		gt_clear(GT_BLACK);

		// Draw a "floor" line
		gt_draw_box(0, GT_SCREEN_H - 1, 127, 1, GT_DARK_GRAY);

		for (byte i = 0; i < NUM_BOXES; i++)
		{
			// Apply gravity to vertical velocity
			boxes[i].vy += 1;

			// Advance position
			int nx = boxes[i].sx + boxes[i].vx;
			int ny = boxes[i].sy + boxes[i].vy;

			// Bounce off left/right walls
			if (nx < 0 || nx > RIGHT_X)
			{
				boxes[i].vx = -boxes[i].vx;
			}
			else
			{
				boxes[i].sx = nx;
			}

			// Bounce off floor/ceiling
			if (ny > FLOOR_Y)
			{
				boxes[i].sy = FLOOR_Y;
				// Reverse and dampen vertical velocity
				boxes[i].vy = -(boxes[i].vy * 7 / 8);
			}
			else if (ny < 0)
			{
				boxes[i].sy = 0;
				boxes[i].vy = -boxes[i].vy;
			}
			else
			{
				boxes[i].sy = ny;
			}

			// Convert fixed-point to pixel coordinates and draw
			gt_draw_box(
				(byte)(boxes[i].sx >> FBITS),
				(byte)(boxes[i].sy >> FBITS),
				BOX_SIZE, BOX_SIZE, boxes[i].color);
		}

		gt_sync();
	}

	return 0;
}
