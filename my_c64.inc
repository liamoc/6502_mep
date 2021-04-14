

MAX_HEIGHT=12
MAX_WIDTH=20

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


.PROC print_intersections
    ; PTR stores the current row in video ram
    ; PTR2 stores the current row in color ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    lda #41
    sta PTR
    lda #$04
    sta PTR+1
    lda #41
    sta PTR2
    lda #$D8
    sta PTR2+1
    ldy #0
    ldx #0
@loop:  tya 
        pha 
        lda intersections,X
        tay 
        lda @char_table,y
        sta TEMP
        lda @color_table,y 
        sta TEMP2 
        pla 
        tay         
        tya 
        pha
        asl 
        tay        
        lda TEMP
        sta (PTR),Y
        lda TEMP2
        sta (PTR2),Y
        pla
        tay
        iny    
        cpy width_m1 ; M1 (-1) because we don't print the right edge
        bne :++ 
            ; we are at the end of the row
            inx
            ldy #0
            lda PTR2
            clc
            adc #80
            sta PTR2
            bcc :+
                inc PTR2+1
:           lda PTR
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
@char_table:  .byte $20 | BLOCK_COL_BG, 29 | BLOCK_COL_BG, 28 | BLOCK_COL_BG, $20 | BLOCK_HI_COL_BG, 30 | BLOCK_COL_BG, $20 | BLOCK_HI_COL_BG, 31 | BLOCK_LO_COL_BG, 31 | SPACE_COL_BG, 31 | BLOCK_COL_BG, 29 | BLOCK_LO_COL_BG, $20 | BLOCK_LO_COL_BG, 30 | SPACE_COL_BG, $20 | BLOCK_LO_COL_BG, 28 | SPACE_COL_BG, 29 | SPACE_COL_BG   , $20 | SPACE_COL_BG
@color_table: .byte BLOCK_LO_COL      , BLOCK_HI_COL     , BLOCK_LO_COL     , BLOCK_HI_COL         , BLOCK_LO_COL     , BLOCK_LO_COL         , BLOCK_HI_COL        , BLOCK_HI_COL     , BLOCK_LO_COL     , BLOCK_HI_COL        , BLOCK_LO_COL         , BLOCK_HI_COL     , BLOCK_LO_COL         , BLOCK_HI_COL     , BLOCK_LO_COL        , BLOCK_LO_COL
.ENDPROC 

.PROC print_board
    lda #0
    ; PTR stores the current row in video ram
    ; PTR2 stores the current row in color ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    sta PTR
    lda #$04
    sta PTR+1
    lda #0 
    sta PTR2
    lda #$D8
    sta PTR2+1
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
                lda #1
                sta TEMP2
                lda #43 | BLOCK_COL_BG
                sta TEMP
                jmp @draw
:           ; cell is covered
            lda #BLOCK_COL_BG
            sta TEMP2
            lda #$20 | BLOCK_COL_BG ; space
            sta TEMP
        jmp @draw
@check_mine:
        lda board, X
        and #MINE_PLACED
        cmp #MINE_PLACED
        bne :+
            ; mine is here
            lda #2 ;red
            sta TEMP2
            lda #42 ; '*' char
            sta TEMP     
            jmp @draw
:       lda board,X 
        and #NEIGHBOURS
        cmp #0 
        bne :+
            ; no neighbouring mines
            lda #0; black
            sta TEMP2
            lda #$20; ' ' char
            sta TEMP          
            jmp @draw
        ; number for neighbouring mines
:           pha
            clc
            adc #$30
            sta TEMP
            pla
            sta TEMP2
            txa 
            pha 
            ldx TEMP2
            lda @colors,X
            sta TEMP2
            pla 
            tax             
@draw:  tya 
        pha
        asl 
        tay        
        lda TEMP
        sta (PTR),Y
        lda TEMP2
        sta (PTR2),Y
        pla
        tay
        iny    
        cpy width 
        bne @continue
            ; we are at the end of the row, move the pointers.
            ldy #0
            lda PTR2
            clc
            adc #80
            sta PTR2
            bcc :+
                inc PTR2+1
:           lda PTR
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
@colors: .byte $6,$5,$4,$3,$E,$1,$1,$1
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
.ENDPROC


.PROC won_screen 
    jsr conclude_music
    jsr stop_clock    
    ldx #0
    ldy #0
    lda #$A5
    sta PTR
    lda #$07
    sta PTR+1
    lda #$A5
    sta PTR2
    lda #$DB
    sta PTR2+1
@loop:
    lda @won_message,X 
    cmp #0 
    beq @ret
    ;ora #SPACE_COL_BG  
    sta (PTR),Y
    cmp #63
    beq :+
    cmp #62 
    beq :+  
    lda #7
    jmp :++
:   lda #1

:   sta (PTR2),Y
    lda PTR
    clc
    adc #1
    bcc :+
    inc PTR+1
