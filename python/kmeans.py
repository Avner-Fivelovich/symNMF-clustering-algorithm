#!/usr/bin/python3
# Copied from homework 1.

from sys import argv
from math import sqrt
from statistics import mean


K_ERROR = "Invalid number of clusters!"
ITER_ERROR = "Invalid maximum iterations!"
GENERIC_ERROR = "An Error Has Occured"


EPSILON = 1e-4
DEFAULT_ITER = "300"


class BadK(Exception):
    pass


class BadIter(Exception):
    pass


def string_to_vector(s):
    nums = s.split(",")
    return tuple(float(n) for n in nums)


def read_args():
    if len(argv) == 3:
        _, k, input_file = argv
        iterations = DEFAULT_ITER
    elif len(argv) == 4:
        _, k, iterations, input_file = argv
    else:
        raise RuntimeError("bad number of args")
    with open(input_file) as f:
        data = f.readlines()
    data = [string_to_vector(v) for v in data]
    assert len({len(v) for v in data}) == 1
    try:
        k = int(k)
    except ValueError:
        raise BadK()
    if not 1 < k < len(data):
        raise BadK()
    try:
        iterations = int(iterations)
    except ValueError:
        raise BadIter()
    if not 1 < iterations < 1000:
        raise BadIter()
    return (k, iterations, data)


def distance(v1, v2):
    return sqrt(sum((a - b)**2 for a, b in zip(v1, v2)))


def vmean(vectors):
    return tuple(map(mean, zip(*vectors)))


def kmeans(k, iterations, data):
    centroids = data[:k]
    for i in range(iterations):
        clusters = [[] for i in range(k)]
        for v in data:
            nearest = min(range(k), key=lambda i: distance(v, centroids[i]))
            clusters[nearest].append(v)
        new_centroids = list(map(vmean, clusters))
        if all(distance(new, old) < EPSILON
                for new, old in zip(new_centroids, centroids)):
            break
        centroids = new_centroids
    return centroids


def print_output(centroids):
    for c in centroids:
        print(",".join("%.4f" % x for x in c))


def main():
    k, iterations, data = read_args()
    centroids = kmeans(k, iterations, data)
    print(centroids)


if __name__ == "__main__":
    try:
        main()
    except BadK:
        print(K_ERROR)
        exit(1)
    except BadIter:
        print(ITER_ERROR)
        exit(1)
    except:
        print(GENERIC_ERROR)
        exit(1)

