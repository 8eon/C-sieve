#include "sieve.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

// WHEEL FACTORIZATION (MOD 6) + BIT ARRAY
// Only store numbers of form 6k+1 and 6k+5 (all primes > 3 are of this form)
// This gives us 2 numbers per 6 consecutive integers = 33% storage (vs 50% odd-only)

// BIT ARRAY MACROS
#define GET_BIT(arr, i)   ((arr)[(i) >> 3] & (1 << ((i) & 7)))
#define CLEAR_BIT(arr, i) ((arr)[(i) >> 3] &= ~(1 << ((i) & 7)))

// WHEEL MOD 6 HELPER FUNCTIONS

// Check if a number is a candidate (form 6k+1 or 6k+5)
static inline bool is_wheel_candidate(size_t n) {
    size_t mod = n % 6;
    return (mod == 1 || mod == 5);
}

// Convert number (6k+1 or 6k+5) to bit array index
// Number 1 → 0, 5 → 1, 7 → 2, 11 → 3, 13 → 4, 17 → 5, ...
static inline size_t number_to_index(size_t n) {
    size_t k = n / 6;
    size_t offset = (n % 6 == 1) ? 0 : 1;
    return 2 * k + offset;
}

// Convert bit array index to number
// 0 → 1, 1 → 5, 2 → 7, 3 → 11, 4 → 13, 5 → 17, ...
static inline size_t index_to_number(size_t idx) {
    size_t k = idx / 2;
    size_t offset = idx % 2;
    return 6 * k + (offset == 0 ? 1 : 5);
}

size_t sieve_of_eratosthenes(size_t n, const char *output_file) {
    if (n < 2) {
        return 0;  // No primes less than 2
    }
    
    // WHEEL FACTORIZATION (MOD 6) + BIT ARRAY:
    // - Only store numbers of form 6k+1 and 6k+5 (all primes > 3 fit this pattern)
    // - Pack 8 candidates per byte using bit manipulation
    // - This gives us 24x memory reduction vs baseline (3x wheel * 8x bit packing)
    
    // Calculate how many wheel candidates (6k+1 or 6k+5) exist up to n
    size_t bit_count = number_to_index(n) + 1;   // Number of candidates to track
    size_t byte_count = (bit_count + 7) / 8;     // Bytes needed for bits
    uint8_t sieve[byte_count];                   // Still on stack (VLA)
    memset(sieve, 0xFF, byte_count);             // Initialize all bits to 1 (prime)
    
    // Index 0 represents 1, which is not prime
    CLEAR_BIT(sieve, 0);
    
    // Sieve algorithm: check wheel candidates up to sqrt(n)
    size_t sqrt_n = (size_t)sqrt((double)n);
    
    for (size_t i = 1; index_to_number(i) <= sqrt_n; i++) {
        if (GET_BIT(sieve, i)) {
            size_t p = index_to_number(i);  // Convert index to actual number
            
            // Mark multiples of p as composite
            // Start from p*p, only mark wheel candidates (6k+1 or 6k+5)
            for (size_t multiple = p * p; multiple <= n; multiple += p) {
                if (is_wheel_candidate(multiple)) {
                    size_t idx = number_to_index(multiple);
                    CLEAR_BIT(sieve, idx);
                }
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
    
    // Special cases: 2 and 3 are primes not covered by wheel
    if (n >= 2) {
        prime_count++;
        if (fp != NULL) {
            fprintf(fp, "2\n");
        }
    }
    if (n >= 3) {
        prime_count++;
        if (fp != NULL) {
            fprintf(fp, "3\n");
        }
    }
    
    // Count/output wheel candidate primes (6k+1 and 6k+5)
    for (size_t i = 1; i < bit_count; i++) {
        if (GET_BIT(sieve, i)) {
            size_t prime = index_to_number(i);
            if (prime <= n) {  // Boundary check
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

