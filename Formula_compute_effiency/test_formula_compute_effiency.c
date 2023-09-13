#include <stdio.h>
#include <time.h>

#define ITERATIONS 10000000
int NUM_A = 128;
int NUM_B = 4;
int NUM_CAND = 4;
int NUM_N = 10;

int main() {
    int i, y = 11223;  // Example starting value
    int LOCATION;

    int precomputedDivisor = NUM_B * NUM_CAND;
    int precomputedModulus = NUM_A / NUM_B;

    clock_t start, end;

    // Original method (Intuition method)
    start = clock();
    for (i = 0; i < ITERATIONS; i++) {
        if (i % 4 == 0) {
            y++;  // Increment y or modify it as needed
        }
        LOCATION = (y + ((i * NUM_A) / (NUM_B * NUM_CAND)) + NUM_N) %  NUM_A / NUM_B);
    }
    end = clock();
    printf("Original method took: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    // Reset y
    y = 12345;

    // Pre-computation method
    start = clock();
    for (i = 0; i < ITERATIONS; i++) {
        if (i % 4 == 0) {
            y++;  // Increment y or modify it as needed
        }
        LOCATION = (y + (i * NUM_A) / precomputedDivisor + NUM_N) % precomputedModulus;
    }
    end = clock();
    printf("Pre-computation method took: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    // Reset y
    y = 12345;

    // Optimized Division & Modulus
    int shiftValue = 2;  // Assuming NUM_B is 4, which is 2^2
    start = clock();
    for (i = 0; i < ITERATIONS; i++) {
        if (i % 4 == 0) {
            y++;  // Increment y or modify it as needed
        }
        LOCATION = (y + (i * NUM_A) / precomputedDivisor + NUM_N) &  NUM_A >> shiftValue);
    }
    end = clock();
    printf("Optimized Division & Modulus method took: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}