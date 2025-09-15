#include <stdio.h>

double calculate_pi(const size_t N) {
    double sum = 0;
    int sign = 1;
    for (size_t i = 0; i < N; ++i) {
        sum += (double) sign / (2 * i + 1);
        sign = -sign;
    }
    return 4 * sum;
}

int main(void) {
    size_t N = 100;
    double pi = calculate_pi(N);
    printf("Число пи: %.10f\n", pi);
    return 0;
}

