; (c) 2026 Zayn Otley https://github.com/IntuitionAmiga

; Branch-focused microbenchmark ported from the Golang 6502 benchmark bytes.
; This sequence is built to spend most of its time on flag production and
; conditional control flow. It alternates between taking and not taking BEQ
; based on X parity, while DEY/BNE forms the outer loop.
;
; The byte stream itself does not initialize Y, so the exact trip count depends
; on entry state. In the original exported benchmark design Y is cleared before
; entry, so the first DEY wraps to $FF and the loop executes 256 iterations.

$0600  A2 00     LDX #$00      ; X tracks loop progress and provides a changing
                               ; value whose low bit controls BEQ direction.
$0602  E8        INX           ; Advance X every iteration so parity flips on
                               ; every trip through the loop.
$0603  8A        TXA           ; Copy X into A because the branch condition is
                               ; derived from accumulator flags.
$0604  29 01     AND #$01      ; Isolate bit 0. Result is either 0 or 1, making
                               ; Z flag alternate cleanly between set and clear.
$0606  F0 01     BEQ $0609     ; Branch on even X values. This is taken about
                               ; half the time, giving the emulator both taken
                               ; and not-taken conditional paths to execute.
$0608  EA        NOP           ; Only runs when BEQ is not taken. It pads the
                               ; fall-through path so the branch has meaningful
                               ; alternate control flow rather than a no-op jump.
$0609  88        DEY           ; Outer loop countdown. Y provides the actual
                               ; termination condition for the benchmark.
$060A  D0 F6     BNE $0602     ; Branch back until DEY eventually leaves Y at
                               ; zero. With Y entering as $00, this yields a
                               ; 256-iteration wraparound loop.
$060C  02        JAM           ; Stop marker recognized by the harness.
