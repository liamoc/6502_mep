.ifndef __BBC__
SEED0 = $E0;
SEED1 = $E1;
SEED2 = $E2
SEED3 = $E3
TMP = $52
MOD = TMP+4
.else 
SEED0 = $A0;
SEED1 = $A1;
SEED2 = $A2
SEED3 = $A3
TMP = $A4
MOD = TMP+4
.endif



.proc RAND
         LDA SEED0 ; TMP = SEED * 2
         ASL
         STA TMP
         LDA SEED1
         ROL
         STA TMP+1
         LDA SEED2
         ROL
         STA TMP+2
         LDA SEED3
         ROL
         STA TMP+3
         CLC       ; TMP = TMP + SEED (= SEED * 3)
         LDA SEED0
         ADC TMP
         STA TMP
         LDA SEED1
         ADC TMP+1
         STA TMP+1
         LDA SEED2
         ADC TMP+2
         STA TMP+2
         LDA SEED3
         ADC TMP+3
         STA TMP+3
         CLC       ; SEED = SEED + $10000 * SEED
         LDA SEED2
         ADC SEED0
         TAX       ; keep byte 2 in X for now (for speed)
         LDA SEED3
         ADC SEED1
         TAY       ; keep byte 3 in Y for now
         CLC       ; SEED = SEED + $100 * SEED
         LDA SEED1
         ADC SEED0
         PHA       ; push byte 1 onto stack
         TXA
         ADC SEED1
         TAX
         TYA
         ADC SEED2
         TAY
         LDA TMP   ; TMP = TMP * 4 (= old seed * $0C)
         ASL
         ROL TMP+1
         ROL TMP+2
         ROL TMP+3
         ASL
         ROL TMP+1
         ROL TMP+2
         ROL TMP+3
         STA TMP
         CLC       ; SEED = SEED + TMP
         ADC SEED0
         STA SEED0
         PLA       ; pull byte 1 from stack
         ADC TMP+1
         STA SEED1
         TXA
         ADC TMP+2
         TAX
         TYA
         ADC TMP+3
         TAY
         CLC
         LDA TMP   ; SEED = SEED + TMP * $100
         ADC SEED1
         STA SEED1
         TXA
         ADC TMP+1
         TAX
         TYA
         ADC TMP+2
         TAY
         LDA TMP   ; TMP = TMP * $10 (= old seed * $C0)
         ASL       ; put byte 0 of TMP in the accumulator
         ROL TMP+1
         ROL TMP+2
         ROL TMP+3
         ASL
         ROL TMP+1
         ROL TMP+2
         ROL TMP+3
         ASL
         ROL TMP+1
         ROL TMP+2
         ROL TMP+3
         ASL
         ROL TMP+1
         ROL TMP+2
         ROL TMP+3
         SEC       ; SEED = SEED + TMP + 1
         ADC SEED0
         STA SEED0
         LDA TMP+1
         ADC SEED1
         STA SEED1
         TXA
         ADC TMP+2
         STA SEED2
         TYA
         ADC TMP+3
         STA SEED3
         RTS
.endproc 

.proc RANDOM8  
         STA MOD    ; store modulus in MOD
         JSR RAND   ; get next seed
         LDA #0     ; multiply SEED by MOD
         STA TMP+2
         STA TMP+1
         STA TMP
         SEC
         ROR MOD    ; shift out modulus, shifting in a 1 (will loop 8 times)
R8A:     BCC R8B    ; branch if a zero was shifted out
         CLC        ; add SEED, keep upper 8 bits of product in accumulator
         TAX
         LDA TMP
         ADC SEED0
         STA TMP
         LDA TMP+1
         ADC SEED1
         STA TMP+1
         LDA TMP+2
         ADC SEED2
         STA TMP+2
         TXA
         ADC SEED3
R8B:     ROR        ; shift product right
         ROR TMP+2
         ROR TMP+1
         ROR TMP
         LSR MOD    ; loop until all 8 bits of MOD have been shifted out
         BNE R8A
         RTS
.ENDPROC