load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//:defs.bzl", "pico_firmware")

cc_library(
    name = "printf",
    srcs = ["printf.c"],
    hdrs = ["printf.h"],
    deps = [
        "@tinyusb",
    ],
)

cc_library(
    name = "tusb_config",
    hdrs = ["tusb_config.h"],
    includes = ["."],
)

cc_library(
    name = "usb_descriptors",
    srcs = [
        "usb_descriptors.c",
    ],
    defines = [
        "LIB_PICO_STDIO_USB=1",
    ],
    deps = [
        "@pico-sdk//src/common/pico_usb_reset_interface_headers",
        "@pico-sdk//src/rp2_common/pico_stdlib",
        "@pico-sdk//src/rp2_common/pico_unique_id",
        "@tinyusb",
    ],
    alwayslink = True,
)

cc_library(
    name = "hid_callbacks",
    srcs = ["hid_callbacks.c"],
    deps = [
        "@tinyusb",
    ],
    alwayslink = True,
)

cc_binary(
    name = "main",
    srcs = [
        "main.c",
    ],
    deps = [
        ":hid_callbacks",
        ":printf",
        ":usb_descriptors",
        "@pico-sdk//src/rp2_common/pico_stdlib",
        "@tinyusb",
    ],
)

pico_firmware(
    name = "controller",
    binary = ":main",
    tinyusb_config = ":tusb_config",
)
