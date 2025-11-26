#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;

uint64_t getCpuTicks() {
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int* initArray(size_t fragCount, size_t size, size_t offset) {
    int* array = new int[offset*fragCount];
    for (size_t i = 0; i < size / fragCount; ++i) {
        for (size_t j = 0; j < fragCount - 1; ++j) {
            array[i + j * offset] = i + (j + 1) * offset;
        }
        array[i + (fragCount - 1) * offset] = (i + 1) % (size / fragCount);
    }
    return array;
}

uint64_t getMinTicks(int* array, size_t size) {
    uint64_t start, end;
    uint64_t min_ticks = UINT64_MAX;
    for (size_t j = 0; j < 10; ++j){
        start = getCpuTicks();
        for(volatile size_t k = 0, i = 0; i < size; ++i) {
            k = array[k];
        }
        end = getCpuTicks();
        min_ticks = min(min_ticks, end - start);
    }
    return min_ticks/size;
}

int main() {
    size_t offset = (16 * 1024 * 1024) / sizeof(int);   //16 Мб
    size_t size = (32 * 1024) / sizeof(int);
    ofstream file("results.csv");
    if (!file.is_open()) {
        cout << "Cannot open file" << endl;
        return 1;
    }
    file << "Fragment Count, Ticks" << endl;
    for (size_t fragCount = 1; fragCount <= 32; ++fragCount){
        int* array = initArray(fragCount, size, offset);
        file << fragCount << "," << getMinTicks(array, size) << endl;
        delete[] array;
    }
    file.close();
    return 0;
}
