VRAM = $0400
CRAM = $d800

MAX_HEIGHT=12
MAX_WIDTH=20
SPACE_COL    = 0
BLOCK_COL    = 8
TITLE_COL = 7
BLOCK_HI_COL = 10
BLOCK_LO_COL = 9

FLAG_COL = 1
MINE_COL = 2
WONMSG_COL = 7
LOSTMSG_COL = 7
STATIC_HUD_COL = 1
PRESSRET_COL = 8
SIDE_FLAGS_COL = 1
CLOCK_COL = 1
BSIZE_OFF_COL = 11
BSIZE_ON_COL = 10

MENU_BODY_COL = 1
MENU_HL_COL = 7

BYLINE_COL = 11
TITLE_BOX_COL = 7
MENU_BOX_COL = 1
BACKDROP_COL = 11


SPACE_COL_BG    = 0
BLOCK_COL_BG    = %01000000
BLOCK_LO_COL_BG = %10000000
BLOCK_HI_COL_BG = %11000000

CURSOR_LEFT_MARGIN = 16
CURSOR_TOP_MARGIN  = 42
PTR2 = $FB
PTR  = $FD
TEMP2 = $4A
BoardScreen = $0400

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
    jmp $ea31
.ENDMACRO
.MACRO CLEAR_CURSOR
.ENDMACRO
.macro GETIN
jsr $FFE4 ; GETIN kernal function
.endmacro
.PROC init 
    lda #0
    sta VIC_BORDERCOLOR
    lda VIC_VIDEO_ADR
    ora #$0e       ; set chars location to $3800 for displaying the custom font
    sta VIC_VIDEO_ADR
    lda VIC_CTRL1  ; mode register
    ora #%01000000 ; extended color mode
    sta VIC_CTRL1
    lda #SPACE_COL
    sta VIC_BG_COLOR0
    lda #BLOCK_COL
    sta VIC_BG_COLOR1
    lda #BLOCK_LO_COL
    sta VIC_BG_COLOR2
    lda #BLOCK_HI_COL
    sta VIC_BG_COLOR3
    lda #$10
    sta VIC_SPR_MCOLOR
    lda #0
    sta VIC_SPR_BG_PRIO
    lda #7
    sta VIC_SPR0_COLOR
    lda #CURSOR_LEFT_MARGIN
    sta VIC_SPR0_X
    lda #CURSOR_TOP_MARGIN
    sta VIC_SPR0_Y
    lda #$80
    sta $07f8 ; sprite pointer 1
.ENDPROC 




.MACRO show_cursor 
    lda #$01
    sta VIC_SPR_ENA
.ENDMACRO

.MACRO hide_cursor
    lda #$00
    sta VIC_SPR_ENA
.ENDMACRO
.MACRO SILENCE
    lda #0
    sta SID_Ctl3
.ENDMACRO
.PROC setup_timer 
  ;  sei
   ; lda #%01010010
   ; sta $dc0f
    lda #$FF
    sta $dc04
    lda #$44
    sta $dc05
    lda #$ff
    sta $dc06
    sta $dc07
    lda #<clock
    sta $0314
    lda #>clock
    sta $0315
    lda #%00010001
    sta $dc0e
 ;   cli
    rts
.ENDPROC
.PROC setup_sound
    ; volume
    lda #%00001111
    sta SID_Amp
    ; sound effects control
    lda #%01000111
    sta SID_AD1
    ; sound effects frequency
    lda #$8
    sta SID_S1Hi
    lda #$8
    sta SID_PB1Hi
    rts
.ENDPROC 

.PROC dig_sound
    lda #0
    sta SID_Ctl1
    lda #%10000001
    sta SID_Ctl1
    rts
.ENDPROC


