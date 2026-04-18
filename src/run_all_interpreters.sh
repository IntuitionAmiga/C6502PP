#!/bin/bash
#
# run_all_interpreters.sh - Benchmark every 6502 interpreter variant in this
# tree against the same five workloads and print a single MIPS table.
#
# Rows covered:
#   1. Eight C++ interpreters (BENCHMARK_TARGETS from benchmark_matrix.mk).
#   2. Two cpp->Go interpreters from src/go/ (nopin, pin).
#   3. Two rows from the prebuilt Go testing.B binary src/6502_bench.test
#      (Interpreter, JIT) when present.
#
# All rows are normalized to the same instr_per_op counts so every cell is an
# apples-to-apples MIPS figure.
#
# Run from src/:
#   ./run_all_interpreters.sh
#
# Environment:
#   BENCH_SECONDS  wall-clock seconds per C++ / Go-cpp cell (default: 5)
#   BENCH_TIME     -test.benchtime for 6502_bench.test     (default: ${BENCH_SECONDS}s)
#   BENCH_BIN      path to the Go testing.B bench binary   (default: ./6502_bench.test)
#   RAW            if set, also dump the raw 6502_bench.test output

set -eu

BENCH_SECONDS="${BENCH_SECONDS:-5}"
BENCH_TIME="${BENCH_TIME:-${BENCH_SECONDS}s}"
BENCH_BIN="${BENCH_BIN:-./6502_bench.test}"

# Workload name, binary path, instructions executed per one run-to-JAM.
# The instr/op numbers are defined by the .bin content and match the values
# reported by 6502_bench.test's Interpreter/JIT benchmarks.
workloads=(
    "ALU    data/alu_bench.bin    2306"
    "Memory data/memory_bench.bin 1025"
    "Call   data/call_bench.bin   1281"
    "Branch data/branch_bench.bin 1537"
    "Mixed  data/mixed_bench.bin  2050"
)

results_file=$(mktemp)
cleanup() { rm -f "$results_file"; }
trap cleanup EXIT

run_one() {
    local label=$1 bin=$2 wname=$3 wbin=$4 winstr=$5
    local out mips
    if ! out=$(timeout $((BENCH_SECONDS + 5))s "$bin" "$wbin" "$winstr" "$BENCH_SECONDS" 2>&1); then
        echo "  ! $label failed on $wbin: $out" >&2
        printf '%s,%s,0.0\n' "$label" "$wname" >> "$results_file"
        return
    fi
    mips=$(printf '%s\n' "$out" | grep -oP '\[\K[0-9.]+(?= MIPS\])')
    printf '%s,%s,%s\n' "$label" "$wname" "${mips:-0.0}" >> "$results_file"
}

# --- Part 1: 8 C++ bench harnesses -------------------------------------------
echo "==> Building + running C++ interpreter matrix"
mapfile -t configs < <(make -s print_benchmark_configs)
cpp_names=()
declare -A cfg_flags cfg_ldflags
for c in "${configs[@]}"; do
    IFS='|' read -r n f l <<< "$c"
    cpp_names+=("$n")
    cfg_flags[$n]=$f
    cfg_ldflags[$n]=$l
done

for n in "${cpp_names[@]}"; do
    echo "  [C++] $n"
    if ! make -s build_bench BIN_NAME="bench_$n" FLAGS="${cfg_flags[$n]}" LDFLAGS="${cfg_ldflags[$n]}" >/dev/null 2>&1; then
        echo "  ! compile failed: $n" >&2
        continue
    fi
    for w in "${workloads[@]}"; do
        # shellcheck disable=SC2086
        set -- $w
        run_one "$n" "./bench_$n" "$1" "$2" "$3"
    done
done

# --- Part 2: cpp->Go variants (optional; skipped if Go is absent) ------------
gocpp_available=0
if command -v go >/dev/null 2>&1; then
    echo "==> Building + running cpp->Go interpreter variants"
    if make -s -C go >/dev/null 2>&1; then
        gocpp_available=1
    else
        echo "  ! cpp->Go build failed; skipping CppGo rows" >&2
    fi
else
    echo "==> Go toolchain not found; skipping cpp->Go interpreter variants"
fi

