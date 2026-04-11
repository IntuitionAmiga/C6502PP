; (c) 2026 Zayn Otley https://github.com/IntuitionAmiga

; Subroutine-call microbenchmark ported from the Golang 6502 benchmark bytes.
; This benchmark isolates JSR/RTS overhead, including stack traffic and return
; address handling, by making the called routine intentionally tiny.
;
; As in several other benchmarks, X begins at $00 so that the first DEX wraps
; to $FF and the main loop executes 256 times before the terminating fallthrough.

$0600  A2 00     LDX #$00      ; Byte-sized loop counter for a fixed 256-call run.
$0602  20 10 06  JSR $0610     ; Call the tiny subroutine. This pushes a return
                               ; address onto the stack and transfers control.
$0605  CA        DEX           ; Count the call just completed.
$0606  D0 FA     BNE $0602     ; Repeat until X wraps back to zero.
$0608  02        JAM           ; Harness-visible stop once all calls are done.

; Padding between the main loop and the subroutine entry point.
$0609  00        BRK           ; Unused filler to place the subroutine at $0610,
$060A  00        BRK           ; which makes the target address visually obvious
$060B  00        BRK           ; in the JSR encoding above and keeps the listing
$060C  00        BRK           ; easy to read during debugging.
$060D  00        BRK
$060E  00        BRK
$060F  00        BRK

$0610  C8        INY           ; Minimal routine body: mutate one register so the
                               ; call does some real architectural work while
                               ; keeping almost all cost in JSR/RTS mechanics.
$0611  60        RTS           ; Pop the return address and resume the caller.
