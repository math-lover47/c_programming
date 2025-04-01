# Armstrong Numbers Calculator

## Overview

This program finds and displays Armstrong numbers (also known as narcissistic numbers) within a specified range using parallel processing with OpenMP for improved performance.

An **Armstrong number** is a number that equals the sum of its own digits each raised to the power of the number of digits in the number.

For example:

- 153 = 1³ + 5³ + 3³ = 1 + 125 + 27 = 153
- 1634 = 1⁴ + 6⁴ + 3⁴ + 4⁴ = 1 + 1296 + 81 + 256 = 1634

## Features

- Fast parallel calculation using OpenMP
- Support for ranges up to 10¹⁸ (1 quintillion)
- Configurable thread count for optimal performance
- Thread-local buffers and dynamic scheduling for efficiency
- Command-line interface with argument parsing
- Execution time measurement

## Requirements

- C compiler with C11 support
- OpenMP library
- Math library

## Building

Use the provided Makefile:

```bash
make
```

Or compile manually:

```bash
gcc -Wall -Wextra -std=c11 -fopenmp -o start main.c -lm
```

## Usage

### Command-line Arguments

```
Usage: ./start [options]

Options:
  -h, --help        Show help message
  -n, --num LIMIT   Set upper search limit (1-1000000000000000000)
  -t, --threads N   Set number of threads (1-16)
```

### Interactive Mode

If no limit is provided via command line, the program will prompt for input:

```
Enter upper limit (1-1000000000000000000):
```

### Examples

Find Armstrong numbers up to 10,000 using the default thread count (4):

```
./start -n 10000
```

Find Armstrong numbers up to 1,000,000 using 8 threads:

```
./start -n 1000000 -t 8
```

Show help message:

```
./start --help
```

## Test Cases

### Test Case 1: Small Range (1-1000)

```
./start -n 1000
```

Expected output:

```
Armstrong numbers up to 1000:
1 2 3 4 5 6 7 8 9 153 370 371 407
Time: 0.0005 seconds
```

### Test Case 2: Medium Range (1-10000)

```
./start -n 10000
```

Expected output:

```
Armstrong numbers up to 10000:
1 2 3 4 5 6 7 8 9 153 370 371 407 1634 8208 9474
Time: 0.0023 seconds
```

### Test Case 3: Different Thread Counts

Compare performance with different thread counts:

```
./start -n 1000000 -t 1
./start -n 1000000 -t 4
./start -n 1000000 -t 8
```

Expected result: Execution time should generally decrease as thread count increases, up to the number of physical cores in your system.

### Test Case 4: Large Range (100 million)

```
./start -n 100000000 -t 8
```

This will test the program's ability to handle larger ranges efficiently. Expected output will include additional Armstrong numbers such as 54748, 92727, 93084, etc.

## Performance Notes

- For smaller ranges (under 10,000), the overhead of thread creation may exceed the benefits of parallelization
- For larger ranges, parallel execution can provide significant speedup
- The optimal thread count typically equals the number of physical CPU cores
- Performance may degrade if thread count greatly exceeds available cores

## Algorithm Explanation

The program uses several optimizations:

1. **Thread-local buffers** to minimize synchronization overhead
2. **Dynamic scheduling** for balanced workload across threads
3. **Early termination** during Armstrong check if sum exceeds original number
4. **Efficient power function** using exponentiation by squaring
5. **Logarithmic digit counting** for large numbers

## Known Limitations

- Memory requirements increase with thread count
- For extremely large ranges (approaching 10¹⁸), execution time may be considerable
- Results are limited to the first 100 Armstrong numbers found

## License

This project is open source and available under the MIT License.
