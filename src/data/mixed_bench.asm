; (c) 2026 Zayn Otley https://github.com/IntuitionAmiga

; Mixed-workload microbenchmark ported from the Golang 6502 benchmark bytes.
; This combines indexed memory reads/writes, arithmetic, stack traffic, and a
; simple register-controlled loop so the emulator executes a more representative
; instruction mix than the specialized microbenchmarks.
;
; The accumulator is preserved across the store path by pushing it to the stack,
; transforming a copy, storing that transformed value, then restoring the
; original running sum. That makes the stack an intentional part of the mix.
;
; This byte stream does not explicitly clear carry before ADC. In the original
; exported benchmark design status is cleared before entry, so the addition
; chain starts with carry clear, but that precondition is external to the bytes.

$0600  A2 00     LDX #$00      ; Indexed loop variable covering a full 256-byte
                               ; working set.
$0602  A9 00     LDA #$00      ; Initialize the running accumulator state before
                               ; entering the hot loop.
$0604  75 10     ADC $10,X     ; Add a byte from zero page source window
                               ; ($10 + X) & $FF, mixing ALU work with a memory read.
$0606  48        PHA           ; Save the running sum so the next operation can
                               ; derive a transformed output value without losing
                               ; the original accumulator state.
$0607  49 AA     EOR #$AA      ; Create an output pattern by XORing with a fixed
                               ; mask. This adds a second ALU op with a distinct
                               ; data dependency.
$0609  95 80     STA $80,X     ; Store the transformed value into the destination
                               ; zero-page window.
$060B  68        PLA           ; Restore the pre-XOR running sum so the next ADC
                               ; continues the accumulation chain.
$060C  E8        INX           ; Advance to the next element.
$060D  D0 F5     BNE $0604     ; Repeat for all 256 offsets until X wraps to zero.
$060F  02        JAM           ; End marker for the benchmark harness.
