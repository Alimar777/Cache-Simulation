# Cache Lab (CS:APP) - Portfolio Version

This repository contains my implementation of the CS:APP Cache Lab: a cache
simulator (`src/csim.c`) and a cache-optimized matrix transpose (`src/trans.c`).
The focus is correctness, clear structure, and reproducible evaluation.

## What this project does
- Simulates a set-associative cache with LRU eviction and reports hit/miss/evict
  counts for memory traces.
- Implements specialized matrix transpose kernels to minimize cache misses on
  specific matrix sizes (32x32, 64x64, 61x67).

## Key learnings
- Built a working LRU cache simulator from trace files and validated it against a reference.
- Tuned block-based transpose strategies to reduce cache conflict misses.
- Measured and reported performance results using the provided tooling.

## Repository layout
- `src/`
  - `csim.c` - cache simulator implementation
  - `trans.c` - optimized transpose implementation
  - `cachelab.c`, `cachelab.h` - provided helpers
  - `test-trans.c`, `tracegen.c` - test harness/tool sources
- `traces/` - trace files used by the simulator tests
- `Makefile`, `driver.py` - build and driver scripts
- `test-csim`, `csim-ref` - course-provided binaries needed by `driver.py`
- `Cache Simulation Report.pdf` - my report for the project
- `RESULTS.md` - miss counts summary from the report

## Build and run
Requirements:
- C compiler with C99 support (gcc/clang recommended)
- `make`
- Python 3 (for `driver.py`)

From the repo root:

```bash
make
./test-csim
./test-trans -M 32 -N 32
./test-trans -M 64 -N 64
./test-trans -M 61 -N 67
./driver.py            # runs simulator + transpose tests
```

Note: `test-csim` and `csim-ref` are Linux binaries. WSL can run them, but
`valgrind` can be slow or time out on some WSL setups. For the most
reproducible results, use a native Linux environment.

## Troubleshooting (WSL)
- If `test-trans` times out under valgrind, move the repo into the WSL
  filesystem (e.g., `~/project-3`) instead of `/mnt/c/...` and rerun.
- Install dependencies with `sudo apt install build-essential valgrind`.

## Design notes
### Cache simulator (`src/csim.c`)
The simulator parses trace files and tracks cache lines with valid/tag bits and
an LRU counter. Each access updates the per-line counters to approximate least
recently used eviction for each set.

### Transpose (`src/trans.c`)
The optimized transpose uses block-based strategies to reduce conflict misses:
- 32x32 and 61x67 use tuned block sizes to improve spatial locality.
- 64x64 uses a staged block strategy to avoid conflict misses in direct-mapped
  cache behavior.

## Notes on provenance
This project is based on the CS:APP Cache Lab handout. 

## Suggested exploration
- Read the cache simulator logic in `src/csim.c` to see how tags,
  sets, and LRU counters are derived.
- Compare the transpose strategies in `src/trans.c` across matrix
  sizes and consider alternative block sizes.
