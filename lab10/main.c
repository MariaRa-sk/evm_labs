#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <x86intrin.h>

#define PAGE_SIZE 4096
#define CACHE_LINE 64
#define STRIDE (PAGE_SIZE + CACHE_LINE)
#define MAX_ENTRIES 8192
#define ITERATIONS 1000000
#define REPEAT_COUNT 100
#define STRIDE_MULTIPLIER 4

static inline uint64_t read_tsc(void) {
    return __rdtsc();
}

void flush_cache(void *addr, size_t size) {
    const size_t FLUSH_SIZE = 12 * 1024 * 1024;
    volatile char *flush_buffer = malloc(FLUSH_SIZE);
    if (flush_buffer) {
        for (size_t i = 0; i < FLUSH_SIZE; i += CACHE_LINE) {
            flush_buffer[i] = i % 256;
        }
        free((void *)flush_buffer);
    }
    _mm_mfence();
}


double measure_single_run(void **pages, int entries, int iterations, int stride_multiplier) {
    int effective_stride = stride_multiplier * STRIDE;

    for (int i = 0; i < entries - 1; ++i) {
        int next_idx = (i + 1) % entries;
        *(void **)pages[i * stride_multiplier] = pages[next_idx * stride_multiplier];
    }
    *(void **)pages[(entries - 1) * stride_multiplier] = pages[0];

    void *ptr = pages[0];
    for (int i = 0; i < entries; ++i) {
        ptr = *(void **)ptr;
    }

    flush_cache(pages[0], entries * effective_stride);

    uint64_t start = read_tsc();

    for (int i = 0; i < iterations; ++i) {
        ptr = *(void **)ptr;
    }
    __asm__ __volatile__("" : : "r"(ptr));
    uint64_t end = read_tsc();

    uint64_t total_cycles = end - start;
    return (double)total_cycles / iterations;
}


void measure_tlb(void **pages, int entries, int stride_multiplier) {
    double min_cycles = 1e9;
    printf("%d\t", entries);
    fflush(stdout);
    for (int i = 0; i < REPEAT_COUNT; ++i) {
        double cycles = measure_single_run(pages, entries, ITERATIONS, stride_multiplier);
        if (cycles < min_cycles) {
            min_cycles = cycles;
        }
    }

    printf("%.2f\n", min_cycles);
}

int main() {
    printf("Entries\tTicks\n");
    printf("-------\t------\n");

    size_t total_size = MAX_ENTRIES * STRIDE_MULTIPLIER * STRIDE;
    void *buffer;

    if (posix_memalign(&buffer, PAGE_SIZE, total_size) != 0) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    memset(buffer, 0, total_size);
    void **pages = (void **)malloc(MAX_ENTRIES * STRIDE_MULTIPLIER * sizeof(void *));
    if (!pages) {
        free(buffer);
        return 1;
    }
    for (int i = 0; i < MAX_ENTRIES * STRIDE_MULTIPLIER; i++) {
        pages[i] = (char *)buffer + i * STRIDE;
    }
    for (int entries = 8; entries <= 256; entries += 8) {
        measure_tlb(pages, entries, STRIDE_MULTIPLIER);
    }
    for (int entries = 272; entries <= MAX_ENTRIES; entries += 16) {
        measure_tlb(pages, entries, STRIDE_MULTIPLIER);
    }
    free(pages);
    free(buffer);
    return 0;
}
