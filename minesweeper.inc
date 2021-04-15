MINE_PLACED   = %01000000
FLAG_PLACED   = %00100000
NOT_FLAG_PLACED   = %00100000 ^ $FF
UNCOVERED     = %10000000
NEIGHBOURS    = %00001111
TOP_LEFT      = %0001
TOP_RIGHT     = %0010
BOTTOM_LEFT   = %0100
BOTTOM_RIGHT  = %1000

board:         .res 240, 0
intersections: .res 240, 0
difficulty:    .byte 46
width:         .byte MAX_WIDTH
width_m1:      .byte (MAX_WIDTH-1)
height:        .byte MAX_HEIGHT
flags:         .byte 46
size:          .byte (MAX_WIDTH*MAX_HEIGHT)
size_m1:       .byte (MAX_WIDTH*(MAX_HEIGHT-1))
cursor:        .byte 0
cursor_row:    .byte 0
cursor_col:    .byte 0

.FEATURE c_comments

.PROC initialise_board
    INITIALISE_RANDOM
    ; zero out everything
    ldx #0
:   cpx #240
    beq :+ 
        lda #0
        sta board,x
        sta intersections,x
        inx
    jmp :-
    ; place DIFFICULTY number of mines
:   ldy #0
    ldx #0
    lda #0
    sta TEMP
    lda difficulty
:       cmp #0        
        beq :+++ 
        pha    
:       LDX_RANDOM
        cpx size 
        bcs :-
        lda board, X
        and #MINE_PLACED
        cmp #MINE_PLACED 
        beq :-
        txa 
        sec 
:       sbc width
        bcs :-
        adc width
        tay
        jsr place_mine
        pla
        clc ; decrement loop counter (in a)
        sbc #0
        jmp :---
:  rts

.ENDPROC

.PROC place_mine            
    lda board,X
    ora #MINE_PLACED
    sta board,X
    cpx width
    bcc :+++ ; we have an up
        txa
        sec
        sbc width
        tax
        inc board, X
        cpy #0
        beq :+
            ; we have an up-left
            dex
            inc board, X
            inx
:       cpy width_m1
        bcs :+
            ; we have an up-right
            inx
            inc board, X
            dex
:       txa
        clc
        adc width
        tax
:   cpx size_m1
    bcs :+++ ; we have a down
        txa
        clc
        adc width
        tax
        inc board, x
        cpy #0
        beq :+
            ; we have an up-left
            dex 
            inc board,x
            inx
:       cpy width_m1
        bcs :+
            ; we have an up-right
            inx
            inc board,x
            dex
:       txa
        sec
        sbc width
        tax            
:   cpy #0
    beq :+
        ; we have a left
        dex
        inc board, x
        inx
:   cpy width_m1
    bcs :+
        ; we have an right
        inx
        inc board, x
        dex
:   rts 
.ENDPROC

.PROC cursor_up 
    lda cursor_row
    cmp #0
    bne :+
        rts
:   sec 
    sbc #1
    sta cursor_row
    lda cursor
    sec
    sbc width
    sta cursor
    rts
.ENDPROC

.PROC cursor_left
    lda cursor_col
    cmp #0
    bne :+
        rts
:   dec cursor_col
    dec cursor
    rts
.ENDPROC

.PROC cursor_down
    lda cursor_row
    clc
    adc #1
    cmp height
    bne :+ 
        rts
:   sta cursor_row
    lda cursor
    clc
    adc width
    sta cursor
    rts 
.ENDPROC 

.PROC cursor_right
    lda cursor_col
    clc
    adc #1
    cmp width
    bne :+ 
        rts
:   sta cursor_col
    inc cursor
    rts 
.ENDPROC 

.PROC flag    
    ldx cursor
    ldy cursor_col
    lda board, X
    and #FLAG_PLACED
    cmp #FLAG_PLACED
    beq @placed
    lda board, X
    and #UNCOVERED
    cmp #UNCOVERED
    beq :+
    lda flags
    cmp #0
    beq :+
    dec flags
    lda board, X
    ora #FLAG_PLACED
    sta board, X 
:   rts
@placed: 
    lda board, X
    and #(NOT_FLAG_PLACED)
    sta board, X
    inc flags
    rts
.ENDPROC

.PROC dig
    ldx cursor
    ldy cursor_col
    jsr uncover_cell
:   ldx cursor
    ldy cursor_col
    jsr flood_fill
    lda TEMP
    cmp #0
    beq :- 
    rts
.ENDPROC

    ; 1 2
    ; 3 4
