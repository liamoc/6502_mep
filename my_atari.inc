MAX_HEIGHT=12
MAX_WIDTH=20

.MACRO start_irq
    pha
    txa
    pha
    tya
    pha
.ENDMACRO
.MACRO ack_irq
    pla
    tay
    pla
    tax
    pla
    rti
.ENDMACRO
.MACRO CLEAR_CURSOR
.ENDMACRO
.macro GETIN
:   lda CONSOL
    cmp #%00000110
    beq @start
    lda KEYDEL
    cmp #3
    bne :-
    lda CH
    pha
    lda #$FF
    sta CH 
    pla
    jmp @done
@start: lda #$FD
@done:  nop        
.endmacro

PTR  = $FD

KEY_UP3 = 142
KEY_DOWN3 = 143
KEY_RIGHT3 = 135
KEY_RIGHT2 = 58 ; d
KEY_LEFT3 = 134
KEY_LEFT2 = 63 ; a
KEY_UP2 = 46 ; w
KEY_DOWN2 = 62 ; s
KEY_FLAG = 12
KEY_DIG = $21

KEY_FW = 135
KEY_HW = 134
KEY_HARDER = 142
KEY_EASIER = 143
KEY_START = $FD
.MACRO show_cursor 
    nop
.ENDMACRO
.export DL, DL_END, BoardScreen, HudScreen, PlayerMissiles, HighResChars, LowResChars,board,clock,board,intersections
.MACRO hide_cursor
    jsr clear_cursor
.ENDMACRO
.MACRO SILENCE
.ENDMACRO
.PROC won_screen
    jsr conclude_music
    jsr stop_clock
    jsr print_pressstart
    jsr print_youwin
@waitforkey:
    GETIN
    cmp #$FD
    beq :+
    jmp @waitforkey
:   jmp main_menu
.ENDPROC
.PROC lost_screen
    jsr conclude_music
    jsr stop_clock
    jsr reveal_mines
    jsr print_board
    jsr print_intersections
    jsr print_horizontals
    jsr print_verticals
    jsr print_pressstart
    jsr print_youlose
@waitforkey:
    GETIN
    cmp #$FD
    beq :+
    jmp @waitforkey
:   jmp main_menu
.ENDPROC

.PROC print_youlose
    lda #<(HudScreen+95)
    sta PTR
    lda #>(HudScreen+95)
    sta PTR+1
    ldy #0    
@loop:
    lda @data,y
    cmp #0
    beq @ret
    sta (PTR),y
    iny
    jmp @loop
@ret:    
    rts
@data:
  .byte 25, 15, 21, $20, 12,15,19,5,33,0
.ENDPROC
.PROC print_youwin
    lda #<(HudScreen+96)
    sta PTR
    lda #>(HudScreen+96)
    sta PTR+1
    ldy #0    
@loop:
    lda @data,y
    cmp #0
    beq @ret
    sta (PTR),y
    iny
    jmp @loop
@ret:    
    rts
@data:
  .byte 25, 15, 21, $20, 23,9,14,0
.ENDPROC
.PROC setup_timer
    rts
.ENDPROC
.PROC fade_sfx
    lda fade_clock
    cmp #0
    beq :+    
    dec fade_clock
:   sta AUDC1
    lda bass_clock
    cmp #0
    beq :+    
    dec bass_clock
:   ora #%11000000
    sta AUDC2
    lda treble_clock
    cmp #0
    beq :+    
    dec treble_clock
:   ora #%11100000 
    sta AUDC3
    rts
fade_clock:
    .byte 0
bass_clock:
    .byte 0
treble_clock:
    .byte 0
.ENDPROC
.PROC bass_note
lda @bass_table,x
sta AUDF2
lda #%00001111
sta fade_sfx::bass_clock
rts
@bass_table:
  .byte 63; 243 ; C
  .byte 57; 217 ; D
  .byte 55; 204 ; Eflat
