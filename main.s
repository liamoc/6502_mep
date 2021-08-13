.if .defined(__RC6502__)
__DISABLE_MUSIC__ = 1
__SPIN_CLOCK__ = 1
__SOFTWARE_CURSOR__ = 1
.include "apple2_rand.inc"
LEDS = $A000
KEYS = $A000
VDU_WINDOW = $8C00
VDU_MODE = $8800
VDU_BASE = $8000
TEMP  = $49
.CODE
.elseif .defined(__BBC__)
__SOFTWARE_CURSOR__ = 1
.include "apple2_rand.inc"
TEMP  = $DF
.code
.elseif .defined (__APPLE2__)
__SPIN_CLOCK__ = 1
__SOFTWARE_CURSOR__ = 1
__DISABLE_MUSIC__ = 1
.include "apple2_rand.inc"
.include "apple2.inc"
TEMP  = $49
.segment "LOWCODE"
.elseif .defined(__ATARI__)
.include "atari.inc"
.MACRO INITIALISE_RANDOM
nop
.ENDMACRO
.macro LDX_RANDOM
ldx RANDOM
.endmacro
.code
.elseif .defined(__PLUS4__)
__DISABLE_BASS__ = 1
__SOFTWARE_CURSOR__ = 1
.include "apple2_rand.inc"
.include "plus4.inc"
TEMP  = $2A
.CODE
.elseif .defined(__C64__)

.include "c64.inc"
.macro INITIALISE_RANDOM
    lda #0
    sta SID_Amp  ; quiet the sid chip
    lda #$ff     ; maximum frequency value
    sta SID_S3Lo ; voice 3 frequency low byte
    sta SID_S3Hi ; voice 3 frequency high byte
    lda #$80     ; noise waveform, gate bit off
    sta SID_Ctl3 ; voice 3 control register  
.endmacro
RANDOM = SID_Noise
.macro LDX_RANDOM
ldx RANDOM
.endmacro
TEMP  = $49
.CODE
.endif

Main:
    jsr init
    jsr setup_timer
    jmp main_menu

.ifdef __APPLE2__
.segment "HGR"
.CODE
.endif

.include "minesweeper.inc"

.ifdef __RC6502__
.include "my_rc6502.inc"
SOFTWARE_RANDOM_CODE

.elseif .defined(__BBC__)
.include "my_bbc.inc"
SOFTWARE_RANDOM_CODE

.elseif .defined(__APPLE2__)
.include "my_apple2.inc"
SOFTWARE_RANDOM_CODE
.segment "EXEHDR"
.word Main ; 2 byte BLAOD address
.word END - Main ; 2 byte BLOAD size

.elseif .defined(__ATARI__)
.include "my_atari.inc"
.segment "EXEHDR"
.word    $FFFF
.word    Main
.word    END - 1
.segment "AUTOSTRT"
.word    RUNAD            ; defined in atari.h
.word    RUNAD+1
.word    Main

.elseif .defined(__C64__)
.include "my_c64.inc"
.elseif .defined(__PLUS4__)
.include "my_plus4.inc"
.code
SOFTWARE_RANDOM_CODE
.endif

.CODE

.PROC menu_loop
    GETIN  

    cmp #KEY_FW
    beq full_width
    cmp #KEY_EASIER
    beq @less_difficulty
    cmp #KEY_HW
    beq @half_width
    cmp #KEY_HARDER
    beq @more_difficulty
.ifdef __APPLE2__
    cmp #KEY_CHARSWITCH
    bne :+
    lda CHARSET
    eor #$FF
    sta CHARSET
    jmp main_menu
:   nop
.endif
    cmp #KEY_START
    bne menu_loop
    jmp start_game
@less_difficulty:
    lda difficulty
    cmp #10
    bcc menu_loop
    dec difficulty
    dec difficulty
    dec flags
    dec flags
    jsr print_hud
    jmp menu_loop    
@more_difficulty:
    lda difficulty
    cmp #46
    bcs menu_loop
    inc difficulty
    inc difficulty
    inc flags
    inc flags
    jsr print_hud
    jmp menu_loop    
@half_width:
    lda #10
    sta width
    lda #9 
    sta width_m1
    lda #(MAX_HEIGHT*10)
    sta size
    lda #((MAX_HEIGHT-1)*10)
    sta size_m1
    lda #0
    sta cursor_col
    sta cursor_row
    sta cursor
    jsr print_board_size
    jmp menu_loop
