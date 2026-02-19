// 1330_CollidingBoxes — Bouncing boxes with collision detection
//
// GameTank port of OscarTutorials/1330_CollidingSprite.
// The original bounces 8 VIC-II sprites and uses hardware sprite-sprite
// collision detection to change colors on impact.
// Here we bounce 4 boxes with software AABB collision detection.
// Colliding boxes turn yellow; non-colliding boxes show their base color.

#include "gt.h"

#define NUM_BOXES  4
#define BOX_SIZE   10

// 4-bit fixed-point for sub-pixel movement
#define FBITS      4
#define RIGHT_X    ((GT_SCREEN_W - BOX_SIZE - 1) << FBITS)
#define BOTTOM_Y   ((GT_SCREEN_H - BOX_SIZE - 1) << FBITS)

struct Box
{
	int sx, sy;         // position (fixed-point)
	int vx, vy;         // velocity (fixed-point)
	byte base_color;    // normal color
	byte draw_color;    // color this frame (yellow if colliding)
};

// AABB overlap test using pixel coordinates
static bool boxes_overlap(int x1, int y1, int x2, int y2)
{
	if (x1 + BOX_SIZE <= x2 || x2 + BOX_SIZE <= x1)
		return false;
	if (y1 + BOX_SIZE <= y2 || y2 + BOX_SIZE <= y1)
		return false;
	return true;
}

int main(void)
{
	gt_init();

	struct Box boxes[NUM_BOXES];

	// Initialize with varied positions and velocities
	boxes[0].sx = 10 << FBITS;  boxes[0].sy = 20 << FBITS;
	boxes[0].vx = 10;           boxes[0].vy = 6;
	boxes[0].base_color = GT_RED;

	boxes[1].sx = 80 << FBITS;  boxes[1].sy = 10 << FBITS;
	boxes[1].vx = -7;           boxes[1].vy = 9;
	boxes[1].base_color = GT_GREEN;

	boxes[2].sx = 50 << FBITS;  boxes[2].sy = 90 << FBITS;
	boxes[2].vx = 8;            boxes[2].vy = -5;
	boxes[2].base_color = GT_CYAN;

	boxes[3].sx = 30 << FBITS;  boxes[3].sy = 60 << FBITS;
	boxes[3].vx = -6;           boxes[3].vy = -8;
	boxes[3].base_color = GT_MAGENTA;

	for (;;)
	{
		gt_clear(GT_BLACK);

		// Advance positions and bounce off walls
		for (byte i = 0; i < NUM_BOXES; i++)
		{
			int nx = boxes[i].sx + boxes[i].vx;
			int ny = boxes[i].sy + boxes[i].vy;

			if (nx < 0 || nx > RIGHT_X)
				boxes[i].vx = -boxes[i].vx;
			else
				boxes[i].sx = nx;

			if (ny < 0 || ny > BOTTOM_Y)
				boxes[i].vy = -boxes[i].vy;
			else
				boxes[i].sy = ny;

			// Default to base color
			boxes[i].draw_color = boxes[i].base_color;
		}

		// Check all pairs for AABB overlap
		for (byte i = 0; i < NUM_BOXES; i++)
		{
			int px_i = boxes[i].sx >> FBITS;
			int py_i = boxes[i].sy >> FBITS;

			for (byte j = i + 1; j < NUM_BOXES; j++)
			{
				int px_j = boxes[j].sx >> FBITS;
				int py_j = boxes[j].sy >> FBITS;

				if (boxes_overlap(px_i, py_i, px_j, py_j))
				{
					boxes[i].draw_color = GT_YELLOW;
					boxes[j].draw_color = GT_YELLOW;
				}
			}
		}

		// Draw all boxes
		for (byte i = 0; i < NUM_BOXES; i++)
		{
			gt_draw_box(
				(byte)(boxes[i].sx >> FBITS),
				(byte)(boxes[i].sy >> FBITS),
				BOX_SIZE, BOX_SIZE, boxes[i].draw_color);
		}

		gt_sync();
	}

	return 0;
}
