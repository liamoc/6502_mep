MAX_WIDTH=16
MAX_HEIGHT=11

PTR2 = $FB
PTR  = $FD
TEMP2 = $4A
CURSOR_ON = $4B
CURSOR_ENABLED = $4C
CHARSET = $4D
KEY_TICKS = $03
CURSOR_TICKS = $21
CURSOR_TICKS2 = $22
LAST_KEY = $02
KEY_SWITCH = $01
RNDL = $04
RNDH = $05
STRING_PTR = $0A
LEDS_PTR = $0C
LEDS_TICKS = $31
LEDS_TICKS2 = $32
.macro hide_cursor
.endmacro
.macro show_cursor
.endmacro 
.MACRO start_irq
nop
.ENDMACRO
.MACRO ack_irq
rts
.ENDMACRO
.MACRO GETIN
jsr rc6502_keyin
.ENDMACRO 


.PROC draw_symbol_text
; Y - column
; X - row 
; TEMP - symbol to draw (ASCII code)
; TEMP2 - color mask
txa
and #%111
ora #$80
sta PTR+1
txa
lsr
lsr
lsr
sta VDU_WINDOW
sty PTR

lda TEMP
sec
sbc #$30
bcc done
asl
asl
clc
asl
bcs loop2_preamble
tax
lda #0
loop: 
tay

lda symbols_chars, x
and TEMP2
sta (PTR),y
tya
clc
adc #$20
inx
bcs done
jmp loop
loop2_preamble:
tax
lda #0
loop2: 
tay

lda symbols_chars2, x
and TEMP2
sta (PTR),y
tya
clc
adc #$20
inx
bcs done
jmp loop2

done: 
rts
.ENDPROC
.proc draw_whole_symbol
ldx TMP
.endproc
.PROC draw_symbol
; Y - column
; X - row 
; TEMP - symbol to draw
txa
and #%111
ora #$80
sta PTR+1
txa
lsr
lsr
lsr
sta VDU_WINDOW
sty PTR
lda #0
ldx TEMP
loop: 
tay
lda symbols, x 
sta (PTR),y
tya
clc
adc #$20
inx
bcs done
jmp loop
done: 
rts
.ENDPROC
.include "rc6502_symbols.inc"

.PROC draw_whole_cursor
; Y - column
; TMP -row
;ldy #0
;ldx #0
ldx TMP 
txa
and #%111
ora #$80
sta PTR+1
txa
lsr
lsr
lsr
sta VDU_WINDOW
sty PTR
lda #0
ldx #0
loop: 
tay
lda cursor_symbol, x 
eor (PTR),y
sta (PTR),y
tya
clc
adc #$20
inx
bcs done
jmp loop
done: 
rts
cursor_symbol:
.byte %11111111
.byte %11000011
.byte %11000011
.byte %11000011
.byte %11000011
.byte %11000011
.byte %11000011
.byte %11111111
.ENDPROC
.proc draw_string
txa
pha
ldx #0
lda (STRING_PTR,X)
cmp #0
beq done
sta TEMP
pla
pha
tax
tya
pha
jsr draw_symbol_text
lda STRING_PTR
clc
adc #1
sta STRING_PTR
bcc :+ 
inc STRING_PTR+1
: pla
tay
pla
tax 
iny
jmp draw_string
done:
pla
rts
.endproc

