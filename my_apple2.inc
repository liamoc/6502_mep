MAX_WIDTH=20
MAX_HEIGHT=10

PTR2 = $FB
PTR  = $FD
TEMP2 = $4A
CURSOR_ON = $4B
CURSOR_ENABLED = $4C
CHARSET = $4D

KEY_RIGHT = $95
KEY_RIGHT2 = 58 ; d
KEY_LEFT = $88
KEY_LEFT2 = 63 ; a
KEY_UP = $8B
KEY_UP2 = 46 ; w
KEY_DOWN = $8A
KEY_DOWN2 = 62 ; s
KEY_FLAG = $8D
KEY_DIG = $A0
KEY_FW = $95
KEY_HW = $88
KEY_HARDER = $8B
KEY_EASIER = $8A
KEY_START = $8D
KEY_CHARSWITCH = $C3

.MACRO CLEAR_CURSOR
jsr clear_cursor
.ENDMACRO
.MACRO GETIN
jsr apple_keyin
.ENDMACRO 
.MACRO start_irq
nop
.ENDMACRO
.MACRO ack_irq
rts
.ENDMACRO
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