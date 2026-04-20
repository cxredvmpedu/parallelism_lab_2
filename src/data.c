#define _XOPEN_SOURCE 800

#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "data.h"

long min(long a, long b) { return a < b ? a : b; }

// Інізіалізує стан для ГПВЧ
void state_init(unsigned short seed[3]) {
    time_t t = time(nullptr);

    seed[0] = t + pthread_self();
    seed[1] = t >> 16;
    seed[2] = t >> 32;
}

// Повертає випадкове число від `low` до `high`
long random_in(unsigned short state[3], long low, long high) {
    return low + nrand48(state) % (high - low + 1);
}

static inline const long* matrix_at(const Matrix* matrix, size_t row,
                                    size_t col) {
    return &matrix->elems[row * matrix->col_n + col];
}

static inline long* matrix_at_mut(Matrix* matrix, size_t row, size_t col) {
    return &matrix->elems[row * matrix->col_n + col];
}

void matrix_init(Matrix* matrix, size_t row_n, size_t col_n) {
    matrix->elems = malloc(row_n * col_n * sizeof(long));
    matrix->row_n = row_n;
    matrix->col_n = col_n;
}

void matrix_destroy(Matrix* matrix) { free(matrix->elems); }

void matrix_init_clone(Matrix* clone, const Matrix* matrix) {
    matrix_init(clone, matrix->row_n, matrix->col_n);
    memcpy(clone->elems, matrix->elems,
           matrix->row_n * matrix->col_n * sizeof(long));
}

void matrix_init_random(Matrix* matrix, size_t row_n, size_t col_n,
                        unsigned short state[3], long low, long high) {
    matrix_init(matrix, row_n, col_n);
    for (size_t i = 0; i < row_n; i++) {
        for (size_t j = 0; j < col_n; j++) {
            *matrix_at_mut(matrix, i, j) = random_in(state, low, high);
        }
    }
}

void matrix_init_slice_cols(Matrix* slice, const Matrix* matrix,
                            size_t start_col, size_t col_n) {
    matrix_init(slice, matrix->row_n, col_n);

    for (size_t i = 0; i < slice->row_n; i++) {
        const long* src_row = matrix->elems + (i * matrix->col_n) + start_col;
        long* dst_row = slice->elems + (i * slice->col_n);
        memcpy(dst_row, src_row, col_n * sizeof(long));
    }
}

void matrix_scale(Matrix* matrix, long scalar) {
    for (size_t i = 0; i < matrix->row_n; i++) {
        for (size_t j = 0; j < matrix->col_n; j++) {
            *matrix_at_mut(matrix, i, j) *= scalar;
        }
    }
}

void matrix_add(Matrix* left, const Matrix* right) {
    assert(left->row_n == right->row_n);
    assert(left->col_n == right->col_n);

    for (size_t i = 0; i < left->row_n; i++) {
        for (size_t j = 0; j < left->col_n; j++) {
            *matrix_at_mut(left, i, j) += *matrix_at(right, i, j);
        }
    }
}

int long_cmp(const void* left, const void* right) {
    long left_int = *(const long*)left;
    long right_int = *(const long*)right;

    if (left_int > right_int) {
        return 1;
    } else if (left_int < right_int) {
        return -1;
    } else {
        return 0;
    }
}

void matrix_sort(Matrix* matrix) {
    qsort(matrix->elems, matrix->row_n * matrix->col_n, sizeof(long), long_cmp);
}

long matrix_min(const Matrix* matrix) {
    long min = *matrix_at(matrix, 0, 0);

    for (size_t i = 0; i < matrix->row_n; i++) {
        for (size_t j = 0; j < matrix->col_n; j++) {
            long curr = *matrix_at(matrix, i, j);
            if (curr < min) {
                min = curr;
            }
        }
    }

    return min;
}

void matrix_init_transposed(Matrix* transposed, const Matrix* matrix) {
    matrix_init(transposed, matrix->col_n, matrix->row_n);

    for (size_t i = 0; i < matrix->row_n; i++) {
        for (size_t j = 0; j < matrix->col_n; j++) {
            *matrix_at_mut(transposed, j, i) = *matrix_at(matrix, i, j);
        }
    }
}

void matrix_init_product(Matrix* product, const Matrix* left,
                         const Matrix* right) {
    assert(left->col_n == right->row_n);

    matrix_init(product, left->row_n, right->col_n);

    for (size_t i = 0; i < product->row_n; i++) {
        for (size_t j = 0; j < product->col_n; j++) {
            long sum = 0;
            for (size_t k = 0; k < left->col_n; k++) {
                sum += *matrix_at(left, i, k) * *matrix_at(right, k, j);
            }
            *matrix_at_mut(product, i, j) = sum;
        }
    }
}

void matrix_fmt(char** str, const Matrix* matrix) {
    // Number: 1 + floor(log10(2^63-1)) = 19 chars
    // Space / newline: 1 char
    // \0: 1 char
    size_t size = (matrix->row_n * matrix->col_n * 19) + 1;
    *str = malloc(size);
    (*str)[0] = '\0';

    char* ptr = *str;
    for (size_t i = 0; i < matrix->row_n; i++) {
        for (size_t j = 0; j < matrix->col_n; j++) {
            ptr += sprintf(ptr, "%li", *matrix_at(matrix, i, j));

            if (j != matrix->col_n - 1) {
                ptr += sprintf(ptr, " ");
            }
        }

        if (i != matrix->row_n - 1) {
            ptr += sprintf(ptr, "\n");
        }
    }
}

const long* vector_at(const Vector* vector, size_t i) {
    return &vector->elems[i];
}

long* vector_at_mut(Vector* vector, size_t i) { return &vector->elems[i]; }

void vector_init(Vector* vector, size_t n) {
    vector->elems = malloc(n * sizeof(long));
    vector->n = n;
}

void vector_destroy(Vector* vector) { free(vector->elems); }

void vector_init_clone(Vector* clone, const Vector* vector) {
    vector_init(clone, vector->n);
    memcpy(clone->elems, vector->elems, vector->n * sizeof(long));
}

void vector_init_random(Vector* vector, size_t n, unsigned short state[3],
                        long low, long high) {
    vector_init(vector, n);
    for (size_t i = 0; i < n; i++) {
        *vector_at_mut(vector, i) = random_in(state, low, high);
    }
}

void vector_init_slice(Vector* slice, const Vector* vector, size_t start,
                       size_t n) {
    vector_init(slice, n);
    const long* src = vector->elems + start;
    long* dst = slice->elems;
    memcpy(dst, src, n * sizeof(long));
}

void vector_scale(Vector* vector, long scalar) {
    for (size_t i = 0; i < vector->n; i++) {
        *vector_at_mut(vector, i) *= scalar;
    }
}

void vector_add(Vector* left, const Vector* right) {
    assert(left->n == right->n);

    for (size_t i = 0; i < left->n; i++) {
        *vector_at_mut(left, i) += *vector_at(right, i);
    }
}

void vector_sort(Vector* vector) {
    qsort(vector->elems, vector->n, sizeof(long), long_cmp);
}

long vector_scalar_product(const Vector* left, const Vector* right) {
    assert(left->n == right->n);

    long product = 0;
    for (size_t i = 0; i < left->n; i++) {
        product += *vector_at(left, i) * *vector_at(right, i);
    }

    return product;
}

long vector_min(const Vector* vector) {
    long min = *vector_at(vector, 0);
    for (size_t i = 0; i < vector->n; i++) {
        long curr = *vector_at(vector, i);
        if (curr < min) {
            min = curr;
        }
    }
    return min;
}

void vector_init_matrix_product(Vector* product, const Vector* left,
                                const Matrix* right) {
    assert(left->n == right->row_n);

    vector_init(product, right->col_n);

    for (size_t i = 0; i < right->col_n; i++) {
        long sum = 0;
        for (size_t j = 0; j < left->n; j++) {
            sum += *vector_at(left, j) * *matrix_at(right, j, i);
        }
        *vector_at_mut(product, i) = sum;
    }
}

void matrix_init_vector_product(Vector* product, const Matrix* left,
                                const Vector* right) {
    assert(left->col_n == right->n);

    vector_init(product, left->row_n);

    for (size_t i = 0; i < left->row_n; i++) {
        long sum = 0;
        for (size_t j = 0; j < left->col_n; j++) {
            sum += *matrix_at(left, i, j) * *vector_at(right, j);
        }
        *vector_at_mut(product, i) = sum;
    }
}

void vector_fmt(char** str, const Vector* vector) {
    // Number: 1 + floor(log10(2^63-1)) = 19 chars
    // Space / newline: 1 char
    // \0: 1 char
    size_t size = (vector->n * 19) + 1;
    *str = malloc(size);
    (*str)[0] = '\0';

    char* ptr = *str;
    for (size_t i = 0; i < vector->n; i++) {
        ptr += sprintf(ptr, "%li", *vector_at(vector, i));

        if (i != vector->n - 1) {
            ptr += sprintf(ptr, " ");
        }
    }
}

void vector_print(const Vector* vector) {
    printf("[");
    for (size_t i = 0; i < vector->n; i++) {
        printf("%li", *vector_at(vector, i));
        if (i != vector->n - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}
