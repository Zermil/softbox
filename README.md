# Softbox
Softbox is a crude mockery of [Hit Box Arcade](https://www.hitboxarcade.com/products/hit-box) controller, this repository contains the microcontroller code responsible for sending XInput 'packets'. The microcontroller used was [STM32F401CCU6 aka Blackpill (84MHz variant)](https://www.st.com/en/microcontrollers-microprocessors/stm32f401cc.html).

## Compiling
To compile this project you're going to need [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) mainly `arm-none-eabi-gcc` and `arm-none-eabi-ld`. Other than that you need [OpenOCD](https://openocd.org/) for debugging.

Surprisingly a lot of people asked: 'Why not use [XYZ] when working with STM32?' and to that I say: 'You're a grown man working with embedded, use the hecking compiler'.