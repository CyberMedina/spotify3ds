#include "screen_devices.h"

#include <stdio.h>
#include <string.h>

#include "ui.h"

#define BOT_W 320.0f
#define BOT_H 240.0f
#define HEADER_H 30.0f
#define PAD_X 16.0f
#define ROW_H 42.0f

#define CLR_HEADER      C2D_Color32(0x11, 0x11, 0x11, 0xFF)
#define CLR_NAME        C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF)
#define CLR_SUB         C2D_Color32(0x8A, 0x8A, 0x8A, 0xFF)
#define CLR_GREEN       C2D_Color32(0x1D, 0xB9, 0x54, 0xFF)
#define CLR_GREEN_PRESS C2D_Color32(0x28, 0xD8, 0x68, 0xFF)
#define CLR_ROW_PRESS   C2D_Color32(0x24, 0x24, 0x24, 0xFF)
#define CLR_ROW_CURSOR  C2D_Color32(0x1A, 0x1A, 0x1A, 0xFF)
#define CLR_DIVIDER     C2D_Color32(0x22, 0x22, 0x22, 0xFF)
#define CLR_IDLE        C2D_Color32(0xB3, 0xB3, 0xB3, 0xFF)

static void draw_chevron_left(float x, float y, u32 clr)
{
	C2D_DrawLine(x + 5.0f, y - 6.0f, clr, x, y, clr, 2.0f, 0.0f);
	C2D_DrawLine(x, y, clr, x + 5.0f, y + 6.0f, clr, 2.0f, 0.0f);
}

static void draw_chevron_right(float x, float y, u32 clr)
{
	C2D_DrawLine(x - 5.0f, y - 6.0f, clr, x, y, clr, 2.0f, 0.0f);
	C2D_DrawLine(x, y, clr, x - 5.0f, y + 6.0f, clr, 2.0f, 0.0f);
}

