/**
 * @file armstrong_optimized.c
 * @brief Optimized parallel Armstrong numbers calculator using OpenMP
 *
 * An Armstrong number (also known as narcissistic number) is a number that is
 * equal to the sum of its own digits each raised to the power of the number of digits.
 * Example: 153 = 1³ + 5³ + 3³ = 1 + 125 + 27 = 153
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define MAX_RANGE 1000000000000000000LL // 10^18 (maximum supported range)
#define MAX_INPUT_LEN 256               // Maximum length for user input
#define MAX_THREADS 16                  // Maximum number of threads

/**
 * @brief Program configuration structure
 */
typedef struct
{
  long long limit; ///< Upper limit for number search
  bool help;       ///< Flag to show help message
  int threads;     ///< Number of threads to use
} Armstrong;

/* Function prototypes */
void parse_args(int argc, char *argv[], Armstrong *arm);
void print_help(void);
bool parse_long_long(const char *str, long long *value);
void handle_input(Armstrong *arm);
void process_input(Armstrong *arm, const char *input);
bool is_armstrong_number(long long num);
int count_digits(long long num);
long long ipow(long long base, int exp);
void print_error(const char *msg);
void find_armstrong_numbers(long long limit, int thread_count);
void print_armstrong_numbers(long long limit);

/**
 * @brief Main program entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status (0 for success)
 */
int main(int argc, char *argv[])
{
  Armstrong arm = {.limit = 0, .help = false, .threads = 4};
  double start_time, end_time;

  parse_args(argc, argv, &arm);

  if (arm.help)
  {
    print_help();
    return 0;
  }

  handle_input(&arm);

  start_time = omp_get_wtime();

  if (arm.threads > 1)
  {
    find_armstrong_numbers(arm.limit, arm.threads);
  }
  else
  {
    print_armstrong_numbers(arm.limit);
  }

  end_time = omp_get_wtime();
  printf("Time: %.4f seconds\n", end_time - start_time);

  return 0;
}

/**
 * @brief Finds Armstrong numbers in parallel using OpenMP
 * @param limit Upper limit for the search
 * @param thread_count Number of threads to use
 *
 * Uses thread-local buffers and dynamic scheduling for efficient parallelization.
 * Suitable for ranges up to 10 billion on modest hardware.
 */
void find_armstrong_numbers(long long limit, int thread_count)
{
  long long results[100] = {0}; // Fixed-size result buffer
  int result_count = 0;         // Number of found Armstrong numbers

#pragma omp parallel num_threads(thread_count)
  {
    // Thread-local storage to minimize synchronization
    long long local_results[20] = {0};
    int local_count = 0;

// Process numbers in dynamically assigned chunks
#pragma omp for schedule(dynamic, 10000)
    for (long long i = 1; i <= limit; i++)
    {
      if (is_armstrong_number(i))
      {
        local_results[local_count++] = i;

        // Periodically merge to global results
        if (local_count >= 10)
        {
#pragma omp critical
          {
            for (int j = 0; j < local_count && result_count < 100; j++)
            {
              results[result_count++] = local_results[j];
            }
          }
          local_count = 0;
        }
      }
    }

// Merge any remaining numbers
#pragma omp critical
    {
      for (int j = 0; j < local_count && result_count < 100; j++)
      {
        results[result_count++] = local_results[j];
      }
    }
  }

  // Output results (already nearly ordered)
  for (int i = 0; i < result_count; i++)
  {
    printf("%lld ", results[i]);
  }
  printf("\n");
}

/**
 * @brief Checks if a number is an Armstrong number
 * @param num Number to check
 * @return true if Armstrong number, false otherwise
 *
 * Uses optimized algorithm with custom power function (ipow)
 */
