; (c) 2026 Zayn Otley https://github.com/IntuitionAmiga

; ALU-focused microbenchmark ported from the Golang 6502 benchmark bytes.
; The goal here is to keep execution almost entirely inside the accumulator
; datapath so the workload measures arithmetic/logic instruction throughput
; more than memory bandwidth.
;
; X starts at $00 and is immediately decremented at the end of each iteration.
; Because 6502 registers wrap modulo 256, the first DEX turns X into $FF and
; the loop then runs a full 256 iterations before finally reaching X == $00,
; where BNE falls through to the terminating JAM.
;
; This byte stream does not initialize carry before ADC. In the original
; exported benchmark design, status is cleared before entry so ADC begins with
; carry clear, but that assumption comes from the harness rather than the bytes.

$0600  A9 07     LDA #$07      ; Seed A with a non-trivial bit pattern so the
                               ; following ALU ops exercise carry/zero/sign
                               ; flag updates on changing data.
$0602  A2 00     LDX #$00      ; Loop counter. Starting at zero intentionally
                               ; produces a 256-iteration wraparound loop.
$0604  69 03     ADC #$03      ; Add an immediate constant, touching the main
                               ; adder and carry propagation logic each pass.
$0606  29 7F     AND #$7F      ; Clear the top bit so the next transforms do
                               ; not saturate into a permanently negative value.
$0608  09 01     ORA #$01      ; Force bit 0 high, ensuring another flag-setting
                               ; logic operation with a predictable mask.
$060A  49 55     EOR #$55      ; Flip alternating bits to keep the accumulator
                               ; state moving rather than converging to a fixed
                               ; small cycle too quickly.
$060C  0A        ASL A         ; Shift left through carry, stressing bit motion
                               ; and carry generation in the ALU.
$060D  4A        LSR A         ; Shift right again, exercising the opposite
                               ; direction and another carry update.
$060E  2A        ROL A         ; Rotate left through carry so the carry flag
                               ; participates in the accumulator dataflow.
$060F  CA        DEX           ; Count down one iteration. The first decrement
                               ; changes $00 -> $FF, which is why the loop runs
                               ; a full byte range.
$0610  D0 F2     BNE $0604     ; Stay in the ALU hot loop until X wraps back to
                               ; zero after 256 total passes.
$0612  02        JAM           ; Illegal stop opcode used by the harness as an
                               ; unmistakable end-of-benchmark marker.
