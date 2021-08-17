INITVCTRS := $E45B
INITBA    := $E3A4
FREMSG    := $E404
READY     := $C474
charset_dest := $1800
.segment "LOADADDR"
        .export __LOADADDR__: absolute = 1
        .addr   *+2

.segment "STARTUP"

        .word reset
        .word   $FEA9

        ; Cart signature
        .byte   $41,$30,"CBM"

        INITMEM := $FD8D
        FRESTOR := $FD52
        INITVIA := $FDF9
        INITSK  := $E518

reset:
        jsr     INITMEM                 ; initialise and test RAM
        jsr     FRESTOR                 ; restore default I/O vectors
        jsr     INITVIA                 ; initialise I/O registers
        jsr     INITSK                  ; initialise hardware
        jsr     INITVCTRS               ; initialise BASIC vector table
        jsr     INITBA                  ; initialise BASIC RAM locations
        jsr     FREMSG                  ; print start up message and initialise memory pointers
        cli                             ; enable interrupts

        .macro copy_loop src,dest
        ldx #0
        : lda src,X
          sta dest,X
          inx
          cpx #0
          beq :+
          jsr :-
        :
        .endmacro

        copy_loop charset_raw, charset_dest
        copy_loop charset_raw+256, charset_dest+256
        copy_loop charset_raw+512, charset_dest+512
        copy_loop charset_raw+512+256, charset_dest+512+256
        ;set up the charset        
        lda $9005
        and #$F0
        ora #$0E
        sta $9005
        LDX #$00
        lda #%0001000
        sta $900F
        jmp Main
charset_raw:
.code
.incbin "chars_vic20-charset.bin",0,1024
