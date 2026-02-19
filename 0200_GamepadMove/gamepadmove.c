// 0200_GamepadMove — Move a box around with the gamepad
//
// GameTank port of OscarTutorials/0200_CursorMove.
// The original uses keyboard input to move a cursor on a text screen.
// Here we use gamepad d-pad to move a small box on the framebuffer.

#include "gt.h"

#define BOX_SIZE 8

int main(void)
{
	gt_init();

	byte x = (GT_SCREEN_W - BOX_SIZE) / 2;
	byte y = (GT_SCREEN_H - BOX_SIZE) / 2;

	for (;;)
	{
		// Read gamepad
		unsigned pad = gt_read_gamepad();

		// Move box according to d-pad (with boundary checks)
		if ((pad & INPUT_UP) && y > 0)
			y--;
		if ((pad & INPUT_DOWN) && y < GT_SCREEN_H - BOX_SIZE - 1)
			y++;
		if ((pad & INPUT_LEFT) && x > 0)
			x--;
		if ((pad & INPUT_RIGHT) && x < GT_SCREEN_W - BOX_SIZE - 1)
			x++;

		// Clear screen and redraw box
		gt_clear(GT_BLACK);
		gt_draw_box(x, y, BOX_SIZE, BOX_SIZE, GT_WHITE);

		gt_sync();
	}

	return 0;
}
