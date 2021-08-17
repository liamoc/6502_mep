
FLAG_TILE = 89 
COVERED_TILE = 91
SPACE_TILE = 80
MINE_TILE = 90
OFFSET = 80
SPACE_COL    = 3
BLOCK_COL    = 7
TITLE_COL = 7
BLOCK_HI_COL = 5
BLOCK_LO_COL = 5

FLAG_COL = 7
MINE_COL = 1
WONMSG_COL = 7
LOSTMSG_COL = 2
STATIC_HUD_COL = 1
PRESSRET_COL = 3
SIDE_FLAGS_COL = 1
CLOCK_COL = 1
BSIZE_OFF_COL = 2
BSIZE_ON_COL = 7

MENU_BODY_COL = 1
MENU_HL_COL = 7

BYLINE_COL = 3
TITLE_BOX_COL = 1
MENU_BOX_COL = 1
BACKDROP_COL = 2


.PROC won_screen 
    jsr conclude_music
    lda #0
    sta CURSOR_ENABLED
    jsr stop_clock    
    ldx #0
    ldy #0
    lda #<(VRAM + $01A6 + 44)
    sta PTR
    lda #>(VRAM + $01A6 + 44)
    sta PTR+1
    lda #<(CRAM + $01A6 + 44)
    sta PTR2
    lda #>(CRAM + $01A6 + 44)
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
    lda #WONMSG_COL
    jmp :++
:   lda #STATIC_HUD_COL

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
@won_message: .byte 63, $20, 25, 15, 21, $20, 23,9,14,33, $20, 62, 0
.ENDPROC


.PROC lost_screen 
    lda #0
    sta CURSOR_ENABLED
    jsr conclude_music
    jsr stop_clock
    jsr reveal_mines
    jsr print_board
    jsr print_intersections
    ldx #0
    ldy #0
    lda #<(VRAM + $01A6 + 44)
    sta PTR
    lda #>(VRAM + $0100 + 44)
    sta PTR+1
    lda #<(CRAM + $01A6 + 44)
    sta PTR2
    lda #>(CRAM + $0100 + 44)
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
    lda #LOSTMSG_COL
    jmp :++
:   lda #STATIC_HUD_COL

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
@lost_message: .byte 63, $20, 25, 15, 21, $20, 12,15,19,5,$20, 62, 0
.ENDPROC


.PROC press_return
    ldx #0
    ldy #0
    lda #<(VRAM + $01A6 + 44 +28)
    sta PTR
    lda #>(VRAM + $01A6 + 44 + 28)
    sta PTR+1
    lda #<(CRAM + $01A6 + 44 + 28)
    sta PTR2
    lda #>(CRAM + $01A6 + 44 + 28)
    sta PTR2+1
@loop:
    lda pak_message,X 
    cmp #0 
    beq wait_for_return
    ;ora #SPACE_COL_BG  
    sta (PTR),Y
    lda #PRESSRET_COL
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
    lda #CLOCK_COL
    sta CRAM + $01CD + 22
    sta CRAM + $01CD + 22 + 1
    sta CRAM + $01CD + 22 + 2
    sta CRAM + $01CD + 22 + 3
    sta CRAM + $01CD + 22 + 4
    sta CRAM + $01CD + 22 + 5
    lda #58
    sta VRAM + $01CD+22+3
    lda #$20
    sta VRAM + $01CD+22
    sta VRAM + $01CD+22+1
    sta VRAM + $01CD+22+2
    sta VRAM + $01CD+22+4
    ldx #0
:   cpx #22
    beq :+
    lda #61 ; top border
    sta VRAM + $01CD, x
    lda #STATIC_HUD_COL
    sta CRAM + $01CD, x
    inx
    jmp :-
:   jmp print_hud
.ENDPROC

.PROC print_hud
    ldx flags
    cpx #44
    bcc :+
    ldx #44 
