load("@pico-sdk//tools:uf2_aspect.bzl", "pico_uf2_aspect")

def _pico_usb_device_transition_impl(settings, attr):
    tusb_config_label = str(attr.tinyusb_config)
    return {
        "//command_line_option:platforms": "@pico-sdk//bazel/platform:rp2350",
        "@pico-sdk//bazel/config:PICO_STDIO_USB": attr.stdio_usb,
        "@pico-sdk//bazel/config:PICO_STDIO_UART": attr.stdio_uart,
        "@pico-sdk//bazel/config:PICO_TINYUSB_CONFIG": str(attr.tinyusb_config),
        "//command_line_option:copt": ["-DLIB_TINYUSB_DEVICE=1"],        
    }

pico_rp2350_transition = transition(
    implementation = _pico_usb_device_transition_impl,
    inputs = [],
    outputs = [
        "//command_line_option:platforms",
        "@pico-sdk//bazel/config:PICO_STDIO_USB",
        "@pico-sdk//bazel/config:PICO_STDIO_UART",
        "@pico-sdk//bazel/config:PICO_TINYUSB_CONFIG",
        "//command_line_option:copt",
    ],
)

def _pico_firmware_impl(ctx):
    binary_to_convert = ctx.attr.binary[0][DefaultInfo].files_to_run.executable
    uf2_output = ctx.actions.declare_file(binary_to_convert.basename + ".uf2")
    ctx.actions.run(
        outputs = [uf2_output],
        inputs = [binary_to_convert],
        tools = [ctx.executable._picotool],
        executable = ctx.executable._picotool,
        arguments = [
            "uf2",
            "convert",
            "--quiet",
            "-t",
            "elf",
            binary_to_convert.path,
            uf2_output.path,
        ],
    )
    return [DefaultInfo(files = depset([uf2_output], transitive = [ctx.attr.binary[0].files]))]

pico_firmware = rule(
    implementation = _pico_firmware_impl,
    attrs = {
        "binary": attr.label(
            cfg = pico_rp2350_transition,
            mandatory = True,
        ),
        # 1. Add the new attribute here
        "tinyusb_config": attr.label(
            mandatory = True,
        ),
        "stdio_usb": attr.bool(
            default = True,
            doc = "Enable USB stdio",
        ),
        "stdio_uart": attr.bool(
            default = True,
            doc = "Enable UART stdio",
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "_picotool": attr.label(default = "@picotool//:picotool", executable = True, cfg = "exec"),
    },
)
