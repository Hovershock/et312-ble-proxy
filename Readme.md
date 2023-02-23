# BLE Wrapper for the MK312-BT

This project aims to provide a Bluetooth Low Energy wrapper for the MK312-BT clone of the Erostek ET312 box.

The mk312 control code was pretty well lifted from https://github.com/bkifft/MK-312WS/, with minor modifications to allow the setting of the power level and removal of some unneeded code, such as increment and decrement operators.

*WARNING*: There is zero security on this right now. The box will only allow a single connection at a time, but anyone nearby that knew about your MK312 would be able to connect and override your box. Use at your own risk.

## TODO

- Expose the "on battery" setting
- Integrate this with the original repo, if bkifft wants so that his boards can be flashed to allow either web control or BLE control, though this might be challenging since the radio, I believe, only works with WiFi or BLE at a single time.
- Add security

## Security thoughts

Right now my plan is to use the "display only" + "keyboard entry" mode of Bluetooth security. When a device wants to connect (at least for the first time) I will hijack the LCD screen to show a six digit standard Bluetooth pairing code. The device connecting will then have to input the same six digit code, preventing the box control from being hijacked by someone without physical access.

## How this interfaces with the box

Currently I am using the carrier bor I got from [bkifft](https://github.com/bkifft/MK-312WS/tree/master/kicad). I then purchased an additional pin-compatible [ESP32 carrier board from Amazon](https://www.amazon.com/dp/B08L5XFWN6?psc=1&ref=ppx_yo2ov_dt_b_product_details) and soldered female headers onto it. That ESP32 module gets connected to the carrier board and is put right into the box in place of the original radio modules.

## References and credits

- The Buttplug.io documentation on the ET312: https://docs.buttplug.io/docs/stpihkal/protocols/erostek-et312b
- Bkifft's MK-312WS control board, which I pretty well stole to do the box interface: https://github.com/bkifft/MK-312WS/
- The Rubberfate mk312 Python module, which I used as a secondary reference for how the box communicates: https://github.com/Rubberfate/mk312com/

## Problems I ran in to

My original attempt at this was running from an ESP32 into the link port, using a max3232 breakout board I got on Amazon. I kept getting weird values back just doing the key exchange, with my hypothesis being that the box was sinking too much current for the max3232 chip to handle - at least the one I got. I can control my box just find through a USB RS232 interface, so I know the box works. Instead of continuing to troubleshoot my max3232 chip, I just switched to the embedded controller inside of the MK312 and never resolved the issues with the link port.