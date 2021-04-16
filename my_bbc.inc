MAX_WIDTH=20
MAX_HEIGHT=12
.macro hide_cursor
.endmacro
.macro show_cursor
.endmacro 
.macro SILENCE
.endmacro
; Y = col 
; TMP = row
; A = line to draw
; uses X, A
.proc draw_whole_cursor
    tya
    pha
    ldy #0
:   cpy #8
    beq :+
    lda #$FF
    eor (PTR),Y
    sta (PTR),Y
    iny
    jmp :-
:   pla
    tay
    rts
.ENDPROC
; TEMP = symbol to draw(multiple of 8)
.PROC draw_symbol
    txa
    pha
    tya
    pha
    ldx TEMP
    ldy #0
:   cpy #8
    beq :+
    lda symbols,X
    sta (PTR),Y
    iny
    inx
    jmp :-
:   pla
    tay
    pla
    tax
    rts 
symbols:
.byte %01010000
.byte %10100000
.byte %01010000
.byte %10100000
.byte %01010000
.byte %10100000
.byte %01010000
.byte %10100000

.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000

.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000

.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000

.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %11110000
.byte %11110000

.byte %00000000
.byte %11110000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000

.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %11110000
.byte %11110000

.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01110000
.byte %11110000

.byte %00010000
.byte %11110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000
.byte %00110000

.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00010000
.byte %00110000

.byte %00000000
.byte %01110000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000
.byte %01000000

.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %10000000
.byte %01000000

.byte %01000000
.byte %10000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000

.byte %00110000
.byte %00010000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000

.byte %01000000
.byte %10000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00010000
.byte %00110000

.byte %00110000
.byte %00010000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %00000000
.byte %10000000
.byte %01000000


.byte %00000000
.byte %01100110
.byte %11011101
.byte %10011001
.byte %10011001
.byte %10111011
.byte %01100110
.byte %00000000

.byte %00000000
.byte %01100110
.byte %11101110
.byte %01100110
.byte %01100110
.byte %01100110
.byte %11111111
.byte %00000000

.byte %00000000
.byte %01100110
.byte %11011101
.byte %00110011
.byte %01100110
.byte %11001100
.byte %11111111
.byte %00000000

.byte %00000000
.byte %11101110
.byte %00110011
.byte %11101110
.byte %00110011
.byte %00110011
.byte %11101110
.byte %00000000

.byte %00000000
.byte %10111011
.byte %10111011
.byte %11111111
.byte %00110011
.byte %00110011
.byte %00110011
.byte %00000000

.byte %00000000
.byte %11111111
.byte %11001100
.byte %11101110
.byte %00110011
.byte %00110011
.byte %11101110
.byte %00000000

.byte %00000000
.byte %01110111
.byte %11001100
.byte %11101110
.byte %11011101
.byte %10111011
.byte %01100110
.byte %00000000

.byte %00000000
.byte %11111111
.byte %00110011
.byte %00110011
.byte %01100110
.byte %01100110
.byte %11001100
.byte %11001100

.byte %00000000
.byte %01101110
.byte %10111011
.byte %11101110
.byte %10111011
.byte %10111011
.byte %01101110
.byte %00000000

.byte %00000000
.byte %01100110
.byte %10111011
.byte %10101010
.byte %01110111
.byte %00110011
.byte %11101110
.byte %00000000

.byte %11110000
.byte %10010110
.byte %10010110
.byte %10110100
.byte %10110100
.byte %10100100
.byte %10110100
.byte %11100000
.byte %00000000

.byte %00000110
.byte %00001111
.byte %00001111
.byte %01101111
.byte %00001111
.byte %00001111
.byte %00000110
.ENDPROC
.proc draw_cursor
    rts
.endproc

.PROC init
OSWRCH = $FFEE
OSBYTE = $FFF4
    LDA #22
    JSR OSWRCH
    LDA #$5
    JSR OSWRCH
    LDA #23
    JSR OSWRCH
    LDA #1
    JSR OSWRCH
    LDA #0
    JSR OSWRCH
   

SendULA:
    ldx #0
NextULAreg:
    lda ULAConfig,X
    sta $FE21                ;Load in color config
    inx
    txa
    cmp #16
    bne NextULAreg    
    lda #$FF;
    sta $5800; 
    rts
;scr=$3000 when 256x192
;Set screen height to 25, and screen offset so we're using &3
    
CRTCConfig:
    .byte $7F        ;0 - Horizontal total
;    ifdef ScrWid256 
;        db $40        ;1 - Horizontal displayed characters;
;        db $62        ;2 - Horizontal sync position
;        db $28        ;3 - Horizontal sync width/Vertical sync time
;    else
        .byte $50        ;1 - Horizontal displayed characters
        .byte $62        ;2 - Horizontal sync position
        .byte $28        ;3 - Horizontal sync width/Vertical sync time
