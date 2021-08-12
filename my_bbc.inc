MAX_WIDTH=20
MAX_HEIGHT=12
RNDL = $0240
RNDH = $0240
PTR2 = $FB
PTR  = $FD
TEMP2 = $4A
CURSOR_ON = $4B
CURSOR_ENABLED = $4C

KEY_RIGHT = 137
KEY_RIGHT2 = 68 ; d
KEY_LEFT = 136
KEY_LEFT2 = 65 ; a
KEY_UP = 139
KEY_UP2 = 87 ; w
KEY_DOWN = 138
KEY_DOWN2 = 83 ; s
KEY_FLAG = $0D
KEY_DIG = $20

KEY_START = $0D
KEY_FW = 137
KEY_HW = 136
KEY_HARDER = 139
KEY_EASIER = 138
.MACRO start_irq
nop
.ENDMACRO
.MACRO ack_irq
jmp restore_irq
.ENDMACRO
.MACRO CLEAR_CURSOR
jsr clear_cursor
.ENDMACRO
.MACRO GETIN
JSR $FFE0
.ENDMACRO 
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
    lda #%00001111
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
.include "bbc_symbols.inc"
.ENDPROC

.PROC draw_symbol2
    txa
    pha
    tya
    pha
    ldx TEMP
    ldy #0
:   cpy #8
    beq :+
    lda symbols2,X
    and TEMP2
    sta (PTR),Y
    iny
    inx
    jmp :-
:   pla
    tay
    pla
    tax
    rts 
symbols2:
.include "bbc_symbols2.inc"
.ENDPROC

.PROC draw_symbol2_shifted
    txa
    pha
    tya
    pha
    ldx TEMP
    ldy #0
:   cpy #8
    beq :+
    lda draw_symbol2::symbols2,X
    and #$0F
    lsr
    ora (PTR),Y
    sta (PTR),Y
    iny
    inx
    jmp :-
:   pla
    tay
    pla
    tax
    rts 
.ENDPROC
.macro draw_string_shadow len, addr
    
    lda #$FF
    sta TEMP2
    ldx #0
:   cpx #len 
    beq :++
    lda addr,X
    asl
    asl
    asl
    sta TEMP
    jsr draw_symbol2
    jsr draw_symbol2_shifted
    lda PTR
    clc
    adc #8
    sta PTR
    bcc :+
        inc PTR+1
:   inx 
    jmp :--
    : 
.endmacro
.macro draw_string len, addr, mask
    lda mask
    sta TEMP2
    ldx #0
:   cpx #len 
    beq :+++
    lda addr,X
    cmp #0
    beq :+
    asl
    asl
    asl
    sta TEMP
    jsr draw_symbol2
:   lda PTR
    clc
    adc #8
    sta PTR
    bcc :+
        inc PTR+1
:   inx 
    jmp :---
    : 
.endmacro

.PROC init
OSWRCH = $FFEE
OSBYTE = $FFF4
sei
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
    lda #65 
    jsr $FFE3
    jsr $FFE3
    jsr $FFE3
    jsr $FFE3
    jsr $FFE3   
    jsr $FFE7
cli   

SendULA:
    ldx #0
NextULAreg:
    lda ULAConfig,X
    sta $FE21                ;Load in color config
    inx
    txa
    cmp #16
    bne NextULAreg    
    
    lda #4
    ldx #1
    jsr OSBYTE 
    rts
    
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

.PROC clock_irq
    pha
    txa
    pha
    tya
    pha
    lda TEMP
    pha
    lda PTR
    pha
    lda PTR+1
    pha
    jmp clock
.ENDPROC 

.PROC restore_irq
    pla
    sta PTR+1
    pla
    sta PTR
    pla
    sta TEMP
    pla
    tay
    pla
    tax
    pla
    rts
.ENDPROC
.PROC setup_timer
OSBYTE = $FFF4
    lda #<clock_irq
    sta $220
    lda #>clock_irq
    sta $221
    lda #14
    ldx #4
    jsr OSBYTE
    rts
.ENDPROC
.PROC won_screen
    lda #0
    sta CURSOR_ENABLED
    jsr stop_clock
    jsr reveal_mines
    jsr print_board
    jsr conclude_music
    jsr print_intersections
    jsr print_horizontals
    jsr print_verticals
    lda #($E0 - (5*8))
    sta PTR
    lda #$77
    sta PTR+1
    draw_string 8, str, #$F0
    jmp press_return
