
main.xex: main.s MyAtari.cfg my_atari.inc minesweeper.inc
	cl65 -t atari -C MyAtari.cfg  main.s -o main.xex 

main.prg: main.s MyC64.cfg my_c64.inc minesweeper.inc
	cl65 -t c64 -C MyC64.cfg -u __EXEHDR__ main.s -o main.prg

main.bbc: main.s bbc.cfg
	cl65 -t bbc -C MyBBC.cfg --no-target-lib main.s -o main.bbc

main.as: main.s apple2_rand.inc my_apple2.inc MyA2.cfg minesweeper.inc
	cl65 -t apple2 -C MyA2.cfg main.s -o main.as

tools/a2in: tools a2tools.c
	cd tools && gcc -DUNIX a2tools.c -o a2in

main.dsk: main.as tools/a2in tools/blank.dsk
	cp ./tools/blank.dsk main.dsk && ./tools/a2in -r b main.dsk MAIN main.as

apple2: main.dsk
	open ~/Applications/Virtual\ \]\[.app

bbc: main.bbc
	~/Applications/b2.app/Contents/MacOS/b2 main.bbc
	
atari: main.xex
	~/Applications/Atari800MacX/Atari800MacX.app/Contents/MacOS/Atari800MacX main.xex

c64: main.prg
	~/Applications/VICE.app/Contents/Resources/bin/x64sc main.prg
	
