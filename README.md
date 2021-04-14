# 6502 MEP

This is a work in progress port of the [Micro Entertainment Pack](https://itch.io/c/707330/micro-entertainment-pack) game [Archaeologist](https://itch.io/queue/c/707330/micro-entertainment-pack?game_id=555076) (essentially a clone of Minesweeper) to 6502-based 8 bit micros.


It compiles (using the provided makefile) using the `ca65` assembler from the `cc65` tool chain. Currently it supports:

- Atari 400, 800, and XE/XL series machines (`main.xex`)
- Commodore 64 machines (`main.prg`)
- Apple ][ (both monochrome and colour displays) (`main.dsk`), also works on later Apple II models.

Work is currently in progress on BBC Micro and other variants.