:   sta PTR
    lda PTR2
    clc
    adc #1
    bcc :+
    inc PTR2+1
:   sta PTR2
    inx
    jmp @loop
@ret:
    jmp press_return
@won_message: .byte 63, $20, $20, 25, 15, 21, $20, 23,9,14,33, $20, $20, 62, 0
.ENDPROC

.PROC lost_screen 
    jsr conclude_music
    jsr stop_clock
    jsr reveal_mines
    jsr print_board
    jsr print_intersections
    jsr print_horizontals
    jsr print_verticals
    ldx #0
    ldy #0
    lda #$A5
    sta PTR
    lda #$07
    sta PTR+1
    lda #$A5
    sta PTR2
    lda #$DB
    sta PTR2+1
@loop:
    lda @lost_message,X 
    cmp #0 
    beq @ret
    ;ora #SPACE_COL_BG  
    sta (PTR),Y
    cmp #63
    beq :+
    cmp #62 
    beq :+  
    lda #7
    jmp :++
:   lda #1

:   sta (PTR2),Y
    lda PTR
    clc
    adc #1
    bcc :+
    inc PTR+1
:   sta PTR
    lda PTR2
    clc
    adc #1
    bcc :+
    inc PTR2+1
:   sta PTR2
    inx
    jmp @loop
@ret:
    jmp press_return
@lost_message: .byte 63, $20, $20, 25, 15, 21, $20, 12,15,19,5,$20,$20, 62, 0
.ENDPROC

.PROC press_return
    ldx #0
    ldy #0
    lda #$CE
    sta PTR
    lda #$07
    sta PTR+1
    lda #$CE
    sta PTR2
    lda #$DB
    sta PTR2+1
@loop:
    lda pak_message,X 
    cmp #0 
    beq wait_for_return
    ;ora #SPACE_COL_BG  
    sta (PTR),Y
    lda #8
    sta (PTR2),Y
    lda PTR
    clc
    adc #1
    bcc :+
    inc PTR+1
:   sta PTR
    lda PTR2
    clc
    adc #1
    bcc :+
    inc PTR2+1
:   sta PTR2
    inx
    jmp @loop
wait_for_return:
    jsr $FFE4; GETIN kernal
    cmp #$0D
    beq :+
    jmp wait_for_return
:   jmp main_menu
pak_message: .byte 16,18,5,19,19,$20,18,5,20,21,18,14,0
.ENDPROC


.PROC print_static_hud
    lda #1
    sta $dbc0
    sta $dbc1 
    sta $dbc2 
    sta $dbc3 
    sta $dbc4
    lda #58
    sta $07c2
    ldx #0  
:   cpx #40
    beq :+
    lda #61 ; top border
    sta $0798, x
    lda #1 ; white
    sta $db98, x
    inx
    jmp :-
:   jmp print_hud
.ENDPROC

.PROC print_hud
    ldx flags
    ldy #0
    lda #$97
    sta PTR
    lda #$07
    sta PTR+1
    lda #$97
    sta PTR2
    lda #$Db
    sta PTR2+1
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
    lda #1
    sta (PTR2),Y    
    lda PTR

    sec
    sbc #40
    bcs :+
    dec PTR+1
:   sta PTR
    lda PTR2
    sec
    sbc #40
    bcs :+
    dec PTR2+1
:   sta PTR2    
    jmp @loop
@done:
    lda #$20
    sta (PTR),Y 
    rts
.ENDPROC

.PROC print_menu_title
    lda #92
    sta PTR
    lda #$04
    sta PTR+1
    lda #92
    sta PTR2
    lda #$D8
    sta PTR2+1
    ldy #0
:       lda @title, y
        cmp #0 
        beq :+ 
        sta (PTR), y
        lda @color
        sta (PTR2), y
        iny
        jmp :-
:   rts 
@title: .byte $20, 1, 18, 3, 8, 1, 5, 15, 12, 15, 7, 9, 19, 20, 0
@color: .byte 7
.ENDPROC

.PROC print_board_size
    lda width
    cmp #20
    bne @hw
        lda #11 
        sta left_color
        lda #10
        sta right_color
        jmp @onwards
@hw:
    lda #10
    sta left_color
    lda #11
    sta right_color
@onwards:
    lda #156
    sta PTR
    lda #$05
    sta PTR+1
    lda #156
    sta PTR2
    lda #$D9
    sta PTR2+1
    lda left_color
    sta TEMP
    ldy #0
:       lda @string, y
        cmp #0 
        beq :++
        cmp #$FF
        bne :+
        lda right_color
        sta TEMP
        lda #$20
:       sta (PTR), y
        lda TEMP
        sta (PTR2), y
        iny
        jmp :--
:   rts 
@string:      .byte $20, $20, 8, 1, 12, 6, $20, $FF, $20, 6, 21, 12, 12, 0
left_color:  .byte 11
right_color: .byte 10
.ENDPROC