KEY_RIGHT = %00001000
KEY_FW = %00001000
KEY_RIGHT2 = %00001001
KEY_LEFT = %00000100
KEY_HW = %00000100
KEY_LEFT2 = %00000110
KEY_UP = %00000001
KEY_HARDER = %00000001
KEY_UP2 = %000000101
KEY_DOWN = %00000010
KEY_EASIER = %00000010
KEY_DOWN2 = %00001010 
KEY_FLAG = %00100000
KEY_DIG = %00010000
KEY_START = %00010000
rc6502_keyin_empty:
lda #0
sta LAST_KEY
.proc rc6502_keyin
jsr clock
INC   RNDL
BNE   KEYIN2     ;INCR RND NUMBER
INC   RNDH
KEYIN2:   
inc CURSOR_TICKS
lda CURSOR_TICKS
cmp #0 
bne :+
inc CURSOR_TICKS2
lda CURSOR_TICKS2
cmp #8 
bne :+
lda #0
sta CURSOR_TICKS2
jsr flash_cursor
: lda LEDS
cmp #0
beq rc6502_keyin_empty
cmp LAST_KEY
bne new
pha 
inc KEY_TICKS 
lda KEY_TICKS
cmp #0
beq ticked
pla
jmp rc6502_keyin
ticked:
lda KEY_SWITCH
cmp #0
beq ticked2
dec KEY_SWITCH
pla
jmp rc6502_keyin
ticked2:
lda #$1
sta KEY_SWITCH
pla
rts
new: 
sta LAST_KEY
lda #$10
sta KEY_SWITCH
lda #0
sta KEY_TICKS
lda LAST_KEY
rts
.endproc

.proc end_screen
    sta is_win
    lda #0
    sta CURSOR_ENABLED
    jsr stop_clock
    jsr reveal_mines
    jsr print_board
    jsr print_intersections
    jsr print_horizontals
    jsr print_verticals
    ldx #22
    ldy #12
    lda is_win
    cmp #0
    beq @lose_msg
@win_msg:
    lda #<string1_w
    sta STRING_PTR
    lda #>string1_w
    sta STRING_PTR+1
    lda #%01010101
    sta TEMP2
    jsr draw_string
    lda #<leds_w
    sta LEDS_PTR
    lda #>leds_w
    sta LEDS_PTR+1
    jmp @prompt_msg
@lose_msg:
    lda #<string1_l
    sta STRING_PTR
    lda #>string1_l
    sta STRING_PTR+1
    lda #%10101010
    sta TEMP2
    jsr draw_string
    lda #<leds_l
    sta LEDS_PTR
    lda #>leds_l
    sta LEDS_PTR+1
@prompt_msg:
    ldx #23
    ldy #11
    lda #<string2
    sta STRING_PTR
    lda #>string2
    sta STRING_PTR+1
    lda #$FF
    sta TEMP2
    jsr draw_string
@waitforkey:
    GETIN
    cmp #KEY_DIG
    beq :+
    jmp @waitforkey
:   jmp main_menu
is_win: .byte 0
string1_w: .asciiz "YOU WIN="
string1_l: .asciiz "YOU LOSE"
string2: .asciiz "PRESS FIRE"
leds_l: .byte %11111111, %00000000, %11111111, %00000000, %11111111, %00000000, %11111111 , %01111110, %00111100, %00011000, $00, $00
leds_w: .byte %10101010, %01010101, %10101010, %01010101, %00000000, %11111111, %00000000, %11111111, %00000000, %11111111 , $00, $00
.endproc

.proc won_screen
lda #1
jmp end_screen
.endproc

.proc lost_screen
lda #0
jmp end_screen
.endproc

.proc dig_sound
rts
.endproc

.proc print_static_hud
ldx #(MAX_HEIGHT*2-1)
ldy #0
lda #(28*8)
sta TEMP
loop: 
cpy #31
beq done
tya 
pha
txa 
pha 
jsr draw_symbol
pla
tax 
pla
tay
iny
jmp loop
done:
rts
.endproc

.proc start_music
rts
.endproc

.proc setup_sound
rts
.endproc

.proc draw_halfwidth_backdrop
lda #24
sta TEMP
ldy #19
ldx #22
loop1:
txa 
pha 
tya 
pha 
jsr draw_symbol
pla
tay
pla
tax
iny
cpy #31
beq lp2
jmp loop1
lp2:
ldy #19
inx
cpx #22
beq done
jmp loop1
done:
rts
.endproc


.proc hide_byline
lda #24
sta TEMP
ldy #0
ldx #22
loop1:
txa 
pha 
tya 
pha 
jsr draw_symbol
pla
tay
pla
tax
iny
cpy #32
beq lp2
jmp loop1
lp2:
ldy #0
ldx #23
loop2:
txa 
pha 
tya 
pha 
jsr draw_symbol
pla
tay
pla
tax
iny
cpy #32
beq done
jmp loop2
done:
rts
.endproc

