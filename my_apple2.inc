
.macro AS text
    .repeat .strlen(text), I
        .byte   .strat(text, I) | $80
    .endrep
.endmacro
.macro hide_cursor
.endmacro
.macro show_cursor
.endmacro 
.macro SILENCE
.endmacro
.PROC init
    rts
.ENDPROC
.PROC treble_note
    rts
.ENDPROC
.PROC bass_note
    rts
.ENDPROC

.PROC setup_timer
    rts
.ENDPROC
.PROC won_screen
rts
.ENDPROC
.PROC lost_screen
rts
.ENDPROC
.proc setup_sound
rts
.endproc
.proc dig_sound
rts
.endproc
.proc position_cursor_sprite
rts
.endproc
.proc draw_halfwidth_backdrop
rts
.endproc
.proc hide_byline
rts
.endproc
.proc print_static_hud
rts
.endproc
.proc print_board
rts
.endproc
.proc print_board_size
rts
.endproc
.proc print_intersections 
rts
.endproc
.PROC print_menu
    jsr print_static_hud
    jsr print_board_size
    rts
.ENDPROC
.PROC print_hud
    rts
.ENDPROC
.PROC printy
            COUT = $FDED
            LDX    #0
            LDA    MSG,X          ; load initial char
PRINTCHAR:  JSR    COUT
            INX
            LDA    MSG,X
            BNE    PRINTCHAR
            JMP $3D0 ; warmstart
MSG: AS "Hello world, Apple, from Liam!"
            .byte $8D            ; CR with high-bit set
            .byte $00
.ENDPROC