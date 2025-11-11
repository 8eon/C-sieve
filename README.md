Project Overview
Goal: Implement the Sieve of Eratosthenes algorithm in C with a focus on maximum performance through incremental optimization.

The Sieve of Eratosthenes - An efficient algorithm to find all prime numbers up to a given limit by iteratively marking multiples of each prime as composite.

Project Structure
Files:

sieve.h - Header file with function declarations and constants
sieve.c - Implementation of the sieve algorithm
Makefile - Compilation with optimization flags
PLANNING.md - This file
Key Components:

Core sieve algorithm function
CLI interface with command-line arguments
File output support for prime analysis
Performance timing instrumentation
Error handling (memory allocation, input validation)
Progress Status
✅ Completed: Baseline Implementation
What we built:

sieve.h, sieve.c, main.c - Core implementation
Simple uint8_t array with VLA (stack allocation, no malloc)
Algorithm optimizations: sieve up to √n, start marking from p²
CLI with command-line arguments
Optional file output for prime analysis
Performance timing with clock()
Makefile with aggressive optimization flags: -O3 -march=native -flto
Test results (all correct):

n = 100: 25 primes ✓
n = 1,000: 168 primes ✓
n = 10,000: 1,229 primes ✓
n = 100,000: 9,592 primes ✓
Performance: Very fast for small/medium n (< 0.0001s for n=100k)
