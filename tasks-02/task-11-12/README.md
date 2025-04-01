# Single Pyramid Printer

A C program that generates customizable centered pyramids with support for colors, random symbols, and parallel processing using OpenMP.

## Features

- Prints perfectly centered pyramid shapes
- Configurable number of rows (1-300)
- Customizable symbols for pyramid construction
- Random symbol selection from provided set
- ANSI color support (8 colors available)
- Parallel processing for large pyramids
- Clean, efficient output formatting

## Requirements

- C compiler (GCC recommended)
- OpenMP support (usually included with GCC)
- Linux/macOS (or Windows with proper OpenMP setup)

## Installation

1. Compile the program:

```bash
make
```

## Usage

Basic command:

```bash
Single Pyramid Printer
Usage: start [options]
Options:
    -h, --help        Show this help message
    -r N, --rows N    Set number of rows (1-300, default: 5)
    -s CHARS, --symbols CHARS  Set symbols to use (default: 'A')
    -rand, --random   Use random symbols from the provided set
    -c COLOR, --color COLOR  Apply color (red, green, blue, etc.)
    -t N, --threads N Set number of threads for parallel processing
```

### Options

| Option                        | Description                                   | Example                  |
| ----------------------------- | --------------------------------------------- | ------------------------ |
| `-h`, `--help`                | Show help message                             | `./start -h`             |
| `-r N`, `--rows N`            | Set number of rows (1-300, default: 5)        | `-r 7`                   |
| `-s CHARS`, `--symbols CHARS` | Set symbols to use (default: 'A')             | `-s "ABC"` or `-s "#$%"` |
| `-rand`, `--random`           | Use random symbols from provided set          | `-s "123" -rand`         |
| `-c COLOR`, `--color COLOR`   | Apply color to pyramid                        | `-c green`               |
| `-t N`, `--threads N`         | Set number of threads for parallel processing | `-t 4`                   |

### Available Colors

- red, green, yellow, blue, magenta, cyan, white

## Examples

1. Basic pyramid with 5 rows:

```bash
./start -r 5
```

Output:

```
    A
   AAA
  AAAAA
 AAAAAAA
AAAAAAAAA
```

2. Colorful pyramid with random symbols:

```bash
./start -r 4 -s "XYZ123" -rand -c magenta
```

Sample Output:

```
   X
  2Y3
 Z1X2Y
3X1Y2Z
```

3. Large pyramid with parallel processing:

```bash
./start -r 20 -s "@#$%" -t 4
```

## Performance Notes

- For pyramids with >50 rows, parallel processing (`-t N`) provides significant speed improvements
- The program automatically calculates proper spacing and centering
- Color output works in most modern terminals
