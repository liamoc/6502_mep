MAX_HEIGHT=11
MAX_WIDTH=21

PTR2 = $D8
PTR  = $DA
RNDL = $DC
RNDH = $DD
CURSOR_ENABLED = $DE
CURSOR_ON = $DF
;CURSOR_TMP = $E8
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
jmp $EABF
.ENDMACRO
.MACRO CLEAR_CURSOR
jsr clear_cursor
.ENDMACRO
.macro GETIN
inc RNDL
jsr $FFE4 ; GETIN kernal function
.endmacro
.PROC init 
lda #$0
sta CURSOR_ENABLED
sta CURSOR_ON
jsr stop_clock
jsr stop_music
    rts
.ENDPROC 

VRAM = $1E00
CRAM = $9600

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
    adc #44
    bcc :+
    inc PTR+1
    : sta PTR 
    dex
    jmp loop
    done:
    ldx #0
    lda (PTR,X)
    cmp #104
    bcs put_back
    clc
    adc #24
    sta (PTR,X)
    jmp :+
put_back:
    sec
    sbc #24
    sta (PTR,X)
:   rts
.endproc
.MACRO SILENCE
.ENDMACRO

.PROC setup_timer 
   ; $9125:$9216 contain the timer count, not sure if it needs to be set
    
    sei 
    lda #<clock
    sta $0314
    lda #>clock
    sta $0315
    cli 
    rts
.ENDPROC
.data
SOUND_TICKS: .byte 0
MUSIC_TICKS: .byte 0
.code
.PROC fade_sfx
lda SOUND_TICKS
cmp #0
beq done
dec SOUND_TICKS
lda SOUND_TICKS
cmp #0
beq stop
jmp done
stop:
    lda #0
    sta $900D
done:
lda MUSIC_TICKS
cmp #0
beq done2
dec MUSIC_TICKS
lda MUSIC_TICKS
cmp #0
beq stop2
jmp done2
stop2:
    lda #0
    sta $900C
    sta $900A
done2:

rts
.endproc
.PROC setup_sound
    lda $900E
    ora #$0F
    sta $900E
    rts
.ENDPROC 

.PROC dig_sound
    lda #129
    ora #%1100000
    sta $900D
    lda #$A
    sta SOUND_TICKS
    rts
.ENDPROC


.PROC bass_note
    lda notes, X
    ora #%1100000
    sta $900A
    lda #$8
    sta MUSIC_TICKS
rts
notes:
  .byte 225; C
  .byte 228 ; D
  .byte 229 ; Eflat
.ENDPROC
; x input
.PROC treble_note
    lda notes, X
    ora #%1100000
    sta $900C
    lda #$8
    sta MUSIC_TICKS
rts
notes:
  .byte 225; C
  .byte 228 ; D
  .byte 229 ; Eflat
  .byte 233 ; fsharp
  .byte 235 ; G
  .byte 237 ; A
.ENDPROC 


.PROC position_cursor_sprite    
    lda #$0
    sta CURSOR_ON
    lda #1
    sta CURSOR_ENABLED
    jmp flash_cursor
.ENDPROC


