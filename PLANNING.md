# Sieve of Eratosthenes - Project Plan

## Table of Contents
1. [Project Overview](#project-overview)
2. [Progress Status](#progress-status) ⬅️ **SEE HERE FOR WHAT'S DONE**
3. [Next Optimization: Odd-Only Sieve](#-next-optimization-odd-only-sieve) ⬅️ **SEE HERE FOR NEXT STEP**
4. [Command-Line Interface](#command-line-interface)
5. [Memory Allocation Strategy](#memory-allocation-strategy-no-malloc-policy)
6. [Baseline Implementation](#baseline-implementation-start-here)
7. [Performance Optimization Roadmap](#performance-optimization-roadmap)
8. [Testing Strategy](#testing-strategy)

---

## Project Overview

**Goal:** Implement the Sieve of Eratosthenes algorithm in C with a focus on maximum performance through incremental optimization.

**The Sieve of Eratosthenes** - An efficient algorithm to find all prime numbers up to a given limit by iteratively marking multiples of each prime as composite.

## Project Structure

**Files:**
- `sieve.h` - Header file with function declarations and constants
- `sieve.c` - Implementation of the sieve algorithm
- `Makefile` - Compilation with optimization flags
- `PLANNING.md` - This file

**Key Components:**
1. Core sieve algorithm function
2. CLI interface with command-line arguments
3. File output support for prime analysis
4. Performance timing instrumentation
5. Error handling (memory allocation, input validation)

## Progress Status

### ✅ Completed: Baseline Implementation

**What we built:**
- `sieve.h`, `sieve.c`, `main.c` - Core implementation
- Simple `uint8_t` array with VLA (stack allocation, no malloc)
- Algorithm optimizations: sieve up to √n, start marking from p²
- CLI with command-line arguments
- Optional file output for prime analysis
- Performance timing with `clock()`
- Makefile with aggressive optimization flags: `-O3 -march=native -flto`

**Test results (all correct):**
- n = 100: 25 primes ✓
- n = 1,000: 168 primes ✓
- n = 10,000: 1,229 primes ✓
- n = 100,000: 9,592 primes ✓

**Performance:** Very fast for small/medium n (< 0.0001s for n=100k)

### ✅ Completed: Odd-Only Optimization

**What changed:**
- Only store odd numbers (array index i = number 2*i+1)
- Handle prime 2 as special case
- Skip even multiples when marking

**Results:**
- Memory: 2x reduction (n/2 array size)
- Speed: **2.4x faster** (0.000079s → 0.000033s for n=100k)
- All tests still pass ✓

---

## Command-Line Interface

```bash
./sieve <limit> [output_file]
```

**Output:**
- Print count of primes found
- Print execution time
- Optionally write all primes to output file for analysis

## Memory Allocation Strategy (NO MALLOC POLICY)

### Non-malloc Options Comparison

#### **Option 1: Stack VLA + Increased Stack Limit** ⭐ RECOMMENDED
```c
// Set stack size programmatically
#include <sys/resource.h>
struct rlimit rl;
rl.rlim_cur = 256 * 1024 * 1024;  // 256MB
rl.rlim_max = 256 * 1024 * 1024;
setrlimit(RLIMIT_STACK, &rl);

// Then use VLA
uint8_t sieve[n];
```

**Pros:**
- ✅ Pure stack, no malloc
- ✅ Fast allocation
- ✅ Automatic cleanup
- ✅ Can programmatically set limits
- ✅ Dynamic sizing with VLA

**Cons:**
- ⚠️ setrlimit() may fail (needs privileges or already running)
- ⚠️ User can also set: `ulimit -s 262144` before running
- ⚠️ Must check return value and handle failure

**Best for:** Most use cases, maximum flexibility

---

#### **Option 2: Static/Global Array**
```c
#define MAX_N 100000000  // 100M
static uint8_t sieve[MAX_N];  // In BSS segment, not stack

void sieve_algorithm(size_t n) {
    if (n > MAX_N) { error; return; }
    // Use sieve[0..n-1]
}
```

**Pros:**
- ✅ No malloc
- ✅ No stack overflow risk
- ✅ Lives in BSS (zero-initialized by OS)
- ✅ Very simple code

**Cons:**
- ⚠️ Fixed maximum size at compile time
- ⚠️ Wastes memory (BSS allocated even if unused)
- ⚠️ Only one array (not thread-safe without care)

**Best for:** Known maximum n, single-threaded

---

#### **Option 3: mmap() - Direct OS Memory**
```c
#include <sys/mman.h>
uint8_t *sieve = mmap(NULL, n, 
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, 
                      -1, 0);
// ... use sieve ...
munmap(sieve, n);
```

**Pros:**
- ✅ Not malloc (bypasses libc heap)
- ✅ Can handle huge sizes (GBs)
- ✅ Page-aligned (cache-friendly)
- ✅ OS-level control (can use MAP_POPULATE, MAP_HUGETLB)

**Cons:**
- ⚠️ Still "dynamic allocation" (philosophically similar to malloc)
- ⚠️ Must munmap manually
- ⚠️ POSIX only (not on Windows without abstraction)

**Best for:** Very large n, OS-level control, avoiding malloc overhead

---

#### **Option 4: Hybrid Static + VLA**
```c
#define STATIC_MAX 10000000  // 10M
static uint8_t static_sieve[STATIC_MAX];

void sieve_algorithm(size_t n) {
    if (n <= STATIC_MAX) {
        // Use static array
        memset(static_sieve, 1, n);
        // ... algorithm using static_sieve
    } else {
        // Increase stack limit and use VLA
        struct rlimit rl;
        getrlimit(RLIMIT_STACK, &rl);
        rl.rlim_cur = rl.rlim_max;  // Use max available
        setrlimit(RLIMIT_STACK, &rl);
        
        uint8_t sieve[n];
        // ... algorithm using sieve
    }
}
```

**Best for:** Common case optimized, rare large case handled

---

### **Decision: Start Simple, Leave Room to Grow**

**Baseline: Plain VLA (Option 1 simplified)**

```c
void sieve_of_eratosthenes(size_t n) {
    uint8_t sieve[n];  // Simple VLA, uses default stack
    // ... algorithm
}
```

**Why this approach:**
- ✅ Dead simple (no setrlimit, no hybrid logic)
- ✅ Pure stack, no malloc
- ✅ Dynamic sizing (n chosen at runtime)
- ✅ Clean code, easy to understand
- ✅ Default stack (8MB on macOS) handles n up to ~8M
- ✅ Room to grow later (add setrlimit, static array, or segmented sieve)

**Growth path when needed:**
1. **Need n > 8M?** → Add `setrlimit()` wrapper (5 lines of code)
2. **Want to optimize common case?** → Add static array for small n
3. **Need very large n?** → Implement segmented sieve (natural next step)

**For now:** Keep it simple, handle reasonable n values, focus on correctness and clean code.

### Implementation Plan

1. **Baseline:** Simple VLA, default stack limits
2. **Document:** Works up to n ≈ 8M (byte array), larger values need `ulimit -s`
3. **Later:** Add setrlimit or segmentation when needed

## Baseline Implementation (Start Here)

- Simple `bool` array (or `uint8_t` array)
- **Pure stack allocation with VLA** (no malloc!)
- Programmatically increase stack limit with `setrlimit()`
- Only sieve up to √n (standard optimization)
- Start marking multiples from p² instead of 2p
- Basic algorithm correctness
- CLI argument parsing
- Timing instrumentation from the start

## Performance Optimization Roadmap

### **Tier 1: High Impact, Low Complexity**

1. **Skip even numbers (odd-only sieve)**
   - Only store odd numbers in array (2x memory reduction)
   - Array index i represents number 2i+1
   - ~2x speed improvement
   - Easy to implement

2. **Bit array instead of byte array**
   - Use bits instead of bytes (8x memory reduction)
   - Better cache utilization
   - Can combine with odd-only for 16x memory reduction total
   - Moderate implementation complexity

3. **Aggressive compiler optimization flags**
   - `-O3` - Maximum optimization
   - `-march=native` - Use CPU-specific instructions
   - `-flto` - Link-time optimization
   - Free performance gains

### **Tier 2: Medium Impact, Medium Complexity**

4. **Segmented sieve**
   - Process in L1/L2 cache-sized chunks
   - Critical for n > ~10 million
   - Maintains cache locality
   - More complex implementation

5. **Wheel factorization (mod 6)**
   - Skip multiples of 2 and 3
   - Store only numbers ≡ 1, 5 (mod 6)
   - 3x memory reduction vs baseline
   - Moderate speedup
   - Increased indexing complexity

6. **Output optimization**
   - Buffered file writes
   - Binary format option
   - Matters mainly for large outputs

### **Tier 3: Diminishing Returns**

7. **Wheel factorization (mod 30)** - Skip 2, 3, 5
8. **Multi-threading** - Parallel marking (complex, limited gains)
9. **SIMD instructions** - Manual vectorization
10. **Assembly optimization** - Hand-tuned critical sections

## Recommended Implementation Order

1. ✓ **Baseline implementation** - Correctness first
2. **Odd-only sieve** - Easiest big performance win
3. **Bit array** - If targeting large n (>1M)
4. **Compiler flags** - Free performance
5. **Segmented sieve** - If targeting n > 10M
6. **Further optimizations** - Based on profiling results

## Design Decisions To Make

- **Target range:** What's the maximum n we want to support?
  - < 1M: Simple array is fine
  - 1M - 10M: Bit array recommended
  - > 10M: Segmented sieve required
  
- **Bit array strategy:** Implement separately or combine with odd-only immediately?

- **File output format:** Text (human-readable) or binary (faster)?

- **Timing granularity:** Wall-clock time or CPU cycles?

## Testing Strategy

**Correctness tests:**
- n = 10: primes are 2, 3, 5, 7 (4 primes)
- n = 100: 25 primes
- n = 1000: 168 primes
- n = 10000: 1229 primes
- n = 100000: 9592 primes
- n = 1000000: 78498 primes

**Performance tests:**
- Benchmark each optimization against baseline
- Measure time improvement percentage
- Test with various n values (100K, 1M, 10M, 100M)

**Edge cases:**
- n = 0, 1, 2 (boundary conditions)
- Very large n (memory allocation limits)
- Invalid input handling

## Notes

- Incremental approach: implement and test each optimization separately
- Measure performance impact of each change
- Active collaboration on which optimizations to implement next
- Focus on speed as primary goal

---

## 🎯 Next Optimization: Bit Array (Odd-Only + Bit Packing)

### The Idea

**Current state:** Using 1 byte per odd number (wasteful - only need 1 bit!)

**Solution:** Pack 8 odd numbers into each byte using bit manipulation.

### How It Works

**Current (odd-only with bytes):**
```
Array: [byte0] [byte1] [byte2] ...
Stores: 1 odd  1 odd   1 odd   ...
```

**Bit array (odd-only with bits):**
```
Array: [byte0 = 8 bits] [byte1 = 8 bits] ...
Stores: 8 odds          8 odds          ...
```

**Bit operations:**
- Set bit: `array[byte_idx] |= (1 << bit_pos)`
- Clear bit: `array[byte_idx] &= ~(1 << bit_pos)`
- Check bit: `array[byte_idx] & (1 << bit_pos)`

**Mapping:** Odd number `n` → byte index `(n-1)/2/8`, bit position `((n-1)/2) % 8`

### Benefits

- **8x memory reduction** vs odd-only bytes (16x vs baseline!)
- **Better cache performance:** More data fits in L1/L2 cache
- **Can handle huge n on stack:** 8MB stack → 64M odd numbers → n up to 128M!
- **Still no malloc**

### Implementation Changes

**Key modifications:**
1. Array size: `uint8_t sieve[(array_size + 7) / 8]` (bytes needed for bits)
2. Bit manipulation macros for cleaner code
3. Adjust marking loop to use bit operations

**Code approach:**
```c
#define GET_BIT(arr, i) ((arr)[(i)/8] & (1 << ((i)%8)))
#define CLEAR_BIT(arr, i) ((arr)[(i)/8] &= ~(1 << ((i)%8)))

size_t array_size = (n + 1) / 2;  // Number of odd numbers
size_t byte_size = (array_size + 7) / 8;  // Bytes needed
uint8_t sieve[byte_size];
memset(sieve, 0xFF, byte_size);  // All bits = 1 (prime)

// Use GET_BIT and CLEAR_BIT in loops
```

### Complexity

**Implementation difficulty:** Medium
- Bit manipulation required
- More complex indexing
- Need to handle bit/byte conversion correctly

**Risk:** Medium
- Easy to get bit operations wrong
- Must test carefully

### Expected Performance

- **Memory:** 8x reduction (16x total from baseline)
- **Speed:** Potentially faster due to cache (or slightly slower due to bit ops)
- **Stack capacity:** Can now handle n up to ~128M on 8MB stack!

---

## 🏁 Previous Optimization: Odd-Only Sieve

### The Idea

**Observation:** All even numbers (except 2) are composite. We're wasting half our memory storing them!

**Solution:** Only store odd numbers in the array. Handle 2 as a special case.

### How It Works

**Current baseline:**
```
Array index:  0   1   2   3   4   5   6   7   8   9   10
Number:       0   1   2   3   4   5   6   7   8   9   10
```

**Odd-only approach:**
```
Array index:  0   1   2   3   4   5
Number:       1   3   5   7   9   11  (odd numbers only)

Mapping: index i represents number (2*i + 1)
         number n maps to index (n-1)/2
```

### Benefits

- **2x memory reduction:** Array size drops from n to n/2
- **~2x speed improvement:** Half the memory to initialize, half to iterate
- **Better cache utilization:** More data fits in L1/L2 cache
- **Still stack-friendly:** Can handle 2x larger n with same memory

### Implementation Changes

**Key modifications:**
1. Array size: `uint8_t sieve[(n+1)/2]` instead of `uint8_t sieve[n+1]`
2. Index mapping: Use `(num-1)/2` to convert number → index
3. Special case: Handle prime 2 separately (always include it)
4. Marking multiples: Adjust loop to skip even multiples

**Code sketch:**
```c
// Special case: 2 is prime
if (n >= 2) prime_count = 1;

// Sieve for odd numbers only
size_t array_size = (n + 1) / 2;
uint8_t sieve[array_size];  // Still on stack!
memset(sieve, 1, array_size);

// sieve[i] represents number 2*i+1
for (size_t i = 1; i < array_size; i++) {  // Start at 1 (number 3)
    if (sieve[i]) {
        size_t p = 2*i + 1;  // Convert index to number
        // Mark odd multiples only: p*3, p*5, p*7, ...
        for (size_t multiple = p*p; multiple <= n; multiple += 2*p) {
            size_t idx = (multiple - 1) / 2;
            sieve[idx] = 0;
        }
    }
}
```

### Complexity

**Implementation difficulty:** Low-Medium
- Requires index arithmetic (`2*i+1` and `(n-1)/2`)
- Need to handle 2 as special case
- Adjust multiple-marking loop

**Risk:** Low
- Algorithm stays the same, just different indexing
- Easy to test (same expected outputs)

### Expected Performance Gain

- **Time:** 1.8x - 2.0x faster (roughly half the work)
- **Memory:** Exactly 2x reduction
- **Cache:** Better locality, especially for large n

### Testing Strategy

Run same tests as baseline, expect identical prime counts but faster times:
- n = 100: should still find 25 primes
- n = 1,000: should still find 168 primes
- n = 100,000: should still find 9,592 primes (but ~2x faster)

### Decision Point

**Ready to implement?** This is the easiest high-impact optimization.

**Alternative:** Skip to bit array (8x memory reduction but more complex)

