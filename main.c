#include "sieve.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s <limit> [output_file]\n", program_name);
    fprintf(stderr, "  limit        - Find all primes up to this number (inclusive)\n");
    fprintf(stderr, "  output_file  - Optional: Write primes to this file\n");
    fprintf(stderr, "\nExample:\n");
    fprintf(stderr, "  %s 1000000\n", program_name);
    fprintf(stderr, "  %s 1000000 primes.txt\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Parse limit argument
    char *endptr;
    long limit_long = strtol(argv[1], &endptr, 10);
    
    if (*endptr != '\0' || limit_long < 0) {
        fprintf(stderr, "Error: Invalid limit '%s'. Must be a non-negative integer.\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }
    
    size_t limit = (size_t)limit_long;
    const char *output_file = (argc == 3) ? argv[2] : NULL;
    
    // Run sieve with timing
    clock_t start = clock();
    size_t prime_count = sieve_of_eratosthenes(limit, output_file);
    clock_t end = clock();
    
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    // Print results
    printf("Primes found: %zu\n", prime_count);
    printf("Time elapsed: %.6f seconds\n", elapsed_time);
    
    if (output_file != NULL) {
        printf("Primes written to: %s\n", output_file);
    }
    
    return 0;
}

