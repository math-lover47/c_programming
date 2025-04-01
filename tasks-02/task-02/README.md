# Floating-Point to Binary Converter

A C utility that converts decimal floating-point numbers to their binary representation, handling both integer and fractional parts with configurable precision.

## Features

- Converts positive/negative numbers
- Handles both integer and fractional parts
- Configurable precision threshold
- Residual fraction reporting
- Memory-safe with buffer size limits

## Usage

### Compilation

```bash
gcc -o start main.c -lm
```

### Command Line

```bash
./start [number]
```

### Interactive Mode

```bash
./start
> Enter a decimal number: 12.375
12.375 => 1100.011
```

## API

```c
/**
 * @brief Converts decimal to binary representation
 * @param number Input decimal number
 * @param binary Output buffer (min 64 chars recommended)
 * @param length Output length of binary string
 */
void convert_decimal_to_binary(double number, char *binary, int *length);
```

## Test Cases

### Basic Conversions

| Decimal | Binary                                |
| ------- | ------------------------------------- |
| 0       | 0                                     |
| 5       | 101                                   |
| -3      | -11                                   |
| 12.375  | 1100.011                              |
| 0.1     | 0.000110011001... (residue: 0.006250) |

### Edge Cases

| Input         | Output             |
| ------------- | ------------------ |
| NaN           | nan                |
| INF           | inf                |
| -INF          | -inf               |
| 1.797693e+308 | 111...(1024 bits)  |
| 4.940656e-324 | 0.000...(denormal) |

## Precision Control

The conversion stops when either:

1. Fractional residue < 0.005 (default threshold)
2. Maximum binary digits (64) reached

Example with residue:

```bash
0.2 => 0.0011001100110011001101 (residue: 0.003906)
```

## Limitations

1. **Precision**: Some fractions can't be represented exactly in binary
2. **Range**: Limited by `double` type precision (~15 decimal digits)
3. **Performance**: Not optimized for bulk conversions

## Example Outputs

```bash
$ ./start 18.75
18.75 => 10010.11

$ ./start -0.333333333
-0.333333333 => -0.010101010101010101011 (residue: 0.000000238)

$ ./start 3.141592653589793
3.141593 => 11.00100100001111110111 (residue: 0.0000000000035527)
```

## Development

To modify:

1. Adjust `RESIDUE_THRESHOLD` in converter.h for different precision
2. Change `MAX_BINARY_DIGITS` for longer outputs
3. Comment or remove print lines to see only time complexity information