void screen_devices_draw(const screen_devices_args *a)
{
	/* --- Header -------------------------------------------------------- */
	C2D_DrawRectSolid(0.0f, 0.0f, 0.0f, BOT_W, HEADER_H, CLR_HEADER);

	/* Back button */
	const bool back_pressed = a->pressed_id == DEVICE_BTN_BACK;
	const u32 back_clr = back_pressed ? CLR_GREEN : CLR_IDLE;
	draw_chevron_left(PAD_X + 2.0f, HEADER_H / 2.0f, back_clr);
	ui_text(a->buf, "Player", PAD_X + 14.0f,
	        ui_baseline((HEADER_H - ui_px(TY_ROW_NAME)) / 2.0f, TY_ROW_NAME),
	        TY_ROW_NAME, 60.0f, back_clr);
	tb_add(a->tb, 0.0f, 0.0f, 80.0f, HEADER_H, DEVICE_BTN_BACK);

	/* Header title */
	const float title_w = ui_text_width(a->buf, "DEVICES", TY_MICRO);
	ui_text_tracked(a->buf, "DEVICES", (BOT_W - title_w) / 2.0f,
	                ui_baseline((HEADER_H - ui_px(TY_MICRO)) / 2.0f, TY_MICRO),
	                TY_MICRO, 0.6f, CLR_NAME);

	/* Refresh action */
	const bool refresh_pressed = a->pressed_id == DEVICE_BTN_REFRESH;
	const u32 refresh_clr = refresh_pressed ? CLR_GREEN : CLR_SUB;
	const float ref_w = ui_text_width(a->buf, "REFRESH", TY_MICRO);
	ui_text_tracked(a->buf, "REFRESH", BOT_W - PAD_X - ref_w,
	                ui_baseline((HEADER_H - ui_px(TY_MICRO)) / 2.0f, TY_MICRO),
	                TY_MICRO, 0.45f, refresh_clr);
	tb_add(a->tb, BOT_W - PAD_X - ref_w - 8.0f, 0.0f, ref_w + 16.0f, HEADER_H,
	       DEVICE_BTN_REFRESH);

	C2D_DrawRectSolid(0.0f, HEADER_H - 1.0f, 0.0f, BOT_W, 1.0f, CLR_DIVIDER);

	/* --- Device List or Fallback States --------------------------------- */
	const worker_devices_snapshot *snap = a->devices;
	const int count = snap ? snap->devices.count : 0;

	if (snap && snap->state == DEVICES_LOADING && count == 0) {
		const float y = 110.0f;
		const float tw = ui_text_width(a->buf, "Finding Spotify devices...", TY_ROW_NAME);
		ui_text(a->buf, "Finding Spotify devices...", (BOT_W - tw) / 2.0f,
		        ui_baseline(y, TY_ROW_NAME), TY_ROW_NAME, BOT_W - 32.0f, CLR_SUB);
		return;
	}

	if (snap && snap->state == DEVICES_ERROR && count == 0) {
		const float y = 90.0f;
		const char *msg = snap->error[0] ? snap->error : "Could not fetch devices";
		const float tw = ui_text_width(a->buf, msg, TY_ROW_NAME);
		ui_text(a->buf, msg, (BOT_W - tw) / 2.0f,
		        ui_baseline(y, TY_ROW_NAME), TY_ROW_NAME, BOT_W - 32.0f, CLR_SUB);

		const float hint_w = ui_text_width(a->buf, "Tap REFRESH to try again", TY_ROW_SUB);
		ui_text(a->buf, "Tap REFRESH to try again", (BOT_W - hint_w) / 2.0f,
		        ui_baseline(y + 24.0f, TY_ROW_SUB), TY_ROW_SUB, BOT_W - 32.0f, CLR_SUB);
		return;
	}

	if (count == 0) {
		const float y = 85.0f;
		const float w1 = ui_text_width(a->buf, "No available devices found", TY_ROW_NAME);
		ui_text(a->buf, "No available devices found", (BOT_W - w1) / 2.0f,
		        ui_baseline(y, TY_ROW_NAME), TY_ROW_NAME, BOT_W - 32.0f, CLR_NAME);

		const float w2 = ui_text_width(a->buf, "Open Spotify on your PC, phone, or speaker", TY_ROW_SUB);
		ui_text(a->buf, "Open Spotify on your PC, phone, or speaker", (BOT_W - w2) / 2.0f,
		        ui_baseline(y + 22.0f, TY_ROW_SUB), TY_ROW_SUB, BOT_W - 32.0f, CLR_SUB);

		const float w3 = ui_text_width(a->buf, "and tap REFRESH above.", TY_ROW_SUB);
		ui_text(a->buf, "and tap REFRESH above.", (BOT_W - w3) / 2.0f,
		        ui_baseline(y + 38.0f, TY_ROW_SUB), TY_ROW_SUB, BOT_W - 32.0f, CLR_SUB);
		return;
	}

	float y = HEADER_H + 4.0f;
	for (int i = 0; i < count; i++) {
		if (y + ROW_H > BOT_H)
			break;

		const spotify_device *dev = &snap->devices.items[i];
		const int row_id = DEVICE_ROW0 + i;
		const bool is_pressed = a->pressed_id == row_id;
		const bool is_cursor = a->cursor_id == row_id;

		/* Row background highlight */
		if (is_pressed)
			C2D_DrawRectSolid(0.0f, y, 0.0f, BOT_W, ROW_H, CLR_ROW_PRESS);
		else if (is_cursor)
			C2D_DrawRectSolid(0.0f, y, 0.0f, BOT_W, ROW_H, CLR_ROW_CURSOR);

		/* Device dot indicator */
		const float dot_x = PAD_X + 6.0f;
		const float dot_y = y + ROW_H / 2.0f;
		if (dev->is_active)
			ui_disc(dot_x, dot_y, 4.0f, CLR_GREEN);
		else
			ui_disc(dot_x, dot_y, 3.0f, CLR_SUB);

		/* Device name */
		const u32 name_clr = dev->is_active ? CLR_GREEN : CLR_NAME;
		ui_text(a->buf, dev->name, PAD_X + 20.0f,
		        ui_baseline(y + 4.0f, TY_ROW_NAME), TY_ROW_NAME,
		        BOT_W - PAD_X * 2.0f - 40.0f, name_clr);

		/* Device subtitle */
		if (dev->is_active) {
			ui_text_tracked(a->buf, "CURRENT DEVICE", PAD_X + 20.0f,
			                ui_baseline(y + 23.0f, TY_MICRO), TY_MICRO, 0.5f,
			                CLR_GREEN);
		} else {
			char sub[64];
			const char *type = dev->type[0] ? dev->type : "Device";
			snprintf(sub, sizeof sub, "%s%s", type,
			         dev->supports_volume ? " \xC2\xB7 Volume" : "");
			ui_text(a->buf, sub, PAD_X + 20.0f,
			        ui_baseline(y + 23.0f, TY_ROW_SUB), TY_ROW_SUB,
			        BOT_W - PAD_X * 2.0f - 40.0f, CLR_SUB);
		}

		/* Right chevron or active indicator */
		if (dev->is_active) {
			const float active_w = ui_text_width(a->buf, "ACTIVE", TY_MICRO);
			ui_text_tracked(a->buf, "ACTIVE", BOT_W - PAD_X - active_w,
			                ui_baseline(y + 14.0f, TY_MICRO), TY_MICRO, 0.5f,
			                CLR_GREEN);
		} else {
			draw_chevron_right(BOT_W - PAD_X - 4.0f, y + ROW_H / 2.0f,
			                   is_pressed ? CLR_GREEN : CLR_SUB);
		}

		/* Row divider */
		C2D_DrawRectSolid(PAD_X, y + ROW_H - 1.0f, 0.0f, BOT_W - PAD_X * 2.0f,
		                  1.0f, CLR_DIVIDER);

		/* Hitbox */
		tb_add(a->tb, 0.0f, y, BOT_W, ROW_H, row_id);

		y += ROW_H;
	}
}
