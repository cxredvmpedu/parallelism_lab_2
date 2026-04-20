/*
 * Лабораторна робота 2.1
 * на тему "Бібліотека POSIX Threads. Семафори, м’ютекси, події, критичні
 * секції, атомік-змінні, бар’єри"
 * з дисципліни "Програмне забезпечення високопродуктивних комп’ютерних систем"
 * Варіант: 11
 * Вираз:   e = ((p * (A * MB)) * (B * (MZ * MR)) + min(B)
 * Виконав: Чигрин Віталій Сергійович, студент групи ІМ-32
 * Дата:    01.05.2026
 */

#define _GNU_SOURCE

#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"

#define N 1000
#define P 4
#define LOW -10
#define HIGH 10

// Змінні для вхідних даних
static long p;
static Vector A, B;
static Matrix MB, MR, MZ;

// Змінна результату
static long e;

// Проміжні змінні
static atomic_long a = 0; // A1
static long b = LONG_MAX;

// Засоби синхронізації
static pthread_barrier_t B1;
static pthread_mutex_t M1;
static sem_t S1;

// Обчислює вираз за алгоритмом
// `thread_id` - номер потоку, що виконуватиме обчислення (починаючи від 1).
void compute_expr(int thread_id) {
    size_t chunk = N / P;
    size_t start = (thread_id - 1) * (chunk);
    // Якщо це останній потік, то беремо залишок
    size_t n = (thread_id == P) ? (N - start) : chunk;

    // 1
    Vector vec1;
    Matrix MBH;
    matrix_init_slice_cols(&MBH, &MB, start, n);
    vector_init_matrix_product(&vec1, &A, &MBH);
    vector_scale(&vec1, p);

    Matrix mat1;
    Matrix MRH;
    matrix_init_slice_cols(&MRH, &MR, start, n);
    matrix_init_product(&mat1, &MZ, &MRH);

    Vector vec2;
    vector_init_matrix_product(&vec2, &B, &mat1);

    long ai = vector_scalar_product(&vec1, &vec2);

    matrix_destroy(&mat1);
    matrix_destroy(&MBH);
    matrix_destroy(&MRH);
    vector_destroy(&vec1);
    vector_destroy(&vec2);

    // 2
    a += ai;

    // 3
    Vector BH;
    vector_init_slice(&BH, &B, start, n);
    long bi = vector_min(&BH);

    vector_destroy(&BH);

    // 4
    pthread_mutex_lock(&M1);
    b = min(b, bi);
    pthread_mutex_unlock(&M1);
}

void* run_T1(void* arg) {
    printf("Thread T1 has started\n");

    unsigned short state[3];
    state_init(state);

    // Введення
    matrix_init_random(&MZ, N, N, state, LOW, HIGH);

    // Виведення
    char* MZ_str;
    matrix_fmt(&MZ_str, &MZ);
    printf("MZ:\n%s\n", MZ_str);
    free(MZ_str);

    // Чекати завершення введення
    pthread_barrier_wait(&B1);

    // Обчислити свою частину
    compute_expr(1);

    // Сигнал про завершення обчислення
    sem_post(&S1);

    printf("Thread T1 has finished\n");
    return nullptr;
}

void* run_T2(void* arg) {
    printf("Thread T2 has started\n");

    unsigned short state[3];
    state_init(state);

    // Введення
    vector_init_random(&A, N, state, LOW, HIGH);
    matrix_init_random(&MR, N, N, state, LOW, HIGH);

    // Виведення
    char* A_str;
    char* MR_str;
    vector_fmt(&A_str, &A);
    matrix_fmt(&MR_str, &MR);
    printf("A:\n%s\n", A_str);
    printf("MR:\n%s\n", MR_str);
    free(A_str);
    free(MR_str);

    // Завершення виведення

    // Чекати завершення введення
    pthread_barrier_wait(&B1);

    // Обчислити свою частину
    compute_expr(2);

    // Чекати завершення обчислення трьома іншими задачами
    for (int i = 0; i < 3; i++) {
        sem_wait(&S1);
    }

    // Обчислення та виведення результату
    e = a + b;
    printf("e = %li\n", e);

    printf("Thread T2 has finished\n");
    return nullptr;
}

void* run_T3(void* arg) {
    printf("Thread T3 has started\n");

    unsigned short state[3];
    state_init(state);

    // Введення
    p = random_in(state, LOW, HIGH);
    vector_init_random(&B, N, state, LOW, HIGH);
    matrix_init_random(&MB, N, N, state, LOW, HIGH);

    // Виведення
    char* B_str;
    char* MB_str;
    vector_fmt(&B_str, &B);
    matrix_fmt(&MB_str, &MB);
    printf("p: %li\n", p);
    printf("B:\n%s\n", B_str);
    printf("MB:\n%s\n", MB_str);
    free(B_str);
    free(MB_str);

    // Чекати завершення введення
    pthread_barrier_wait(&B1);

    // Обчислити свою частину
    compute_expr(3);

    // Сигнал про завершення обчислення
    sem_post(&S1);

    printf("Thread T3 has finished\n");
    return nullptr;
}

void* run_T4(void* arg) {
    printf("Thread T4 has started\n");

    // Чекати завершення введення
    pthread_barrier_wait(&B1);

    // Обчислити свою частину
    compute_expr(4);

    // Сигнал про завершення обчислення
    sem_post(&S1);

    printf("Thread T4 has finished\n");
    return nullptr;
}

void lab_2() {
    printf("Lab 2 has started\n");

    // Ініціалізація засобів синхронізації
    pthread_barrier_init(&B1, nullptr, 4);
    pthread_mutex_init(&M1, nullptr);
    sem_init(&S1, 0, 0);

    pthread_t T[4];
    void* (*run[4])(void*) = {
        run_T1,
        run_T2,
        run_T3,
        run_T4,
    };

    // Запуск задач
    for (size_t i = 0; i < 4; i++) {
        pthread_create(&T[i], nullptr, run[i], nullptr);
    }

    // Очікування завершення задач
    for (size_t i = 0; i < 4; i++) {
        pthread_join(T[i], nullptr);
    }

    // Звільнення засобів синхронізації
    pthread_barrier_destroy(&B1);
    pthread_mutex_destroy(&M1);
    sem_destroy(&S1);

    // Звільнення змінних
    matrix_destroy(&MB);
    matrix_destroy(&MR);
    matrix_destroy(&MZ);
    vector_destroy(&A);
    vector_destroy(&B);

    printf("Lab 2 has finished\n");
}

int main() {
    lab_2();
    return EXIT_SUCCESS;
}