.ENDPROC
.PROC treble_note
lda @treble_table,x
sta AUDF3
lda #$10
sta fade_sfx::treble_clock
rts
@treble_table:
  .byte 121 ; C
  .byte 108 ; D
  .byte 102 ; Eflat
  .byte 84 ; fsharp
  .byte 81 ; G
  .byte 72 ; A
.ENDPROC
.PROC dig_sound
    lda #$10
    sta AUDF1
    lda #%00001111
    sta fade_sfx::fade_clock
    rts
.ENDPROC
.PROC clear_cursor 
    ldx #$FF
    lda #0
:   dex 
    sta PlayerMissiles,x
    cpx #0
    beq :+
    jmp :-
:   rts
.ENDPROC

CURSOR_LEFT_MARGIN = (5*8)+6
CURSOR_TOP_MARGIN = (1*16)+12
.PROC position_cursor_sprite
    jsr clear_cursor
    lda cursor_col
    asl
    asl
    asl
    clc
    adc #CURSOR_LEFT_MARGIN
    sta HPOSP0
    lda cursor_row
    asl
    asl
    asl
    asl
    clc
    adc #CURSOR_TOP_MARGIN
    tax
    lda #$FF
    sta PlayerMissiles,x
    inx
    sta PlayerMissiles,x
    inx
    ldy #0
:   cpy #12
    beq :+
    lda #%10000001
    sta PlayerMissiles,x
    iny
    inx
    jmp :-
:   lda #$FF
    sta PlayerMissiles,x
    inx
    sta PlayerMissiles,x
    inx
    rts
.ENDPROC 

.PROC setup_sound
    lda #0
    sta AUDCTL
    rts
.ENDPROC

.PROC draw_halfwidth_backdrop
    lda #<(BoardScreen+19)
    sta PTR
    lda #>(BoardScreen+19)
    sta PTR+1
    ldx #0
@rowloop:
    cpx #23
    beq @done
    ldy #0
@colloop:
        cpy #20
        beq @nextrow
        lda #0
        sta (PTR),y
        iny
        jmp @colloop
@nextrow:
    lda PTR
    clc
    adc #40
    bcc :+
        inc PTR+1
:   sta PTR
    inx
    jmp @rowloop
@done: 
   rts
.ENDPROC

.PROC hide_byline
    lda #<(HudScreen + 40)
    sta PTR
    lda #>(HudScreen + 40)
    sta PTR+1
    ldy #0
@loop:
    cpy #80
    beq @ret
    lda #$20;
    sta (PTR),y
    iny
    jmp @loop
@ret:    
    rts
.ENDPROC

.PROC dli
    start_irq
    
    lda #>HighResChars
    sta WSYNC
    sta CHBASE
    lda #$F0
    sta COLPF2
    lda #$FF
    sta COLPF1
    lda #<dli2
    sta VDSLST
    lda #>dli2
    sta VDSLST+1
   
    jmp clock
.ENDPROC
.PROC dli2 
    pha
    lda #$FD
    sta COLPF1
    lda #<dli3
    sta VDSLST
    lda #>dli3
    sta VDSLST+1
    pla
    rti
.ENDPROC 
.PROC dli3
    pha
    lda #$F9
    sta COLPF1
    lda #<dli
    sta VDSLST
    lda #>dli
    sta VDSLST+1
    pla
    rti
.ENDPROC

.PROC init
    lda #10
    sta KRPDEL
    lda #5
    sta KEYREP
    lda #>LowResChars
    sta CHBAS
    lda #7
    sta 710
    lda #<dli
    sta VDSLST
    lda #>dli
    sta VDSLST+1
    ; activate display list interrupt and vertical blank interrupt
    lda #(%11000000)
    sta NMIEN
    lda #<DL
    sta SDLSTL
    lda #>DL
    sta SDLSTH
    lda #$FA
    sta COLOR0
    lda #$F8
    sta COLOR1 
    lda #$fC
    sta COLOR2
    lda #$0f
    sta COLOR3 
    lda #$F0
    sta COLOR4 
    lda #%00111010
    sta $022f           ;DMA control (SDMCTL)
    lda #>PlayerMissiles
    sta PMBASE    ;Store player sprite base
    lda #%00000010
    sta GRACTL; GRACTL       
    lda #%00000000 ;width 0-3
    sta SIZEP0 ; player sprite 0
    lda #$40
    sta HPOSP0 ; p0 xpos
    lda #$6E
    sta 704
    lda #2
    sta GPRIOR
