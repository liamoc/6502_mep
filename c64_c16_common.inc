.PROC won_screen 
    jsr conclude_music
    lda #0
    .ifdef __PLUS4__
    sta CURSOR_ENABLED
    .endif 
    jsr stop_clock    
    ldx #0
    ldy #0
    lda #$A5
    sta PTR
    lda #>(VRAM + $0300)
    sta PTR+1
    lda #$A5
    sta PTR2
    lda #>(CRAM + $0300)
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
@won_message: .byte 63, $20, $20, 25, 15, 21, $20, 23,9,14,33, $20, $20, 62, 0
.ENDPROC


.PROC lost_screen 
    lda #0
    .ifdef __PLUS4__
    sta CURSOR_ENABLED
    .endif 
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
    lda #>(VRAM + $0300)
    sta PTR+1
    lda #$A5
    sta PTR2
    lda #>(CRAM + $0300)
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
@lost_message: .byte 63, $20, $20, 25, 15, 21, $20, 12,15,19,5,$20,$20, 62, 0
.ENDPROC


.PROC press_return
    ldx #0
    ldy #0
    lda #$CE
    sta PTR
    lda #>(VRAM + $0300)
    sta PTR+1
    lda #$CE
    sta PTR2
    lda #>(CRAM + $0300)
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
    sta CRAM + $03c0
    sta CRAM + $03c1 
    sta CRAM + $03c2 
    sta CRAM + $03c3 
    sta CRAM + $03c4
    lda #58
    sta VRAM + $03c2
    ldx #0  
:   cpx #40
    beq :+
    lda #61 ; top border
    sta VRAM + $0398, x
    lda #STATIC_HUD_COL
    sta CRAM + $0398, x
    inx
    jmp :-
:   jmp print_hud
.ENDPROC

.PROC print_hud
    ldx flags
    ldy #0
    lda #$97
    sta PTR
    lda #>(VRAM + $0300) 
    sta PTR+1
    lda #$97
    sta PTR2
    lda #>(CRAM + $0300)
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
    lda #>VRAM
    sta PTR+1
    lda #92
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
    cmp #20
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
    lda #156
    sta PTR
    lda #>(VRAM + $0100) 
    sta PTR+1
    lda #156
    sta PTR2
    lda #>(CRAM + $0100)
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
left_color:  .byte 11
right_color: .byte 10
.ENDPROC

.PROC print_menu_body
    lda #36
    sta PTR
    lda #>(VRAM + $0100)
    sta PTR+1
    lda #36
    sta PTR2
    lda #>(CRAM + $0100)
    sta PTR2+1
    ldy #0
@loop:  lda @menu_body, y
        cmp #0 
        beq @ret
        cmp #$FF
        beq @continue
        sta (PTR), y
        cmp #44
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
  .byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $20, 45, 44, $20, 19, 20, 1, 18, 20, $20, 7, 1, 13, 5, 0
@menu_body_color: .byte MENU_BODY_COL
@menu_hl_color:   .byte MENU_HL_COL
.ENDPROC

.PROC print_byline
;MADE BY LIAM O'CONNOR IN 2020
    lda #$C0
    sta PTR
    lda #>(VRAM + $0300)
    sta PTR+1
    lda #$C0
    sta PTR2
    lda #>(CRAM + $0300)
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
color:  .byte BYLINE_COL
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
    lda #>VRAM
    sta PTR+1
    lda #51
    sta PTR2
    lda #>CRAM
    sta PTR2+1
    lda #TITLE_BOX_COL
    jsr draw_box  
    ldx #8
    ldy #14
    lda #211
    sta PTR
    lda #>VRAM
    sta PTR+1
    lda #211
    sta PTR2
    lda #>CRAM
    sta PTR2+1
    lda #MENU_BOX_COL
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
    lda #>VRAM
    sta PTR+1
    lda #19
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
