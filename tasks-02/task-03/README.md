# Parallel Pyramid Printer

A C program that generates customizable character pyramids in parallel using OpenMP, with support for colors, random symbols, and multi-threading.

## Features

- Prints two growing pyramids side by side
- Configurable symbols for left and right pyramids
- Random symbol selection option
- ANSI color support for both pyramids
- Parallel processing with OpenMP for large pyramids
- Adjustable number of rows and columns
- Dynamic spacing between pyramids as they grow

## Requirements

- C compiler (GCC recommended)
- OpenMP support (usually included with GCC)
- Linux/macOS (or Windows with proper OpenMP setup)

1. Compile the program:

```bash
make
```

## Usage

Basic command:

```bash
./pyramid_printer [options]
```

### Options

| Option                             | Description                                   | Example                |
| ---------------------------------- | --------------------------------------------- | ---------------------- |
| `-h`, `--help`                     | Show help message                             | `./pyramid_printer -h` |
| `-row N`, `--rows N`               | Set number of rows (1-300)                    | `--rows 10`            |
| `-col N`, `--columns N`            | Set number of columns (1-300)                 | `-col 8`               |
| `-s CHARS`, `--symbols CHARS`      | Set symbols (format: 'LEFT,RIGHT' or 'BOTH')  | `-s "A,B"` or `-s "#"` |
| `-r`, `--random`                   | Use random symbols from provided set          | `-s "ABCDEF" -r`       |
| `-cl COLOR`, `--color-left COLOR`  | Color left pyramid (see colors below)         | `-cl red`              |
| `-cr COLOR`, `--color-right COLOR` | Color right pyramid                           | `--color-right green`  |
| `-t N`, `--threads N`              | Set number of threads for parallel processing | `-t 4`                 |

### Available Colors

- red, green, yellow, blue, magenta, cyan, white

## Examples

1. Basic pyramids (5 rows, A and B):

```bash
./pyramid_printer
```

2. Colorful pyramids with random symbols:

```bash
./pyramid_printer -row 8 -col 6 -s "ABCDEF,XYZ" -r -cl blue -cr magenta
```

3. Large pyramids with parallel processing:

```bash
./pyramid_printer -row 100 -col 50 -s "*" -t 8
```

## Output Example

```
Character Pyramids:

A          B
AA        BB
AAA      BBB
AAAA    BBBB
AAAAA  BBBBB
AAAAAABBBBBB
```

## Performance

The program automatically uses parallel processing when:

- More than 1 thread is specified (`-t N`)
- Pyramid size exceeds 5000 characters (rows × columns > 5000)