.PROC bass_note
    lda #0
    sta SID_Ctl2
    txa 
    asl
    tax
    inx
    lda @bass_table,x
    sta SID_S2Lo ;  frequency
    dex
    lda @bass_table,x
    sta SID_S2Hi

    lda #$8
    sta SID_PB2Lo ; pulse width
    sta SID_PB2Hi
    lda #%00100001 ; control register
    sta SID_Ctl2
    lda #%00000111 ; attack decay
    sta SID_AD2
    lda #%00001111 ; sustain release
    sta SID_SUR2
    rts
@bass_table:
  .byte $04, $54 ; C
  .byte $04, $DC ; D
  .byte $05, $26 ; Eflat
.ENDPROC

.PROC treble_note
    lda #0
    sta SID_Ctl3
    ;sta SID_C
    txa 
    asl
    tax
    inx
    lda @treble_table,x
    sta SID_S3Lo ;  frequency
    dex
    lda @treble_table,x
    sta SID_S3Hi

    lda #$8
    sta SID_PB3Lo ; pulse width
    sta SID_PB3Hi

    lda #%00010001 ; control register
    sta SID_Ctl3 
    lda #%01000111 ; attack decay
    sta SID_AD3
    lda #%00011111 ; sustain release
    sta SID_SUR3
    rts
@treble_table:
  .byte $11, $50 ; C
  .byte $13, $6F ; D
  .byte $14, $96 ; Eflat
  .byte $18, $7C ; fsharp
  .byte $19, $F0 ; G
  .byte $1D, $1E ; A
.ENDPROC 


.PROC position_cursor_sprite
    lda #0
    sta TEMP
    lda cursor_col
    asl
    bcc :+
    inc TEMP
:   clc 
    asl
    bcc :+
    inc TEMP
:   clc 
    asl
    bcc :+
    inc TEMP
:   clc 
    asl
    bcc :+
    inc TEMP
:   clc 
    adc #CURSOR_LEFT_MARGIN
    bcc :+ 
    inc TEMP
:   sta VIC_SPR0_X
    lda TEMP
    sta VIC_SPR_HI_X
    lda cursor_row
    asl
    asl
    asl
    asl 
    adc #CURSOR_TOP_MARGIN
    sta VIC_SPR0_Y
    lda cursor_col
    cmp width_m1
    beq :++
        lda cursor
        cmp size_m1
        bcs :+
            lda #$80
            sta $07f8
            rts
:       lda #$82
        sta $07f8
        rts
:   lda cursor
    cmp size_m1
    bcs :+
        lda #$81
        sta $07f8
        rts
:   lda #$83
    sta $07f8
    rts
.endproc
.segment "CURSORS"
cursor_data:
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00001111, %11111111, %11110000
.byte %00001000, %00000000, %00010000
.byte %00001011, %11111111, %11010000
.byte %00001011, %11111111, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %11111111, %11010000
.byte %00001011, %11111111, %11010000
.byte %00001000, %00000000, %00010000
.byte %00001111, %11111111, %11110000
.byte %00000000, %00000000, %00000000
.byte $FE

cursor_right_edge:
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00001111, %11111111, %00000000
.byte %00001000, %00000000, %00000000
.byte %00001011, %11111111, %00000000
.byte %00001011, %11111111, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %11111111, %00000000
.byte %00001011, %11111111, %00000000
.byte %00001000, %00000000, %00000000
.byte %00001111, %11111111, %00000000
.byte %00000000, %00000000, %00000000
.byte $FE

cursor_bottom_edge:
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00001111, %11111111, %11110000
.byte %00001000, %00000000, %00010000
.byte %00001011, %11111111, %11010000
.byte %00001011, %11111111, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00001011, %00000000, %11010000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte $FE
cursor_bottom_right_edge:
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00001111, %11111111, %00000000
.byte %00001000, %00000000, %00000000
.byte %00001011, %11111111, %00000000
.byte %00001011, %11111111, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00001011, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte %00000000, %00000000, %00000000
.byte $FE
.segment "CHARS"
.incbin "chars_hires-charset.bin",0,512