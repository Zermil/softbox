@echo off

set INC=-I"./code/dependencies"
set FILES="./code/first.c" "./code/main.c" "./code/system_stm32f4xx.c"
pushd %~dp0
if not exist ./build mkdir ./build

if "%1" == "release" (
  arm-none-eabi-gcc %INC% %FILES% -DSTM32F401xC -T"./code/linker.ld" -mcpu=cortex-m4 -mthumb -nostdlib -o ./build/softbox.elf
  openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program ./build/softbox.elf verify reset exit"
) else (
  arm-none-eabi-gcc %INC% %FILES% -DSTM32F401xC -O0 -g -T"./code/linker.ld" -mcpu=cortex-m4 -mthumb -nostdlib -o ./build/softbox.elf
  openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program ./build/softbox.elf verify reset exit"
)

popd
