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
