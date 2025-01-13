#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>
#include "symnmf.h"

/* Creates a new matrix of the given shape, filled with zeros. */
Matrix matrix_new(size_t width, size_t height) {
    Matrix result;
    result.width = width;
    result.height = height;
    result.data = calloc(width * height, sizeof(double));
    assert(result.data);
    return result;
}

/* Frees the memory previously allocated by matrix_new.
 * Does nothing if the matrix is matrix_null.
 */
void matrix_delete(Matrix *matrix) {
    free(matrix->data);
}

/* Gets two matrices of the same shape.
 * Copies the contents of the second matrix into the first.
 */
void matrix_copy(Matrix *dst, Matrix const *src) {
    assert(src->width == dst->width);
    assert(src->height == dst->height);
    memcpy(dst->data, src->data, src->width * src->height * sizeof(double));
}

/* Gets a matrix, an x coordinate, and a y coordinate.
 * Returns a pointer to the cell at those coordinates.
 */
static double *matrix_cell(Matrix *matrix, size_t x, size_t y) {
    return &matrix->data[matrix->width*y + x];
}

/* Gets a matrix, an x coordinate, and a y coordinate.
 * Returns the value of the cell at those coordinates.
 */
static double matrix_get(Matrix const *matrix, size_t x, size_t y) {
    return *matrix_cell((Matrix*)matrix, x, y);
}

/* Returns a pointer to the yth row in the matrix. */
static double const *matrix_row(Matrix const *matrix, size_t y) {
    return matrix->data + matrix->width * y;
}

/* Gets two matrices.
 * Copies the first matrix into the second, transposed.
 * The second matrix must have the correct shape.
 */
static void matrix_transpose(Matrix const *src, Matrix *dst) {
    size_t x, y;
    assert(src->width == dst->height);
    assert(src->height == dst->width);
    for (x = 0; x < src->width; x++) {
        for (y = 0; y < src->height; y++) {
            *matrix_cell(dst, y, x) = matrix_get(src, x, y);
        }
    }
}

/* Gets two arrays of doubles that represent vectors, and their length.
 * Returns the distance between the vectors squared.
 */
static double norm_squared(
    double const *vector1,
    double const *vector2,
    size_t length
) {
    double result = 0;
    size_t i;
    for (i = 0; i < length; i++) {
        double diff = vector1[i] - vector2[i];
        result += diff * diff;
    }
    return result;
}

/* Gets an array of doubles and its length.
 * Returns its sum.
 */
static double sum(double const *vector, size_t length) {
    double result = 0;
    size_t i;
    for (i = 0; i < length; i++) {
        result += vector[i];
    }
    return result;
}

/* Gets three matrices.
 * Writes the product of the first two matrices into the third matrix.
 * The matrices must have valid shapes.
 */
static void matrix_product(
    Matrix const *left,
    Matrix const *right,
    Matrix *output
) {
    size_t x, y, i;
    assert(left->width == right->height);
    assert(output->width == right->width);
    assert(output->height == left->height);
    for (x = 0; x < output->width; x++) {
        for (y = 0; y < output->height; y++) {
            double cell = 0;
            for (i = 0; i < left->width; i++) {
                double left_cell = matrix_get(left, i, y);
                double right_cell = matrix_get(right, x, i);
                cell += left_cell * right_cell;
            }
            *matrix_cell(output, x, y) = cell;
        }
    }
}

/* Gets two matrices.
 * Returns the euclidean norm of the difference between the matrices, squared.
 */
static double matrix_distance_squared(Matrix const *m1, Matrix const *m2) {
    double result = 0;
    size_t x, y;
    assert(m1->width == m2->width);
    assert(m1->height == m2->height);
    for (x = 0; x < m1->width; x++) {
        for (y = 0; y < m1->height; y++) {
            double diff = matrix_get(m1, x, y) - matrix_get(m2, x, y);
            result += diff * diff;
        }
    }
    return result;
}

/* Gets a diagonal matrix.
 * Replaces every cell c in its main diagonal with 1/sqrt(c).
 */
static void diagonal_matrix_recip_sqrt(Matrix *mat) {
    size_t i;
    size_t n = mat->width;
    assert(mat->width == mat->height);
    for (i = 0; i < n; i++) {
        double *cell = matrix_cell(mat, i, i);
        *cell = 1 / sqrt(*cell);
    }
}

/* Gets a list of points, stored as rows in a matrix.
 * Returns their similarity matrix.
 */
