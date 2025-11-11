#ifndef SIEVE_H
#define SIEVE_H

#include <stddef.h>
#include <stdint.h>

/**
 * Find all prime numbers up to n using the Sieve of Eratosthenes algorithm.
 * 
 * @param n The upper limit (inclusive)
 * @param output_file Optional file path to write primes to (NULL to skip)
 * @return The count of primes found
 */
size_t sieve_of_eratosthenes(size_t n, const char *output_file);

#endif /* SIEVE_H */

