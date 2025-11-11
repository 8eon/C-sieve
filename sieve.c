#include "sieve.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// BIT ARRAY MACROS for odd-only sieve
// Bit index i represents odd number (2*i + 1)
#define GET_BIT(arr, i)   ((arr)[(i) >> 3] & (1 << ((i) & 7)))
#define CLEAR_BIT(arr, i) ((arr)[(i) >> 3] &= ~(1 << ((i) & 7)))

size_t sieve_of_eratosthenes(size_t n, const char *output_file) {
    if (n < 2) {
        return 0;  // No primes less than 2
    }
    
    // BIT ARRAY OPTIMIZATION (combined with odd-only):
    // - Only store odd numbers: bit index i represents number (2*i + 1)
    // - Pack 8 odd numbers per byte using bit manipulation
    // - This gives us 16x memory reduction vs baseline (2x odd-only * 8x bit packing)
    
    size_t bit_count = (n + 1) / 2;              // Number of odd numbers to track
    size_t byte_count = (bit_count + 7) / 8;     // Bytes needed for bits
    uint8_t sieve[byte_count];                   // Still on stack (VLA)
    memset(sieve, 0xFF, byte_count);             // Initialize all bits to 1 (prime)
    
    // Bit 0 represents 1, which is not prime
    CLEAR_BIT(sieve, 0);
    
    // Sieve algorithm: only check odd numbers up to sqrt(n)
    // Bit index i represents odd number (2*i + 1)
    for (size_t i = 1; (2*i + 1) * (2*i + 1) <= n; i++) {
        if (GET_BIT(sieve, i)) {
            size_t p = 2*i + 1;  // Convert bit index to actual odd number
            
            // Mark odd multiples of p as composite
            // Start from p*p, increment by 2*p (skip even multiples)
            for (size_t multiple = p * p; multiple <= n; multiple += 2*p) {
                size_t bit_idx = (multiple - 1) / 2;  // Convert odd number to bit index
                CLEAR_BIT(sieve, bit_idx);
            }
        }
    }
    
    // Count primes and optionally write to file
    size_t prime_count = 0;
    FILE *fp = NULL;
    
    if (output_file != NULL) {
        fp = fopen(output_file, "w");
        if (fp == NULL) {
            fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
            // Continue without file output
        }
    }
    
    // Special case: 2 is the only even prime
    if (n >= 2) {
        prime_count = 1;
        if (fp != NULL) {
            fprintf(fp, "2\n");
        }
    }
    
    // Count/output odd primes
    for (size_t i = 1; i < bit_count; i++) {
        if (GET_BIT(sieve, i)) {
            size_t prime = 2*i + 1;
            if (prime <= n) {  // Boundary check for last byte
                prime_count++;
                if (fp != NULL) {
                    fprintf(fp, "%zu\n", prime);
                }
            }
        }
    }
    
    if (fp != NULL) {
        fclose(fp);
    }
    
    return prime_count;
}