Matrix similarity_matrix(Matrix const *points) {
    size_t d = points->width;
    size_t n = points->height;
    size_t x, y;
    Matrix result = matrix_new(n, n);
    for (x = 0; x < n; x++) {
        for (y = 0; y < n; y++) {
            double const *xrow, *yrow;
            double xyvalue;
            if (x == y) continue;
            xrow = matrix_row(points, x);
            yrow = matrix_row(points, y);
            xyvalue = exp(-norm_squared(xrow, yrow, d) / 2);
            *matrix_cell(&result, x, y) = xyvalue;
        }
    }
    return result;
}

/* Gets the similarity matrix A.
 * Returns the degree matrix D.
 */
Matrix degree_matrix(Matrix const *similarity) {
    size_t n = similarity->width;
    size_t i;
    double const *row;
    Matrix result = matrix_new(n, n);
    for (i = 0; i < n; i++) {
        row = matrix_row(similarity, i);
        *matrix_cell(&result, i, i) = sum(row, n);
    }
    return result;
}

/* Gets the similarity matrix A and the degree matrices D.
 * Returns the normalized similarity matrix W.
 */
Matrix normalized_similarity(Matrix const *similarity, Matrix const *degree) {
    Matrix d1, d1a, result;
    size_t n = similarity->width;
    assert(similarity->height == n);
    assert(degree->width == n);
    assert(degree->height == n);
    d1 = matrix_new(degree->width, degree->height);
    matrix_copy(&d1, degree);
    diagonal_matrix_recip_sqrt(&d1);
    d1a = matrix_new(n, n);
    matrix_product(&d1, similarity, &d1a);
    result = matrix_new(n, n);
    matrix_product(&d1a, &d1, &result);
    matrix_delete(&d1);
    matrix_delete(&d1a);
    return result;
}

/* Constants from the instructions. */
static double const beta = 0.5;
static int const max_iter = 300;
static double const epsilon = 1e-4;

/* All the objects necessary for the optimization process. */
typedef struct {
    /* The matrix being optimized (H). */
    Matrix matrix;

    /* The normalized similarity matrix W. */
    Matrix const *normalized;

    /* An n-by-n matrix that stores temporary data. */
    Matrix tmp_nn;

    /* An n-by-k matrix that stores temporary data. */
    Matrix tmp_nk1;

    /* An n-by-k matrix that stores temporary data. */
    Matrix tmp_nk2;

    /* A k-by-n matrix that stores temporary data. */
    Matrix tmp_kn;
} Optimizer;

/* Gets the initial matrix H(0) and the normalized similarity matrix.
 * Creates a new Optimizer object.
 * The returned Optimizer owns the initial matrix
 * and contains a pointer to the normalized similarity matrix.
 */
static Optimizer optimizer_new(Matrix init, Matrix const *normalized) {
    size_t n = init.height;
    size_t k = init.width;
    Optimizer result;
    result.matrix = init;
    result.normalized = normalized;
    result.tmp_nn = matrix_new(n, n);
    result.tmp_nk1 = matrix_new(k, n);
    result.tmp_nk2 = matrix_new(k, n);
    result.tmp_kn = matrix_new(n, k);
    return result;
}

/* Runs a single step in the optimization process. */
static void optimizer_update(Optimizer *opt) {
    size_t x, y;
    matrix_product(opt->normalized, &opt->matrix, &opt->tmp_nk1);
    matrix_transpose(&opt->matrix, &opt->tmp_kn);
    matrix_product(&opt->matrix, &opt->tmp_kn, &opt->tmp_nn);
    matrix_product(&opt->tmp_nn, &opt->matrix, &opt->tmp_nk2);
    for (x = 0; x < opt->matrix.width; x++) {
        for (y = 0; y < opt->matrix.height; y++) {
            double *cell = matrix_cell(&opt->matrix, x, y);
            double a = matrix_get(&opt->tmp_nk1, x, y);
            double b = matrix_get(&opt->tmp_nk2, x, y);
            *cell *= 1 - beta + beta*a/b;
        }
    }
}

/* Finishes the optimization process:
 * frees all the memory allocated by optimizer_new,
 * and returns the optimized matrix.
 */
static Matrix optimizer_finish(Optimizer *optimizer) {
    matrix_delete(&optimizer->tmp_nn);
    matrix_delete(&optimizer->tmp_nk1);
    matrix_delete(&optimizer->tmp_nk2);
    matrix_delete(&optimizer->tmp_kn);
    return optimizer->matrix;
}

/* Gets an initial random matrix H(0) and the normalized similarity matrix W.
 * Optimizes H(0) and returns the final H.
 * The returned matrix will reuse the allocation from H(0).
 */
