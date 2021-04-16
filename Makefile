
main.xex: main.s MyAtari.cfg my_atari.inc minesweeper.inc
	cl65 -t atari -C MyAtari.cfg  main.s -o main.xex 

main.prg: main.s MyC64.cfg my_c64.inc minesweeper.inc
	cl65 -t c64 -C MyC64.cfg -u __EXEHDR__ main.s -o main.prg

main.bbc: main.s MyBBC.cfg my_bbc.inc minesweeper.inc
	cl65 -t bbc -C MyBBC.cfg --no-target-lib main.s -o main.bbc

main.inf: main.bbc
	tools/generate_inf.sh main.bbc MAIN 003000 003000 0 1 main.inf

main.ssd: main.bbc main.inf tools/!boot tools/!boot.inf
	rm -f main.ssd
	cp main.bbc main && cp tools/!boot* . && tools/beebtools/beeb blank_ssd main.ssd && tools/beebtools/beeb putfile main.ssd main !boot && tools/beebtools/beeb opt4 main.ssd 3

main.as: main.s apple2_rand.inc my_apple2.inc MyA2.cfg minesweeper.inc apple_symbols.inc
	cl65 -t apple2 -C MyA2.cfg main.s -o main.as

tools/a2in: tools/a2tools.c
	cd tools && gcc -DUNIX a2tools.c -o a2in

main.dsk: main.as tools/a2in tools/blank.dsk
	cp ./tools/blank.dsk main.dsk && ./tools/a2in -r b main.dsk MAIN main.as

apple2: main.dsk
	open ~/Applications/Virtual\ \]\[.app

bbc: main.ssd
	~/Applications/b2.app/Contents/MacOS/b2 -b -0 main.ssd
	
atari: main.xex
	~/Applications/Atari800MacX/Atari800MacX.app/Contents/MacOS/Atari800MacX main.xex

c64: main.prg
	~/Applications/VICE.app/Contents/Resources/bin/x64sc main.prg
	
clean:
	rm -f main.prg main.xex main.ssd main.o main.bbc main.as main.dsk main.bin main.inf main !boot !boot.inf 