str: .byte 25, 15, 21, 28, 23, 9, 14, 27
.ENDPROC
.PROC lost_screen
    lda #0
    sta CURSOR_ENABLED
    jsr stop_clock
    jsr reveal_mines
    jsr conclude_music
    jsr print_board
    jsr print_intersections
    jsr print_horizontals
    jsr print_verticals
    lda #($E0 - (5*8))
    sta PTR
    lda #$77
    sta PTR+1
    draw_string 8, str, #$0F
    jmp press_return
str: .byte 25, 15, 21, 28, 12, 15, 19, 5
.endproc
.proc setup_sound
;ENVELOPE 1,1,1,-1,0,1,1,200,126,-2,-1,-20,126,60
ldx #<envelope_data
ldy #>envelope_data
lda #8
jsr $FFF1 ; OSWORD
ldx #<envelope_data2
ldy #>envelope_data2
lda #8
jsr $FFF1 ; OSWORD
rts
envelope_data:
.byte 1,1,1,$FF,0,1,1,200,126,$FE,$FF,($FF-19),126,60
envelope_data2:
.byte 2,1,0,0,0,0,0,0,50,0,0,($FF-49),126,126
.endproc
.proc dig_sound
ldx #<sound_data
ldy #>sound_data
lda #7
jsr $FFF1 ; OSWORD
rts
sound_data:
  .word $0000, $FFF1, 101, $6
rts
.endproc
.proc bass_note
tya 
pha
txa
asl
asl
asl
clc
adc #<sound_data
tax
ldy #>sound_data
lda #7
jsr $FFF1 ; OSWORD
pla
tay
rts
sound_data:
  .word $0001, $0002, 5, $6
  .word $0001, $0002, 13, $6
  .word $0001, $0002, 17, $6
.endproc
.proc treble_note  
tya 
pha
txa
asl
asl
asl
clc
adc #<sound_data
tax
ldy #>sound_data
lda #7
jsr $FFF1 ; OSWORD
pla
tay
rts
sound_data:
  .word $0002, $0001, 101, $0003
  .word $0002, $0001, 109, $0003
  .word $0002, $0001, 113, 3
  .word $0002, $0001, 125, 3
  .word $0002, $0001, 129, 3
  .word $0002, $0001, 137, 3
  .byte $13, $6F ; D
  .byte $14, $96 ; Eflat
  .byte $18, $7C ; fsharp
  .byte $19, $F0 ; G
  .byte $1D, $1E ; A
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
    lda #$98
    sta PTR
    lda #$58
    sta PTR+1
    ldy #0
    ldx #0
    lda #(13*8)
    sta TEMP   
    lda #$0F
    sta TEMP2
@rowloop:   
    cpx #24
    beq @end
    ldy #0
    @colloop:
        cpy #20
        beq @endcolloop 
        jsr draw_symbol2
        iny
        lda PTR
        clc
        adc #8
        bcc :+
        inc PTR+1
:       sta PTR
        jmp @colloop
    @endcolloop:
    inx
    lda PTR
    clc
    adc #(20*8)
    bcc :+
    inc PTR+1
:   sta PTR
    jmp @rowloop
@end:
    rts
.endproc

.proc press_return
    lda #$E8
    sta PTR
    lda #$78
    sta PTR+1
    draw_string 12, str, #$FF
@waitforkey:
    GETIN
    cmp #$0D
    beq :+
    jmp @waitforkey
:   jmp main_menu 
str: .byte 16, 18, 5, 19, 19,0, 18, 5, 20, 21, 18, 14 
.endproc 
.proc hide_byline
    lda #$FF
    sta TEMP2
    lda #$40
    sta PTR
    lda #$77
    sta PTR+1
    lda #(28*8) ;(256-8)
    sta TEMP
    ldx #0
:   cpx #80
    beq :++
    jsr draw_symbol2
    lda PTR
    clc
    adc #8
    bcc :+
    inc PTR+1
:   sta PTR
    inx
    jmp :--
: 
    lda #$00
    sta PTR
    lda #$7B
    sta PTR+1
    draw_string 40, string_four, #$FF
    rts
string_four: .byte 18, 5, 20, 21, 18, 14, 13, 6, 12, 1, 7,28,28,28
.byte 19, 16, 1, 3, 5, 13, 4, 9, 7, 28,28,28,28
.byte 1, 18, 18, 15, 23, 19,13, 3, 21, 18, 19, 15, 18,28
.endproc
.proc print_static_hud
    lda #$FF
    sta TEMP2
    lda #$C0
    sta PTR
    lda #$74
    sta PTR+1
    lda #0 ;(256-8)
    sta TEMP
    ldx #0
