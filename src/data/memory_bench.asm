; (c) 2026 Zayn Otley https://github.com/IntuitionAmiga

; Memory-access microbenchmark ported from the Golang 6502 benchmark bytes.
; This loop is intended to stress indexed zero-page loads and stores with very
; little arithmetic overhead. It copies one full 256-byte span from the source
; window starting at $0010 to the destination window starting at $0080.
;
; Because zero-page indexed addressing wraps modulo 256 on the 6502, the source
; and destination effective addresses naturally stay in zero page for all X.

$0600  A2 00     LDX #$00      ; Start at offset 0 within the source/destination
                               ; zero-page windows.
$0602  B5 10     LDA $10,X     ; Read from zero page address ($10 + X) & $FF.
                               ; This exercises indexed address calculation plus
                               ; the memory read path.
$0604  95 80     STA $80,X     ; Write the byte to zero page address ($80 + X)
                               ; & $FF, stressing the mirrored indexed store path.
$0606  E8        INX           ; Advance to the next byte offset.
$0607  D0 F9     BNE $0602     ; Continue until X wraps from $FF back to $00,
                               ; for exactly 256 load/store pairs.
$0609  02        JAM           ; End-of-benchmark sentinel.
