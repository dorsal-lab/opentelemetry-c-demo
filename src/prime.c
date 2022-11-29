#include "prime.h"

long get_nth_prime(long nth) {
	long primes[nth];
	long n_primes_found = 0;
	for (long i = 2; n_primes_found < nth; i++) {
		long is_prime = 1;
		for (long j = 0; j < n_primes_found; j++) {
			if (i % primes[j] == 0) {
				is_prime = 0;
				break;
			}
		}
		if (!is_prime) {
			continue;
		}
		primes[n_primes_found] = i;
		n_primes_found++;
	}
	return primes[n_primes_found - 1];
}
