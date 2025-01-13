#!/usr/bin/python3

from sys import argv
import numpy as np
from  sklearn import metrics
from kmeans import kmeans
from symnmf import symnmf_run


# Gets a matrix that represents points and a matrix that represents clusters.
# Returns a vector that represents the label of each point,
# i.e. the index of the cluster to which each point belongs.
def cluster_indices(X_input, clusters):
    return np.array([
        np.argmin(np.sum((clusters - v) ** 2, axis=1))
        for v in X_input
    ])


# The main function.
def analysis():
    k = int(argv[1])
    file_name = argv[2]
    X_input = np.genfromtxt(file_name, dtype=np.float64, delimiter=",")
    H_final = symnmf_run(X_input, k)
    kmean_centroids = np.array(kmeans(k, 300, X_input))
    print("nmf:", "%.4f" % metrics.silhouette_score(X_input, np.argmax(H_final, axis = 1)))
    print("kmeans:","%.4f" % metrics.silhouette_score(X_input, cluster_indices(X_input, kmean_centroids)))
        

analysis()