Matrix optimize(Matrix init, Matrix const *normalized) {
    int i;
    Optimizer opt = optimizer_new(init, normalized);
    Matrix old = matrix_new(init.width, init.height);
    for (i = 0; i < max_iter; i++) {
        matrix_copy(&old, &opt.matrix);
        optimizer_update(&opt);
        if (matrix_distance_squared(&old, &opt.matrix) < epsilon) break;
    }
    matrix_delete(&old);
    return optimizer_finish(&opt);
}

/* Prints a matrix. */
static void matrix_print(Matrix const *matrix) {
    size_t x, y;
    for (y = 0; y < matrix->height; y++) {
        for (x = 0; x < matrix->width; x++) {
            char end = x == matrix->width - 1 ? '\n' : ',';
            printf("%.4f%c", matrix_get(matrix, x, y), end);
        }
    }
}

/* Returns 1 if s contains only space characters and control characters.
 * Returns 0 otherwise.
 */
static int is_blank(char const *s) {
    for (; *s; s++) {
        if (*s > ' ') return 0;
    }
    return 1;
}

/* Counts c in s. */
static size_t count_char(char const *s, char c) {
    size_t result = 0;
    for (; *s; s++) {
        if (*s == c) result++;
    }
    return result;
}

/* Gets the shape (number of rows and columns) of csv_file.
 * Stores the number of rows in *height, and the number of columns in *width.
 */
static void csv_get_shape(FILE *csv_file, size_t *width, size_t *height) {
    char *line = NULL;
    size_t line_capacity = 0;
    *width = *height = 0;
    while (1) {
        size_t current_width;
        ssize_t n_read = getline(&line, &line_capacity, csv_file);
        if (n_read <= 0) break;
        if (is_blank(line)) continue;
        (*height)++;
        current_width = count_char(line, ',') + 1;
        if (*width == 0) {
            *width = current_width;
        }
        else if (*width != current_width) {
            fputs("Lines have different widths.\n", stderr);
            exit(1);
        }
    }
    free(line);
}

/* Initializes a matrix from a file.
 * The matrix must have the same width and height as the data in the file.
 */
static void init_points(Matrix *points, FILE *input) {
    char *line = NULL;
    size_t line_capacity = 0;
    size_t num_read = 0;
    char const *num_ptr;
    while (getline(&line, &line_capacity, input) > 0)
    {
        if (is_blank(line)) continue;
        num_ptr = strtok(line, ",");
        while (num_ptr)
        {
            points->data[num_read] = atof(num_ptr);
            num_read++;
            num_ptr = strtok(NULL, ",");
        }
    }
    free(line);
}

/* Reads a matrix from a file. */
static Matrix points_from_file(char const *filename) {
    size_t width, height;
    Matrix result;
    FILE *file = fopen(filename, "r");
    assert(file);
    csv_get_shape(file, &width, &height);
    result = matrix_new(width, height);
    rewind(file);
    init_points(&result, file);
    fclose(file);
    return result;
}

/* Gets two arguments from argv: the goal and a file name.
 * The goal must be "sym", "ddg", or "norm".
 * The file must contain a matrix in csv format.
 * Depending on the goal, calculates and outputs the matrix's
 * similarity matrix, degree matrix, or normalized similarity matrix,
 */
int main(int argc, char **argv) {
    char const *goal;
    Matrix points, simil_matrix, deg_matrix, norm_matrix;
    points = simil_matrix = deg_matrix = norm_matrix = matrix_null;
    if (argc != 3) {
        fputs("Expected 2 arguments.\n", stderr);
        abort();
    }
    goal = argv[1];
    points = points_from_file(argv[2]); 
    simil_matrix = similarity_matrix(&points);
    if (!strcmp(goal, "sym")) {
        matrix_print(&simil_matrix);
    }
    else if (!strcmp(goal, "ddg")) {
        deg_matrix = degree_matrix(&simil_matrix);
        matrix_print(&deg_matrix);
    }
    else if (!strcmp(goal, "norm")) {
        deg_matrix = degree_matrix(&simil_matrix);
        norm_matrix = normalized_similarity(&simil_matrix, &deg_matrix);
        matrix_print(&norm_matrix);
    }
    else {
        fputs("goal error", stderr);
        abort();
    }
    matrix_delete(&points);
    matrix_delete(&simil_matrix);
    matrix_delete(&deg_matrix);
    matrix_delete(&norm_matrix);
    return 0;
}

