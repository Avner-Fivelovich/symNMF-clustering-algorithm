#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include <string.h>
#include "symnmf.h"

/**
 * Helper function to create a Matrix struct from a NumPy array pointer.
 */
Matrix create_matrix_from_numpy(unsigned long long data_ptr, size_t width, size_t height) {
    Matrix matrix;
    matrix.width = width;
    matrix.height = height;
    matrix.data = (double*)data_ptr;  /* Cast the data pointer to a double array. */
    return matrix;
}

/**
 * Performs symnmf.
 * Returns the result as a new matrix.
 */
Matrix symnmf(const Matrix* H_matrix, const Matrix* W_matrix) {
    Matrix H_copy = matrix_new(H_matrix->width, H_matrix->height);
    matrix_copy(&H_copy, H_matrix);
    return optimize(H_copy, W_matrix);
}

/**
 * Wrapper function for symnmf.
 */
static PyObject* py_symnmf(PyObject* self, PyObject* args) {
    unsigned long long H_matrix_ptr, W_matrix_ptr, H_output_ptr;
    size_t H_width, H_height, W_width, W_height;
    Matrix H_matrix, W_matrix, H_result, out_matrix;

    /* Parse the arguments: pointers to NumPy arrays and dimensions. */
    if (!PyArg_ParseTuple(args, "KKKnnnn", &H_matrix_ptr, &W_matrix_ptr, &H_output_ptr, 
                          &H_width, &H_height, &W_width, &W_height)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments passed to py_symnmf.");
        return NULL;
    }

    /* Create Matrix structs from the provided pointers and dimensions. */
    H_matrix = create_matrix_from_numpy(H_matrix_ptr, H_width, H_height);
    W_matrix = create_matrix_from_numpy(W_matrix_ptr, W_width, W_height);
    H_result = symnmf(&H_matrix, &W_matrix);

    /* Copy the result to the output matrix. */
    out_matrix = create_matrix_from_numpy(H_output_ptr, H_width, H_height);
    matrix_copy(&out_matrix, &H_result);

    /* Free the temporary result matrix. */
    matrix_delete(&H_result);

    Py_RETURN_NONE;
}

/**
 * Wrapper function for similarity_matrix.
 * Note: it's the caller's responsibility to allocate memory for the output matrix.
 */
static PyObject* py_sym(PyObject* self, PyObject* args) {
    unsigned long long X_matrix_ptr, sym_output_ptr;
    size_t X_width, X_height;
    Matrix X_matrix, result, out_matrix;

    /* Parse the arguments: pointer to a NumPy arrays and their dimensions. */
    if (!PyArg_ParseTuple(args, "KKnn", &X_matrix_ptr, &sym_output_ptr, &X_width, &X_height)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments passed to py_sym.");
        return NULL;
    }

    /* Create Matrix struct from the provided pointer and dimensions. */
    X_matrix = create_matrix_from_numpy(X_matrix_ptr, X_width, X_height);
    result = similarity_matrix(&X_matrix);

    /* Copy the result to the output matrix. */
    out_matrix = create_matrix_from_numpy(sym_output_ptr, X_height, X_height);
    matrix_copy(&out_matrix, &result);

    /* Free the temporary result matrix. */
    matrix_delete(&result);

    Py_RETURN_NONE;
}

/**
 * Creates the degree matrix from points.
 */
Matrix ddg(const Matrix* points) {
    Matrix simil_matrix = similarity_matrix(points);
    Matrix result = degree_matrix(&simil_matrix);
    matrix_delete(&simil_matrix);
    return result;
}

/**
 * Wrapper function for ddg using PyObjects.
 */
static PyObject* py_ddg(PyObject* self, PyObject* args) {
    unsigned long long X_matrix_ptr, diag_output_ptr;
    size_t X_width, X_height;
    Matrix X_matrix, result, out_matrix;

    /* Parse the arguments: pointer to a NumPy array and its dimensions. */
    if (!PyArg_ParseTuple(args, "KKnn", &X_matrix_ptr, &diag_output_ptr, &X_width, &X_height)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments passed to py_ddg.");
        return NULL;
    }

    /* Create Matrix struct from the provided pointer and dimensions. */
    X_matrix = create_matrix_from_numpy(X_matrix_ptr, X_width, X_height);
    result = ddg(&X_matrix);

    /* Copy the result to the output matrix. */
    out_matrix = create_matrix_from_numpy(diag_output_ptr, X_height, X_height);
    matrix_copy(&out_matrix, &result);

    /* Free the temporary result matrix. */
    matrix_delete(&result);

    Py_RETURN_NONE;
}

/**
 * Returns the normalized similarity matrix of points.
 */
Matrix norm(const Matrix* points) {
    Matrix simil_matrix = similarity_matrix(points);
    Matrix deg_matrix = degree_matrix(&simil_matrix);
    Matrix result = normalized_similarity(&simil_matrix, &deg_matrix);
    matrix_delete(&simil_matrix);
    matrix_delete(&deg_matrix);
    return result;
}

/**
 * Wrapper function for norm using PyObjects.
 */
static PyObject* py_norm(PyObject* self, PyObject* args) {
    unsigned long long X_matrix_ptr, norm_output_ptr;
    size_t X_width, X_height;
    Matrix X_matrix, result, out_matrix;

    /* Parse the arguments: pointer to a NumPy array and its dimensions. */
    if (!PyArg_ParseTuple(args, "KKnn", &X_matrix_ptr, &norm_output_ptr, &X_width, &X_height)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments passed to py_norm.");
        return NULL;
    }

    /* Create Matrix struct from the provided pointer and dimensions. */
    X_matrix = create_matrix_from_numpy(X_matrix_ptr, X_width, X_height);
    result = norm(&X_matrix);

    /* Copy the result to the output matrix. */
    out_matrix = create_matrix_from_numpy(norm_output_ptr, X_height, X_height);
    matrix_copy(&out_matrix, &result);

    /* Free the temporary result matrix. */
    matrix_delete(&result);

    Py_RETURN_NONE;
}

/**
 * Module method table listing the methods.
 */
static PyMethodDef SymnmfMethods[] = {
    {"symnmf", py_symnmf, METH_VARARGS, "Perform symmetric non-negative matrix factorization (SymNMF)."},
    {"sym", py_sym, METH_VARARGS, "Perform the sym operation."},
    {"ddg", py_ddg, METH_VARARGS, "Perform the ddg operation."},
    {"norm", py_norm, METH_VARARGS, "Perform the norm operation."},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

/**
 * Module definition structure.
 */
static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmflib",  /* Module name */
    "Module providing SymNMF and related matrix operations.",  /* Module documentation */
    -1,  /* Module state size */
    SymnmfMethods
};

/**
 * Module initialization function.
 */
PyMODINIT_FUNC PyInit_symnmflib(void) {
    return PyModule_Create(&symnmfmodule); }

