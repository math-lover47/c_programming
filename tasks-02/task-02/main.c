#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define MAX_INPUT_LEN 256       // Maximum length for user input
#define MAX_THREADS 16          // Maximum number of threads
#define MAX_NUMBERS 1000        // Maximum numbers to process in batch mode
#define MAX_BINARY_DIGITS 100   // Maximum binary digits to calculate
#define RESIDUE_THRESHOLD 0.005 // Threshold for stopping the conversion

/**
 * @brief Program configuration structure
 */
typedef struct
{
  int count;                    ///< Number of decimal values to process
  bool help;                    ///< Flag to show help message
  int threads;                  ///< Number of threads to use
  bool file_mode;               ///< Read numbers from file instead of stdin
  char filename[MAX_INPUT_LEN]; ///< Input file name
  double numbers[MAX_NUMBERS];  ///< Array of numbers to process
} ConverterConfig;

/* Function prototypes */
void parse_args(int argc, char *argv[], ConverterConfig *config);
void print_help(void);
bool parse_double(const char *str, double *value);
void handle_input(ConverterConfig *config);
void process_batch(ConverterConfig *config);
void convert_decimal_to_binary(double number, char *binary, int *length);
void print_error(const char *msg);
void read_numbers_from_file(ConverterConfig *config);
void convert_decimal_to_binary_parallel(ConverterConfig *config);

/**
 * @brief Main program entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status (0 for success)
 */
int main(int argc, char *argv[])
{
  ConverterConfig config = {
      .count = 0,
      .help = false,
      .threads = 4,
      .file_mode = false,
      .filename = {0}};
  double start_time, end_time;

  parse_args(argc, argv, &config);

  if (config.help)
  {
    print_help();
    return 0;
  }

  // If no numbers provided via command-line or file, get from stdin
  if (config.count == 0 && !config.file_mode)
  {
    handle_input(&config);
  }
  else if (config.file_mode)
  {
    read_numbers_from_file(&config);
  }

  start_time = omp_get_wtime();

  if (config.threads > 1 && config.count > 1)
  {
    convert_decimal_to_binary_parallel(&config);
  }
  else
  {
    process_batch(&config);
  }

  end_time = omp_get_wtime();
  printf("Time: %.4f seconds\n", end_time - start_time);

  return 0;
}

/**
 * @brief Converts decimal floating point numbers to binary representation in parallel
 * @param conv Pointer to ConverterConfig configuration structure
 *
 * Uses OpenMP to process multiple numbers concurrently for improved performance.
 */
void convert_decimal_to_binary_parallel(ConverterConfig *config)
{
  int num_threads = (config->count < config->threads) ? config->count : config->threads;
  printf("Processing %d numbers using %d threads\n", config->count, num_threads);

#pragma omp parallel num_threads(num_threads)
  {
    char binary[MAX_BINARY_DIGITS + 1];
    int length;

#pragma omp for schedule(dynamic, 1)
    for (int i = 0; i < config->count; i++)
    {
      convert_decimal_to_binary(config->numbers[i], binary, &length);

#pragma omp critical
      {
        printf("Decimal: %f\n", config->numbers[i]);
        printf("Binary: %s\n", binary);
        printf("Length: %d binary digits\n\n", length);
      }
    }
  }
}

/**
 * @brief Processes a batch of numbers in single-threaded mode
 * @param conv Pointer to ConverterConfig configuration structure
 */
void process_batch(ConverterConfig *config)
{
  char binary[MAX_BINARY_DIGITS + 1];
  int length;

  for (int i = 0; i < config->count; i++)
  {
    convert_decimal_to_binary(config->numbers[i], binary, &length);
    printf("Decimal: %f\n", config->numbers[i]);
    printf("Binary: %s\n", binary);
    printf("Length: %d binary digits\n\n", length);
  }
}

/**
 * @brief Converts a decimal floating point number to binary representation
 * @param number The decimal number to convert
 * @param binary Output parameter for binary string
 * @param length Output parameter for the length of binary representation
 *
 * Uses the repeated multiplication method for the fractional part until
 * the residue is less than the threshold (0.005).
 */
