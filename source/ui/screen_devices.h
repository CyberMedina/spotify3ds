#pragma once

#include <citro2d.h>
#include <stdbool.h>

#include "../spotify/player.h"
#include "../worker.h"
#include "touch.h"

enum {
	DEVICE_BTN_BACK = 0,
	DEVICE_BTN_REFRESH,
	DEVICE_ROW0 = 10, /* .. DEVICE_ROW0 + MAX_DEVICES - 1 */
};

typedef struct {
	C2D_TextBuf buf;
	touch_builder *tb;
	const worker_devices_snapshot *devices;
	int pressed_id;
	int cursor_id;
} screen_devices_args;

void screen_devices_draw(const screen_devices_args *a);