;    endif
    .byte $26            ;4 - Vertical total
    .byte $00            ;5 - Vertical total adjust
    .byte $20            ;6 - Vertical displayed characters (25)
    .byte $22            ;7 - Vertical sync position
    .byte $01            ;8 - Interlace/Display delay/Cursor delay
    .byte $07            ;9 - Scan lines per character
    .byte $67    ;10 - Cursor start line and blink type
    .byte $08            ;11 - Cursor end line
;    ifdef ScrWid256 
;        db $0A        ;12 - Screen start address H (Address /8)
;        db $00        ;13 - Screen start address L 
;    else
        .byte $08        ;12 - Screen start address H (Address /8)
        .byte $30        ;13 - Screen start address L ($4130/8=$0830)
;    endif
    
ULAConfig:    
Palette0:    ;Colour&07 &17 &27 &37 &47 &57 &67 &77
;        SC  SC        -    S=Screen C=Color
    .byte $07,$17    ;0
    .byte $26,$36    ;0
Palette1:
    .byte $47,$57        ;1
    .byte $66,$76        ;1
Palette2:
    .byte $84,$94            ;2
    .byte $A0,$B0            ;2
Palette3:
    .byte $C4,$D4                ;3
    .byte $E0,$F0                ;3
    
;EOR True   Color
;7  (0)     black
;6  (1)     red
;5  (2)     green
;4  (3)     yellow (green—red)
;3  (4)     blue
;2  (5)     magenta (red—blue)
;1  (6)     cyan (green—blue)
;0  (7)     white
.ENDPROC

.PROC setup_timer
    rts
.ENDPROC
.PROC won_screen
    lda #0
    sta CURSOR_ENABLED
    jsr stop_clock
    jsr reveal_mines
    jsr print_board
    jsr print_intersections
    jsr print_horizontals
    jsr print_verticals
@waitforkey:
    GETIN
    cmp #$0D
    beq :+
    jmp @waitforkey
:   jmp main_menu 
.ENDPROC
.PROC lost_screen
    lda #0
    sta CURSOR_ENABLED
    jsr stop_clock
    jsr reveal_mines
    jsr print_board
    jsr print_intersections
    jsr print_horizontals
    jsr print_verticals
@waitforkey:
    GETIN
    cmp #$0D
    beq :+
    jmp @waitforkey
:   jmp main_menu 
.endproc
.proc setup_sound
rts
.endproc
.proc dig_sound
  rts
.endproc
.proc clear_cursor
    lda CURSOR_ON
    cmp #0
    bne :+
    rts
:   jmp flash_cursor
.endproc
.proc position_cursor_sprite
    lda #$0
    sta CURSOR_ON
    lda #1
    sta CURSOR_ENABLED
    jmp flash_cursor
.endproc
.proc flash_cursor
    lda CURSOR_ENABLED
    cmp #0
    beq @end
    lda CURSOR_ON
    eor #$FF
    sta CURSOR_ON
    lda #0
    sta PTR
    lda #$58
    sta PTR+1
    ldx #0
    lda PTR
@loop:
    cpx cursor_col
    beq @loop_end
    clc
    adc #16
    bcc :+
    inc PTR+1
:   inx
    jmp @loop
@loop_end:
    ldx #0    
@loop2:
    cpx cursor_row
    beq @loop2_end    
    inc PTR+1
    inc PTR+1
    clc
    adc #$80
    bcc :+
    inc PTR+1
:   inx
    jmp @loop2
@loop2_end:
    sta PTR
    jmp draw_whole_cursor    
@end:   rts
.endproc
.proc draw_halfwidth_backdrop
   rts
rts
.endproc
.proc hide_byline
    rts
.endproc
.proc print_static_hud
    rts
.endproc
.proc print_board
    lda #0
    ; PTR stores the current row in video ram
    ; PTR2 stores the current row in color ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    sta PTR
    lda #$58
    sta PTR+1
    ldy #0
    ldx #0
@loop:  lda board,x
        and #UNCOVERED
        cmp #UNCOVERED
        beq @check_mine
            ;it's covered, is there a flag?
            lda board, x
            and #FLAG_PLACED
            cmp #FLAG_PLACED
            bne :+
                ; cell is flag
                lda #208
                sta TEMP
                jmp @draw
:           ; cell is covered
            lda #0
            sta TEMP
        jmp @draw
@check_mine:
        lda board, X
        and #MINE_PLACED
        cmp #MINE_PLACED
        bne :+
            ; mine is here
            lda #216 ; '*' char
            sta TEMP     
            jmp @draw