void convert_decimal_to_binary(double number, char *binary, int *length)
{
  // Initialize binary string
  binary[0] = '\0';
  *length = 0;

  // Handle negative numbers
  if (number < 0)
  {
    number = -number;
    strcat(binary, "-");
    (*length)++;
  }

  // Extract integer and fractional parts
  long long int_part = (long long)number;
  double frac_part = number - (double)int_part;

  // Handle integer part
  if (int_part == 0)
  {
    strcat(binary, "0");
    (*length)++;
  }
  else
  {
    // Convert integer part to binary
    char int_binary[MAX_BINARY_DIGITS] = {0};
    int int_pos = 0;

    while (int_part > 0)
    {
      int_binary[int_pos++] = (int_part % 2) + '0';
      int_part /= 2;
    }

    // Reverse the integer binary
    for (int i = int_pos - 1; i >= 0; i--)
    {
      char digit[2] = {int_binary[i], '\0'};
      strcat(binary, digit);
      (*length)++;
    }
  }

  // Add decimal point if fractional part exists
  if (frac_part > 0)
  {
    strcat(binary, ".");
    (*length)++;

    // Process fractional part until residue < 0.005
    int frac_digits = 0;
    while (frac_part >= RESIDUE_THRESHOLD && frac_digits < MAX_BINARY_DIGITS - *length)
    {
      frac_part *= 2;
      int bit = (int)frac_part;

      // Add the bit to binary string
      char digit[2] = {bit + '0', '\0'};
      strcat(binary, digit);
      (*length)++;
      frac_digits++;

      // Subtract the integer part
      frac_part -= bit;
    }

    // Add info about residue if we stopped due to threshold
    if (frac_part >= RESIDUE_THRESHOLD)
    {
      char residue_info[50];
      snprintf(residue_info, sizeof(residue_info), " (residue: %.6f)", frac_part);
      strcat(binary, residue_info);
    }
  }
}

/**
 * @brief Reads decimal numbers from an input file
 * @param conv Pointer to ConverterConfig configuration structure
 */
void read_numbers_from_file(ConverterConfig *config)
{
  FILE *file = fopen(config->filename, "r");
  if (file == NULL)
  {
    print_error("Could not open input file");
    exit(EXIT_FAILURE);
  }

  char line[MAX_INPUT_LEN];
  config->count = 0;

  while (fgets(line, sizeof(line), file) && config->count < MAX_NUMBERS)
  {
    // Skip empty lines and comments
    if (line[0] == '\n' || line[0] == '#')
      continue;

    // Remove newline character
    line[strcspn(line, "\n")] = 0;

    double value;
    if (parse_double(line, &value))
    {
      config->numbers[config->count++] = value;
    }
  }

  fclose(file);
  printf("Read %d numbers from %s\n", config->count, config->filename);
}

/**
 * @brief Handles user input for decimal numbers
 * @param conv Pointer to ConverterConfig configuration structure
 */
void handle_input(ConverterConfig *config)
{
  char input[MAX_INPUT_LEN];
  double value;

  printf("Enter decimal numbers (one per line, empty line to finish):\n");

  while (config->count < MAX_NUMBERS)
  {
    printf("> ");
    if (fgets(input, sizeof(input), stdin) == NULL || input[0] == '\n')
      break;

    if (parse_double(input, &value))
    {
      config->numbers[config->count++] = value;
    }
    else
    {
      printf("Invalid input. Please enter a valid decimal number.\n");
    }
  }
}

/**
 * @brief Parses a string to double
 * @param str String to parse
 * @param value Output parameter for parsed value
 * @return true if parsing succeeded, false otherwise
 */
bool parse_double(const char *str, double *value)
{
  char *endptr;
  *value = strtod(str, &endptr);
  return (endptr != str) && (*endptr == '\0' || *endptr == '\n');
}

/**
 * @brief Prints error messages to stderr
 * @param msg Error message to display
 */
void print_error(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
}

/**
 * @brief Parses command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @param conv Configuration structure to populate
 */
void parse_args(int argc, char *argv[], ConverterConfig *config)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      config->help = true;
    }
    else if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
    {
      if (++i < argc)
      {
        config->file_mode = true;
        strncpy(config->filename, argv[i], MAX_INPUT_LEN - 1);
        config->filename[MAX_INPUT_LEN - 1] = '\0';
      }
      else
      {
        print_error("Missing filename after -f/--file");
        exit(EXIT_FAILURE);
      }
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
    {
      long long threads;
      if (++i < argc && sscanf(argv[i], "%lld", &threads) == 1 &&
          threads >= 1 && threads <= MAX_THREADS)
      {
        config->threads = (int)threads;
      }
      else
      {
        print_error("Thread count must be between 1 and MAX_THREADS");
        exit(EXIT_FAILURE);
      }
    }
    else if (strcmp(argv[i], "--number") == 0 || strcmp(argv[i], "-n") == 0)
    {
      if (++i < argc && parse_double(argv[i], &config->numbers[config->count]))
      {
        config->count++;
      }
      else
      {
        print_error("Invalid number specified");
        exit(EXIT_FAILURE);
      }
    }
  }
}

/**
 * @brief Displays program usage information
 */
void print_help()
{
  printf("\nDecimal to Binary ConverterConfig\n\n");
  printf("This program converts decimal floating point numbers to their binary representation\n");
  printf("until the residue (remaining fractional part) is less than 0.005.\n\n");
  printf("Usage: decimal_to_binary [options]\n\n");
  printf("Options:\n");
  printf("  -h, --help         Show this help message\n");
  printf("  -n, --number NUM   Add a number to process\n");
  printf("  -f, --file FILE    Read numbers from file (one per line)\n");
  printf("  -t, --threads N    Set number of threads (1-%d)\n\n", MAX_THREADS);
  printf("Examples:\n");
  printf("  ./decimal_to_binary -n 123.456 -n 0.1 -t 2\n");
  printf("  ./decimal_to_binary -f numbers.txt -t 4\n\n");
}