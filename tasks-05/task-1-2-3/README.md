# Pointer Operations Program

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Command Options](#command-options)
- [Test Cases](#test-cases)
- [Performance](#performance)

## Overview

This program implements fundamental pointer-based data structures and algorithms, focusing on linked lists and tree operations. It demonstrates key concepts such as linked list manipulation, tree traversal, and recursive depth calculation, with optional parallel processing support.

## Features

- **Linked List Operations**: Create, reverse, and visualize linked lists
- **Binary Tree Operations**: Create and measure depth of binary trees
- **M-branch Tree Support**: Create and analyze trees with variable branching factors
- **Tree Visualization**: ASCII-based tree structure visualization
- **Multi-threading Support**: OpenMP implementation for parallel tree creation
- **Configurable Parameters**: Adjustable depth, branch count, and thread count

## Installation

1. Ensure you have GCC and OpenMP installed
2. Clone this repository
3. Compile the program with:

```bash
make
```

## Usage

```bash
./start [OPTIONS]
```

## Command Options

| Option                    | Description                                     |
| ------------------------- | ----------------------------------------------- |
| `-h, --help`              | Show this help message                          |
| `-r, --reverse-list N`    | Create and reverse a list of length N (1-26)    |
| `-l, --list-operations N` | Perform operations on a list of length N (1-26) |
| `-b, --binary-depth`      | Calculate depth of a binary tree                |
| `-m, --mbranch-depth M`   | Calculate depth of M-branch tree                |
| `-g, --generate-tree`     | Generate and visualize a random tree            |
| `-d, --depth D`           | Specify tree depth (1-10)                       |
| `-o, --output FILE`       | Output file (default: out.txt)                  |
| `-v, --verbose`           | Show verbose output including timing            |
| `-t, --threads N`         | Number of threads (default: 4)                  |
| `-np, --no-parallel`      | Disable parallel processing                     |
| `-s, --show-original`     | For list operations, preserve original list     |

## Test Cases

### 1. Linked List Reverse

```bash
# Basic list reversal
./start -r 5
Expected:
Original list: A->B->C->D->E->NIL
Reversed list: E->D->C->B->A->NIL

# List with maximum length
./start -r 26 -o alphabet.txt
Expected:
Original list: A->B->C->...->Z->NIL
Reversed list: Z->Y->X->...->A->NIL

# Invalid list length
./start -r 30
Expected:
Error: List length must be between 1 and 26
```

### 2. List Operations with Preservation

```bash
# Show original list after operations
./start -l 4 -s -v
Expected:
Original list: A->B->C->D->NIL
Reversed list: D->C->B->A->NIL
Original list preserved: A->B->C->D->NIL
Execution time: 0.0001 seconds
Thread count: 4
```

### 3. Binary Tree Depth

```bash
# Random binary tree (verbose)
./start -b -v
Expected:
Binary tree structure (depth=X):
[Tree visualization]
Binary tree depth: X
Execution time: 0.XXXX seconds
Thread count: 4

# Specific depth binary tree
./start -b -d 5 -o binary_tree.txt
Expected:
Binary tree depth: 5
```

### 4. M-branch Tree Depth

```bash
# 3-branch tree
./start -m 3 -d 4
Expected:
M-branch tree depth (M=3): 4

# 5-branch tree with visualization
./start -m 5 -d 3 -v
Expected:
M-branch tree structure (M=5, depth=3):
[Tree visualization]
M-branch tree depth (M=5): 3
Execution time: 0.XXXX seconds
Thread count: 4
```

### 5. Generate Tree

```bash
# Random tree generation
./start -g
Expected:
Generated tree (branches=X, depth=Y):
[Tree visualization]

# Specific tree generation
./start -g -m 4 -d 3 -v -o tree.txt
Expected:
Generated tree (branches=4, depth=3):
[Tree visualization]
```

### 6. Thread Control

```bash
# Use 8 threads
./start -b -d 7 -t 8 -v
Expected:
Binary tree structure (depth=7):
[Tree visualization]
Binary tree depth: 7
Execution time: 0.XXXX seconds
Thread count: 8

# Disable parallel processing
./start -m 3 -d 5 -np -v
Expected:
M-branch tree structure (M=3, depth=5):
[Tree visualization]
M-branch tree depth (M=3): 5
Execution time: 0.XXXX seconds
Thread count: 4
```

### 7. Combined Options

```bash
# Generate deep tree with many branches
./start -g -m 5 -d 6 -t 12 -o big_tree.txt -v
Expected:
Generated tree (branches=5, depth=6):
[Tree visualization]
Execution time: 0.XXXX seconds
Thread count: 12
Output written to big_tree.txt
```

## Performance

The program uses OpenMP for parallel tree creation:

- **Thread Scaling**: Tree creation performance scales linearly with thread count up to hardware limits
- **Memory Usage**: Increases exponentially with tree depth and branch count
- **Performance Examples**:
  - Binary tree (depth 8): ~0.01s with 4 threads
  - 5-branch tree (depth 4): ~0.05s with 4 threads
  - 5-branch tree (depth 8): ~2.5s with 8 threads

Performance comparisons with/without OpenMP:

| Tree Type | Depth | Branches | Serial Time | Parallel Time (8 threads) |
| --------- | ----- | -------- | ----------- | ------------------------- |
| Binary    | 8     | 2        | 0.042s      | 0.008s                    |
| M-branch  | 5     | 3        | 0.135s      | 0.030s                    |
| M-branch  | 4     | 5        | 0.156s      | 0.034s                    |
