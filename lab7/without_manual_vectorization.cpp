#include <cmath>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <climits>
#include <cstring>
#include <random>

using namespace std;

void addMatrix(const float* A, const float* B, float* Res, const size_t N) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Res[i * N + j] = A[i * N + j] + B[i * N + j];
        }
    }
}

void subMatrix(const float* A, const float* B, float* Res, const size_t N) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Res[i * N + j] = A[i * N + j] - B[i * N + j];
        }
    }
}

void mulMatrix(const float* A, const float* B, float* Res, const size_t N) {
    for (size_t i = 0; i < N * N; i++) {
        Res[i] = 0.0f;
    }
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            for (size_t k = 0; k < N; k++) {
                Res[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

void divMatrix(const float* A, float* Res, const float k, const size_t N) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            Res[i * N + j] = A[i * N + j] / k;
        }
    }
}

void transposeMatrix(const float* A, float* AT, const size_t N) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            AT[j * N + i] = A[i * N + j];
        }
    }
}

float findMaxAbsSumByRows(const float* A, const size_t N) {
    float res = INT_MIN;
    for (size_t i = 0; i < N; ++i) {
        float sum = 0;
        for (size_t j = 0; j < N; ++j) {
            sum += fabsf(A[i * N + j]);
        }
        res = max(sum, res);
    }
    return res;
}

float findMaxAbsSumByColumns(const float* A, const size_t N) {
    float res = INT_MIN;
    for (size_t i = 0; i < N; ++i) {
        float sum = 0;
        for (size_t j = 0; j < N; ++j) {
            sum += fabsf(A[j*N + i]);
        }
        res = max(sum, res);
    }
    return res;
}

void initIdentityMatrix(float* Res, const size_t N) {
    memset(Res, 0, N * N * sizeof(float));
    for (size_t i = 0; i < N; ++i) {
        Res[i * N + i] = 1;
    }
}

double matrixConversion(float* A, float* Res, const size_t N, const size_t M) {
    float* B = new float[N * N];
    float* AT = new float[N * N];
    float* R = new float[N * N];
    float* identityMatrix = new float[N * N];
    float* tmp = new float[N * N];
    float* tmp2 = new float[N * N];  // Дополнительный буфер

    clock_t inversion_start = clock();
    transposeMatrix(A, AT, N);
    divMatrix(AT, B, findMaxAbsSumByColumns(A, N) * findMaxAbsSumByRows(A, N) , N);

    mulMatrix(B, A, tmp, N);
    initIdentityMatrix(identityMatrix, N);
    subMatrix(identityMatrix, tmp, R, N);

    initIdentityMatrix(Res, N);
    memcpy(tmp, R, N * N * sizeof(float));

    for (size_t i = 1; i < M; ++i) {
        addMatrix(Res, tmp, Res, N);
        if (i < M - 1) {
            mulMatrix(tmp, R, tmp2, N);
            memcpy(tmp, tmp2, N * N * sizeof(float));
        }
    }
    mulMatrix(Res, B, tmp, N);
    memcpy(Res, tmp, N * N * sizeof(float));
    clock_t inversion_end = clock();

    delete[] B;
    delete[] AT;
    delete[] R;
    delete[] identityMatrix;
    delete[] tmp;
    delete[] tmp2;
    return static_cast<double>(inversion_end - inversion_start) / CLOCKS_PER_SEC;
}

void fillRandomMatrix(float* A, const size_t N) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dis(-10.0f, 10.0f);
    for (size_t i = 0; i < N * N; ++i) {
        A[i] = dis(gen);
    }
}

void printMatrix(const float* A, const size_t N){
    for (size_t i = 0; i < N*N; ++i){
        cout << "Matrix[" << i << "]" << A[i] << endl;
    }
}

int main(void) {
    const size_t N = 2048;
    const size_t M = 10;
    float* A = new float[N * N];
    float* A_inv = new float[N * N];
	fillRandomMatrix(A, N);
    double inversion_time = matrixConversion(A, A_inv, N, M);
    cout << "Время обращения: " << inversion_time << " sec " << endl;
    delete[] A_inv;
    cout << "Программа завершена." << endl;
    return 0;
}
