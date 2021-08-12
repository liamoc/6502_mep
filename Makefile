releases: main.dsk main.xex main.ssd main.prg main.hex

clean:
	rm -f \
	    main.dsk main.as \
	    main.xex \
	    main.ssd main.bbc main.inf main !boot !boot.inf \
	    main.prg \
	    main.o main.bin \


####################################################################
#   Run, release and build targets
#
#   The first target for every machine is usually a "run" target
#   that uses a simulator or sends the program to the real machine.
#
#   The second is the target to build the release version (often
#   a disk image.)
#

##############################
#   Apple II

apple2: main.dsk
	open ~/Applications/Virtual\ \]\[.app

main.dsk: main.as tools/a2in tools/blank.dsk
	cp ./tools/blank.dsk main.dsk && ./tools/a2in -r b main.dsk MAIN main.as

main.as: main.s apple2_rand.inc my_apple2.inc MyA2.cfg minesweeper.inc apple_symbols.inc apple2_rc6502_visuals.inc
	cl65 -t apple2 -C MyA2.cfg main.s -o main.as

tools/a2in: tools/a2tools.c
	cd tools && gcc -DUNIX a2tools.c -o a2in

##############################
#   Atari

atari: main.xex
	~/Applications/Atari800MacX/Atari800MacX.app/Contents/MacOS/Atari800MacX main.xex

main.xex: main.s MyAtari.cfg my_atari.inc minesweeper.inc atari_c64_visuals.inc
	cl65 -t atari -C MyAtari.cfg  main.s -o main.xex


##############################
#   BBC

bbc: main.ssd
	~/Applications/b2.app/Contents/MacOS/b2 -b -0 main.ssd

main.ssd: main.bbc main.inf tools/!boot tools/!boot.inf
	rm -f main.ssd
	cp main.bbc main \
	    && cp tools/!boot* . \
	    && tools/beebtools/beeb blank_ssd main.ssd \
	    && tools/beebtools/beeb putfile main.ssd main !boot \
	    && tools/beebtools/beeb opt4 main.ssd 3

main.bbc: main.s MyBBC.cfg my_bbc.inc minesweeper.inc bbc_symbols.inc bbc_symbols2.inc
	cl65 -t bbc -C MyBBC.cfg --no-target-lib main.s -o main.bbc

main.inf: main.bbc
	tools/generate_inf.sh main.bbc MAIN 001900 001900 0 1 main.inf

##############################
#   Commodore 64

c64: main.prg
	~/Applications/VICE.app/Contents/Resources/bin/x64sc main.prg

main.prg: main.s MyC64.cfg my_c64.inc minesweeper.inc atari_c64_visuals.inc
	cl65 -t c64 -C MyC64.cfg -u __EXEHDR__ main.s -o main.prg

##############################
#   RC6502
#   https://github.com/tebl/RC6502-Apple-1-Replica

rc6502: main.hex
	python3 test.py

main.hex: main.rc6
	printf "0300" > main.hex
	hexdump -v -e '8/1 " %02X" "\n"' main.rc6 | sed 's/^/:/g' >> main.hex

main.rc6: main.s MyRC6502.cfg my_rc6502.inc minesweeper.inc apple2_rand.inc apple2_rc6502_visuals.inc rc6502_symbols.inc
	cl65 --asm-define __RC6502__ -t none -C MyRC6502.cfg --no-target-lib main.s -o main.rc6
