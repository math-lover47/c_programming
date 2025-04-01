# Text Converter

A versatile text processing tool that performs various transformations including case inversion, Caesar cipher encoding/decoding, and Morse code conversion. Supports both interactive input and file processing with parallel execution.

## Features

- Invert character cases (uppercase ↔ lowercase)
- Apply Caesar cipher with custom shift value
- Decode Caesar cipher with custom shift value
- Convert text to Morse code
- Decode Morse code to text
- Process multiple files in parallel
- Specify number of threads for parallel processing

## Installation

Compile the program with GCC (requires OpenMP support):

```bash
make
```

## Usage

```
./start [options]

Options:
  -h, --help              Show help message
  -i, --invert            Invert character cases
  -c [N], --caesar [N]    Apply Caesar cipher (default shift: 13)
  -dc [N], --decode-caesar [N]  Decode Caesar cipher (default shift: 13)
  -m, --morse             Convert text to Morse code
  -dm, --decode-morse     Convert Morse code to text
  -in FILE, --input FILE  Input file(s) (multiple files allowed)
  -out FILE, --output FILE Output file(s) (must match input files count)
  -t N, --threads N       Number of threads for parallel processing (default: 1)
```

## Examples

1. Interactive mode with case inversion and Caesar cipher (shift 5):

```bash
./start -i -c 5
```

2. Convert text to Morse code from input file:

```bash
./start -m -in input.txt -out output.txt
```

3. Decode Caesar cipher (shift 3) and Morse code with 4 threads:

```bash
./start -dc 3 -dm -in encoded.txt -out decoded.txt -t 4
```

4. Multiple file processing with case inversion:

```bash
./start -i -in file1.txt file2.txt -out out1.txt out2.txt
```

## Test Cases

The repository includes three test files:

### text-01.txt

```
Hello World!
This is a test.
12345
```

### text-02.txt

```
The quick brown fox jumps over the lazy dog.
ABCDEFGHIJKLMNOPQRSTUVWXYZ
0123456789
```

### text-03.txt

```
.-.. --- .-.. / .-- .... -.-- / .. ... / - .... .. ... / ... --- / .... .- .-. -.. ..--..
```

### Test Commands and Expected Results

1. Invert case:

```bash
./start -i -in text-01.txt -out inverted.txt
```

Output:

```
hELLO wORLD!
tHIS IS A TEST.
12345
```

2. Caesar cipher (shift 3) + Morse code:

```bash
./start -c 3 -m -in text-02.txt -out morse.txt
```

Output (partial):

```
...-- .... ..- .-.. -.-. -.- -. --- .--. ... --.. --.- ..-. -.-- .--. ...- . .-. ...-- .... ..- .-.. .- -.. ..-- -.. --- --.
.-.. ...-- .-.. -.-. ..--- ..-. --. ...-- .... ..--- .--- --.. .-.. --.. -.-- .--. --..-- -..-. ...-- ----- ...-- .---- ...-- ..--- ...-- ...-- ...-- ....- ...-- ..... ...-- -.... ...-- --... ...-- ---.. ...-- ----.
```

3. Decode Morse code:

```bash
./start -dm -in text-03.txt -out decoded.txt
```

Output:

```
LOL WHY IS THIS SO HARD?
```

4. Combined operations (invert + Caesar 5 + Morse):

```bash
./start -i -c 5 -m -in text-01.txt -out combined.txt
```

Output:

```
.... .--- --... --... --- -..-. .--- --- .-. --. ..--- ..--..
--.. ...- ... .... ... .... ...- --.. ...- - .... ...- - ...--
...-- ..--- ...-- ....- ...-- .....
```

## Performance Notes

- For large files, use the `-t` option to specify multiple threads
- Processing time is displayed at the end of execution
- Output files are created in the same directory as the executable if no path is specified

## Limitations

- Maximum input line length: 4095 characters
- Maximum filename length: 255 characters
- Morse code conversion only handles alphanumeric characters and spaces