full_width:
    lda #(MAX_WIDTH)
    sta width
    lda #(MAX_WIDTH-1)
    sta width_m1
    lda #(MAX_WIDTH*MAX_HEIGHT)
    sta size
    lda #(MAX_WIDTH*(MAX_HEIGHT-1))
    sta size_m1
    lda #0
    sta cursor_col
    sta cursor_row
    sta cursor
    jsr print_board_size
    jmp menu_loop
.ENDPROC
.PROC main_menu
    .ifndef __DISABLE_MUSIC__
    jsr stop_music
    .endif

    lda #26
    sta difficulty
    lda #(MAX_WIDTH)
    sta width 
    lda #(MAX_WIDTH-1)
    sta width_m1
    lda #(MAX_HEIGHT)
    sta height
    lda #(MAX_WIDTH*(MAX_HEIGHT))
    sta size 
    lda #(MAX_WIDTH*(MAX_HEIGHT-1))
    sta size_m1 
    hide_cursor
    
    jsr initialise_board
    jsr reveal_zeros
    jsr print_board    
    jsr print_intersections    
    jsr print_horizontals
    jsr print_verticals
    lda #46
    sta difficulty
    sta flags
    jsr print_menu
    jmp menu_loop::full_width
.ENDPROC
.PROC start_game
    show_cursor
    jsr hide_byline
    jsr initialise_board
    jsr print_board
    jsr print_intersections
    jsr print_horizontals
    jsr print_verticals
    .if !(.defined(__DISABLE_MUSIC__))
    jsr stop_music
    .endif
    lda width
    cmp #10
    bne :+
    jsr draw_halfwidth_backdrop
:   jsr setup_sound
    .if !(.defined(__DISABLE_MUSIC__))
    jsr start_music
    .endif
    jsr position_cursor_sprite
    jsr start_clock
    jsr print_static_hud    
    jmp main_loop
.ENDPROC


.PROC update_display
.ifdef __SOFTWARE_CURSOR__
    lda #0
    sta CURSOR_ENABLED
.endif     
    jsr print_board
    jsr print_horizontals
    jsr print_verticals
    jsr print_intersections
.ifdef __SOFTWARE_CURSOR__
    lda #$FF
    sta CURSOR_ENABLED
.endif 
    rts
.ENDPROC 

.PROC main_loop
    GETIN
    cmp #KEY_RIGHT
    beq @right
    cmp #KEY_RIGHT2
    beq @right
    .ifdef KEY_RIGHT3
    cmp #KEY_RIGHT3
    beq @right
    .endif
    cmp #KEY_DOWN
    beq @down
    cmp #KEY_DOWN2
    beq @down
    .ifdef KEY_DOWN3
    cmp #KEY_DOWN3
    beq @down
    .endif
    cmp #KEY_LEFT
    beq @left
    cmp #KEY_LEFT2
    beq @left
    .ifdef KEY_LEFT3
    cmp #KEY_LEFT3
    beq @left
    .endif
    cmp #KEY_UP
    beq @up
    cmp #KEY_UP2
    beq @up
    .ifdef KEY_UP3
    cmp #KEY_UP3
    beq @up
    .endif
    cmp #KEY_DIG
    beq @dig
    cmp #KEY_FLAG
    beq @flag
    jmp main_loop
@right:
    CLEAR_CURSOR
    jsr cursor_right
    jsr position_cursor_sprite
    jmp main_loop
@down:
    CLEAR_CURSOR
    jsr cursor_down
    jsr position_cursor_sprite
    jmp main_loop
@up:
    CLEAR_CURSOR
    jsr cursor_up
    jsr position_cursor_sprite
    jmp main_loop
@left:
    CLEAR_CURSOR
    jsr cursor_left
    jsr position_cursor_sprite
    jmp main_loop
@dig:
    CLEAR_CURSOR
    jsr dig
    jsr dig_sound
    jsr update_display
    jsr has_lost
    cmp #0 
    beq :+
    jmp lost_screen
:   jsr game_over
    cmp #0
    beq :+
    jmp won_screen
:   jsr position_cursor_sprite
    jmp main_loop
@flag:
    CLEAR_CURSOR
    jsr flag
    jsr update_display
    jsr print_hud 
    jsr position_cursor_sprite
    jmp main_loop    
.ENDPROC

.PROC clock    
.if .defined(__RC6502__)
    jsr leds_tick
.endif
    inc musiccounter
