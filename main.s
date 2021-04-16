; main assembler part
;
; -> (c) 2012 C.Kr�ger <-
.if .defined(__BBC__)
.MACRO INITIALISE_RANDOM
lda $0240
sta SEED0
sta SEED3
lda $0240
sta SEED1
sta SEED2
.ENDMACRO
.MACRO start_irq
nop
.ENDMACRO
.MACRO ack_irq
rts
.ENDMACRO
.MACRO CLEAR_CURSOR
jsr clear_cursor
.ENDMACRO
.MACRO GETIN
JSR $FFE0
.ENDMACRO 
PTR2 = $FB
PTR  = $FD
TEMP  = $49
TEMP2 = $4A
CURSOR_ON = $4B
CURSOR_ENABLED = $4C
.macro LDX_RANDOM
lda #$FF
jsr RANDOM8
tax
.endmacro
.elseif .defined (__APPLE2__)
.include "apple2.inc"
.MACRO INITIALISE_RANDOM
lda RNDL
sta SEED0
sta SEED3
lda RNDH
sta SEED1
sta SEED2
.ENDMACRO
.MACRO start_irq
nop
.ENDMACRO
.MACRO ack_irq
rts
.ENDMACRO
.MACRO CLEAR_CURSOR
jsr clear_cursor
.ENDMACRO
.MACRO GETIN
jsr apple_keyin
.ENDMACRO 
PTR2 = $FB
PTR  = $FD
TEMP  = $49
TEMP2 = $4A
CURSOR_ON = $4B
CURSOR_ENABLED = $4C
CHARSET = $4D
.macro LDX_RANDOM
lda #$FF
jsr RANDOM8
tax
.endmacro
.elseif .defined(__ATARI__)
.MACRO CLEAR_CURSOR
.ENDMACRO
.include "atari.inc"
.MACRO INITIALISE_RANDOM
nop
.ENDMACRO
.macro LDX_RANDOM
ldx RANDOM
.endmacro
PTR  = $FD

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

.elseif .defined(__C64__)
.MACRO start_irq
.ENDMACRO
.MACRO ack_irq
    jmp $ea31
.ENDMACRO

.MACRO CLEAR_CURSOR
.ENDMACRO
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

SPACE_COL    = 0
BLOCK_COL    = 8
BLOCK_HI_COL = 10
BLOCK_LO_COL = 9

SPACE_COL_BG    = 0
BLOCK_COL_BG    = %01000000
BLOCK_LO_COL_BG = %10000000
BLOCK_HI_COL_BG = %11000000

CURSOR_LEFT_MARGIN = 16
CURSOR_TOP_MARGIN  = 42
RANDOM = SID_Noise
.macro LDX_RANDOM
ldx RANDOM
.endmacro
PTR2 = $FB
PTR  = $FD
TEMP  = $49
TEMP2 = $4A
BoardScreen = $0400
.macro GETIN
jsr $FFE4 ; GETIN kernal function
.endmacro
.endif

.ifdef __APPLE2__
.segment "LOWCODE"
.else
.CODE
.endif
Main:
  ;  lda #2
  ;  sta 710
  ;  jmp Main
    jsr init
    jsr setup_timer
    jmp main_menu

.ifdef __APPLE2__
.segment "HGR"
.CODE
.endif
.include "minesweeper.inc"
.if .defined(__BBC__)
.include "my_bbc.inc"
.include "apple2_rand.inc"
.elseif .defined(__APPLE2__)
.include "my_apple2.inc"
.include "apple2_rand.inc"
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
.elseif __C64__
.include "my_c64.inc"
.endif
.CODE

.PROC menu_loop
.if .defined(__BBC__)
KEY_FW = $15
KEY_HW = $08
KEY_HARDER = $0B
KEY_EASIER = $0A
KEY_START = $0D
.elseif .defined(__APPLE2__)
KEY_FW = $95
KEY_HW = $88
KEY_HARDER = $8B
KEY_EASIER = $8A
KEY_START = $8D
KEY_CHARSWITCH = $C3
.elseif .defined(__ATARI__)
KEY_FW = 135
KEY_HW = 134
KEY_HARDER = 142
KEY_EASIER = 143
KEY_START = $FD
.elseif .defined(__C64__)
KEY_FW = $1d
KEY_HW = $9d
KEY_EASIER = $11
KEY_HARDER = $91
KEY_START = 136
.endif
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
    .if !(.defined(__APPLE2__) || .defined(__BBC__))
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
    jsr printy
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
    .if !(.defined(__APPLE2__) || .defined(__BBC__))
    jsr stop_music
    .endif
    lda width
    cmp #10
    bne :+
    jsr draw_halfwidth_backdrop
:   jsr setup_sound
    .if !(.defined(__APPLE2__) || .defined(__BBC__))
    jsr start_music
    .endif
    jsr position_cursor_sprite
    jsr start_clock
    jsr print_static_hud    
    jmp main_loop
.ENDPROC


.PROC update_display
    jsr print_board
    jsr print_horizontals
    jsr print_verticals
    jsr print_intersections
    rts
.ENDPROC 

.PROC main_loop
.if .defined(__BBC__)

KEY_RIGHT = $05
KEY_RIGHT2 = 68 ; d
KEY_LEFT = $08
KEY_LEFT2 = 65 ; a
KEY_UP = $0B
KEY_UP2 = 87 ; w
KEY_DOWN = $0A
KEY_DOWN2 = 83 ; s
KEY_FLAG = $0D
KEY_DIG = $20

.elseif .defined(__APPLE2__)

