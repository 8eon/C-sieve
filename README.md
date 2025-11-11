# C Sieve of Eratosthenes

A high-performance implementation of the Sieve of Eratosthenes algorithm in C, optimized for speed with no `malloc` usage.

## Project Overview

**Goal:** Implement the Sieve of Eratosthenes with maximum performance through incremental optimization.

The Sieve of Eratosthenes is an efficient algorithm to find all prime numbers up to a given limit by iteratively marking multiples of each prime as composite.

## Features

- ✅ **No malloc** - Pure stack allocation using VLA
- ✅ **Odd-only optimization** - Only stores odd numbers (2x memory reduction)
- ✅ **Bit array** - Packs 8 odd numbers per byte (16x total memory reduction)
- ✅ **Optimized algorithm** - Sieves up to √n, starts marking from p²
- ✅ **Fast** - Aggressive compiler optimizations with `-O3 -march=native -flto`
- ✅ **Large range support** - Handles n up to ~128M on 8MB stack

## Usage

### Build

```bash
make
```

### Run

```bash
./sieve <limit> [output_file]
```

**Examples:**
```bash
# Find all primes up to 1 million
./sieve 1000000

# Find primes and save to file
./sieve 1000000 primes.txt
```

### Output

The program prints:
- Number of primes found
- Execution time
- Optional: writes all primes to file (one per line)

## Project Structure

**Files:**
- `sieve.h` - Header file with function declarations
- `sieve.c` - Core sieve algorithm implementation
- `main.c` - CLI interface and timing
- `Makefile` - Build configuration with optimization flags
- `PLANNING.md` - Detailed optimization roadmap and decisions

## Performance

Current optimizations provide:
- **16x memory reduction** vs baseline (2x odd-only × 8x bit packing)
- **~2.4x speed improvement** from odd-only optimization
- **Better cache utilization** from bit array (more data fits in L1/L2)

**Test results (all correct):**
- n = 100: 25 primes ✓
- n = 1,000: 168 primes ✓
- n = 10,000: 1,229 primes ✓
- n = 100,000: 9,592 primes ✓

**Benchmarks:**
- n = 100,000: ~0.000068 seconds
- Stack capacity: ~128M range on 8MB stack

## Implementation Details

### Memory Strategy

- **Stack allocation** - Uses VLA (Variable Length Array) for dynamic sizing
- **No malloc policy** - All memory on stack, automatic cleanup
- **Bit packing** - Each byte stores 8 odd numbers using bit manipulation
- **Odd-only** - Skips even numbers (except 2), reduces array size by half

### Algorithm Optimizations

1. Only check up to √n (numbers larger than √n can't have factors)
2. Start marking multiples from p² (smaller multiples already marked)
3. Skip even numbers entirely (only 2 is even and prime)
4. Use bit operations for compact storage

## Future Optimizations

See `PLANNING.md` for detailed optimization roadmap:
- Segmented sieve (for n > 10M)
- Wheel factorization (mod 6 or mod 30)
- Output optimizations
- Potential SIMD vectorization

## Requirements

- C compiler with C11 support (gcc/clang)
- POSIX system (macOS, Linux)
- Stack size: default 8MB sufficient for n ≤ 128M

