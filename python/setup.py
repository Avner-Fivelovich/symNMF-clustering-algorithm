#!/usr/bin/python3

from setuptools import Extension, setup


# Define the extension module
module = Extension(
    "symnmflib",  # The name of the module
    sources=["symnmf.c", "symnmfmodule.c"],  # Source files for the module
    include_dirs=["."],  # Include the current directory for headers
)


# Setup configuration
setup(
    name="symnmflib",
    version="1.0",
    description="Symmetric Non-negative Matrix Factorization (SymNMF) and related matrix operations",
    ext_modules=[module],
)

