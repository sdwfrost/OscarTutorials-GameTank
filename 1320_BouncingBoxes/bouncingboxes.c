// 1320_BouncingBoxes — Multiple boxes bouncing off screen edges
//
// GameTank port of OscarTutorials/1320_ReflectingSprite.
// The original bounces 8 VIC-II sprites with random velocities.
// Here we bounce 4 colored boxes, reflecting off all four screen edges.
// Uses 4-bit fixed-point for smooth sub-pixel movement.

#include "gt.h"

#define NUM_BOXES 4
#define BOX_SIZE  10

#define FBITS     4
#define RIGHT_X   ((GT_SCREEN_W - BOX_SIZE - 1) << FBITS)
#define BOTTOM_Y  ((GT_SCREEN_H - BOX_SIZE - 1) << FBITS)

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

	// Initialize with varied positions and velocities
	// Velocities are in fixed-point: 4 = 0.25 px/frame, 8 = 0.5, 16 = 1.0
	boxes[0].sx = 10 << FBITS;  boxes[0].sy = 20 << FBITS;
	boxes[0].vx = 10;           boxes[0].vy = 6;
	boxes[0].color = GT_RED;

	boxes[1].sx = 80 << FBITS;  boxes[1].sy = 10 << FBITS;
	boxes[1].vx = -7;           boxes[1].vy = 9;
	boxes[1].color = GT_GREEN;

	boxes[2].sx = 50 << FBITS;  boxes[2].sy = 90 << FBITS;
	boxes[2].vx = 8;            boxes[2].vy = -5;
	boxes[2].color = GT_CYAN;

	boxes[3].sx = 30 << FBITS;  boxes[3].sy = 60 << FBITS;
	boxes[3].vx = -6;           boxes[3].vy = -8;
	boxes[3].color = GT_YELLOW;

	for (;;)
	{
		gt_clear(GT_BLACK);

		for (byte i = 0; i < NUM_BOXES; i++)
		{
			// Advance position
			int nx = boxes[i].sx + boxes[i].vx;
			int ny = boxes[i].sy + boxes[i].vy;

			// Reflect off left/right edges
			if (nx < 0 || nx > RIGHT_X)
				boxes[i].vx = -boxes[i].vx;
			else
				boxes[i].sx = nx;

			// Reflect off top/bottom edges
			if (ny < 0 || ny > BOTTOM_Y)
				boxes[i].vy = -boxes[i].vy;
			else
				boxes[i].sy = ny;

			gt_draw_box(
				(byte)(boxes[i].sx >> FBITS),
				(byte)(boxes[i].sy >> FBITS),
				BOX_SIZE, BOX_SIZE, boxes[i].color);
		}

		gt_sync();
	}

	return 0;
}
