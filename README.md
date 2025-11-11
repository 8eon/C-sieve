# C Sieve of Eratosthenes

A **blazingly fast** implementation of the Sieve of Eratosthenes in C, capable of finding **50+ million primes up to 1 billion in under 1 second** with **zero malloc**.

## 🚀 Performance Highlights

```
n = 1,000,000,000    →  50,847,534 primes in 872ms  ⚡
n = 100,000,000      →   5,761,455 primes in  85ms  🔥  
n = 10,000,000       →     664,579 primes in 8.5ms  ⭐
n = 100,000          →       9,592 primes in  72μs  ✨
```

**All on the stack. No malloc. Clean code.**

## Features

- ✅ **Segmented Sieve** - Cache-friendly 256KB segments for massive ranges
- ✅ **Dual-path Architecture** - Optimized simple sieve (< 10M) + segmented (≥ 10M)
- ✅ **No malloc** - Pure stack allocation using VLA
- ✅ **Odd-only + Bit array** - 16x memory reduction vs baseline
- ✅ **High-resolution timing** - Microsecond precision with `clock_gettime()`
- ✅ **Optimized algorithm** - Sieves up to √n, starts marking from p²
- ✅ **Aggressive compilation** - `-O3 -march=native -flto`

## Quick Start

### Build

```bash
make
```

### Run

```bash
./sieve <limit> [output_file]
```

### Examples

```bash
# Find all primes up to 1 billion
./sieve 1000000000

# Save primes to file
./sieve 1000000 primes.txt

# Small test
./sieve 100
```

### Output

```
Primes found: 50847534
Time elapsed: 872.340 milliseconds
```

## Architecture

### Two-Path Design

**Small n (< 10M):** Simple sieve with odd-only + bit array
- Fast for small ranges
- Optimal memory usage
- Single-pass algorithm

**Large n (≥ 10M):** Segmented sieve
- Cache-friendly 256KB segments
- Constant memory usage (no matter how large n is!)
- Scales to billions

### Implementation Details

**Phase 1: Base Primes**
- Find all primes up to √n using simple sieve
- Store on stack (√1B ≈ 31,622 → ~3,400 primes → ~30KB)

**Phase 2: Segmented Processing**
- Process 256KB segments sequentially
- Each segment fits in L2 cache
- Use base primes to mark composites
- Output primes on the fly

### Memory Strategy

- **VLA (Variable Length Arrays)** - Dynamic sizing on stack
- **No malloc policy** - Zero heap allocation
- **Bit packing** - 8 odd numbers per byte
- **Odd-only** - Skip all even numbers (except 2)
- **Result:** 16x memory reduction vs naive approach

### Algorithm Optimizations

1. **Odd-only storage** - Only track odd numbers (2x reduction)
2. **Bit array** - Pack 8 values per byte (8x reduction)
3. **Start from p²** - Skip already-marked composites
4. **Check up to √n** - Mathematical optimization
5. **Segmented processing** - Cache-friendly for large n

## Project Structure

```
├── sieve.h        - Function declarations
├── sieve.c        - Core implementation (simple + segmented)
├── main.c         - CLI interface with timing
├── Makefile       - Build configuration
├── PLANNING.md    - Detailed optimization notes
└── README.md      - This file
```

## Branch Structure

This repository contains multiple optimization approaches:

### **`main`** (Recommended) 
Current production version with segmented sieve
- Best performance for all ranges
- Handles up to billions
- Clean dual-path architecture

### **`wheel-factorization`**
Experimental wheel factorization (mod 6) optimization
- 24x memory reduction vs baseline
- Slightly slower for small n
- Interesting alternative approach

### **`backup`**
Original bit array implementation
- Simple odd-only + bit array
- No segmentation
- Good reference implementation

### **`before-segmented`**
Checkpoint before segmented sieve
- Simple sieve only
- Clean starting point

## Benchmarks

All tests run on 8-core system with `-O3 -march=native -flto`:

| n | Primes Found | Time | Throughput |
|---|--------------|------|------------|
| 100 | 25 | 0 μs | - |
| 1,000 | 168 | 1 μs | - |
| 100,000 | 9,592 | 72 μs | 1.4B/s |
| 10,000,000 | 664,579 | 8.5 ms | 1.2B/s |
| 100,000,000 | 5,761,455 | 85 ms | 1.2B/s |
| 1,000,000,000 | 50,847,534 | 872 ms | 1.1B/s |

**Throughput** = numbers processed per second

## Correctness Verification

Known prime counts used for testing:
- π(100) = 25
- π(1,000) = 168
- π(10,000) = 1,229
- π(100,000) = 9,592
- π(1,000,000) = 78,498
- π(10,000,000) = 664,579
- π(100,000,000) = 5,761,455
- π(1,000,000,000) = 50,847,534

All verified ✓

## Requirements

- **Compiler:** gcc or clang with C11 support
- **System:** POSIX (macOS, Linux)
- **Stack:** Default 8MB sufficient
  - For n > 1B, may need: `ulimit -s unlimited`
- **Math library:** `-lm` for sqrt()

## Compilation Flags

```makefile
CFLAGS = -Wall -Wextra -std=c11 -O3 -march=native -flto
```

- `-O3` - Maximum optimization
- `-march=native` - Use CPU-specific instructions
- `-flto` - Link-time optimization

## Future Optimizations

Potential improvements (see `PLANNING.md`):
- Wheel factorization (mod 30)
- Multi-threading (parallel segments)
- SIMD vectorization
- Better output buffering
- Primality testing integration

## Development Philosophy

This project follows strict principles:
- ✅ **No band-aid fixes** - Clean solutions only
- ✅ **Incremental optimization** - Test each change
- ✅ **Git branching** - Preserve all approaches
- ✅ **No malloc** - Stack allocation policy
- ✅ **Maintainability** - Code clarity matters

## License

Open source - use freely!

---

**Built with care, optimized for speed, maintained with git.** 🚀
