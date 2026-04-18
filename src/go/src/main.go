package main

import (
    "fmt"
    "os"
    "strconv"
    "time"
)

#include "cpu.gh"

// Batch size for the deadline check. Each RunFrom call takes a few microseconds;
// checking time.Now() per call would add noticeable syscall overhead at 1M+ ops/sec.
// At BATCH=256 the deadline is checked ~every 1-3ms of work, keeping overshoot
// under 0.1% of a multi-second bench window.
const BATCH = 256

func main() {
    if len(os.Args) < 4 {
        fmt.Fprintf(os.Stderr, "Usage: %s <bin_file> <instr_per_op> <seconds>\n", os.Args[0])
        os.Exit(1)
    }
    binFile := os.Args[1]
    instrPerOp, err := strconv.ParseUint(os.Args[2], 10, 64)
    if err != nil || instrPerOp == 0 {
        fmt.Fprintf(os.Stderr, "bad instr_per_op: %v\n", os.Args[2])
        os.Exit(1)
    }
    seconds, err := strconv.ParseInt(os.Args[3], 10, 64)
    if err != nil || seconds <= 0 {
        fmt.Fprintf(os.Stderr, "bad seconds: %v (must be > 0)\n", os.Args[3])
        os.Exit(1)
    }

    payload, err := os.ReadFile(binFile)
    if err != nil {
        fmt.Fprintf(os.Stderr, "Failed to load binary %s: %v\n", binFile, err)
        os.Exit(1)
    }

    const LOAD_ADDR Address = 0x0600

    mem := new(AddressSpace)
    copy(mem[LOAD_ADDR:], payload)

    cpu := &MOS6502{}
    cpu.Init(mem)

    var totalOps uint64
    tStart := time.Now()
    deadline := tStart.Add(time.Duration(seconds) * time.Second)
    for time.Now().Before(deadline) {
        for i := 0; i < BATCH; i++ {
            cpu.RunFrom(LOAD_ADDR)
        }
        totalOps += BATCH
    }
    tElapsed := time.Since(tStart)
    iNanoSeconds := uint64(tElapsed.Nanoseconds())
    totalInstrs := totalOps * instrPerOp
    fMIPS := 1.0e3 * float64(totalInstrs) / float64(iNanoSeconds)

    fmt.Printf("%s: Ran %d instructions in %f seconds [%f MIPS] (%d ops)\n",
        binFile, totalInstrs, float64(iNanoSeconds)/1e9, fMIPS, totalOps)
}