:   cpx #40
    beq :++
    jsr draw_symbol2
    lda PTR
    clc
    adc #8
    bcc :+
    inc PTR+1
:   sta PTR
    inx
    jmp :--
:   rts
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
        cpy width
        bne :+++
            lda PTR
:           cpy #20
            beq :+
            iny
            clc
            adc #(8*2)
            bcc :-
            inc PTR+1
            jmp :-
:           sta PTR 
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
    lda #$F0
    sta TEMP2
    lda width
    cmp #20
    bne @hw
        lda #$0F
        sta TEMP2
@hw: 
    lda #$E8
    sta PTR
    lda #$78
    sta PTR+1
    draw_string 8, str, TEMP2
    lda TEMP2
    eor #$FF
    sta TEMP2
    draw_string 4, str2, TEMP2
    rts 
str: .byte 8, 1, 12, 6,28,28,28,28
str2: .byte 6, 21, 12, 12
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
        cpy width
        bne :+++
            lda PTR
:           cpy #20
            beq :+
            iny
            clc
            adc #(8*2)
            bcc :-
            inc PTR+1
            jmp :-
:           sta PTR 
            ldy #0
            inc PTR+1
            lda PTR
            clc
            adc #($40)
            sta PTR
            bcc :+
            inc PTR+1
:       cpx size_m1
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
        cpy width_m1
        bne :+++
            lda PTR
:           cpy #19
            beq :+
            iny
            clc
            adc #(8*2)
            bcc :-
            inc PTR+1
            jmp :-
:           sta PTR 
            inx
            ldy #0
            inc PTR+1
            lda PTR
            clc
            adc #($50)
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
        cpy width_m1
        bne :+++
            lda PTR
:           cpy #19
            beq :+
            iny
            clc
            adc #(8*2)
            bcc :-
            inc PTR+1
            jmp :-
:           sta PTR 
            inx
            ldy #0
            inc PTR+1
            lda PTR
            clc
            adc #($50)
            sta PTR
            bcc :+
            inc PTR+1
:       cpx size_m1
        beq :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl    ;tr;tw ;bl ;lc     ;lr ;BR ;br ;rl     :rc :BL ;bw ;TR ;TL ;CL
.byte 0,48,56,32,64,16,112,72,80,120,24,88,40,104,96,8
.ENDPROC
.PROC print_menu
    jsr print_static_hud
    jsr print_hud
    jsr print_board_size
    lda #($18+8)
    sta PTR
    lda #$5C
    sta PTR+1
    draw_string 95, string_two, #$FF
    lda #104
    sta PTR
    lda #$5D
    sta PTR+1
    draw_string_shadow 13, string_one
    lda #($E0 - (6*8))
    sta PTR
    lda #$77
    sta PTR+1
    draw_string 10, string_three, #$FF
    lda #$00
    sta PTR
    lda #$7B
    sta PTR+1
    draw_string 40, string_four, #$FF
    rts
string_one: .byte 1, 18, 3, 8, 1, 5, 15, 12, 15, 7, 9, 19, 20     
string_two:                       .byte 10,31,31,31,31,31,31,31,31,31,31,31,31,31,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0
            .byte 0,0,0,0,0,0,0,0,0,0,0,29,28,28,28,28,28,28,28,28,28,28,28,28,28,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0
            .byte 0,0,0,0,0,0,0,0,0,0,0,22,24,24,24,24,24,24,24,24,24,24,24,24,24,17
string_three: .byte 2, 15, 1, 18, 4,28, 19, 9, 26, 5 
string_four: .byte 18, 5, 20, 21, 18, 14,13,19, 20, 1 ,18, 20,28,28,28,28,28,28,28,28,28,28,28,1, 18, 18, 15, 23, 19, 13, 4, 9, 6, 6, 9, 3, 21, 12, 20, 25,0
.ENDPROC
.PROC print_hud
    lda #$F8
    sta PTR
    lda #$75
    sta PTR+1
    ldy flags
    ldx #23
@loop: 
    cpx #0 
    beq @done
    lda #(96+64+24+24+32+8)
    cpy #0
    beq :+
      dey
      lda #(96+64+24+24+32)
      cpy #0    
      beq :+
        dey
        lda #(96+64+24+24+16+8)
:   sta TEMP
    dec PTR+1
    lda PTR
    sec
    sbc #$40
    bcs :+
    dec PTR+1
:   sta PTR
    dex
    jsr draw_symbol
    jmp @loop
@done:
    rts
.ENDPROC