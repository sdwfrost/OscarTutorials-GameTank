// 0010_HelloColors — Fill the screen with colored stripes
//
// GameTank port of OscarTutorials/0010_HelloWorld.
// Since GameTank has no text mode, we demonstrate output by filling
// the screen with colorful horizontal stripes instead of printing text.

#include "gt.h"

int main(void)
{
	gt_init();

	// Draw 8 horizontal stripes across the screen
	byte colors[8];
	colors[0] = GT_RED;
	colors[1] = GT_ORANGE;
	colors[2] = GT_YELLOW;
	colors[3] = GT_GREEN;
	colors[4] = GT_CYAN;
	colors[5] = GT_BLUE;
	colors[6] = GT_MAGENTA;
	colors[7] = GT_WHITE;

	byte stripe_h = GT_SCREEN_H / 8;

	for (byte i = 0; i < 8; i++)
	{
		gt_draw_box(0, i * stripe_h, 127, stripe_h - 1, colors[i]);
	}

	// Wait for vblank then flip to show the result
	gt_sync();

	// Halt — nothing more to do
	for (;;)
	{
		gt_wait_vblank();
	}

	return 0;
}
