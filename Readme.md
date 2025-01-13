# Symmetric Non-negative Matrix Factorization (SymNMF)

This project implements a clustering algorithm based on symmetric Non-negative Matrix Factorization (symNMF). The implementation includes both Python and C components, with Python C API integration for optimal performance.

## Overview

The project implements the SymNMF algorithm based on the paper by Kuang, Ding, and Park (2012). The algorithm provides an alternative clustering approach to traditional methods like K-means, working directly with a similarity matrix rather than geometric distances.

### Key Features

- Implementation of the complete SymNMF clustering algorithm
- Python interface with C backend for performance
- Matrix operations implementation (similarity, degree, and normalized matrices)
- Comparison tool with K-means clustering
- Custom build system integrating Python and C components

## Requirements

- Python 3.6 or higher
- NumPy
- scikit-learn (for comparison analysis)
- GCC compiler
- Linux environment (tested on Nova server)

## Installation

1. Clone the repository
2. Build the C extension:
```bash
python3 setup.py build_ext --inplace
```
3. Build the C executable:
```bash
make
```

## Usage

The project provides several interfaces for different operations:

### Python Interface

```bash
python3 symnmf.py k goal input_file
```

Parameters:
- `k`: Number of clusters (integer < N)
- `goal`: Operation to perform
  - `symnmf`: Full SymNMF clustering
  - `sym`: Calculate similarity matrix
  - `ddg`: Calculate Diagonal Degree Matrix
  - `norm`: Calculate normalized similarity matrix
- `input_file`: Path to input data file (.txt format)

Example:
```bash
python3 symnmf.py 2 symnmf input_1.txt
```

### C Interface

```bash
./symnmf goal input_file
```

Parameters:
- `goal`: Operation to perform (sym/ddg/norm)
- `input_file`: Path to input data file (.txt format)

Example:
```bash
./symnmf sym input_1.txt
```

### Analysis Tool

Compare SymNMF with K-means clustering:

```bash
python3 analysis.py k input_file
```

Parameters:
- `k`: Number of clusters
- `input_file`: Path to input data file

## Input Format

The input file should be a text file (.txt) containing data points, with:
- One point per line
- Values separated by commas
- All points must have the same dimension
- No headers

Example:
```
1.0,2.0,3.0
4.0,5.0,6.0
7.0,8.0,9.0
```

## Project Structure

- `symnmf.py`: Python interface and main program
- `symnmf.c`: C implementation of core algorithms
- `symnmf.h`: C header file with function declarations
- `symnmfmodule.c`: Python C API wrapper
- `analysis.py`: Comparison tool for SymNMF vs K-means
- `setup.py`: Build configuration for C extension
- `Makefile`: Build script for C components
- `kmeans.py`: K-means implementation for comparison

## Implementation Details

### Matrix Operations

The project implements several key matrix operations:
1. Similarity Matrix Calculation
2. Diagonal Degree Matrix Computation
3. Normalized Similarity Matrix Generation
4. Matrix Optimization using SymNMF algorithm

### Algorithm Parameters

- Beta: 0.5 (update step size)
- Epsilon: 1e-4 (convergence threshold)
- Maximum iterations: 300
- Random seed: 1234 (for reproducibility)

## Error Handling

The program will print "An Error Has Occurred" and terminate in case of:
- Invalid input files
- Memory allocation failures
- Invalid parameters
- Other runtime errors

## Performance Considerations

- The core algorithms are implemented in C for performance
- Matrix operations use efficient row-major order storage
- Memory management is handled carefully to prevent leaks
- The Python interface uses NumPy for efficient data handling

## Contributing

This project was developed as part of a software project course. While it's not actively maintained, improvements and bug fixes are welcome through pull requests.

## License

This project is provided as is, primarily for educational purposes. Please check with your institution before using this code for academic assignments.

## Authors

- Daniel David Goren
- Avner Fivelovich

## Work Division

Our team collaborated efficiently by dividing the work based on our strengths and ensuring tight integration between components:

### Daniel David Goren
- Primary implementation of the C core algorithms (`symnmf.c`)
- Matrix operations and optimization logic
- Memory management and performance optimization

### Avner Fivelovich
- Python interface development (`symnmf.py`)
- Python C API wrapper implementation (`symnmfmodule.c`)
- Analysis tool and K-means comparison (`analysis.py`)
- Build system configuration (`setup.py` and `Makefile`)

### Collaborative Work
- Integration testing and debugging
- Algorithm design and mathematical formulations
- Interface design and API specifications
- Testing and validation
- Documentation and code review

## Acknowledgments

- Based on the paper by Kuang, Ding, and Park (2012)
- Developed as part of Software Project Course (0368-2161)
- Special thanks to the course staff for their guidance
