#include "bsp/board_api.h"
#include "tusb.h"

#define REPORT_ID_GAMEPAD 1

//--------------------------------------------------------------------+
// Custom Definitions
//--------------------------------------------------------------------+

// Vendor ID, Product ID, and BCD (Binary Coded Decimal) USB version
#define USB_VID 0xCafe
#define USB_BCD 0x0200

// Map the enabled classes to the Product ID to ensure uniqueness.
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID                                                                \
  (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(HID, 1)) // CDC is bit 0, HID is bit 1


//--------------------------------------------------------------------+
// Device Descriptor
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = USB_BCD,

    // Use Interface Association Descriptor (IAD) for CDC, required for
    // composite device
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100, // Device release number

    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,

    .bNumConfigurations = 0x01,
};

// Invoked when received GET DEVICE DESCRIPTOR
uint8_t const *tud_descriptor_device_cb(void) {
  return (uint8_t const *)&desc_device;
}


// Gamepad Report Descriptor: Defines the structure of the data sent to the host
// A basic 16-button, 4-axis (X, Y, Z, Rz) report.
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_GAMEPAD ( HID_REPORT_ID(REPORT_ID_GAMEPAD))  
};

// Invoked when received GET HID REPORT DESCRIPTOR
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
  (void)instance;
  return desc_hid_report;
}


//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

// Interface numbers for the composite device
enum { ITF_NUM_CDC_CONTROL = 0, ITF_NUM_CDC_DATA, ITF_NUM_HID, ITF_NUM_TOTAL };

// Total length of the configuration descriptor, including all interfaces
#define CONFIG_TOTAL_LEN                                                       \
  (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_DESC_LEN)

// Endpoint number mapping (TinyUSB will auto-allocate from 0x01/0x81 up)
// This is typically the default mapping and works well for Pico.
#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT 0x02
#define EPNUM_CDC_IN 0x82
#define EPNUM_HID 0x83

uint8_t const desc_fs_configuration[] = {
    // Config number, interface count, string index, total length, attribute,
    // power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

    // Interface 0 & 1: CDC (Serial)
    // Interface number (ITF_NUM_CDC_CONTROL), string index (4), EPs
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_CONTROL, 4, EPNUM_CDC_NOTIF, 8,
                       EPNUM_CDC_OUT, EPNUM_CDC_IN, CFG_TUD_CDC_EP_BUFSIZE),

    // Interface 2: HID (Gamepad)
    // Interface number (ITF_NUM_HID), string index (5), protocol (0 for none),
    // report descriptor len, EP (interrupt IN), interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 5, HID_ITF_PROTOCOL_NONE,
                       sizeof(desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 5),
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
  (void)index;
  // Pico is Full Speed only, so we only return the FS descriptor
  return desc_fs_configuration;
}


//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// String Descriptor Index
enum {
  STRID_LANGID = 0,
  STRID_MANUFACTURER,
  STRID_PRODUCT,
  STRID_SERIAL,
  STRID_CDC_INTERFACE,
  STRID_HID_INTERFACE
};

// array of pointer to string descriptors
char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04},      // 0: English (0x0409)
    "Raspberry Pi",                  // 1: Manufacturer
    "Pico Composite Gamepad/Serial", // 2: Product Name
    NULL,                            // 3: Serials (will be filled by unique ID)
    "Pico Debug Port (CDC)",         // 4: CDC Interface Name
    "Pico Game Controller (HID)",    // 5: HID Interface Name
};

// Standard string descriptor callback (left minimal for brevity, keep the
// original implementation)
static uint16_t _desc_str[32 + 1];
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void)langid;
  size_t chr_count;
  if (index == STRID_LANGID) {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  } else if (index == STRID_SERIAL) {
    chr_count = board_usb_get_serial(_desc_str + 1, 32);
  } else {
    const char *str = string_desc_arr[index];
    chr_count = strlen(str);
    size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1;
    if (chr_count > max_count)
      chr_count = max_count;
    for (size_t i = 0; i < chr_count; i++) {
      _desc_str[1 + i] = str[i];
    }
  }
  _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
  return _desc_str;
}