.PROC print_menu_body
    lda #36
    sta PTR
    lda #$05
    sta PTR+1
    lda #36
    sta PTR2
    lda #$D9
    sta PTR2+1
    ldy #0
@loop:  lda @menu_body, y
        cmp #0 
        beq @ret
        cmp #$FF
        beq @continue
        sta (PTR), y
        cmp #45
        bcs @hl
        lda @menu_body_color
        jmp @nohl
@hl:    lda @menu_hl_color      
@nohl:  sta (PTR2), y
@continue:
        iny
        jmp @loop
@ret:
    rts 
@menu_body:
  .byte               $20, 46, 58, $20, 4, 9, 6, 6, 9, 3, 21, 12, 20, 25, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $20, 47, 58, $20, 2, 15, 1, 18, 4, $20, 19, 9, 26, 5,$FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF 
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $20, $20, 8, 1, 12, 6, $20, $20, $20, 6, 21, 12, 12, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF 
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $20, 45, 58, $20, 19, 20, 1, 18, 20, $20, 7, 1, 13, 5, 0
@menu_body_color: .byte 1
@menu_hl_color:   .byte 7
.ENDPROC

.PROC print_byline
;MADE BY LIAM O'CONNOR IN 2020
    lda #$C0
    sta PTR
    lda #$07
    sta PTR+1
    lda #$C0
    sta PTR2
    lda #$DB
    sta PTR2+1
    ldy #0
:       lda byline, y
        cmp #0 
        beq :+ 
        sta (PTR), y
        lda color
        sta (PTR2), y
        iny
        jmp :-
:   rts 
byline: .byte $20, $20, $20, $20, $20, 13, 1, 4, 5, $20, 2, 25, $20, 12, 9, 1, 13, $20, 15, 39, 3, 15, 14, 14, 15, 18, $20, 9, 14, $20, 50, 48, 50, 48, 0
color:  .byte 11
.ENDPROC

.PROC hide_byline 
    lda print_byline::color
    pha
    lda #0
    sta print_byline::color
    jsr print_byline
    pla
    sta print_byline::color
    rts
.ENDPROC

.PROC print_menu
    ldx #1
    ldy #14
    lda #51
    sta PTR
    lda #$04
    sta PTR+1
    lda #51
    sta PTR2
    lda #$D8
    sta PTR2+1
    lda #7
    jsr draw_box  
    ldx #8
    ldy #14
    lda #211
    sta PTR
    lda #$04
    sta PTR+1
    lda #211
    sta PTR2
    lda #$D8
    sta PTR2+1
    lda #1
    jsr draw_box  
    jsr print_menu_title
    jsr print_static_hud
    jsr print_byline
    jsr print_menu_body
    jsr print_board_size
    rts
.ENDPROC

.PROC draw_halfwidth_backdrop
    ldx #21
    ldy #17
    lda #19
    sta PTR
    lda #$04
    sta PTR+1
    lda #19
    sta PTR2
    lda #$D8
    sta PTR2+1
    lda #11
    jmp draw_box
.ENDPROC    
; put rows in x and cols in y, color in a
; set PTR and PTR2 to locations to print in screen and color ram
; respectively
.PROC draw_box 
    pha ; color
    iny
    inx
    iny
    sty TEMP2 ; cols
    ; first, draw the top border
    ; top right:    
    sta (PTR2),y
    lda #36
    sta (PTR),y
    dey
    ; top middle
:   cpy #0
    beq :+
    pla
    pha 
    sta (PTR2),y
    lda #61
    sta (PTR),y    
    dey
    jmp :-
:   ;top left 
    pla
    pha
    sta (PTR2),y
    lda #35
    sta (PTR), y
@row_loop:
    dex
    lda PTR
    clc
    adc #40
    bcc :+
    inc PTR+1
:   sta PTR 
    lda PTR2
    clc
    adc #40
    bcc :+
    inc PTR2+1
:   sta PTR2
    ldy TEMP2
    cpx #0
    beq :+++
        ; middle right:    
        pla
        pha
        sta (PTR2),y
        lda #62
        sta (PTR),y
        dey
        ; middle middle
:       cpy #0
        beq :+
        pla
        pha 
        sta (PTR2),y
        lda #$20
        sta (PTR),y    
        dey
        jmp :-
:       ;middle left 
        pla
        pha
        sta (PTR2),y
        lda #63
        sta (PTR), y
        jmp @row_loop
:   ldy TEMP2 ; bottom right 
    pla
    pha 
    sta (PTR2),y
    lda #38
    sta (PTR),y
    dey
    ; bottom middle
:   cpy #0
    beq :+
    pla
    pha 
    sta (PTR2),y
    lda #61
    sta (PTR),y    
    dey
    jmp :-
:   ;bottom left 
    pla
    pha
    sta (PTR2),y
    lda #37
    sta (PTR), y
    pla
    rts
.ENDPROC

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
.incbin "chars.bin",0,512