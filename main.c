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

// Get high-resolution time in seconds
static double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
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
    
    // Run sieve with high-resolution timing
    double start = get_time();
    size_t prime_count = sieve_of_eratosthenes(limit, output_file);
    double end = get_time();
    
    double elapsed_time = end - start;
    
    // Print results with microsecond precision
    printf("Primes found: %zu\n", prime_count);
    if (elapsed_time < 0.001) {
        printf("Time elapsed: %.2f microseconds\n", elapsed_time * 1e6);
    } else if (elapsed_time < 1.0) {
        printf("Time elapsed: %.3f milliseconds\n", elapsed_time * 1e3);
    } else {
        printf("Time elapsed: %.6f seconds\n", elapsed_time);
    }
    
    if (output_file != NULL) {
        printf("Primes written to: %s\n", output_file);
    }
    
    return 0;
}