.PROC uncover_cell
    ; assumes X is the position in the board
    ; and Y is the position in the current row
    lda board, X 
    and #FLAG_PLACED
    cmp #FLAG_PLACED
    beq @ret
    lda board, X 
    and #UNCOVERED 
    cmp #UNCOVERED
    beq @ret
    lda board, X
    ora #UNCOVERED 
    sta board, X
    ; now to fill in the intersections
    cpy #0 
    beq :+
        ; we have a left, so we should update the intersection left of us
        dex 
        lda intersections, X 
        ora #TOP_RIGHT
        sta intersections, X
        inx
:   lda intersections, X 
    ora #TOP_LEFT 
    sta intersections, X 
    
    cpx width
    bcc :++
        ; we have an up, so we should update the intersection above us
        txa 
        sec 
        sbc width 
        tax 
        lda intersections, X
        ora #BOTTOM_LEFT
        sta intersections, X 
    
        cpy #0 
        beq :+
            ; we have an up-left, so we should update that too
            dex 
            lda intersections, X 
            ora #BOTTOM_RIGHT
            sta intersections, X
            inx
:       ;restore x 
        txa 
        clc 
        adc width 
        tax
:   lda #0
    sta TEMP
@ret:
    rts    
.ENDPROC


.PROC flood_fill
    ldx #0
    ldy #0
    lda #$FF
    sta TEMP
    ; Y stores the position in current row
    ; X stores the current index into the board
@loop:  lda board, X
        cmp #UNCOVERED ; no neighbors, uncovered
        beq :+
        jmp @continue 
            ; an uncovered zero cell
:           cpx width
            bcc @down
                ; we have up
                txa 
                sec
                sbc width
                tax
                jsr uncover_cell
                cpy #0
                beq :+
                    ; we have up-left
                    dex
                    dey
                    jsr uncover_cell
                    iny
                    inx
:               cpy width_m1
                beq :+
                    ; we have up-right
                    inx
                    iny
                    jsr uncover_cell
                    dex
                    dey
:               txa
                clc
                adc width
                tax
@down:      cpx size_m1
            bcs @lr
                ; we have down
                txa 
                clc
                adc width
                tax
                jsr uncover_cell
                cpy #0
                beq :+
                    ; we have down-left
                    dex
                    dey
                    jsr uncover_cell
                    iny
                    inx
:               cpy width_m1
                beq :+
                    ; we have down-right
                    inx
                    iny
                    jsr uncover_cell
                    dex
                    dey
:               txa
                sec
                sbc width
                tax
@lr:        cpy #0
            beq :+
                ;we have left
                dex
                dey 
                jsr uncover_cell
                inx
                iny
:           cpy width_m1
            beq @continue
                ;we have right
                inx
                iny
                jsr uncover_cell
                dex
                dey
@continue:
        iny    
        cpy width;
        bne :+ 
            ; we are at the end of the row
            ldy #0
:       inx
        cpx size
        bcs :+
        jmp @loop
:       rts
.ENDPROC 

.PROC reveal_zeros
    ldx #0
    ldy #0
    lda #$FF
    sta TEMP
    ; Y stores the position in current row
    ; X stores the current index into the board
@loop:  lda board, X
        and #(NEIGHBOURS | MINE_PLACED)
        cmp #0
        bne :+
            jsr uncover_cell
:       iny    
        cpy width;
        bne :+ 
            ; we are at the end of the row
            ldy #0
:       inx
        cpx size
        bcs :+
        jmp @loop
:   rts
.ENDPROC 

.PROC reveal_mines
    ldx #0
    ldy #0
    lda #$FF
    sta TEMP
    ; Y stores the position in current row
    ; X stores the current index into the board
@loop:  lda board, X
        and #MINE_PLACED
        cmp #MINE_PLACED; 
        bne :+
            jsr uncover_cell
:       iny    
        cpy width;
        bne :+ 
            ; we are at the end of the row
            ldy #0
:       inx
        cpx size
        bcs :+
        jmp @loop
:   rts
.ENDPROC 

.PROC has_lost
    ldx #0
    ldy #0
:   cpx size
    beq :++
        lda board, X
        and #UNCOVERED
        cmp #0 
        beq :+
            lda board, X
            and #MINE_PLACED
            cmp #0
            beq :+ 
                ldy #1
                jmp :++
:       inx
        jmp :--
:   tya
    rts
.ENDPROC
.PROC game_over
    ldx #0
    ldy #1
:   cpx size
    beq :++
        lda board, X
        and #UNCOVERED
        cmp #0
        bne :+
            lda board, X
            and #MINE_PLACED
            cmp #0
            bne :+ 
                ldy #0
                jmp :++
:       inx
        jmp :--
:   tya
    rts
.ENDPROC
