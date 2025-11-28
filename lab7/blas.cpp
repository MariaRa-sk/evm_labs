#include <cmath>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <climits>
#include <cstring>
#include <random>
#include <cblas.h>

using namespace std;

void addMatrix(const float* A, const float* B, float* Res, const size_t N) {
    cblas_scopy(N * N, A, 1, Res, 1);
    cblas_saxpy(N * N, 1.0f, B, 1, Res, 1);
}

void subMatrix(const float* A, const float* B, float* Res, const size_t N) {
    cblas_scopy(N * N, A, 1, Res, 1);
    cblas_saxpy(N * N, -1.0f, B, 1, Res, 1);
}

void mulMatrix(const float* A, const float* B, float* Res, const size_t N) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                N, N, N,
                1.0f,
                A, N,
                B, N,
                0.0f,
                Res, N);
}


void divMatrix(const float* A, float* Res, const float k, const size_t N) {
    cblas_scopy(N * N, A, 1, Res, 1);
    float inv_k = 1.0f / k;
    cblas_sscal(N * N, inv_k, Res, 1);
}

void transposeMatrix(const float* A, float* AT, const size_t N) {
    for (size_t i = 0; i < N; ++i) {
        cblas_scopy(N, &A[i * N], 1, &AT[i], N);
    }
}

float findMaxAbsSumByRows(const float* A, const size_t N) {
    float res = INT_MIN;
    for (size_t i = 0; i < N; ++i) {
        float sum = cblas_sasum(N, &A[i * N], 1);
        res = max(sum, res);
    }
    return res;
}

float findMaxAbsSumByColumns(const float* A, const size_t N) {
    float res = INT_MIN;
    for (size_t i = 0; i < N; ++i) {
        float sum = cblas_sasum(N, &A[i], N);
        res = max(sum, res);
    }
    return res;
}

void initIdentityMatrix(float* Res, const size_t N) {
    memset(Res, 0, N * N * sizeof(float));
    for (size_t i = 0; i < N; ++i) {
        Res[i * N + i] = 1.0f;
    }
}

double matrixConversion(float* A, float* Res, const size_t N, const size_t M) {
    float* B = new float[N * N];
    float* AT = new float[N * N];
    float* R = new float[N * N];
    float* identityMatrix = new float[N * N];
    float* tmp = new float[N * N];
    float* tmp2 = new float[N * N];

    clock_t inversion_start = clock();
    transposeMatrix(A, AT, N);
    float scale = findMaxAbsSumByColumns(A, N) * findMaxAbsSumByRows(A, N);
    divMatrix(AT, B, scale, N);

    mulMatrix(B, A, tmp, N);
    initIdentityMatrix(identityMatrix, N);
    subMatrix(identityMatrix, tmp, R, N);

    initIdentityMatrix(Res, N);
    cblas_scopy(N * N, R, 1, tmp, 1);

    for (size_t i = 1; i < M; ++i) {
        cblas_saxpy(N * N, 1.0f, tmp, 1, Res, 1);
        if (i < M - 1) {
            mulMatrix(tmp, R, tmp2, N);
            cblas_scopy(N * N, tmp2, 1, tmp, 1);
        }
    }
    mulMatrix(Res, B, tmp, N);
    cblas_scopy(N * N, tmp, 1, Res, 1);

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

void printMatrix(const float* A, const size_t N) {
    for (size_t i = 0; i < min(N, (size_t)10); ++i) {
        for (size_t j = 0; j < min(N, (size_t)10); ++j) {
            cout << A[i * N + j] << " ";
        }
        cout << endl;
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
    delete[] A;
    delete[] A_inv;
    cout << "Программа завершена." << endl;
    return 0;
}