.proc print_title
jsr hide_byline
lda #<strin
sta STRING_PTR
lda #>strin
sta STRING_PTR+1
ldy #9
ldx #21
lda #%10101010
sta TEMP2
jsr draw_string
ldy #0
ldx #22
lda #$FF
sta TEMP2
lda #<strin2
sta STRING_PTR
lda #>strin2
sta STRING_PTR+1
jsr draw_string
ldy #22
ldx #23
lda #$FF
sta TEMP2
lda #<strin3
sta STRING_PTR
lda #>strin3
sta STRING_PTR+1
jsr draw_string
ldy #30
ldx #22
lda #$FF
sta TEMP2
lda #<strin4
sta STRING_PTR
lda #>strin4
sta STRING_PTR+1
jsr draw_string
ldy #13
ldx #22
lda #$FF
sta TEMP2
lda #<strin5
sta STRING_PTR
lda #>strin5
sta STRING_PTR+1
jsr draw_string
ldy #14
ldx #23
lda #%01010101
sta TEMP2
lda #<strin6
sta STRING_PTR
lda #>strin6
sta STRING_PTR+1
jsr draw_string
rts 
strin: 
.asciiz "ARCHAEOLOGIST"
strin2:
.asciiz "BOARD SIZE:"
strin3:
.asciiz "DIFFICULTY"
strin4:
.asciiz " @"
strin5:
.asciiz "FIRE TO"
strin6:
.asciiz "START"
.endproc

.proc print_board_size
    lda width
    cmp #MAX_WIDTH
    bne @hw
        lda #%10101010
        sta right_color
        lda #$FF
        sta left_color
        jmp @onwards
@hw:
    lda #%10101010
    sta left_color
    lda #$FF
    sta right_color
@onwards:
lda left_color
sta TEMP2
lda #<left_str
sta STRING_PTR
lda #>left_str
sta STRING_PTR+1
ldx #23
ldy #0
jsr draw_string
lda right_color
sta TEMP2
lda #<right_str
sta STRING_PTR
lda #>right_str
sta STRING_PTR+1
ldx #23
ldy #6
jsr draw_string
rts
left_str: .asciiz "HALF<"
right_str: .asciiz ">FULL"
left_color:  .byte 0
right_color: .byte 0
.endproc

.proc setup_timer
rts
.endproc 

.proc init
lda #%11111111 ; graphics mode
sta VDU_MODE
lda #0
sta CURSOR_TICKS
sta CURSOR_TICKS2
sta LEDS_TICKS
sta LEDS_TICKS2
sta CURSOR_ENABLED
lda #<start_leds 
sta LEDS_PTR
lda #>start_leds
sta LEDS_PTR+1
jsr test_pattern
rts
start_leds: .byte %00011000,%00100100, %01000010, %10000001, %11000011, %11100111, %11111111, %01111110, %00111100, %00011000, 0, 0
.endproc

.proc test_pattern
lda #24
sta TEMP
ldy #0
ldx #0
drawloop:
txa 
pha 
tya 
pha 
jsr draw_symbol
pla 
tay 
pla 
tax
lda TEMP
;eor #08
sta TEMP
iny
cpy #$20
beq next_row
jmp drawloop
next_row:
inx 
ldy #0
cpx #24
beq ledss
lda TEMP
;eor #08
sta TEMP
jmp drawloop

ledss:
rts
.endproc

.proc leds_tick
inc LEDS_TICKS
lda LEDS_TICKS
cmp #0 
bne :+
inc LEDS_TICKS2
lda LEDS_TICKS2
cmp #8 
bne :+
lda #0
sta LEDS_TICKS2
jmp :++
: rts 
: lda LEDS
ldy #0
lda (LEDS_PTR),Y
sta LEDS
cmp #0
beq possibly_done
not_done:
lda LEDS_PTR
clc
adc #1
sta LEDS_PTR
bcc done
inc LEDS_PTR+1
done:
rts
possibly_done:
iny
lda (LEDS_PTR),Y
cmp #0
beq done
lda #0
jmp not_done
.endproc