bool is_armstrong_number(long long num)
{
  if (num < 10)
    return true; // Single-digit numbers are always Armstrong

  const long long original = num;
  long long sum = 0;
  const int digits = count_digits(num);

  while (num > 0)
  {
    const int digit = num % 10;
    sum += ipow(digit, digits);
    if (sum > original)
      break; // Early exit if sum exceeds original
    num /= 10;
  }

  return sum == original;
}

/**
 * @brief Fast integer power function
 * @param base Base number
 * @param exp Exponent
 * @return base raised to the power of exp
 *
 * Uses exponentiation by squaring for efficiency.
 */
long long ipow(long long base, int exp)
{
  long long result = 1;
  while (exp > 0)
  {
    if (exp % 2 == 1)
    {
      result *= base;
    }
    exp >>= 1;
    base *= base;
  }
  return result;
}

/**
 * @brief Counts digits in a number
 * @param num Number to process
 * @return Number of digits
 *
 * Uses log10 for optimal performance with large numbers.
 */
int count_digits(long long num)
{
  if (num == 0)
    return 1;
  return (int)floor(log10(llabs(num))) + 1;
}

/**
 * @brief Handles user input for the upper limit
 * @param arm Pointer to Armstrong configuration structure
 *
 * Prompts user for input if limit wasn't provided via command line.
 * Validates input range and format.
 */
void handle_input(Armstrong *arm)
{
  if (arm->limit == 0)
  {
    char input[MAX_INPUT_LEN];
    printf("Enter upper limit (1-%lld): ", MAX_RANGE);

    if (fgets(input, sizeof(input), stdin) == NULL)
    {
      print_error("Failed to read input");
      exit(EXIT_FAILURE);
    }
    process_input(arm, input);
  }
}

/**
 * @brief Processes and validates user input
 * @param arm Configuration structure
 * @param input User input string
 */
void process_input(Armstrong *arm, const char *input)
{
  if (!parse_long_long(input, &arm->limit) ||
      arm->limit < 1 ||
      arm->limit > MAX_RANGE)
  {
    print_error("Invalid input. Please enter a number between 1 and 1000000000000000000");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Parses a string to long long integer
 * @param str String to parse
 * @param value Output parameter for parsed value
 * @return true if parsing succeeded, false otherwise
 */
bool parse_long_long(const char *str, long long *value)
{
  char *endptr;
  *value = strtoll(str, &endptr, 10);
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
 * @brief Single-threaded Armstrong number printer
 * @param limit Upper search limit
 *
 * Used when thread_count = 1 for simpler execution
 */
void print_armstrong_numbers(long long limit)
{
  printf("Armstrong numbers up to %lld:\n", limit);
  for (long long i = 1; i <= limit; i++)
  {
    if (is_armstrong_number(i))
    {
      printf("%lld ", i);
    }
  }
  printf("\n");
}

/**
 * @brief Parses command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @param arm Configuration structure to populate
 */
void parse_args(int argc, char *argv[], Armstrong *arm)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      arm->help = true;
    }
    else if (strcmp(argv[i], "--num") == 0 || strcmp(argv[i], "-n") == 0)
    {
      if (++i < argc && parse_long_long(argv[i], &arm->limit))
        continue;
      print_error("Missing or invalid number after -n/--num");
      exit(EXIT_FAILURE);
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (++i < argc && parse_long_long(argv[i], (long long *)&arm->threads) &&
          arm->threads >= 1 && arm->threads <= MAX_THREADS)
        continue;
      print_error("Thread count must be between 1 and MAX_THREADS");
      exit(EXIT_FAILURE);
    }
  }
}

/**
 * @brief Displays program usage information
 */
void print_help()
{
  printf("\nArmstrong Numbers Calculator\n\n");
  printf("Usage: armstrong [options]\n\n");
  printf("Options:\n");
  printf("  -h, --help        Show this help message\n");
  printf("  -n, --num LIMIT   Set upper search limit (1-%lld)\n", MAX_RANGE);
  printf("  -t, --threads N   Set number of threads (1-%d)\n\n", MAX_THREADS);
}