:       lda board,X 
        and #NEIGHBOURS
        cmp #0 
        bne :+
            ; no neighbouring mines
            lda #$8; ' ' char
            sta TEMP          
            jmp @draw
        ; number for neighbouring mines
:          
        asl
        asl
        asl
            clc
            adc #128
            sta TEMP
@draw:  jsr draw_symbol; foo 
        lda PTR
        clc
        adc #16
        sta PTR
        bcc @continue
        inc PTR+1
@continue:
        inx
        iny
        cpy #20
        bne :+
            ldy #0
            inc PTR+1
            lda PTR
            clc
            adc #($40)
            sta PTR
            bcc :+
            inc PTR+1
:       cpx size
        beq :+
        jmp @loop
:   rts
.endproc
.proc print_board_size
    rts 
.endproc
.PROC print_verticals
    lda #$40
    sta PTR
    lda #$59
    sta PTR+1
    ldy #0
    ldx #0
@loop:  tya 
        pha 
        lda intersections,X
        tay 
        lda @char_table,y
        sta TEMP
        pla
        tay
@draw:  jsr draw_symbol; foo 
        lda PTR
        clc
        adc #16
        sta PTR
        bcc @continue
        inc PTR+1
@continue:
        inx
        iny
        cpy #20
        bne :+
            ldy #0
            inc PTR+1
            lda PTR
            clc
            adc #($40)
            sta PTR
            bcc :+
            inc PTR+1
:       cpx size
        beq :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl    ;tr;tw ;bl ;lc     ;lr ;BR ;br ;rl     :rc :BL ;bw ;TR ;TL ;CL
.byte 0,32,0,32,40,8,40,8,0,32,0,32,40,8,40,8
.ENDPROC
.PROC print_horizontals
    lda #8
    sta PTR
    lda #$58
    sta PTR+1
    ldy #0
    ldx #0
@loop:  tya 
        pha 
        lda intersections,X
        tay 
        lda @char_table,y
        sta TEMP
        pla
        tay
@draw:  jsr draw_symbol; foo 
        lda PTR
        clc
        adc #16
        sta PTR
        bcc @continue
        inc PTR+1
@continue:
        inx
        iny
        cpy #20
        bne :+
            ldy #0
            inc PTR+1
            lda PTR
            clc
            adc #($40)
            sta PTR
            bcc :+
            inc PTR+1
:       cpx size
        beq :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl    ;tr;tw ;bl ;lc     ;lr ;BR ;br ;rl     :rc :BL ;bw ;TR ;TL ;CL
.byte 0,16,24,8,0,16,24,8,0,16,24,8,0,16,24,8
.ENDPROC
.PROC print_intersections
    lda #$48
    sta PTR
    lda #$59
    sta PTR+1
    ldy #0
    ldx #0
@loop:  tya 
        pha 
        lda intersections,X
        tay 
        lda @char_table,y
        sta TEMP
        pla
        tay
@draw:  jsr draw_symbol; foo 
        lda PTR
        clc
        adc #16
        sta PTR
        bcc @continue
        inc PTR+1
@continue:
        inx
        iny
        cpy #20
        bne :+
            ldy #0
            inc PTR+1
            lda PTR
            clc
            adc #($40)
            sta PTR
            bcc :+
            inc PTR+1
:       cpx size
        beq :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl    ;tr;tw ;bl ;lc     ;lr ;BR ;br ;rl     :rc :BL ;bw ;TR ;TL ;CL
.byte 0,48,56,32,64,16,112,72,80,120,24,88,40,104,96,8
.ENDPROC
.PROC print_menu
    jsr print_title
    jsr print_static_hud
    jsr print_hud
    jsr print_board_size
    rts
.ENDPROC
.PROC print_title
rts    
.endproc
.PROC print_hud
    rts
.ENDPROC
.proc printy 
    
PrintChar = $FFE3    ;OSASCI - Print Ascii Character to scrn
NewLine   = $FFE7    ;OSNEWL - New Line

    ;Load in the address of the Message into the zero page
    lda #>HelloWorld
    sta $21                ;H Byte
    lda #<HelloWorld
    sta $20                ;L Byte
    
    jsr PrintStr        ;Show to the screen
    
    jsr NewLine            ;Start a new line

    rts                    ;Return to basic


PrintStr:
    ldy #0                ;Set Y to zero
PrintStr_again:
    lda ($20),y            ;Load a character from addr in $20+Y 
    
    cmp #255            ;If we got 255, we're done
    beq PrintStr_Done
    
    jsr PrintChar        ;Print Character
    iny                    ;Inc Y and repeat
    jmp PrintStr_again
PrintStr_Done:
    rts    

HelloWorld:                ;255 terminated string
    .byte "Hello World",255
.endproc