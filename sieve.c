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

// Simple sieve for small/medium n (odd-only + bit array)
static size_t sieve_simple(size_t n, const char *output_file) {
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

// ============================================================================
// SEGMENTED SIEVE for large n
// ============================================================================

#define SEGMENT_SIZE (256 * 1024)  // 256KB segments (cache-friendly)

// Helper: Find base primes up to sqrt(n) using simple odd-only sieve
static size_t find_base_primes(size_t limit, size_t *primes, size_t max_primes) {
    if (limit < 2) return 0;
    
    // Simple odd-only bit sieve for base primes
    size_t bit_count = (limit + 1) / 2;
    size_t byte_count = (bit_count + 7) / 8;
    uint8_t sieve[byte_count];
    memset(sieve, 0xFF, byte_count);
    
    CLEAR_BIT(sieve, 0);  // 1 is not prime
    
    // Sieve odd numbers
    for (size_t i = 1; (2*i + 1) * (2*i + 1) <= limit; i++) {
        if (GET_BIT(sieve, i)) {
            size_t p = 2*i + 1;
            for (size_t multiple = p * p; multiple <= limit; multiple += 2*p) {
                size_t idx = (multiple - 1) / 2;
                CLEAR_BIT(sieve, idx);
            }
        }
    }
    
    // Collect primes into output array
    size_t count = 0;
    
    // Special case: 2
    if (limit >= 2 && count < max_primes) {
        primes[count++] = 2;
    }
    
    // Odd primes
    for (size_t i = 1; i < bit_count && count < max_primes; i++) {
        if (GET_BIT(sieve, i)) {
            size_t prime = 2*i + 1;
            if (prime <= limit) {
                primes[count++] = prime;
            }
        }
    }
    
    return count;
}

// Segmented sieve for large n
static size_t sieve_segmented(size_t n, const char *output_file) {
    if (n < 2) {
        return 0;
    }
    
    // Phase 1: Find base primes up to sqrt(n)
    size_t sqrt_n = (size_t)sqrt((double)n);
    size_t max_base_primes = sqrt_n;  // Upper bound
    size_t base_primes[max_base_primes];
    size_t base_count = find_base_primes(sqrt_n, base_primes, max_base_primes);
    
    // Open output file if needed
    FILE *fp = NULL;
    if (output_file != NULL) {
        fp = fopen(output_file, "w");
        if (fp == NULL) {
            fprintf(stderr, "Error: Could not open output file '%s'\n", output_file);
        }
    }
    
    size_t prime_count = 0;
    
    // Output base primes (already found)
    for (size_t i = 0; i < base_count; i++) {
        prime_count++;
        if (fp != NULL) {
            fprintf(fp, "%zu\n", base_primes[i]);
        }
    }
    
    // Phase 2: Process segments from sqrt(n)+1 to n
    size_t segment_low = sqrt_n + 1;
    
    while (segment_low <= n) {
        size_t segment_high = segment_low + SEGMENT_SIZE - 1;
        if (segment_high > n) {
            segment_high = n;
        }
        
        // Calculate first odd in segment and count
        size_t first_odd = (segment_low % 2 == 0) ? segment_low + 1 : segment_low;
        size_t last_odd = (segment_high % 2 == 0) ? segment_high - 1 : segment_high;
        size_t odd_count = (last_odd - first_odd) / 2 + 1;
        size_t byte_count = (odd_count + 7) / 8;
        
        uint8_t seg_sieve[byte_count];
        memset(seg_sieve, 0xFF, byte_count);
        
        // Mark composites using base primes
        for (size_t i = 0; i < base_count; i++) {
            size_t p = base_primes[i];
            
            if (p == 2) continue;  // We only store odds
            
            // Find first odd multiple of p in [first_odd, last_odd]
            size_t first_multiple = ((first_odd + p - 1) / p) * p;
            if (first_multiple % 2 == 0) {
                first_multiple += p;
            }
            
            // Don't start before p*p (smaller multiples already marked by smaller primes)
            if (first_multiple < p * p) {
                first_multiple = p * p;
                if (first_multiple % 2 == 0) {
                    first_multiple += p;
                }
            }
            
            // Mark all odd multiples in segment
            for (size_t multiple = first_multiple; multiple <= last_odd; multiple += 2*p) {
                if (multiple >= first_odd) {
                    size_t local_idx = (multiple - first_odd) / 2;
                    if (local_idx < odd_count) {
                        CLEAR_BIT(seg_sieve, local_idx);
                    }
                }
            }
        }
        
        // Count and output primes in segment
        for (size_t i = 0; i < odd_count; i++) {
            if (GET_BIT(seg_sieve, i)) {
                size_t prime = first_odd + 2*i;
                prime_count++;
                if (fp != NULL) {
                    fprintf(fp, "%zu\n", prime);
                }
            }
        }
        
        segment_low = segment_high + 1;
    }
    
    if (fp != NULL) {
        fclose(fp);
    }
    
    return prime_count;
}

// ============================================================================
// Main dispatcher
// ============================================================================

#define THRESHOLD 10000000  // 10 million

size_t sieve_of_eratosthenes(size_t n, const char *output_file) {
    if (n < THRESHOLD) {
        return sieve_simple(n, output_file);
    } else {
        return sieve_segmented(n, output_file);
    }
}

