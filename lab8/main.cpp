#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

uint64_t getCpuTicks() {
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return (static_cast<unsigned long long>(hi) << 32) | lo;
}

void warmCache(const int* arr, const size_t size) {
    volatile size_t k = 0;
    for (size_t i = 0; i < size; ++i) {
        k = arr[k];
    }
}

size_t getK(size_t n) {
    if (n <= 1024) return 1000;
    if (n <= 16384) return 100;
    if (n <= 262144) return 20;
    if (n <= 2097152) return 10;
    return 5;
}

uint64_t measure(const int* arr, const size_t size, const size_t K) {
    warmCache(arr, size);
    uint64_t min_ticks = UINT64_MAX;
    for (size_t i = 0; i < 5; ++i) {
        volatile size_t k = 0;
        uint64_t start = getCpuTicks();
        for (size_t i = 0; i < size * K; ++i) {
            k = arr[k];
        }
        uint64_t end = getCpuTicks();
        uint64_t ticks = (end - start) / (size * K);
        if (ticks < min_ticks) {
            min_ticks = ticks;
        }
    }
    return min_ticks;
}

void forwardFill(int* arr, const size_t n){
    for (size_t i = 0; i < n; ++i) {
        arr[i] = (i + 1) % n;
    }
}

void backwardFill(int* arr, const size_t n){
    for (size_t i = 0; i < n; ++i){
        arr[i] = (i == 0) ? (n - 1) : (i - 1);
    }
}

void randomFill(int* arr, const size_t n) {
    vector<size_t> index(n);
    for (size_t i = 0; i < n; ++i) {
        index[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(index.begin(), index.end(), g);
    for (size_t i = 0; i < n; ++i) {
        arr[index[i]] = index[(i + 1) % n];
    }
}

int main(void){
    const size_t minN = 256;         //1Кб
    const size_t maxN = 8388608;     //32Мб

    ofstream file("results.csv");
    if (!file.is_open()) {
        cout << "Cannot open file" << endl;
        return 1;
    }

    size_t sizes[] = {
        256, 512, 768, 1024, 1280, 1536, 1792, 2048,
        2560, 3072, 3584, 4096, 5120, 6144, 7168, 8192,
        16384, 24576, 32768, 49152, 65536,
        98304, 131072, 196608, 262144, 393216, 524288,
        786432, 1048576, 1572864, 2097152, 3145728, 4194304,
        6291456, 8388608
    };

    const size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    file << "N,Forward,Backward,Random" << endl;
    for(size_t i = 0; i < num_sizes; ++i){
        size_t n = sizes[i];
        size_t K = getK(n);
        int* arr_forward = new int[n];
        int* arr_backward = new int[n];
        int* arr_random = new int[n];
        forwardFill(arr_forward, n);
        backwardFill(arr_backward, n);
        randomFill(arr_random, n);
        uint64_t ticks_forward = measure(arr_forward, n, K);
        uint64_t ticks_backward = measure(arr_backward, n, K);
        uint64_t ticks_random = measure(arr_random, n, K);

        file << (n * 4) / 1024 << "," << ticks_forward << "," << ticks_backward << "," << ticks_random << endl;
        delete[] arr_forward;
        delete[] arr_backward;
        delete[] arr_random;
    }
    file.close();
    return 0;
}