if [ "$gocpp_available" -eq 1 ]; then
    for variant in nopin pin; do
        bin="go/bin/G65O2PP_$variant"
        label="CppGo_$variant"
        if [ ! -x "$bin" ]; then
            echo "  ! missing $bin" >&2
            continue
        fi
        echo "  [Go/cpp] $label"
        for w in "${workloads[@]}"; do
            # shellcheck disable=SC2086
            set -- $w
            run_one "$label" "$bin" "$1" "$2" "$3"
        done
    done
fi

# --- Part 3: prebuilt Go testing.B bench binary -------------------------------
gobench_ran=0
if [ -x "$BENCH_BIN" ]; then
    echo "==> Running $BENCH_BIN (Interpreter + JIT rows)"
    if gobench_out=$("$BENCH_BIN" \
            -test.run '^$' \
            -test.bench 'Benchmark6502_(ALU|Memory|Call|Branch|Mixed)_(Interpreter|JIT)' \
            -test.benchtime "$BENCH_TIME" \
            -test.count 1 \
            -test.benchmem 2>&1); then
        gobench_ran=1
        [ -n "${RAW:-}" ] && { echo "--- raw $BENCH_BIN output ---"; echo "$gobench_out"; echo; }
        printf '%s\n' "$gobench_out" | awk -v F="$results_file" '
            /^Benchmark6502_/ {
                name = $1; sub(/-[0-9]+$/, "", name)
                if (!match(name, /_(ALU|Memory|Call|Branch|Mixed)_(Interpreter|JIT)$/)) next
                pair = substr(name, RSTART+1, RLENGTH-1)
                split(pair, p, "_")
                ns = 0; ip = 0
                for (i = 2; i <= NF; i++) {
                    if ($i == "ns/op"           && i > 2) ns = $(i-1) + 0
                    if ($i == "instructions/op" && i > 2) ip = $(i-1) + 0
                }
                if (ns > 0 && ip > 0) {
                    mips = ip / ns * 1000.0
                    printf "IntuitionEngine_%s,%s,%.2f\n", p[2], p[1], mips >> F
                }
            }'
    else
        echo "  ! $BENCH_BIN failed; IntuitionEngine rows will be empty" >&2
    fi
else
    echo "==> Skipping $BENCH_BIN (not found or not executable)"
fi

# --- Table -------------------------------------------------------------------
rows=( "${cpp_names[@]}" )
if [ "$gocpp_available" -eq 1 ]; then
    rows+=( "CppGo_nopin" "CppGo_pin" )
fi
if [ "$gobench_ran" -eq 1 ]; then
    rows+=( "IntuitionEngine_Interpreter" "IntuitionEngine_JIT" )
fi
cols=(ALU Memory Call Branch Mixed)

# Load all results into an associative array once; later-written rows win.
declare -A results
while IFS=, read -r r c v; do
    results[$r,$c]=$v
done < "$results_file"

# Sort rows ascending by Mixed-column MIPS so the current fastest is at the bottom.
sorted_rows=()
while IFS=$'\t' read -r _ r; do
    [ -n "$r" ] && sorted_rows+=("$r")
done < <(for r in "${rows[@]}"; do
    printf '%s\t%s\n' "${results[$r,Mixed]:-0}" "$r"
done | sort -k1,1n)

LABEL_W=28
printf -v bar '%93s' ''; bar=${bar// /=}
printf -v dashes "%${LABEL_W}s" ''; dashes=${dashes// /-}
echo
echo "$bar"
printf '%*s\n' 70 "UNIFIED 6502 INTERPRETER BENCHMARK (MIPS)"
printf '%*s\n' 77 "(rows sorted ascending by Mixed column; fastest at bottom)"
echo "$bar"
printf "%-${LABEL_W}s" "Interpreter"
for c in "${cols[@]}"; do printf " | %10s" "$c"; done
printf "\n"
printf '%s' "$dashes"
for c in "${cols[@]}"; do printf '%s' "-+-----------"; done
printf "\n"
for r in "${sorted_rows[@]}"; do
    printf "%-${LABEL_W}s" "$r"
    for c in "${cols[@]}"; do
        printf " | %10.2f" "${results[$r,$c]:-0.0}"
    done
    printf "\n"
done
echo "$bar"

rm -f $(printf 'bench_%s ' "${cpp_names[@]}")
