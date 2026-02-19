// 1310_MovingBox — Boxes moving downward, wrapping around
//
// GameTank port of OscarTutorials/1310_MovingSprite.
// The original moves 8 VIC-II hardware sprites downward each frame.
// Here we move 4 colored boxes that wrap from bottom to top.

#include "gt.h"

#define NUM_BOXES 4
#define BOX_SIZE  12

int main(void)
{
	gt_init();

	byte bx[NUM_BOXES];
	byte by[NUM_BOXES];
	byte bc[NUM_BOXES];

	// Initialize box positions and colors
	bx[0] = 10;  by[0] = 10;  bc[0] = GT_RED;
	bx[1] = 40;  by[1] = 30;  bc[1] = GT_GREEN;
	bx[2] = 70;  by[2] = 50;  bc[2] = GT_BLUE;
	bx[3] = 100; by[3] = 70;  bc[3] = GT_YELLOW;

	for (;;)
	{
		gt_clear(GT_BLACK);

		// Draw and advance each box
		for (byte i = 0; i < NUM_BOXES; i++)
		{
			gt_draw_box(bx[i], by[i], BOX_SIZE, BOX_SIZE, bc[i]);

			// Move downward, wrap at bottom
			by[i] += 1 + i;
			if (by[i] >= GT_SCREEN_H - BOX_SIZE)
				by[i] = 0;
		}

		gt_sync();
	}

	return 0;
}
