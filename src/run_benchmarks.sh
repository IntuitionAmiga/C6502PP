#!/bin/bash

# Configuration
CC="g++"
BENCH_SECONDS="${BENCH_SECONDS:-30}"

# Benchmark metadata: bin_file instr_per_op
benchmarks=(
    "data/alu_bench.bin 2306"
    "data/memory_bench.bin 1025"
    "data/call_bench.bin 1281"
    "data/branch_bench.bin 1537"
    "data/mixed_bench.bin 2050"
)

# Prepare result storage
results_file=$(mktemp)

mapfile -t benchmark_configs < <(make -s print_benchmark_configs)

interpreter_names=()
declare -A config_flags=()
declare -A config_ldflags=()

for config in "${benchmark_configs[@]}"; do
    IFS='|' read -r name flags ldflags <<< "$config"
    interpreter_names+=("$name")
    config_flags["$name"]="$flags"
    config_ldflags["$name"]="$ldflags"
done

# Run Benchmarks
for name in "${interpreter_names[@]}"; do
    flags=${config_flags[$name]}
    ldflags=${config_ldflags[$name]}
    echo "=========================================================="
    echo " Compiling and Running Interpreter: $name"
    echo " Flags: $flags"
    echo "=========================================================="

    make -s build_bench BIN_NAME="bench_$name" FLAGS="$flags" LDFLAGS="$ldflags"

    if [ $? -ne 0 ]; then
        echo "Compilation of $name failed."
        continue
    fi

    # Run each benchmark binary
    for b in "${benchmarks[@]}"; do
        set -- $b
        bin=$1
        instrs=$2

        if [ -f "$bin" ]; then
            echo "Running $bin..."
            output=$(timeout $((BENCH_SECONDS + 5))s "./bench_$name" "$bin" "$instrs" "$BENCH_SECONDS")
            mips=$(echo "$output" | grep -oP '\[\K[0-9.]+(?= MIPS\])')
            echo "$name,$bin,${mips:-0.0}" >> "$results_file"
        fi
    done
done

# Print final table
# ... (rest of the script remains same)


# Print final table
echo ""
echo "=============================================================================================="
echo "                           6502 EMULATOR BENCHMARK RESULTS"
echo "=============================================================================================="
printf "%-18s | %-12s | %-12s | %-12s | %-12s | %-12s\n" "Interpreter" "ALU" "Memory" "Call" "Branch" "Mixed"
echo "-------------------|--------------|--------------|--------------|--------------|--------------"

for name in "${interpreter_names[@]}"; do
    alu=$(grep "^$name,data/alu_bench.bin," "$results_file" | cut -d',' -f3)
    alu=${alu:-0.0}
    mem=$(grep "^$name,data/memory_bench.bin," "$results_file" | cut -d',' -f3)
    mem=${mem:-0.0}
    cal=$(grep "^$name,data/call_bench.bin," "$results_file" | cut -d',' -f3)
    cal=${cal:-0.0}
    bra=$(grep "^$name,data/branch_bench.bin," "$results_file" | cut -d',' -f3)
    bra=${bra:-0.0}
    mix=$(grep "^$name,data/mixed_bench.bin," "$results_file" | cut -d',' -f3)
    mix=${mix:-0.0}
    
    printf "%-18s | %12.2f | %12.2f | %12.2f | %12.2f | %12.2f\n" "$name" "$alu" "$mem" "$cal" "$bra" "$mix"
done
echo "=============================================================================================="

rm "$results_file"
rm -f bench_Runtime bench_StaticSC bench_StaticSCPin bench_StaticMaxGoto bench_StaticMaxGotoLTO