.ifndef __SPIN_CLOCK__
    lda musiccounter
    cmp #8 
    bne no_music_pulse
        lda #0
        sta musiccounter
        .ifdef __PLUS4__
        lda CURSOR_ENABLED
        cmp #0
        beq :+
        jsr flash_cursor
        :
        .endif 
        .if !(.defined(__DISABLE_MUSIC__))
        jsr music
        .endif
no_music_pulse:   
.if .defined(__ATARI__) || .defined(__PLUS4__) 
    jsr fade_sfx
.endif
    lda state
    cmp #0
    beq @done

    inc subcounter
.ifdef __PLUS4__ 
@subcounter_cap = 100
.else 
@subcounter_cap = 56
.endif
.else 
@subcounter_cap = 255
.if .defined(__RC6502__)
@musiccounter_cap = 50
.else 
@musiccounter_cap = 5
.endif 
    lda musiccounter
    cmp #@musiccounter_cap
    bne :+ 
        lda #0
        sta musiccounter
        inc subcounter
:
.endif
    lda subcounter
    cmp #@subcounter_cap
    bne @done
    lda #0
    sta subcounter
    inc seconds
    lda seconds
    cmp #10
    bne @done_dis
        lda #0
        sta seconds
        inc seconds10
        lda seconds10
        cmp #6
        bne @done_dis
            lda #0 
            sta seconds10
            inc minutes
            lda minutes
            cmp #10
            bne @done_dis
                lda #0
                sta minutes
                inc minutes10   
                
@done_dis: 
    jmp display_clock
@done: 
    ack_irq
musiccounter: .byte 0
state: .byte 0
subcounter: .byte 0
minutes: .byte 0
seconds: .byte 0
seconds10: .byte 0
minutes10: .byte 0
.ENDPROC

.ifndef __DISABLE_MUSIC__
MUTED      = %00000001
NOT_MUTED  = %10000000
PLAYING    = %00000010
CONCLUDING = %00000100
NOT_CONCLUDING = CONCLUDING ^ $FF

.PROC music
    lda state
    and #MUTED
    cmp #MUTED
    bne :+
        rts
:   lda state
    and #PLAYING
    cmp #PLAYING
    beq :+
        rts
:   ldy bass_position 
    lda bass_line, y
    cmp #$FE
    bne :+
        jmp stop_music
 :  cmp #$FF
    bne :++
        lda state
        and #CONCLUDING
        cmp #CONCLUDING
        bne :+
            inc bass_position
            iny
            lda bass_line, y
            lda #225 ; length of treble line
            sta treble_position
            lda state
            and #(NOT_CONCLUDING)
            sta state
            jmp :++
:       lda #0
        sta bass_position
        ldy bass_position
        lda bass_line, y
:   cmp #$0
    beq :+
        tax
        dex
        jsr bass_note
:   iny
    sty bass_position
    ldy treble_position
    lda treble_line, y
    cmp #$FF
    bne :+
        lda #32
        sta treble_position
        ldy treble_position
        lda treble_line, y
:   cmp #$0
    beq :+
        tax
        dex
        jsr treble_note
:   iny
    sty treble_position
 ;   lda #%00001111 
 ;   sta $d418
    rts
bass_line:       .byte 1,0, 2,0, 0,0, 2,0, 0,0, 3,0, 2,0, 1,0,$FF
conclusion_bass: .byte 1,0, 0,0, 2,0, 2,0, 0,0, 0,0, 2,0, 2,0
                 .byte 0,0, 0,0, 3,0, 3,0, 2,0, 0,0, 0,0, 0,0,$FE
bass_position:   .byte 0
treble_line:
   .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
   .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
   .byte 1,0,2,0,2,0,2,0,3,0,5,0,2,0,0,0 ; loop returns here
   .byte 1,0,2,0,2,0,2,0,3,0,5,0,2,0,0,0
   .byte 1,0,2,0,2,0,2,0,3,0,5,0,2,0,2,2
   .byte 6,0,4,4,5,0,3,0,2,0,3,0,2,0,0,0 
   .byte 1,0,2,0,2,0,2,0,3,0,5,0,2,0,0,0
   .byte 1,0,2,0,2,0,2,0,3,0,5,0,2,0,0,0
   .byte 1,0,2,0,2,0,2,0,3,0,5,0,2,0,2,2
   .byte 6,0,4,4,5,0,3,0,2,0,3,0,2,0,0,0 
   .byte 2,0,6,0,6,6,6,0,6,6,5,0,6,0,0,0
   .byte 2,0,6,0,6,0,6,0,6,0,5,0,6,0,0,0
   .byte 5,0,6,0,5,0,6,0,6,0,5,5,6,0,0,0
   .byte 6,0,4,4,5,0,3,0,2,0,3,0,2,0,0,0 
   .byte $FF ; 225 bytes
