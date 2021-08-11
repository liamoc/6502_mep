
.proc print_board
    ldy #0
    ldx #0
    stx TMP
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
                lda #8
                sta TEMP
                jmp @draw
:           ; cell is covered            
            lda #0
            sta TEMP
        jmp @draw
@check_mine:
        lda board, X
        and #MINE_PLACED
        cmp #MINE_PLACED
        bne :+
            ; mine is here            
            lda #16 ; '*' char
            sta TEMP     
            jmp @draw
:       lda board,X 
        and #NEIGHBOURS
        cmp #0 
        bne :+
            ; no neighbouring mines            
            lda #24; ' ' char
            sta TEMP          
            jmp @draw
        ; number for neighbouring mines
:       asl
        asl
        asl
        clc 
        adc #(24) 
        sta TEMP
@draw:  tya 
        pha
        txa
        pha
        tya
        asl 
        tay
        jsr draw_whole_symbol 
        pla
        tax
        pla
        tay
        iny    
        cpy width 
        bne @continue
            ; we are at the end of the row, move the counters
            ldy TMP
            iny
            iny
            sty TMP
            ldy #0
@continue:
        inx
        cpx size
        beq :+
        jmp @loop
:   rts
.endproc
.PROC print_verticals
    ; Y stores the position in current row
    ; X stores the current index into the board
    ldy #0
    ldx #0
    iny
    sty TMP
    ldy #0
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
        txa
        pha
        jsr draw_whole_symbol
        pla
        tax
        pla
        tay
        iny    
        cpy width 
        bne :+
            ; we are at the end of the row
            ldy TMP
            iny
            iny
            sty TMP
            ldy #0                    
:       inx
        cpx size_m1
        bcs :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl ;tr ;tw ;bl ;lc ;lr ;BR ;br ;rl :rc :BL ;bw ;TR ;TL ;CL
.byte 0,   (8*30),  0, (8*30), (8*31), 24, (8*31), 24, 0, (8*30), 0, (8*30), (8*31), 24, (8*31), 24
.ENDPROC
.PROC print_horizontals
    ; Y stores the position in current row
    ; X stores the current index into the board
    ldy #0
    ldx #0
    sty TMP
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
        txa
        pha
        iny
        jsr draw_whole_symbol
        pla
        tax
        pla
        tay
        iny    
        cpy width_m1 ; M1 (-1) because we don't print the right edge
        bne :+
            ; we are at the end of the row
            inx
            ldy TMP
            iny
            iny
            sty TMP
            ldy #0                    
:       inx
        cpx size
        bcs :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl    ;tr;tw ;bl ;lc     ;lr ;BR ;br ;rl     :rc :BL ;bw ;TR ;TL ;CL
.byte (8*29),    (96+8),96 ,24 ,(8*29)  ,(96+8),96  ,24 ,(8*29)  ,(96+8),96, 24,  (8*29),  (96+8), 96,24
.ENDPROC
.PROC print_intersections
    ; Y stores the position in current row
    ; X stores the current index into the board
    
    ldy #0
    ldx #0
    iny
    sty TMP
    ldy #0
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
        txa
        pha
        iny
        jsr draw_whole_symbol
        pla
        tax
        pla
        tay
        iny    
        cpy width_m1 ; M1 (-1) because we don't print the right edge
        bne :+
            ; we are at the end of the row
            inx
            ldy TMP
            iny
            iny
            sty TMP
            ldy #0                    
:       inx
        cpx size_m1
        bcs :+
        jmp @loop
:   rts
@char_table:  
;     blank ;tl ;tr ;tw ;bl ;lc     ;lr;BR ;br ;rl :rc :BL ;bw ;TR ;TL ;CL
.byte (8*29),    (96+32)  ,(96+32+8) ,112 ,(96+32+16) ,(96+8),(96+32+32+24+16) ,(96+32+32),(96+32+24) ,(96+32+32+24+8),96, (96+32+32+8), 120, (96+32+32+16), (96+32+32+24), 24
.ENDPROC

.PROC print_hud
    ldy flags
    ldx #(MAX_HEIGHT*2)
@loop: 
    cpx #0 
    beq @done
    lda #24
    cpy #0
    beq :+
      dey
      lda #(96+64+24+24+8)
      cpy #0    
      beq :+
        dey
        lda #(96+64+24+24)
:   sta TEMP
    tya
    pha
    ldy #(MAX_WIDTH*2-1)
    dex
    stx TMP    
    jsr draw_whole_symbol
    ldx TMP
    pla
    tay
    jmp @loop
    
@done:
    rts
.ENDPROC
.proc clear_cursor
    lda CURSOR_ON
    cmp #0
    bne :+
    rts
:   jmp flash_cursor
.endproc
.proc position_cursor_sprite
    lda #$0
    sta CURSOR_ON
    lda #1
    sta CURSOR_ENABLED
    jmp flash_cursor
.endproc
.proc flash_cursor

    lda CURSOR_ENABLED
    cmp #0
    beq :+
    lda CURSOR_ON
    eor #$FF
    sta CURSOR_ON
    lda cursor_col
    asl
    tay
    lda cursor_row
    asl
    tax
    stx TMP
    jmp draw_whole_cursor
:   rts
.endproc
.PROC print_menu
    jsr print_static_hud
    jsr print_title
    jsr print_hud
    jsr print_board_size
    rts
.ENDPROC