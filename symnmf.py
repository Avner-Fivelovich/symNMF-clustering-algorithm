#!/usr/bin/python3

from sys import argv
import numpy as np
import symnmflib


# Creates the initial H matrix.
def initialize_H(m, k, n):
    np.random.seed(1234)
    H = np.random.uniform(0, 2 * np.sqrt(m / k), (n, k))
    return H


# Runs the symNMF algorithm on the given matrix, with k clusters.
def symnmf_run(X_input, k):
    norm = np.empty((X_input.shape[0], X_input.shape[0]))
    symnmflib.norm(X_input.ctypes.data, norm.ctypes.data, X_input.shape[1], X_input.shape[0])
    H = initialize_H(np.mean(norm), k, norm.shape[0])
    final_H = np.empty_like(H)
    symnmflib.symnmf(H.ctypes.data, norm.ctypes.data, final_H.ctypes.data, H.shape[1], H.shape[0], norm.shape[1], norm.shape[0])
    return(final_H)


# Prints a matrix in the required format, e.g.
# 1.2345,6.7890
# 0.9876,5.4321
def print_matrix(matrix):
    for y in range(matrix.shape[0]):
        for x in range(matrix.shape[1]):
            end = "\n" if x == matrix.shape[1] - 1 else ","
            print("%.4f" % matrix[y, x], end=end)


# The main function.
def symnmf_main():
    k = int(argv[1])
    goal = argv[2]
    file_name = argv[3]
    X_input = np.genfromtxt(file_name, dtype=np.float64, delimiter=",")
    if goal == "symnmf":
        print_matrix(symnmf_run(X_input, k))
    elif goal == "sym":
        sym = np.empty((X_input.shape[0], X_input.shape[0]))
        symnmflib.sym(X_input.ctypes.data, sym.ctypes.data, X_input.shape[1], X_input.shape[0])
        print_matrix(sym)
    elif goal == "ddg":
        ddg = np.empty((X_input.shape[0], X_input.shape[0]))
        symnmflib.ddg(X_input.ctypes.data, ddg.ctypes.data, X_input.shape[1], X_input.shape[0])
        print_matrix(ddg)
    elif goal == "norm":
        norm = np.empty((X_input.shape[0], X_input.shape[0]))
        symnmflib.norm(X_input.ctypes.data, norm.ctypes.data, X_input.shape[1], X_input.shape[0])
        print_matrix(norm)
    else:
        print("goal error")


if __name__ == "__main__":
    symnmf_main()