:   ldy #0
    lda #227
    sta PTR
    lda #>(VRAM + $0100) 
    sta PTR+1
    lda #227
    sta PTR2
    lda #>(CRAM + $0100)
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
    lda #SIDE_FLAGS_COL
    sta (PTR2),Y    
    lda PTR
    sec
    sbc #22
    bcs :+
    dec PTR+1
:   sta PTR
    lda PTR2
    sec
    sbc #22
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
    lda #25+22
    sta PTR
    lda #>VRAM
    sta PTR+1
    lda #25+22
    sta PTR2
    lda #>CRAM
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
@color: .byte TITLE_COL
.ENDPROC

.PROC print_board_size
    lda width
    cmp #MAX_WIDTH
    bne @hw
        lda #BSIZE_OFF_COL
        sta left_color
        lda #BSIZE_ON_COL
        sta right_color
        jmp @onwards
@hw:
    lda #BSIZE_ON_COL
    sta left_color
    lda #BSIZE_OFF_COL
    sta right_color
@onwards:
    lda #245
    sta PTR
    lda #>(VRAM+$0000)
    sta PTR+1
    lda #245
    sta PTR2
    lda #>(CRAM+$0000)
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
:  
rts 
@string:      .byte $20, $20, 8, 1, 12, 6, $20, $FF, $20, 6, 21, 12, 12, 0
.data
left_color:  .byte 11
right_color: .byte 10
.code
.ENDPROC

.PROC print_menu_body
    lda #24+22+22+22+22+22+1+22
    sta PTR
    lda #>VRAM    
    sta PTR+1
    lda #24+22+22+22+22+22+1+22
    sta PTR2
    lda #>CRAM
    sta PTR2+1
    ldy #0
@loop:  lda @menu_body, y
        cmp #0 
        beq @ret
        cmp #$FF
        beq @continue
        sta (PTR), y
        cmp #27
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
  .byte                $20, 28, 58, $20,   4,   9,    6,   6,   9,   3,  21, 12,   20,  25, $FF, $FF, $FF, $FF, $FF
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF
  .byte $FF, $FF, $FF, $20, 27,  58,  $20, 2,   15,   1,   18,  4,  $20, 19,  9,   26,  5,   $FF, $FF, $FF, $FF, $FF 
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF
  .byte $FF, $FF, $FF, $20, $20, 8,  1,    12,  6,   $20, $20, $20, 6,   21,  12,  12, $FF, $FF, $FF, $FF, $FF, $FF
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF
  .byte $FF, $FF, $FF, $20, 29, 30, $20, 19, 20, 1, 18, 20, $20, 7, 1, 13, 5, 0
@menu_body_color: .byte MENU_BODY_COL
@menu_hl_color:   .byte MENU_HL_COL
.ENDPROC

.PROC print_byline
;MADE BY LIAM O'CONNOR IN 2020
    lda #<(VRAM + $01E4)
    sta PTR
    lda #>(VRAM + $01E4)
    sta PTR+1
    lda #<(CRAM + $01E4)
    sta PTR2
    lda #>(CRAM + $01E4)
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
byline: .byte  13, 1, 4, 5, $20, 2, 25, $20, 12, 9, 1, 13, $20, 15, 39, 3, 15, 14, 14, 15, 18, $20, 0
.data
color:  .byte BYLINE_COL
.code
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
    lda #24
    sta PTR
    lda #>VRAM
    sta PTR+1
    lda #24
    sta PTR2
    lda #>CRAM
    sta PTR2+1
    lda #TITLE_BOX_COL
    jsr draw_box  
    ldx #8
    ldy #14
    lda #24+22+22+22+22
    sta PTR
    lda #>VRAM
    sta PTR+1
    lda #24+22+22+22+22
    sta PTR2
    lda #>CRAM
    sta PTR2+1
    lda #MENU_BOX_COL
    jsr draw_box  
    jsr print_menu_title
    jsr print_static_hud
    lda #BYLINE_COL
    sta print_byline::color
    jsr print_byline
    jsr print_menu_body
    jsr print_board_size
    rts
