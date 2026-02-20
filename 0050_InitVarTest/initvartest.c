// 0050_InitVarTest — Verify that initialized global/static variables are writable
//
// This test demonstrates that initialized variables are correctly copied from
// ROM to RAM at startup, so they can be both read and modified at runtime.
// Previously, initialized globals were placed in ROM and writes were silently
// ignored, requiring a workaround of declaring uninitialized + assigning later.
//
// The test displays a column of colored boxes, one per test case.
// GREEN = pass, RED = fail.

#include "gt.h"

// ---------------------------------------------------------------------------
// Test cases: initialized global and static variables of various types
// ---------------------------------------------------------------------------

// 1. Initialized global integer
int global_counter = 42;

// 2. Initialized global unsigned
unsigned global_mask = 0xACE1;

// 3. Initialized global byte
byte global_byte = 0x55;

// 4. Initialized global array
byte global_array[4] = {10, 20, 30, 40};

// 5. Initialized global pointer value (stored as an integer for testing)
int global_ptr_val = 0x1234;

// Test result tracking
#define NUM_TESTS 8
static byte results[NUM_TESTS];

static void run_tests(void)
{
	byte pass;
	byte t = 0;

	// --- Test 1: Global int has correct initial value ---
	results[t++] = (global_counter == 42) ? 1 : 0;

	// --- Test 2: Global int can be modified ---
	global_counter = 99;
	results[t++] = (global_counter == 99) ? 1 : 0;

	// --- Test 3: Global unsigned has correct initial value and can be modified ---
	pass = (global_mask == 0xACE1) ? 1 : 0;
	global_mask = 0xBEEF;
	pass = pass & ((global_mask == 0xBEEF) ? 1 : 0);
	results[t++] = pass;

	// --- Test 4: Global byte has correct initial value and can be modified ---
	pass = (global_byte == 0x55) ? 1 : 0;
	global_byte = 0xAA;
	pass = pass & ((global_byte == 0xAA) ? 1 : 0);
	results[t++] = pass;

	// --- Test 5: Global array has correct initial values ---
	pass = (global_array[0] == 10 && global_array[1] == 20 &&
	        global_array[2] == 30 && global_array[3] == 40) ? 1 : 0;
	results[t++] = pass;

	// --- Test 6: Global array elements can be modified ---
	global_array[0] = 99;
	global_array[3] = 1;
	pass = (global_array[0] == 99 && global_array[1] == 20 &&
	        global_array[2] == 30 && global_array[3] == 1) ? 1 : 0;
	results[t++] = pass;

	// --- Test 7: Static local with initializer ---
	{
		static int local_static = 777;
		pass = (local_static == 777) ? 1 : 0;
		local_static = 888;
		pass = pass & ((local_static == 888) ? 1 : 0);
		results[t++] = pass;
	}

	// --- Test 8: Global pointer-width value ---
	pass = (global_ptr_val == 0x1234) ? 1 : 0;
	global_ptr_val = 0x5678;
	pass = pass & ((global_ptr_val == 0x5678) ? 1 : 0);
	results[t++] = pass;
}

static void draw_results(void)
{
	// Draw a column of boxes: one per test, green=pass, red=fail
	// Each box is 16x12 pixels, starting at x=8
	byte box_w = 16;
	byte box_h = 12;
	byte gap = 2;
	byte x0 = 8;
	byte y0 = 4;

	for (byte i = 0; i < NUM_TESTS; i++)
	{
		byte color = results[i] ? GT_WHITE : GT_RED;
		byte y = y0 + i * (box_h + gap);
		gt_draw_box(x0, y, box_w, box_h, color);
	}

	// Draw a summary box on the right: green if ALL passed, red otherwise
	byte all_pass = 1;
	for (byte i = 0; i < NUM_TESTS; i++)
	{
		if (!results[i])
		{
			all_pass = 0;
			break;
		}
	}

	byte summary_color = all_pass ? GT_WHITE : GT_RED;
	gt_draw_box(40, 40, 48, 48, summary_color);
}

int main(void)
{
	gt_init();

	run_tests();

	// Draw on both framebuffer pages for stable display
	gt_clear(GT_BLACK);
	draw_results();
	gt_sync();
	gt_clear(GT_BLACK);
	draw_results();
	gt_sync();

	// Halt
	for (;;)
	{
		gt_wait_vblank();
	}

	return 0;
}
