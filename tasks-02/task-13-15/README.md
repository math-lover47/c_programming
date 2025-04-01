# Number Operations Program

A versatile command-line tool for performing various number operations including digit reversal and finding numbers with specific digit sums, with support for parallel processing and customizable output formatting.

## Features

- **Digit Reversal**: Reverse the digits of a given number or a range of numbers
- **Digit Sum Filter**: Find numbers whose digits sum to a specified value
- **Parallel Processing**: Multi-threaded execution for improved performance
- **Customizable Output**: Control the number of values per line and their display width
- **Interactive Mode**: Accepts user input when no arguments are provided

## Installation

1. Ensure you have GCC installed on your system
2. Clone this repository or download the source file
3. Compile the program with:

```bash
make
```

## Usage

### Basic Commands

```bash
# Show help message
./start -h

# Reverse a single number
./start -r 12345

# Find numbers 1-1000 with digit sum of 25
./start -l 1000 -s 25

# Reverse all numbers from 1-100
./start -l 100 -r 0
```

### Advanced Options

```bash
# Use 4 threads for processing
./start -l 1000000 -s 25 -t 4

# Custom output (5 numbers per line, width 10)
./start -l 1000 -s 25 --row 5 -w 10

# Interactive mode (enter number when prompted)
./start
```

### Command Line Options

| Option            | Description                                    | Default Value |
| ----------------- | ---------------------------------------------- | ------------- |
| `-h, --help`      | Show help message                              | N/A           |
| `-l, --limit N`   | Upper limit for number generation              | 0             |
| `-r, --reverse N` | Reverse digits of N (0 reverses range from -l) | N/A           |
| `-s, --sum N`     | Find numbers with digit sum N                  | 25            |
| `--row N`         | Numbers per line in output                     | 10            |
| `-w, --width N`   | Width for each number in output                | 8             |
| `-t, --threads N` | Number of threads to use                       | 1             |

## Examples

1. **Reverse a single number**:

   ```bash
   $ ./start -r 810
   18
   ```

2. **Find numbers with digit sum 30 between 1-10000** (10 per line):

   ```bash
   $ ./start -l 10000 -s 30
       3999     4899     4989     4998     5799     5889     5898     5979     5988     5997
       6699     6789     6798     6879     6888     6897     6969     6978     6987     6996
       ... (output continues)
   ```

3. **Reverse all numbers 1-20** (5 per line, width 4):
   ```bash
   $ ./start -l 20 -r 0 --row 5 -w 4
      1    2    3    4    5
      6    7    8    9    1
     11   21   31   41   51
     61   71   81   91    2
     12   22   32   42   52
     62   72   82   92    3
     13   23   33   43   53
     63   73   83   93    4
     14   24   34   44   54
     64   74   84   94    5
     15   25   35   45   55
     65   75   85   95    6
     16   26   36   46   56
     66   76   86   96    7
     17   27   37   47   57
     67   77   87   97    8
     18   28   38   48   58
     68   78   88   98    9
     19   29   39   49   59
     69   79   89   99    2
   ```

## Performance Notes

- For large ranges (N > 1,000,000), use the `-t` option to specify multiple threads
- The program uses OpenMP for parallel processing
- Memory usage increases with larger ranges due to result collection
