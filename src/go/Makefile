# Build two variants of the G6502PP (cpp -> Go) interpreter:
#   bin/G65O2PP_nopin  (X/Y/S accessed through cpu struct, no register pinning)
#   bin/G65O2PP_pin    (X/Y/S pinned to locals via -DPIN_ALL)
#
# Each variant is preprocessed separately (so cpp sees the right macro set)
# then compiled with the same aggressive Go flags.

CC      := gcc
GO      := go

# -s/-w: strip symbol + DWARF info
# -B: disable bounds checks, -l: disable inlining budget limits (encourage inline)
GO_LDFLAGS := -ldflags=-s -w
GO_GCFLAGS := -gcflags=all=-B -l

ENTRY    := src/main.go
GH_DEPS  := $(wildcard src/*.gh)

VARIANTS := nopin pin
BINS     := $(VARIANTS:%=bin/G65O2PP_%)

# Per-variant cpp flag. Extend here for new variants.
CPPFLAGS_pin   := -DPIN_ALL
CPPFLAGS_nopin :=

.PHONY: all clean $(VARIANTS)

all: $(BINS)

$(VARIANTS): %: bin/G65O2PP_%

bin/G65O2PP_%: build/%/processed.go
	@mkdir -p $(@D)
	$(GO) build "$(GO_LDFLAGS)" "$(GO_GCFLAGS)" -trimpath -o $@ $<

# -P: no linemarkers, -xc: C mode, -undef: no built-in system macros.
build/%/processed.go: $(ENTRY) $(GH_DEPS)
	@mkdir -p $(@D)
	$(CC) -E -P -xc -undef $(CPPFLAGS_$*) $(ENTRY) -o $@
	$(GO) fmt $@ >/dev/null

clean:
	rm -rf build bin