conclusion_treble:
   .byte 6,0,0,0,4,0,4,0,5,0,0,0,3,0,0,0
   .byte 2,0,0,0,3,0,0,0,2,0,0,0,0,0,0,0, $FE

treble_position: .byte 0
state: .byte %00000000
.ENDPROC

.PROC conclude_music 
    lda music::state
    ora #CONCLUDING
    sta music::state
    rts
.ENDPROC
.PROC start_music
    lda #0 
    sta music::treble_position
    sta music::bass_position
    lda music::state
    ora #PLAYING
    sta music::state
    rts
.ENDPROC
.PROC stop_music
    lda music::state
    and #MUTED
    sta music::state
    SILENCE
    sta music::treble_position
    sta music::bass_position
    rts
.ENDPROC
.PROC mute_music
    lda music::state
    ora #MUTED
    sta music::state
.ifdef __C64__
    lda #0
    sta SID_Ctl3 ; the only instrument that will annoyingly sustain
.endif
    rts
.ENDPROC 
.PROC unmute_music
    lda music::state
    and #(NOT_MUTED)
    sta music::state
    rts
.ENDPROC    
.endif 
; 1 = muted
; 2 = playing
; 3 = concluding
.PROC start_clock
    lda #0
    sta clock::subcounter
    sta clock::minutes
    sta clock::minutes10
    sta clock::seconds
    sta clock::seconds10
    lda #1
    sta clock::state
    rts
.ENDPROC 

.PROC stop_clock
    lda #0
    sta clock::state
    rts
.ENDPROC

.if (.defined(__APPLE2__) || .defined(__RC6502__))
.include "apple2_rc6502_visuals.inc"
.elseif !(.defined(__BBC__))
.include "atari_c64_c16_visuals.inc"
.endif 
.if (.defined(__C64__) || .defined(__PLUS4__))
.include "c64_c16_common.inc"
.endif
.PROC display_clock
.ifdef __BBC__
    lda #$00
    sta PTR
    lda #$76
    sta PTR+1
    lda clock::state
    cmp #0
    beq :+
    lda clock::minutes10
    asl
    asl 
    asl
    clc
    adc #(16*8)
    sta TEMP
    jsr draw_symbol
    lda #8
    sta PTR
    lda clock::minutes
    asl
    asl 
    asl
    clc
    adc #(16*8)
    sta TEMP
    jsr draw_symbol
    lda #(28*8)
    sta TEMP
    lda #17
    sta PTR
    jsr draw_symbol
    lda #24
    sta PTR
    lda clock::seconds10
    asl
    asl
    asl
    clc
    adc #(16*8)
    sta TEMP
    jsr draw_symbol
    lda #32
    sta PTR
    lda clock::seconds
    asl
    asl
    asl
    clc
    adc #(16*8)
    sta TEMP
    jsr draw_symbol
    lda clock::subcounter
    lsr
    bcs :+
    lsr 
    bcs :+
    lsr 
    bcs :+
    jsr flash_cursor
:   ack_irq
.else
.if .defined(__RC6502__)
@ScreenPos = ClockData
@Additive = $30
.elseif .defined(__APPLE2__)
@ScreenPos = $06D0
@Additive = 176
.elseif .defined(__ATARI__)
@ScreenPos = HudScreen+40
@Additive = 48
.elseif .defined(__C64__)
@ScreenPos = $07C0
@Additive = 48
.elseif .defined(__PLUS4__)
@ScreenPos = (VRAM + $03C0)
@Additive = 48
.endif
    lda clock::state
    cmp #0
    beq :+
    lda clock::minutes10
    clc
    adc #@Additive
    sta @ScreenPos
    lda clock::minutes
    clc
    adc #@Additive
    sta @ScreenPos+1
    lda clock::seconds10
    clc
    adc #@Additive
    sta @ScreenPos+3
    lda clock::seconds
    clc
    adc #@Additive
    sta @ScreenPos+4
:  
.if .defined(__RC6502__)
ldx #23
ldy #0
lda #$FF
sta TEMP2
lda #<ClockData
sta STRING_PTR
lda #>ClockData
sta STRING_PTR+1
lda clock::state
cmp #0
beq :+
jsr draw_string
: ack_irq
ClockData: .asciiz "00:00"
.else
ack_irq
.endif
.endif
.ENDPROC 
.DATA
END: