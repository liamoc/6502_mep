MAX_WIDTH=20
MAX_HEIGHT=10
.macro AS text
    .repeat .strlen(text), I
        .byte   .strat(text, I) | $80
    .endrep
.endmacro
.macro ASRAW text
    .repeat .strlen(text), I
        .byte   .strat(text, I) 
    .endrep
.endmacro
.macro hide_cursor
.endmacro
.macro show_cursor
.endmacro 
.macro SILENCE
.endmacro
.proc apple_keyin
KEYIN:   jsr flash_cursor
         jsr clock
         INC   RNDL
         BNE   KEYIN2     ;INCR RND NUMBER
         INC   RNDH
KEYIN2:   BIT   KBD        ;KEY DOWN?
         BPL   KEYIN      ;  LOOP
         LDA   KBD        ;GET KEYCODE
         BIT   KBDSTRB    ;CLR KEY STROBE
         RTS
.endproc
; Y = col 
; TMP = row
; A = line to draw
; uses X, A
.proc draw_whole_cursor
    lda #0
    ldx TMP
    jsr draw_cursor
    lda #1
    ldx TMP
    jsr draw_cursor
    lda #2
    ldx TMP
    jsr draw_cursor
    lda #3
    ldx TMP
    jsr draw_cursor
    lda #4
    ldx TMP
    jsr draw_cursor
    lda #5
    ldx TMP
    jsr draw_cursor
    lda #6
    ldx TMP
    jsr draw_cursor
    lda #7
    ldx TMP
    jsr draw_cursor
    rts   
.ENDPROC
.proc draw_whole_symbol
    lda #0
    ldx TMP
    jsr draw_symbol 
    lda #1
    ldx TMP
    jsr draw_symbol 
    lda #2
    ldx TMP
    jsr draw_symbol 
    lda #3
    ldx TMP
    jsr draw_symbol 
    lda #4
    ldx TMP
    jsr draw_symbol 
    lda #5
    ldx TMP
    jsr draw_symbol 
    lda #6
    ldx TMP
    jsr draw_symbol 
    lda #7
    ldx TMP
    jsr draw_symbol 
    rts   
.ENDPROC
; Y = col 
; X = row
; A = line to draw
; TEMP = symbol to draw(multiple of 8)
; 
; uses PTR, X
.PROC draw_symbol
@foo = symbols + $0100
    ; find the coordinate
    pha   
    txa
    asl
    tax
    lda display_rows, X
    sta PTR
    lda display_rows+1, X
    sta PTR+1
    pla 
    pha
    asl
    asl
    adc PTR+1
    sta PTR+1
    pla
    pha
    adc TEMP
    tax
    lda CHARSET
    cmp #0
    bne :+
    lda symbols, X
    jmp :++
:   lda @foo, X
:   sta (PTR),Y
    pla
rts 
display_rows: .word $2000, $2080, $2100, $2180, $2200, $2280, $2300, $2380, $2028, $20A8, $2128, $21A8, $2228, $22A8, $2328, $23A8, $2050, $20D0, $2150, $21D0, $2250 
.include "apple_symbols.inc"
.ENDPROC
.proc draw_cursor
    pha   
    txa
    asl
    tax
    lda draw_symbol::display_rows, X
    sta PTR
    lda draw_symbol::display_rows+1, X
    sta PTR+1
    pla 
    pha
    asl
    asl
    adc PTR+1
    sta PTR+1
    pla
    tax
    lda @symbol, X
    eor (PTR),Y
    sta (PTR),Y
    txa
    rts
@symbol:
.byte %01111111
.byte %01111111
.byte %01100011
.byte %01100011
.byte %01100011
.byte %01100011
.byte %01111111
.byte %01111111
.endproc


.PROC init
    lda #0
    sta CURSOR_ENABLED
    sta CHARSET
    bit $C050
    bit $C053
    bit $C057 ; switch to hires mode
    rts
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
@TEXTRAM2 = $0650   
@TEXTRAM = $07D0    
    ldy #0    
:   lda @str2,Y
    cmp #255
    beq @waitforkey
    sta @TEXTRAM2,Y
    lda @str, Y    
    sta @TEXTRAM, Y
    iny
    jmp :-
@waitforkey:
    GETIN
    cmp #$8D
    beq :+
    jmp @waitforkey
:   jmp main_menu 
@str2: AS "              YOU WIN!                  "
.byte 255
@str: AS "PRESS RETURN TO GO BACK TO THE MAIN MENU!"
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
@TEXTRAM2 = $0650   
@TEXTRAM = $07D0    
    ldy #0    
:   lda @str2,Y
    cmp #255
    beq @waitforkey
    sta @TEXTRAM2,Y
    lda @str, Y    
    sta @TEXTRAM, Y
    iny
    jmp :-
@waitforkey:
    GETIN
    cmp #$8D
    beq :+
    jmp @waitforkey
:   jmp main_menu 
@str2: AS "              YOU LOSE                  "
.byte 255
@str: AS "PRESS RETURN TO GO BACK TO THE MAIN MENU!"
.endproc
.proc setup_sound
rts
.endproc
.proc dig_sound
  ldy #$5F
: cpy #0
  beq :++
  bit $C030
  LDX #$FF
: DEX
  BNE :-
  dey 
  jmp :-- 
: rts
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
    beq :+
    lda CURSOR_ON
    eor #$FF
    sta CURSOR_ON
    lda cursor_col
    asl
    tay
    lda cursor_row
    asl
    tax
    stx TMP
    jmp draw_whole_cursor
:   rts
.endproc
.proc draw_halfwidth_backdrop
    lda #24
    sta TEMP 
    ldx #0
@rowloop:
    cpx #19
    beq @done
    stx TMP
    ldy #18
@colloop:
        cpy #38
        beq @nextrow        
        iny
        jsr draw_whole_symbol
        jmp @colloop
@nextrow:
    ldx TMP
    inx
    jmp @rowloop
@done: 
   rts
rts
.endproc
.proc hide_byline
@TEXTRAM2 = $0650   
@TEXTRAM3 = $06D0    
@TEXTRAM4 = $0750    
@TEXTRAM = $07D0    
    ldy #0    
:   lda #$A0
    cpy #40
    beq :+
    sta @TEXTRAM2,Y
    sta @TEXTRAM, Y
    sta @TEXTRAM3, Y
    sta @TEXTRAM4, Y
    iny
    jmp :-
:   rts
.endproc
.proc print_static_hud
    ldy #39
    
    lda #(28*8)
    sta TEMP
    lda #19
    sta TMP
:   cpy #0
    beq :+
    dey
    jsr draw_whole_symbol
    jmp :-
:   lda CURSOR_ENABLED
    cmp #0
    beq :+ 
    lda #$BA ;colon 
    sta $06D2
:   rts
.endproc
.proc print_board
    ldy #0
    ldx #0
    stx TMP
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
                lda #8
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
            lda #16 ; '*' char
            sta TEMP     
            jmp @draw
:       lda board,X 
        and #NEIGHBOURS
        cmp #0 
        bne :+
            ; no neighbouring mines            
            lda #24; ' ' char
            sta TEMP          
            jmp @draw
        ; number for neighbouring mines
:       asl
        asl
        asl
        clc 
        adc #(24) 
        sta TEMP
@draw:  tya 
        pha
        txa
        pha
        tya
        asl 
        tay
        jsr draw_whole_symbol 
        pla
        tax
        pla
        tay
        iny    
        cpy width 
        bne @continue
            ; we are at the end of the row, move the counters
            ldy TMP
            iny
            iny
            sty TMP
            ldy #0
@continue:
        inx
        cpx size
        beq :+
        jmp @loop
:   rts
.endproc
.proc print_board_size
    lda width
    cmp #20
    bne @hw
        lda #$80
        sta left_color
        lda #0
        sta right_color
        jmp @onwards
@hw:
    lda #0
    sta left_color
    lda #$80
    sta right_color
@onwards:
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
        ora TEMP 
        sta $06D0, y
:       iny
        jmp :---
