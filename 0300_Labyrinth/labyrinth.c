// 0300_Labyrinth — Maze generation with colored cells
//
// GameTank port of OscarTutorials/0300_Labyrinth.
// The original generates a maze on the C64 text screen using iterative
// backtracking (random walk that carves paths and backtracks at dead ends).
// Here we generate a maze on the 128x128 framebuffer using 4x4 pixel cells,
// creating a 32x32 grid. Press Start to generate a new maze.

#include "gt.h"

#define GRID_W    32
#define GRID_H    32
#define CELL_SIZE 4

// Maze grid: 32x32 cells
// 0xFF = unvisited, 0xFE = border wall
// 0-3 = direction that led here (for backtracking)
// 0xFC = start cell marker
static byte maze[GRID_W * GRID_H];

// Direction offsets: right, down, left, up
static const int bdir[4] = {1, GRID_W, -1, -GRID_W};

// Simple 16-bit LFSR for pseudo-random numbers
static unsigned lfsr_state = 0xACE1;

static byte maze_rand(void)
{
	unsigned bit = ((lfsr_state >> 0) ^ (lfsr_state >> 2) ^
	                (lfsr_state >> 3) ^ (lfsr_state >> 5)) & 1;
	lfsr_state = (lfsr_state >> 1) | (bit << 15);
	return (byte)lfsr_state;
}

// Check if we can carve a path two cells in direction d from position p.
// The check ensures we don't break into existing corridors.
static bool maze_check(int p, byte d)
{
	int p1 = p + bdir[d];
	int p2 = p1 + bdir[d];
	int d0 = bdir[(d + 1) & 3];
	int d1 = bdir[(d + 3) & 3];

	return
		maze[p1] == 0xFF &&
		maze[p1 + d0] >= 0xFE &&
		maze[p1 + d1] >= 0xFE &&
		maze[p2] >= 0xFE &&
		maze[p2 + d0] >= 0xFE &&
		maze[p2 + d1] >= 0xFE;
}

static void maze_build(void)
{
	// Fill inner area with 0xFF (unvisited)
	for (int i = 0; i < GRID_W * GRID_H; i++)
		maze[i] = 0xFF;

	// Set border cells to 0xFE (permanent wall)
	for (int i = 0; i < GRID_H; i++)
	{
		maze[i * GRID_W] = 0xFE;
		maze[i * GRID_W + GRID_W - 1] = 0xFE;
	}
	for (int i = 0; i < GRID_W; i++)
	{
		maze[i] = 0xFE;
		maze[i + GRID_W * (GRID_H - 1)] = 0xFE;
	}

	// Start at center
	int p = (GRID_H / 2) * GRID_W + GRID_W / 2;
	maze[p] = 0xFC;

	for (;;)
	{
		// Pick a random starting direction and rotation
		byte d = maze_rand() & 3;
		byte dd = 1 + (maze_rand() & 2);
		byte i = 0;

		// Try all 4 directions
		while (i < 4 && !maze_check(p, d))
		{
			d = (d + dd) & 3;
			i++;
		}

		if (i == 4)
		{
			// Dead end — backtrack along the direction that brought us here
			p -= bdir[maze[p]];

			if (maze[p] == 0xFC)
			{
				// Back at start — maze is complete
				maze[p] = 0;
				return;
			}
		}
		else
		{
			// Carve path: move to next cell, remember direction for backtracking
			p += bdir[d];
			maze[p] = d;
		}
	}
}

static void maze_draw(void)
{
	for (byte y = 0; y < GRID_H; y++)
	{
		for (byte x = 0; x < GRID_W; x++)
		{
			byte val = maze[y * GRID_W + x];
			byte color = (val >= 0x80) ? GT_BLUE : GT_WHITE;
			gt_draw_box(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
		}
	}
}

int main(void)
{
	gt_init();

	for (;;)
	{
		maze_build();

		// Draw maze on BOTH framebuffer pages so it's stable
		gt_clear(GT_BLACK);
		maze_draw();
		gt_sync();
		gt_clear(GT_BLACK);
		maze_draw();
		gt_sync();

		// Wait for A button or Start to regenerate.
		// Use gt_wait_vblank() (not gt_sync) to avoid flipping pages.
		#define REGEN_MASK (INPUT_A | INPUT_START)

		// First wait for any current press to release
		while (gt_read_gamepad() & REGEN_MASK)
			gt_wait_vblank();

		// Then wait for new press
		for (;;)
		{
			unsigned pad = gt_read_gamepad();
			if (pad & REGEN_MASK)
				break;
			gt_wait_vblank();

			// Vary the LFSR based on frames waited (adds randomness)
			maze_rand();
		}
	}

	return 0;
}
