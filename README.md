# 6502 MEP

This is a work in progress port of the [Micro Entertainment Pack](https://itch.io/c/707330/micro-entertainment-pack) game [Archaeologist](https://itch.io/queue/c/707330/micro-entertainment-pack?game_id=555076) (essentially a clone of Minesweeper) to 6502-based 8 bit micros.


It compiles (using the provided makefile) using the `ca65` assembler from the `cc65` tool chain. Currently it supports:

- Atari 400, 800, and XE/XL series machines (`main.xex`)
- Commodore 64 machines (`main.prg`)
- Commodore Plus/4 and Commodore 16 machines (`main-plus4.prg`)
- Commodore VIC-20 (as a cartridge) (`main.crt`)
- Apple ][ (both monochrome and colour displays) (`main.dsk`), also works on later Apple II models.
- BBC Micro (`main.ssd`) (should work on all models, and on Electron)
- RC6502 Apple 1 Replica with a MC6847 VDU and Joystick Controller (`main.hex` is a dump of wozmon input, `main.rc6` is a raw binary of the same)

Work is currently in progress on PET and other variants.