:   rts 
@string:     
ASRAW "              HALF  "
.byte 255
ASRAW "FULL              "
.byte 0
left_color:  .byte 0
right_color: .byte 128
rts
.endproc
.PROC print_verticals
    ; Y stores the position in current row
    ; X stores the current index into the board
    ldy #0
    ldx #0
    iny
    sty TMP
    ldy #0
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
        txa
        pha
        jsr draw_whole_symbol
        pla
        tax
        pla
        tay
        iny    
        cpy width 
        bne :+
            ; we are at the end of the row
            ldy TMP
            iny
            iny
            sty TMP
            ldy #0                    
:       inx
        cpx size_m1
        bcs :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl ;tr ;tw ;bl ;lc ;lr ;BR ;br ;rl :rc :BL ;bw ;TR ;TL ;CL
.byte 0,   (8*30),  0, (8*30), (8*31), 24, (8*31), 24, 0, (8*30), 0, (8*30), (8*31), 24, (8*31), 24
.ENDPROC
.PROC print_horizontals
    ; Y stores the position in current row
    ; X stores the current index into the board
    ldy #0
    ldx #0
    sty TMP
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
        txa
        pha
        iny
        jsr draw_whole_symbol
        pla
        tax
        pla
        tay
        iny    
        cpy width_m1 ; M1 (-1) because we don't print the right edge
        bne :+
            ; we are at the end of the row
            inx
            ldy TMP
            iny
            iny
            sty TMP
            ldy #0                    
:       inx
        cpx size
        bcs :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl    ;tr;tw ;bl ;lc     ;lr ;BR ;br ;rl     :rc :BL ;bw ;TR ;TL ;CL
.byte (8*29),    (96+8),96 ,24 ,(8*29)  ,(96+8),96  ,24 ,(8*29)  ,(96+8),96, 24,  (8*29),  (96+8), 96,24
.ENDPROC
.PROC print_intersections
    ; Y stores the position in current row
    ; X stores the current index into the board
    
    ldy #0
    ldx #0
    iny
    sty TMP
    ldy #0
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
        txa
        pha
        iny
        jsr draw_whole_symbol
        pla
        tax
        pla
        tay
        iny    
        cpy width_m1 ; M1 (-1) because we don't print the right edge
        bne :+
            ; we are at the end of the row
            inx
            ldy TMP
            iny
            iny
            sty TMP
            ldy #0                    
:       inx
        cpx size_m1
        bcs :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl ;tr ;tw ;bl ;lc     ;lr;BR ;br ;rl :rc :BL ;bw ;TR ;TL ;CL
.byte (8*29),    (96+32)  ,(96+32+8) ,112 ,(96+32+16) ,(96+8),(96+32+32+24+16) ,(96+32+32),(96+32+24) ,(96+32+32+24+8),96, (96+32+32+8), 120, (96+32+32+16), (96+32+32+24), 24
.ENDPROC
.PROC print_menu
    jsr print_title
    jsr print_static_hud
    jsr print_hud
    jsr print_board_size
    rts
.ENDPROC
.PROC print_title
@TEXTRAM2 = $0650   
@TEXTRAM3 = $06D0    
@TEXTRAM4 = $0750    
@TEXTRAM = $07D0    
    ldy #0    
:   lda @str2,Y
    cmp #255
    beq :+
    sta @TEXTRAM2,Y
    lda @str, Y    
    sta @TEXTRAM, Y
    lda @str3, Y
    sta @TEXTRAM3, Y
    sta @TEXTRAM4, Y
    iny
    jmp :-
:   
rts    
@str2: AS "              BOARD SIZE:               "
.byte 255
@str3: AS "                                        "
@str: AS "C: COLOUR  ARROWS: ADJUST  RETURN: START "
.endproc
.PROC print_hud
    ldy flags
    ldx #20
@loop: 
    cpx #0 
    beq @done
    lda #24
    cpy #0
    beq :+
      dey
      lda #(96+64+24+24+8)
      cpy #0    
      beq :+
        dey
        lda #(96+64+24+24)
:   sta TEMP
    tya
    pha
    ldy #39
    dex
    stx TMP
    jsr draw_whole_symbol
    ldx TMP
    pla
    tay
    jmp @loop
@done:
    rts
    rts
.ENDPROC