.ENDPROC

.PROC draw_halfwidth_backdrop
    ldx #19
    ldy #8
    lda #10
    sta PTR
    lda #>VRAM
    sta PTR+1
    lda #10
    sta PTR2
    lda #>CRAM
    sta PTR2+1
    lda #BACKDROP_COL
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
    adc #22
    bcc :+
    inc PTR+1
:   sta PTR 
    lda PTR2
    clc
    adc #22
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



.PROC print_intersections
    ; PTR stores the current row in video ram
    ; PTR2 stores the current row in color ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    lda #<(VRAM + 22)
    sta PTR
    lda #>(VRAM + 22)
    sta PTR+1
    lda #<(CRAM + 22)
    sta PTR2
    lda #>(CRAM + 22)
    sta PTR2+1
    ldy #0
    ldx #0
@loop:  tya 
        pha 
        lda intersections,X
        tay 
        clc
        and #$0F
        adc #64
        sta TEMP
        lda @color_table,y 
        sta TEMP2 
        pla 
        tay         
        tya 
        pha
        ;asl 
        tay        
        lda TEMP
        sta (PTR),Y
        lda TEMP2
        sta (PTR2),Y
        pla
        tay
        iny    
        cpy width
        bne @next_col
            ; we are at the end of the row
            
            ldy #0
            lda PTR2
            clc
            adc #44
            sta PTR2
            bcc :+
                inc PTR2+1
:           
            lda PTR
            clc
            adc #44
            sta PTR
            bcc @next_col
                inc PTR+1
@next_col:
        inx
        cpx size_m1
        bcs :+
        jmp @loop
:   rts

@color_table: .byte BLOCK_LO_COL      , BLOCK_HI_COL     , BLOCK_LO_COL     , BLOCK_HI_COL         , BLOCK_LO_COL     , BLOCK_LO_COL         , BLOCK_HI_COL        , BLOCK_HI_COL     , BLOCK_LO_COL     , BLOCK_HI_COL        , BLOCK_LO_COL         , BLOCK_HI_COL     , BLOCK_LO_COL         , BLOCK_HI_COL     , BLOCK_LO_COL        , BLOCK_LO_COL
.ENDPROC 

.PROC print_board
    lda #<VRAM
    ; PTR stores the current row in video ram
    ; PTR2 stores the current row in color ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    sta PTR
    lda #>VRAM
    sta PTR+1
    lda #<CRAM
    sta PTR2
    lda #>CRAM
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
                lda #FLAG_COL
                sta TEMP2
                lda #FLAG_TILE
                sta TEMP
                jmp @draw
:           ; cell is covered
            lda #BLOCK_COL
            sta TEMP2
            lda #COVERED_TILE
            sta TEMP
        jmp @draw
@check_mine:
        lda board, X
        and #MINE_PLACED
        cmp #MINE_PLACED
        bne :+
            ; mine is here
            lda #MINE_COL ;red
            sta TEMP2
            lda #MINE_TILE 
            sta TEMP     
            jmp @draw
:       lda board,X 
        and #NEIGHBOURS
        cmp #0 
        bne :+
            ; no neighbouring mines
            lda #BLOCK_HI_COL
            sta TEMP2
            lda #SPACE_TILE
            sta TEMP          
            jmp @draw
        ; number for neighbouring mines
:           pha
            clc
            adc #OFFSET
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
@draw:  lda intersections, X
        and #TOP_RIGHT
        cmp #TOP_RIGHT
        beq :+
        lda TEMP
        clc
        adc #12
        sta TEMP
:       tya 
        pha
        ;asl 
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
            adc #44
            sta PTR2
            bcc :+
                inc PTR2+1
:           lda PTR
            clc
            adc #44
            sta PTR
            bcc @continue
                inc PTR+1
@continue:
        inx
        cpx size
        beq :+
        jmp @loop
:   rts
@colors: .byte $0,$3,$5,$7,$1,$1,$1
.ENDPROC
        