KEY_FW = $95
KEY_HW = $88
KEY_HARDER = $8B
KEY_EASIER = $8A
KEY_START = $8D

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
.elseif .defined(__ATARI__)
KEY_RIGHT = 135
KEY_RIGHT2 = 58 ; d
KEY_LEFT = 134
KEY_LEFT2 = 63 ; a
KEY_UP = 142
KEY_UP2 = 46 ; w
KEY_DOWN = 143
KEY_DOWN2 = 62 ; s
KEY_FLAG = 12
KEY_DIG = $21
.elseif .defined(__C64__)
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
.endif
    GETIN
    cmp #KEY_RIGHT
    beq @right
    cmp #KEY_RIGHT2
    beq @right
    cmp #KEY_DOWN
    beq @down
    cmp #KEY_DOWN2
    beq @down
    cmp #KEY_LEFT
    beq @left
    cmp #KEY_LEFT2
    beq @left
    cmp #KEY_UP
    beq @up
    cmp #KEY_UP2
    beq @up
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
    inc musiccounter
.if !(.defined(__APPLE2__) || .defined(__BBC__))
    lda musiccounter    
    cmp #8
    bne :+
        lda #0
        sta musiccounter
        jsr music
:   
.ifdef __ATARI__
    jsr fade_sfx
.endif
    lda state
    cmp #0
    beq @done

    inc subcounter
@subcounter_cap = 56
.else 
@subcounter_cap = 255
    lda musiccounter
    cmp #5
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
    bne @done
        lda #0
        sta seconds
        inc seconds10
        lda seconds10
        cmp #6
        bne @done
            lda #0 
            sta seconds10
            inc minutes
            lda minutes
            cmp #10
            bne @done
                lda #0
                sta minutes
                inc minutes10   
@done: 
    jmp display_clock
musiccounter: .byte 0
state: .byte 0
subcounter: .byte 0
minutes: .byte 0
seconds: .byte 0
seconds10: .byte 0
minutes10: .byte 0
.ENDPROC

.if !(.defined(__APPLE2__) || .defined(__BBC__))
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

.if !(.defined(__APPLE2__) || .defined(__BBC__))
.PROC print_horizontals
    ; PTR stores the current row in video ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    lda #<(BoardScreen+1)
    sta PTR
    lda #>(BoardScreen+1)
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
        cpy width_m1  ; minus one because we don't print the right edge
        bne :+
            inx
            ldy #0
            lda PTR
            clc
            adc #80
            sta PTR
            bcc :+
                inc PTR+1
:       inx
        cpx size
        bcs :+
        jmp @loop
:   rts
@char_table:
.ifdef __ATARI__
    .byte 12+64, 11+64, 13+64,0+64,  12+64, 11+64, 13+64,0+64,12+64, 11+64,13+64,0+64,12+64,11+64,13+64,0+64
.elseif .defined(__C64__)
    .byte ($20 | BLOCK_COL_BG), ($20 | BLOCK_HI_COL_BG), ($20 | BLOCK_LO_COL_BG), ($20 | SPACE_COL_BG)
    .byte ($20 | BLOCK_COL_BG), ($20 | BLOCK_HI_COL_BG), ($20 | BLOCK_LO_COL_BG), ($20 | SPACE_COL_BG)
    .byte ($20 | BLOCK_COL_BG), ($20 | BLOCK_HI_COL_BG), ($20 | BLOCK_LO_COL_BG), ($20 | SPACE_COL_BG)
    .byte ($20 | BLOCK_COL_BG), ($20 | BLOCK_HI_COL_BG), ($20 | BLOCK_LO_COL_BG), ($20 | SPACE_COL_BG)
.endif
.ENDPROC

.PROC print_verticals
    ; PTR stores the current row in video ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    lda #<(BoardScreen+40)
    sta PTR
    lda #>(BoardScreen+40)
    sta PTR+1
    ldy #0
    ldx #0
@loop:  tya 
        pha 
        lda intersections,x
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
        cpy width 
        bne :+ 
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
.ifdef __ATARI__
.byte 12+64, 11+64, 12+64, 11+64, 13+64, 0+64, 13+64, 0+64, 12+64, 11+64,12+64, 11+64,13+64, 0+64, 13+64, 0+64
.elseif .defined(__C64__)
.byte ($20 | BLOCK_COL_BG),   ($20 | BLOCK_HI_COL_BG), ($20 | BLOCK_COL_BG),    ($20 | BLOCK_HI_COL_BG)
.byte ($20 | BLOCK_LO_COL_BG),($20 | SPACE_COL_BG),    ($20 | BLOCK_LO_COL_BG), ($20 | SPACE_COL_BG)
.byte ($20 | BLOCK_COL_BG),   ($20 | BLOCK_HI_COL_BG), ($20 | BLOCK_COL_BG),    ($20 | BLOCK_HI_COL_BG)
.byte ($20 | BLOCK_LO_COL_BG),($20 | SPACE_COL_BG),    ($20 | BLOCK_LO_COL_BG), ($20 | SPACE_COL_BG)
.endif
.ENDPROC
.endif 


.PROC display_clock
.if .defined(__BBC__)
@ScreenPos = $06D0
@Additive = 176
.elseif .defined(__APPLE2__)
@ScreenPos = $06D0
@Additive = 176
.elseif .defined(__ATARI__)
@ScreenPos = HudScreen+40
@Additive = 48
.elseif .defined(__C64__)
@ScreenPos = $07C0
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
:   ack_irq
.ENDPROC 
.DATA
END:
