#pragma once

#include <stddef.h>

long min(long a, long b);

// ГПВЧ
void state_init(unsigned short seed[3]);
long random_in(unsigned short state[3], long low, long high);

// Матриця
typedef struct {
    long* elems;
    size_t row_n; // Кількість рядків
    size_t col_n; // Кількість стовпців
} Matrix;

void matrix_init(Matrix* matrix, size_t row_n, size_t col_n);
void matrix_init_clone(Matrix* clone, const Matrix* matrix);
void matrix_init_from_stdin(Matrix* matrix, size_t row_n, size_t col_n);
void matrix_init_random(Matrix* matrix, size_t row_n, size_t col_n,
                        unsigned short state[3], long low, long high);
void matrix_init_slice_cols(Matrix* slice, const Matrix* matrix,
                            size_t start_col, size_t cols_n);
void matrix_destroy(Matrix* matrix);

void matrix_add(Matrix* left, const Matrix* right);
void matrix_scale(Matrix* matrix, long scalar);
void matrix_sort(Matrix* matrix);
long matrix_min(const Matrix* matrix);
void matrix_init_transposed(Matrix* transposed, const Matrix* matrix);
void matrix_init_product(Matrix* product, const Matrix* left,
                         const Matrix* right);

void matrix_fmt(char** str, const Matrix* matrix);

// Вектор
typedef struct {
    long* elems;
    size_t n; // Кількість елементів
} Vector;

void vector_init(Vector* vector, size_t n);
void vector_init_clone(Vector* clone, const Vector* vector);
void vector_init_from_stdin(Vector* vector, size_t n);
void vector_init_random(Vector* vector, size_t n, unsigned short state[3],
                        long low, long high);
void vector_init_slice(Vector* slice, const Vector* vector, size_t start,
                       size_t n);
void vector_destroy(Vector* vector);

void vector_add(Vector* left, const Vector* right);
void vector_scale(Vector* vector, long scalar);
void vector_sort(Vector* vector);
long vector_min(const Vector* vector);
long vector_scalar_product(const Vector* left, const Vector* right);
void vector_init_matrix_product(Vector* product, const Vector* left,
                                const Matrix* right);
void matrix_init_vector_product(Vector* product, const Matrix* left,
                                const Vector* right);

void vector_fmt(char** str, const Vector* vector);
