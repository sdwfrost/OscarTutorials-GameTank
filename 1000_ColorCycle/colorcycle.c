// 1000_ColorCycle — Cycle the background color every frame
//
// GameTank port of OscarTutorials/1000_BorderColor.
// The original increments the VIC-II border color register continuously.
// Here we clear the screen with an incrementing palette index each frame.

#include "gt.h"

int main(void)
{
	gt_init();

	byte color_index = 0;

	for (;;)
	{
		gt_clear(GT_COLOR(color_index));

		gt_sync();

		color_index++;
	}

	return 0;
}
