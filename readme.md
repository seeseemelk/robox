# Robox project

## Project information

All info about the project can be found on the wiki. (work in progress)
This contains info on the hardware, firmware, manuals, ...
(wiki main page here)[https://github.com/seeseemelk/robox/wiki]

## Supporting the project

Do you like the project? Please consider supporting us! We are working on a V2 which will be easier to produce on our side and assemble on your side.

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://ko-fi.com/robox)

## Attiny source code

### Installation Microchip toolchain

https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers

linux: add path of extracted archive to your PATH
(e.g. in .bashrc: `export PATH="$PATH:/<path>/avr8-gnu-toolchain-linux_x86_64/bin"`)

add `/home/<user>/avr8-gnu-toolchain-linux_x86_64/avr/include` to C_INCLUDE_PATH

install package `avr-libc`
(```
/usr/lib/gcc/avr/5.4.0/../../../avr/bin/ld: cannot find crtattiny461a.o: No such file or directory
collect2: error: ld returned 1 exit status
make: *** [Makefile:48: bin/robox.elf] Error 1
```)

### Build source

```bash
$ make help
Targets:
  all       - (default) Builds the firmware. Same as the elf target
  bin       - Produces a bin file
  clean     - Cleans the compiled objects
  elf       - Produces an ELF file
  flash     - Flash the project onto a chip
  help      - Shows this help file
  hex       - Produces a hex file
  hex-text  - Produces an intel hex file
```

## FFT library
https://github.com/kosme/fix_fft

extra function anotations:
https://gist.github.com/Tomwi/3842231

## Owl

### Installation

1. Download from http://www.jtxp.org/tech/onewayloader_en.htm#download

1. check hash:
```bash
$ sha256sum owl.zip 
b97ae12d1bdd31152652383fb7a0708e94f02b179006a6694b1ff04fcfbaf0b5  owl.zip
```

### Usage

http://www.jtxp.org/tech/onewayloader_en.htm#quickstart

#### Add new target
(Make tailored OWL Firmware)

```bash
$ ./owl --device=tn461A --targetname=owl_robox --clock=8000 --rxport=B6

SINGLE TARGET MAKE MODE

MAKE TARGET FIRMWARE

RXPORT NORMAL LOGIC
TXPORT NORMAL LOGIC
MODIFY PORT ASSIGNMENTS
ALL RESET SOURCES ENABLED
MODIFY CLOCK REFERENCE FOR ATTINY

TARGET LOADER DATA

FILEPATH 	: ./targets/owl_robox.hex
DEVICENAME	: tn461
SIGNATURE	: 1E9208
FW BUILD DATE	: 20210223
EEPROMSIZE	: 256
FLASHSIZE	: 4096
PAGESIZE	: 64
BOOTSIZE 	: 512
APPFLASH	: 3580
RXPORT   	: B6
TXPORT   	: --
CLOCK    	: 8000
CIPHER   	: RST
KEY      	: 6A352411627D7E31...
RESET SOURCES	: 1111 W B E P 
BAUD RANGE	: 800...16000
DEFAULT BAUD	: 9600
DEFAULT TIMEOUT	: 1.00
DEFAULT IP	: 1.00
DEFAULT OP	: 0.10

NEW TARGET FILE SUCCESSFULLY SAVED.
```

#### Create audio file

```bash
$ ./owl --targetname=owl_robox --flashfile=../robox/bin/robox.hex --audioexport

TRANSMISSION MODE


TARGET LOADER DATA

FILEPATH 	: ./targets/owl_robox.hex
DEVICENAME	: tn461
SIGNATURE	: 1E9208
FW BUILD DATE	: 20210223
EEPROMSIZE	: 256
FLASHSIZE	: 4096
PAGESIZE	: 64
BOOTSIZE 	: 512
APPFLASH	: 3580
RXPORT   	: B0
TXPORT   	: --
CLOCK    	: 4000
CIPHER   	: RST
KEY      	: 34FB4E1420DF639C...
RESET SOURCES	: 1111 W B E P 
BAUD RANGE	: 400...8000
DEFAULT BAUD	: 4800
DEFAULT TIMEOUT	: 1.00
DEFAULT IP	: 1.00
DEFAULT OP	: 0.10

MAKE OWL TRANSMISSION

FILEPATH	: ./transmissions/20220305__robox_owl_robox.owl
BAUDRATE	: 4800
INTPREAMBLE	: 1.00
OUTPREAMBLE	: 0.10
[S1] AUTH	: 76BA4C20E8736540D0DCF76EB228B3B0  /  3
[S2] EEPROM	: 1053021E6E3BE4A352B2886D64B3C638  /  2
[S3] FLASH	: 3DD6527248B6ECDCE2501FA8F9886A9D  /  6
BYTES TOTAL	: 1112
DURATION   	: 2.32

EXPORT TO AUDIO FILE (PCM 2-CH 16 BIT 44.1K)

SOURCE FILE	: ./transmissions/20220305__robox_owl_robox.owl
WAVFILENAME	: ./transmissions/20220305__robox_owl_robox.owl.wav
WAVFILESIZE	: 408364
NOMINAL BAUD	: 4800
```
