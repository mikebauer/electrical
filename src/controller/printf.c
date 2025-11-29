#include "tusb.h"
#include <stdarg.h>
#include <stdio.h>

void cdc_printf(const char *format, ...) {
  static char tx_buf[CFG_TUD_CDC_TX_BUFSIZE];

  // Check if the USB CDC is connected before trying to write
  if (!tud_cdc_connected()) {
    return;
  }

  // 1. Format the string into the local buffer
  va_list args;
  va_start(args, format);
  int len = vsnprintf(tx_buf, CFG_TUD_CDC_TX_BUFSIZE, format, args);
  va_end(args);

  // Ensure the length is positive and within the buffer size
  if (len > 0 && len < CFG_TUD_CDC_TX_BUFSIZE) {
    // 2. Write the formatted string and flush
    tud_cdc_write(tx_buf, len);
    tud_cdc_write_flush();
  }
}
