#include <cmath>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <climits>
#include <cstring>
#include <random>
#include <immintrin.h>

using namespace std;

void addMatrix(const float* A, const float* B, float* Res, const size_t N) {
    for (size_t i = 0; i < N * N; i += 4) {
        __m128 vecA = _mm_loadu_ps(&A[i]);
        __m128 vecB = _mm_loadu_ps(&B[i]);
        __m128 vecRes = _mm_add_ps(vecA, vecB);
        _mm_storeu_ps(&Res[i], vecRes);
    }
}

void subMatrix(const float* A, const float* B, float* Res, const size_t N) {
    for (size_t i = 0; i < N * N; i += 4) {
        __m128 vecA = _mm_loadu_ps(&A[i]);
        __m128 vecB = _mm_loadu_ps(&B[i]);
        __m128 vecRes = _mm_sub_ps(vecA, vecB);
        _mm_storeu_ps(&Res[i], vecRes);
    }
}

void mulMatrix(const float* A, const float* B, float* Res, const size_t N) {
    const size_t blockSize = 64;
    memset(Res, 0, N * N * sizeof(float));
    for (size_t i = 0; i < N; i += blockSize) {
        for (size_t j = 0; j < N; j += blockSize) {
            for (size_t k = 0; k < N; k += blockSize) {
                for (size_t ii = i; ii < min(i + blockSize, N); ++ii) {
                    for (size_t kk = k; kk < min(k + blockSize, N); ++kk) {
                        __m128 vecA = _mm_set1_ps(A[ii * N + kk]);
                        for (size_t jj = j; jj < min(j + blockSize, N); jj += 4) {
                            __m128 vecB = _mm_loadu_ps(&B[kk * N + jj]);
                            __m128 vecRes = _mm_loadu_ps(&Res[ii * N + jj]);
                            vecRes = _mm_add_ps(vecRes, _mm_mul_ps(vecA, vecB));
                            _mm_storeu_ps(&Res[ii * N + jj], vecRes);
                        }
                    }
                }
            }
        }
    }
}

void divMatrix(const float* A, float* Res, const float k, const size_t N) {
    __m128 vecK = _mm_set1_ps(k);
    for (size_t i = 0; i < N * N; i += 4) {
        __m128 vecA = _mm_loadu_ps(&A[i]);
        __m128 vecRes = _mm_div_ps(vecA, vecK);
        _mm_storeu_ps(&Res[i], vecRes);
    }
}

void transposeMatrix(const float* A, float* AT, const size_t N) {
    for (size_t i = 0; i < N; i += 4) {
        for (size_t j = 0; j < N; j += 4) {
            __m128 row0 = _mm_loadu_ps(&A[i * N + j]);
            __m128 row1 = _mm_loadu_ps(&A[(i + 1) * N + j]);
            __m128 row2 = _mm_loadu_ps(&A[(i + 2) * N + j]);
            __m128 row3 = _mm_loadu_ps(&A[(i + 3) * N + j]);
            
            __m128 tmp0 = _mm_unpacklo_ps(row0, row1);
            __m128 tmp1 = _mm_unpackhi_ps(row0, row1);
            __m128 tmp2 = _mm_unpacklo_ps(row2, row3);
            __m128 tmp3 = _mm_unpackhi_ps(row2, row3);
            
            _mm_storeu_ps(&AT[j * N + i], _mm_movelh_ps(tmp0, tmp2));
            _mm_storeu_ps(&AT[(j + 1) * N + i], _mm_movehl_ps(tmp2, tmp0));
            _mm_storeu_ps(&AT[(j + 2) * N + i], _mm_movelh_ps(tmp1, tmp3));
            _mm_storeu_ps(&AT[(j + 3) * N + i], _mm_movehl_ps(tmp3, tmp1));
        }
    }
}

float findMaxAbsSumByRows(const float* A, const size_t N) {
    float res = INT_MIN;
    for (size_t i = 0; i < N; ++i) {
        __m128 vecSum = _mm_setzero_ps();
        size_t j = 0;
        for (; j + 3 < N; j += 4) {
            __m128 vecA = _mm_loadu_ps(&A[i * N + j]);
            vecA = _mm_andnot_ps(_mm_set1_ps(-0.0f), vecA);
            vecSum = _mm_add_ps(vecSum, vecA);
        }
        float sumArr[4];
        _mm_storeu_ps(sumArr, vecSum);
        float sum = sumArr[0] + sumArr[1] + sumArr[2] + sumArr[3];
        for (; j < N; ++j) {
            sum += fabsf(A[i * N + j]);
        }
        res = max(sum, res);
    }
    return res;
}

float findMaxAbsSumByColumns(const float* A, const size_t N) {
    float res = INT_MIN;
    for (size_t i = 0; i < N; ++i) {
        __m128 vecSum = _mm_setzero_ps();
        for (size_t j; j + 3 < N; j += 4) {
            __m128 vecA0 = _mm_setr_ps(A[j * N + i], A[(j + 1) * N + i], A[(j + 2) * N + i], A[(j + 3) * N + i]);
            vecA0 = _mm_andnot_ps(_mm_set1_ps(-0.0f), vecA0);
            vecSum = _mm_add_ps(vecSum, vecA0);
        }
        float sumArr[4];
        _mm_storeu_ps(sumArr, vecSum);
        float sum = sumArr[0] + sumArr[1] + sumArr[2] + sumArr[3];
        for (size_t; j < N; ++j) {
            sum += fabsf(A[j * N + i]);
        }
        res = max(sum, res);
    }
    return res;
}

void initIdentityMatrix(float* Res, const size_t N) {
    memset(Res, 0, N * N * sizeof(float));
    __m128 zero = _mm_setzero_ps();
    __m128 one = _mm_set_ss(1.0f);
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; j += 4) {
            _mm_storeu_ps(&Res[i * N + j], zero);
        }
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
    divMatrix(AT, B, findMaxAbsSumByColumns(A, N) * findMaxAbsSumByRows(A, N), N);
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
    delete[] A;
    delete[] A_inv;
    cout << "Программа завершена." << endl;
    return 0;
}
