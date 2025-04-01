# Number Operations Program - Advanced

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Test Cases](#test-cases)
- [Performance](#performance)

## Features

This program provides several advanced number operations:

- **Prime Number Detection**: Checks if numbers are prime with efficient algorithm
- **Fibonacci Sequence Generation**: Generates Fibonacci numbers with filters
- **Digit Analysis**: Detects duplicate digits in numbers
- **Multi-threading Support**: Uses OpenMP for parallel processing
- **Custom Formatting**: Adjustable output width and row size

## Installation

1. Ensure you have GCC and OpenMP installed
2. Compile the program with:

```bash
make
```

## Usage

```bash
./start [OPTIONS]
```

### Options:

| Option                     | Description                                   |
| -------------------------- | --------------------------------------------- |
| `-h, --help`               | Show help message                             |
| `-p, --primes`             | Print primes up to 1000                       |
| `-f, --fibonacci`          | Print Fibonacci numbers ≤10000 divisible by 5 |
| `-c, --check-prime N`      | Check if N is prime                           |
| `-d, --check-duplicates N` | Check if N has duplicate digits               |
| `-t, --threads N`          | Set number of threads (default: 4)            |
| `-w, --width N`            | Set output width (default: 5)                 |
| `-r, --row N`              | Set numbers per row (default: 10)             |

## Test Cases

### 1. Prime Number Verification

```bash
# Test small prime
./start -c 17
Expected: "17 is prime."

# Test small non-prime
./start -c 15
Expected: "15 is not prime."

# Test edge case (1)
./start -c 1
Expected: "1 is not prime."

# Test large prime (104729)
./start -c 104729
Expected: "104729 is prime."
```

### 2. Duplicate Digit Detection

```bash
# Test with duplicates
./start -d 112233
Expected: "112233 has duplicate digits."

# Test without duplicates
./start -d 123456789
Expected: "123456789 does not have duplicate digits."

# Test single digit
./start -d 7
Expected: "7 does not have duplicate digits."
```

### 3. Prime Number Generation

```bash
# Generate primes with default settings
./start -p
Expected: Prints all primes 2-1000 in 10 columns of width 5

# Generate primes with custom formatting
./start -p -w 6 -r 5
Expected: Prints primes in 5 columns of width 6
```

### 4. Fibonacci Sequence

```bash
# Generate Fibonacci numbers divisible by 5
./start -f
Expected: Prints Fibonacci numbers ≤10000 divisible by 5 in format:
     0     5    55   610  6765

# With custom width
./start -f -w 8
Expected: Numbers formatted with width 8:
       0       5      55     610    6765
```

### 5. Combined Operations

```bash
# Run multiple operations
./start -p -f -c 97 -d 123123
Expected:
1. Prints primes 2-1000
2. Prints Fibonacci numbers
3. "97 is prime."
4. "123123 has duplicate digits."
```

### 6. Performance Testing

```bash
# Single-threaded
time ./start -p -t 1

# Multi-threaded
time ./start -p -t 8
Expected: Multi-threaded version should be significantly faster
```

## Performance

The program is optimized for performance:

- Prime checking uses trial division up to √n (O(√n) complexity)
- Fibonacci generation uses O(n) iterative approach
- Parallel processing with OpenMP for prime generation
- Memory-efficient storage of results

Typical performance (on i7-10750H CPU):

- Prime generation (1-1000): ~0.001s (single-threaded), ~0.0003s (8 threads)
- Fibonacci generation: ~0.0001s
- Large prime check (104729): ~0.00001s
