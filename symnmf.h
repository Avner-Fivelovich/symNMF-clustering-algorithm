#ifndef symnmf_h
#define symnmf_h

#include <stdlib.h>

/* A two-dimensional array of doubles.
 * Stored in a row-major order.
 */
typedef struct {
    size_t width;
    size_t height;
    double *data;
} Matrix;

/* A placeholder value for a matrix. */
static Matrix const matrix_null = {0};

/* Creates a new matrix of the given shape, filled with zeros. */
Matrix matrix_new(size_t width, size_t height);

/* Frees the memory previously allocated by matrix_new.
 * Does nothing if the matrix is matrix_null.
 */
void matrix_delete(Matrix *matrix);

/* Gets two matrices of the same shape.
 * Copies the contents of the second matrix into the first.
 */
void matrix_copy(Matrix *dst, Matrix const *src);

/* Gets a list of points, stored as rows in a matrix.
 * Returns their similarity matrix.
 */
Matrix similarity_matrix(Matrix const *points);

/* Gets the similarity matrix A.
 * Returns the degree matrix D.
 */
Matrix degree_matrix(Matrix const *similarity);

/* Gets the similarity matrix A and the degree matrices D.
 * Returns the normalized similarity matrix W.
 */
Matrix normalized_similarity(Matrix const *similarity, Matrix const *degree);

/* Gets an initial random matrix H(0) and the normalized similarity matrix W.
 * Optimizes H(0) and returns the final H.
 * The returned matrix will reuse the allocation from H(0).
 */
Matrix optimize(Matrix init, Matrix const *normalized);

#endif

