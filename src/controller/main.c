
#include "bsp/board_api.h"
#include "pico/stdio.h"
#include "tusb.h"
#include <stdio.h>

#define REPORT_ID_GAMEPAD 1

static char hello_msg[33];
static uint32_t hello_msg_len = 32;

static void led_blinking_task(void);
static void cdc_task(void);
static void hid_task(void);

/*------------- MAIN -------------*/
int main(void) {
  board_init();
  tusb_rhport_init_t dev_init = {.role = TUSB_ROLE_DEVICE,
                                 .speed = TUSB_SPEED_AUTO};
  tusb_init(BOARD_TUD_RHPORT, &dev_init);
  stdio_init_all();

  while (1) {
    tud_task();
    hid_task();
    // led_blinking_task();
  }
}

static void hid_task(void) {
  // Check if HID is ready and not busy with a previous report
  if (!tud_hid_ready()) {
    return;
  }

  // Time interval for alternating the X button (e.g., 500 ms)
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;
  static bool x_button_state = false;

  // Only send a report if the interval has elapsed
  if (board_millis() - start_ms < interval_ms) {
    return;
  }
  start_ms += interval_ms;

  // Toggle the X button state
  x_button_state = !x_button_state;

  // Prepare the Gamepad report
  hid_gamepad_report_t report = {
      .x = 0,
      .y = 0,
      .z = 0,
      .rz = 0,
      .rx = 0,
      .ry = 0,
      .hat = 0,
      .buttons = board_millis() % 65535,
  };
  board_led_write(true);
  // Send the HID report
  const bool success =
      tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

  if (success) {
    printf("Sent report!\n");
  }
}
