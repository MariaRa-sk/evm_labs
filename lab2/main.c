#include <stdio.h>
#include <time.h>

#define NUMBER_OF_TESTS 3

double calculate_pi(const size_t N){
        double sum = 0;
        int sign = 1;
        for (size_t i = 0; i < N; ++i){
                sum += (double)sign / (2 * i + 1);
                sign = -sign;
        }
        return 4 * sum;
}

int main(void){
        size_t arr[NUMBER_OF_TESTS] = {3500000000, 4500000000, 6000000000};
        struct timespec start, end;
        double result;
        double time_taken;
        for (size_t i = 0; i < NUMBER_OF_TESTS; ++i){
                clock_gettime(CLOCK_MONOTONIC_RAW, &start);
                result = calculate_pi(arr[i]);
                clock_gettime(CLOCK_MONOTONIC_RAW, &end);
                time_taken =  end.tv_sec - start.tv_sec + 1e-9*(end.tv_nsec - start.tv_nsec);
                printf("N = %zu, Число Пи: %.15f, time = %f\n", arr[i], result, time_taken);
        }
        return 0;
}
