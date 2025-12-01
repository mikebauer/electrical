#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

/********************************************************************************\
 * BOARD CONFIG                                                                 *
\********************************************************************************/

#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

// Pico
#define CFG_TUSB_OS OPT_OS_PICO

/********************************************************************************\
 * DEVICE CONFIG                                                                *
\********************************************************************************/

// Enable the Device stack
#define CFG_TUD_ENABLED 1

// Endpoint 0 Size (Control Endpoint)
// Default is 64 for Full-Speed/High-Speed, necessary for communication.
#define CFG_TUD_ENDPOINT0_SIZE 64

//------------- CLASS -------------//

#define CFG_TUD_CDC 1
#define CFG_TUD_HID 1
#define CFG_TUD_MSC 0
#define CFG_TUD_MIDI 0
#define CFG_TUD_VENDOR 0

//------------- CDC BUFFERS -------------//

#define CFG_TUD_CDC_RX_BUFSIZE 64
#define CFG_TUD_CDC_TX_BUFSIZE 64
#define CFG_TUD_CDC_EP_BUFSIZE 64


//------------- HID BUFFER --------------//
#define CFG_TUD_HID_EP_BUFSIZE    16


//#define LIB_TINYUSB_DEVICE 1
//#define PICO_STDIO_USB_USE_DEFAULT_DESCRIPTORS 0

#endif /* _TUSB_CONFIG_H_ */