.ENDPROC


.PROC print_board
    ; PTR stores the current row in video ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    lda #<BoardScreen
    sta PTR
    lda #>BoardScreen
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
                lda #(14 + 128 + 64)
                sta TEMP
                jmp @draw
:           ; cell is covered            
            lda #(12 + 64)
            sta TEMP
        jmp @draw
@check_mine:
        lda board, X
        and #MINE_PLACED
        cmp #MINE_PLACED
        bne :+
            ; mine is here            
            lda #(15 + 128 + 64) ; '*' char
            sta TEMP     
            jmp @draw
:       lda board,X 
        and #NEIGHBOURS
        cmp #0 
        bne :+
            ; no neighbouring mines            
            lda #64; ' ' char
            sta TEMP          
            jmp @draw
        ; number for neighbouring mines
:       clc 
        adc #(128 +64) 
        sta TEMP
@draw:  tya 
        pha
        asl 
        tay        
        lda TEMP
        sta (PTR),Y
        pla
        tay
        iny    
        cpy width 
        bne @continue
            ; we are at the end of the row, move the pointers.
            ldy #0
            lda PTR
            clc
            adc #80
            sta PTR
            bcc @continue
                inc PTR+1
@continue:
        inx
        cpx size
        beq :+
        jmp @loop
:   rts
.ENDPROC

.PROC print_intersections
    ; PTR stores the current row in video ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    lda #<(BoardScreen + 41)
    sta PTR
    lda #>(BoardScreen + 41)
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
        tya 
        pha
        asl 
        tay        
        lda TEMP
        sta (PTR),Y
        pla
        tay
        iny    
        cpy width_m1 ; M1 (-1) because we don't print the right edge
        bne :+  
            ; we are at the end of the row
            inx
            ldy #0
            lda PTR
            clc
            adc #80
            sta PTR
            bcc :+
                inc PTR+1
:       inx
        cpx size_m1
        bcs :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl ;tr ;tw ;bl ;lc ;lr ;BR ;br ;rl :rc :BL ;bw ;TR ;TL ;CL
.byte 12+64,    20+64, 21+64, 11+64, 22+64,  11+64, 25+64, 16+64, 23+64, 24+64, 13+64, 17+64,13+64, 18+64, 19+64, 0+64
.ENDPROC


.PROC print_title
    lda #<(BoardScreen + 41)
    sta PTR
    lda #>(BoardScreen + 41)
    sta PTR+1
    ldy #0
@loop:
    lda @title_data,y    
    cmp #$FF 
    beq @done
    cmp #$FD
    bcc @noskips
        inc PTR
        inc PTR
        inc PTR
        inc PTR
        inc PTR
        cmp #$FE
        beq :+
        inc PTR
:       iny
        jmp @loop  
@noskips: 
    clc
    adc #128
    adc #64
    sta (PTR),y
    iny
    jmp @loop
@done:
    rts    
@title_data:
.byte $FE,59,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,61,$FD
.byte $FE,37,26,27,28,29,30,31,32,33,26,27,34,35,30,36,32,0 ,30,36,30,31,32,40,41,38,39,9,$FD
.byte $FE,37,42,43,42,44,45,46,42,43,42,43,49,50,45,51,52,53,45,51,45,54,55,57,58,56,0 ,9,$FD
.byte $FE,62,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,63
.byte $FF
.ENDPROC

.PROC print_menu
    jsr print_static_hud
    jsr print_board_size
    jsr print_title
    jsr print_byline
    jsr print_pressstart
    rts
