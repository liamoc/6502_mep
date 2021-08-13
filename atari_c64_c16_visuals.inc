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
.else;if .defined(__C64__)
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
.else;if .defined(__C64__)
.byte ($20 | BLOCK_COL_BG),   ($20 | BLOCK_HI_COL_BG), ($20 | BLOCK_COL_BG),    ($20 | BLOCK_HI_COL_BG)
.byte ($20 | BLOCK_LO_COL_BG),($20 | SPACE_COL_BG),    ($20 | BLOCK_LO_COL_BG), ($20 | SPACE_COL_BG)
.byte ($20 | BLOCK_COL_BG),   ($20 | BLOCK_HI_COL_BG), ($20 | BLOCK_COL_BG),    ($20 | BLOCK_HI_COL_BG)
.byte ($20 | BLOCK_LO_COL_BG),($20 | SPACE_COL_BG),    ($20 | BLOCK_LO_COL_BG), ($20 | SPACE_COL_BG)
.endif
.ENDPROC


.PROC print_intersections
    ; PTR stores the current row in video ram
    ; PTR2 stores the current row in color ram
    ; Y stores the position in current row
    ; X stores the current index into the board
    lda #<(VRAM + 41)
    sta PTR
    lda #>(VRAM + 41)
    sta PTR+1
    .ifndef __ATARI__
    lda #<(CRAM + 41)
    sta PTR2
    lda #>(CRAM + 41)
    sta PTR2+1
    .endif
    ldy #0
    ldx #0
@loop:  tya 
        pha 
        lda intersections,X
        tay 
        lda @char_table,y
        sta TEMP
        .ifndef __ATARI__
        lda @color_table,y 
        sta TEMP2 
        .endif
        pla 
        tay         
        tya 
        pha
        asl 
        tay        
        lda TEMP
        sta (PTR),Y
        .ifndef __ATARI__
        lda TEMP2
        sta (PTR2),Y
        .endif
        pla
        tay
        iny    
        cpy width_m1 ; M1 (-1) because we don't print the right edge
        bne @next_col
            ; we are at the end of the row
            inx
            ldy #0
            .ifndef __ATARI__
            lda PTR2
            clc
            adc #80
            sta PTR2
            bcc :+
                inc PTR2+1
:           .endif
            lda PTR
            clc
            adc #80
            sta PTR
            bcc @next_col
                inc PTR+1
@next_col:
        inx
        cpx size_m1
        bcs :+
        jmp @loop
:   rts
.ifdef __ATARI__
;     blank ;tl ;tr ;tw ;bl ;lc ;lr ;BR ;br ;rl :rc :BL ;bw ;TR ;TL ;CL
@char_table: .byte 12+64,    20+64, 21+64, 11+64, 22+64,  11+64, 25+64, 16+64, 23+64, 24+64, 13+64, 17+64,13+64, 18+64, 19+64, 0+64
.else
@char_table:  .byte $20 | BLOCK_COL_BG, 29 | BLOCK_COL_BG, 28 | BLOCK_COL_BG, $20 | BLOCK_HI_COL_BG, 30 | BLOCK_COL_BG, $20 | BLOCK_HI_COL_BG, 31 | BLOCK_LO_COL_BG, 31 | SPACE_COL_BG, 31 | BLOCK_COL_BG, 29 | BLOCK_LO_COL_BG, $20 | BLOCK_LO_COL_BG, 30 | SPACE_COL_BG, $20 | BLOCK_LO_COL_BG, 28 | SPACE_COL_BG, 29 | SPACE_COL_BG   , $20 | SPACE_COL_BG
@color_table: .byte BLOCK_LO_COL      , BLOCK_HI_COL     , BLOCK_LO_COL     , BLOCK_HI_COL         , BLOCK_LO_COL     , BLOCK_LO_COL         , BLOCK_HI_COL        , BLOCK_HI_COL     , BLOCK_LO_COL     , BLOCK_HI_COL        , BLOCK_LO_COL         , BLOCK_HI_COL     , BLOCK_LO_COL         , BLOCK_HI_COL     , BLOCK_LO_COL        , BLOCK_LO_COL
.endif
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
    .ifndef __ATARI__
    lda #<CRAM
    sta PTR2
    lda #>CRAM
    sta PTR2+1
    .endif
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
                .ifndef __ATARI__
                lda #FLAG_COL
                sta TEMP2
                .endif
                lda #FLAG_TILE
                sta TEMP
                jmp @draw
:           ; cell is covered
            .ifndef __ATARI__ 
            lda #BLOCK_COL_BG
            sta TEMP2
            .endif
            lda #COVERED_TILE
            sta TEMP
        jmp @draw
@check_mine:
        lda board, X
        and #MINE_PLACED
        cmp #MINE_PLACED
        bne :+
            ; mine is here
            .ifndef __ATARI__
            lda #MINE_COL ;red
            sta TEMP2
            .endif
            lda #MINE_TILE 
            sta TEMP     
            jmp @draw
:       lda board,X 
        and #NEIGHBOURS
        cmp #0 
        bne :+
            ; no neighbouring mines
            .ifndef __ATARI__ 
            lda #0
            sta TEMP2
            .endif
            lda #SPACE_TILE
            sta TEMP          
            jmp @draw
        ; number for neighbouring mines
:           pha
            clc
            adc #OFFSET
            sta TEMP
            pla
            .ifndef __ATARI__
            sta TEMP2
            txa 
            pha 
            ldx TEMP2
            lda @colors,X
            sta TEMP2
            pla 
            tax             
            .endif
@draw:  tya 
        pha
        asl 
        tay        
        lda TEMP
        sta (PTR),Y
        .ifndef __ATARI__
        lda TEMP2
        sta (PTR2),Y
        .endif
        pla
        tay
        iny    
        cpy width 
        bne @continue
            ; we are at the end of the row, move the pointers.
            ldy #0
            .ifndef __ATARI__
            lda PTR2
            clc
            adc #80
            sta PTR2
            bcc :+
                inc PTR2+1
:           .endif 
            lda PTR
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
.ifdef __ATARI__
.else ;commodore

.ifdef __PLUS4__
@colors: .byte $F6,$F5,$F4,$F3,$FE,$F1,$F1,$F1
.else ;c64
@colors: .byte $6,$5,$4,$3,$E,$1,$1,$1
.endif
FLAG_TILE = 43 | BLOCK_COL_BG
COVERED_TILE = $20 | BLOCK_COL_BG 
SPACE_TILE = $20
MINE_TILE = 42
OFFSET = $30
.endif
.ENDPROC
            
FLAG_TILE = 14 + 128 + 64
COVERED_TILE = 12 + 64
MINE_TILE = 15 + 128 + 64
SPACE_TILE = 64
OFFSET = 128 +64
        
