# Date Calculator

A command-line utility for date validation and calculations, supporting:

- Date validation (Gregorian calendar)
- Day-of-year calculation
- Day-of-week calculation
- Date difference from today
- Flexible input formats

## Features

- ✅ Validate any date between 1 Jan 0001 and 31 Dec 9999
- 📅 Calculate day of year (1-366)
- 📆 Determine day of week (Sunday-Saturday)
- ⏳ Compute difference from current date
- 🗓️ Accepts month names (e.g., "january") or numbers
- 🛠️ Handles multiple input formats (`YYYY-MM-DD` or `YYYY MM DD`)

## Installation

### Build from Source

1. Compile using the included Makefile:

   ```bash
   make
   ```

1. Run using compiled file:

   ```bash
   ./datecalc
   ```

## Usage

### Basic Syntax

```bash
./datecalc [OPTIONS] [DATE]
```

### Examples

0. **Help**:

```bash
   ./datecalc --help
```

Output:

```
Date Calculator
Usage: main [options] [date]
Options:
  -h, --help     Show this help message
  -m, --m MONTH  Set month by name (e.g. december)
  -dw, --dw      Calculate day of week
  -dy, --dy      Calculate day of year
  -df, --df      Calculate difference from today

Date format: YYYY MM DD or YYYY-MM-DD
```

1. **Validate a date**:

   ```bash
   ./datecalc 2023-12-25
   ```

   or

   ```bash
   ./datecalc
   ```

   Output:

   ```
   Date is valid
   ```

2. **Calculate day of week**:

   ```bash
   ./datecalc --dw 2023-12-25
   ```

   or

   ```bash
   ./datecalc --dw
   ```

   Output:

   ```
   Day of week: Monday
   ```

3. **Calculate day of year**:

   ```bash
   ./datecalc --dy --m december
   ```

   Output:

   ```
   Day of year: 359
   ```

4. **Date difference from today**:

   ```bash
   ./datecalc --df 2024-01-01
   ```

   or

   ```bash
   ./datecalc --df
   ```

   Output (example):

   ```
   Date difference: 5 days after
   ```

### Command Line Options

| Option            | Description                         |
| ----------------- | ----------------------------------- |
| `-h`, `--help`    | Show help message                   |
| `-m`, `--m MONTH` | Set month by name (e.g., "january") |
| `-dw`, `--dw`     | Calculate day of week               |
| `-dy`, `--dy`     | Calculate day of year               |
| `-df`, `--df`     | Calculate difference from today     |

### Date Formats Accepted

- `YYYY-MM-DD` (e.g., `2023-12-25`)
- `YYYY MM DD` (e.g., `2023 12 25`)
- Partial dates with `-m` flag (e.g., `-m january 15 2023`)

## Dependencies

- C compiler (GCC or Clang)
- GNU Make
- Standard C library (no external dependencies)

## Error Handling

The program will report errors for:

- Invalid dates (e.g., `2023-02-30`)
- Malformed arguments
- Out-of-range values
- Unrecognized month names