.ENDPROC

.PROC print_pressstart
    lda #<(HudScreen+13)
    sta PTR
    lda #>(HudScreen+13)
    sta PTR+1
    ldy #0
@loop:
    lda @data,y
    cmp #0
    beq @ret
    sta (PTR),y
    iny
    jmp @loop
@ret:    
    rts
@data:
  .byte 63, 16, 18, 5, 19,19, $20, 19, 20, 1, 18, 20, 62,0
.ENDPROC

.PROC print_byline
    lda #<(HudScreen + 80)
    sta PTR
    lda #>(HudScreen + 80)
    sta PTR+1
    ldy #0
@loop:
    cpy #40
    beq @ret
    lda @data,y
    sta (PTR),y
    iny
    jmp @loop
@ret:    
    rts
@data:
  .byte $20, 47, 58, $20, 2, 15, 1, 18, 4, $20, 19, 9, 26,5
  .byte $20, $20,$20,$20,$20,$20,$20,46, 58, $20, 4, 9, 6, 6, 9, 3, 21, 12, 20, 25
  .byte $20,$20,$20,$20,$20,$20,$20
.ENDPROC

.PROC print_board_size
    lda width
    cmp #20
    bne @hw
        lda #0
        sta left_color
        lda #128
        sta right_color
        jmp @onwards
@hw:
    lda #128
    sta left_color
    lda #0
    sta right_color
@onwards:
    lda #<(HudScreen+40)
    sta PTR
    lda #>(HudScreen+40)
    sta PTR+1
    lda left_color
    sta TEMP
    ldy #0
:       lda @string, y
        cmp #0 
        beq :+++
        cmp #$FF
        bne :+
        lda right_color
        sta TEMP
        lda #$20
:       cmp #$20
        beq :+ 
        clc
        adc TEMP 
:       sta (PTR), y
        iny
        jmp :---
:   rts 
@string:      .byte $20, $20, 8, 1, 12, 6, $20, $FF, $20, 6, 21, 12, 12, $20, $20, $20, 0
left_color:  .byte 0
right_color: .byte 128
.ENDPROC

.PROC print_static_hud
    ldx #39
    ldy #0
    lda #<(HudScreen)
    sta PTR
    lda #>(HudScreen)
    sta PTR+1
@loop:
    cpx #0
    beq @done
    lda #61
    sta (PTR),Y
    dex
    iny
    jmp @loop
@done:     
    lda #$20
    sta (PTR),Y
    lda #58
    sta HudScreen+42
    jmp print_hud
.ENDPROC

.PROC print_hud
    ldx flags
    ldy #22
    lda #<(HudScreen+56)
    sta PTR
    lda #>(HudScreen+56)
    sta PTR+1
@loop: 
    cpx #0 
    beq @done
    dex
    lda #59
    cpx #0    
    beq :+
      dex
      lda #60
:   sta (PTR),Y
    dey
    jmp @loop
@done:
    lda #$20
    sta (PTR),Y 
    ldy #23
    sta (PTR),Y 
    rts
.ENDPROC

DL:
.byte DL_BLK8
.byte DL_BLK8
.byte DL_BLK8
.byte DL_CHR40x8x4 | DL_LMS
.word BoardScreen
.repeat 21
.byte DL_CHR40x8x4 
.endrepeat
.byte DL_CHR40x8x4 | DL_DLI 
.byte DL_CHR40x8x1 | DL_LMS | DL_DLI
.word HudScreen
.byte DL_CHR40x8x1 | DL_DLI
.byte DL_CHR40x8x1
.byte DL_JVB
.word DL
DL_END:
.segment "DISPLAYLIST"
.align 1024
BoardScreen:
.res (40*24),0
.align 1024
HudScreen:
.res (40*5),0
.align 1024
HighResChars:
.incbin "chars_hires.bin", 0,512
LowResChars:
.incbin "chars_4c.bin",0,512
.align 1024 
PlayerMissiles:
.res 512,0