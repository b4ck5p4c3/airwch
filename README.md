# airwch

AirTag firmware for WCH

## Requirements

- [Latest RISC-V toolchain from HydraUSB3](https://github.com/hydrausb3/riscv-none-elf-gcc-xpack)
- CMake >= 3.12
- [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html)

## BLE data update guide

1. Edit MAC address in `src/main.c`
2. Edit data in `src/main.c` after `30, 0xFF`

## Programming guide

1. Open WCHISPTool
2. Put AirWCH on programming rig with battery pin facing to buttons
3. While holding **blue** button connect rig to USB
4. Add your `firmware.hex` as Object File 1 in WCHISPTool
5. Press `Download`
6. Wait for download
7. ???
8. PROFIT