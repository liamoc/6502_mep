

MAX_HEIGHT=12
MAX_WIDTH=20
SPACE_COL    = 0
BLOCK_COL    = 8 | $60
BLOCK_HI_COL = 8 | $F0
BLOCK_LO_COL = 8 | $C0
STATIC_HUD_COL = $F1
WONMSG_COL = $FF
BACKDROP_COL = $61
LOSTMSG_COL = $F2
MENU_BODY_COL = $61
MENU_HL_COL = $FF
BSIZE_ON_COL = $FF
BSIZE_OFF_COL = $21
BYLINE_COL = $F2
SIDE_FLAGS_COL = $61
PRESSRET_COL = $61
CLOCK_COL = $61
MINE_COL = $62
FLAG_COL = $01
TITLE_COL = $FF
TITLE_BOX_COL = $6F
MENU_BOX_COL = $FF
SPACE_COL_BG    = 0
BLOCK_COL_BG    = %01000000
BLOCK_LO_COL_BG = %10000000
BLOCK_HI_COL_BG = %11000000

PTR2 = $D8
PTR  = $DA
RNDL = $DC
RNDH = $DD
CURSOR_ENABLED = $DE
CURSOR_ON = $DF
CURSOR_TMP = $E8
TEMP2 = $5C
BoardScreen = VRAM

KEY_RIGHT = $1d
KEY_RIGHT2 = $44 ;D
KEY_LEFT = $9d
KEY_LEFT2 = $41 ; A
KEY_UP = $91
KEY_UP2 = $57 ; W
KEY_DOWN = $11
KEY_DOWN2 = $53 ; S
KEY_FLAG = $0d
KEY_DIG = $20

KEY_FW = $1d
KEY_HW = $9d
KEY_EASIER = $11
KEY_HARDER = $91
KEY_START = $0d
.MACRO start_irq
.ENDMACRO
.MACRO ack_irq
jmp handle_interrupt
.ENDMACRO
.MACRO CLEAR_CURSOR
jsr clear_cursor
.ENDMACRO
.macro GETIN
inc RNDL
jsr $FFE4 ; GETIN kernal function
.endmacro
.PROC init 
    lda #0
    sta TED_BORDERCOLOR
    lda #0
    lda $FF06  ; mode register
    ora #%01000000 ; extended color mode
    
    sta $FF06 
    lda $FF12
    and #$FB 
    sta $FF12
    lda #$30      ; set chars location to $2200 for displaying the custom font
    sta $FF13
    lda #SPACE_COL
    sta TED_BGCOLOR
    lda #BLOCK_COL
    sta TED_COLOR1
    lda #BLOCK_LO_COL
    sta TED_COLOR2
    lda #BLOCK_HI_COL
    sta TED_COLOR3
    rts
.ENDPROC 

VRAM = $0C00
CRAM = $0800



.proc clear_cursor
    lda CURSOR_ON
    cmp #0
    bne :+
    rts
:   jmp flash_cursor
.endproc
.MACRO show_cursor 
.ENDMACRO
.MACRO hide_cursor
.ENDMACRO
.proc flash_cursor
    lda CURSOR_ENABLED
    cmp #0
    beq :+
    lda CURSOR_ON
    eor #$FF
    sta CURSOR_ON
    lda #>VRAM
    sta PTR+1
    lda cursor_col
    asl
    clc
    adc #<VRAM
    bcc :+
    inc PTR+1
    : 
    sta PTR
    
    ldx cursor_row
    loop:
    cpx #0
    beq done
    lda PTR
    clc
    adc #80
    bcc :+
    inc PTR+1
    : sta PTR 
    dex
    jmp loop
    done:
    ldx #0
    lda (PTR,X)
    cmp #91
    beq put_back
    sta CURSOR_TMP
    lda #91
    sta (PTR,X)
    jmp :+
put_back:
    lda CURSOR_TMP
    sta (PTR,X)
:   rts
.endproc
.MACRO SILENCE
.ENDMACRO
.PROC handle_interrupt
    .byte $4C
    address: .byte $00, $00
.ENDPROC
.PROC setup_timer 
    lda #$FF
    sei 
    lda $0314
    sta handle_interrupt::address
    lda $0315
    sta handle_interrupt::address+1
    lda #<clock
    sta $0314
    lda #>clock
    sta $0315
    cli 
    rts
.ENDPROC
SOUND_TICKS: .byte 0
MUSIC_TICKS: .byte 0
.PROC fade_sfx
lda SOUND_TICKS 
cmp #0
bne :+
lda $FF11
and #%10011111
sta $FF11
jmp :++
:
dec SOUND_TICKS
:
lda MUSIC_TICKS
cmp #0
bne :+
lda $FF11
and #%11101111
sta $FF11
jmp :++
:
dec MUSIC_TICKS
:
rts
.endproc
.PROC setup_sound

lda #%00001111
sta $FF11
lda #$2A
sta $FF0F

lda #$3
sta $FF10
    rts
.ENDPROC 

.PROC dig_sound
lda #20
sta SOUND_TICKS
lda $FF11
ora #%01000000
and #%11011111
sta $FF11
    rts
.ENDPROC


.PROC bass_note
rts
.ENDPROC

.PROC treble_note
    lda #5
    sta MUSIC_TICKS    
    txa 
    asl
    tax
    inx
    lda @treble_table,x
    sta $FF0E ;  frequency
    dex
    lda @treble_table,x
    ora $FF12
    sta $FF12
    lda #$8
    lda $FF11
    ora #%00010000
    sta $FF11
    rts
@treble_table:
  .byte $3, $2A ; C
  .byte $3, $42 ; D
  .byte $3, $46 ; Eflat
  .byte $3, $69 ; fsharp
  .byte $3, $71 ; G
  .byte $3, $81 ; A
.ENDPROC 


.PROC position_cursor_sprite    
    lda #$0
    sta CURSOR_ON
    lda #1
    sta CURSOR_ENABLED
    jmp flash_cursor
.ENDPROC




.segment "CHARS"
.incbin "chars_hires-charset.bin",0,512