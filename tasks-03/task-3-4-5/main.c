#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define MAX_RANGE 1000000000LL
#define MAX_FIB 10000
#define DEFAULT_THREADS 4
#define DEFAULT_WIDTH 5
#define DEFAULT_ROW 10

typedef struct
{
  bool show_primes;
  bool show_fibonacci;
  bool check_prime;
  bool check_duplicates;
  long long number;
  int threads;
  int num_width;
  int row_size;
} Config;

// Function prototypes
void parse_args(int argc, char *argv[], Config *config);
void print_help(void);
bool is_prime(long long num);
bool has_duplicate_digits(long long num);
void print_primes_up_to(long long limit, int threads, int row_size, int width);
void print_fibonacci_divisible_by(int divisor, int max_fib, int width);
void print_numbers_in_rows(long long *numbers, int count, int row_size, int width);
void print_error(const char *msg);
double calculate_execution_time(double start, double end);

int main(int argc, char *argv[])
{
  Config config = {
      .show_primes = false,
      .show_fibonacci = false,
      .check_prime = false,
      .check_duplicates = false,
      .number = 0,
      .threads = DEFAULT_THREADS,
      .num_width = DEFAULT_WIDTH,
      .row_size = DEFAULT_ROW};
  double start_time, end_time;

  parse_args(argc, argv, &config);

  if (config.check_prime && config.number > 0)
  {
    printf("%lld is %sprime.\n", config.number, is_prime(config.number) ? "" : "not ");
    return 0;
  }

  if (config.check_duplicates && config.number > 0)
  {
    printf("%lld %s duplicate digits.\n", config.number,
           has_duplicate_digits(config.number) ? "has" : "does not have");
    return 0;
  }

  start_time = omp_get_wtime();

  if (config.show_primes)
  {
    printf("Prime numbers up to 1000:\n");
    print_primes_up_to(1000, config.threads, config.row_size, config.num_width);
  }

  if (config.show_fibonacci)
  {
    printf("\nFibonacci numbers <= 10000 divisible by 5:\n");
    print_fibonacci_divisible_by(5, MAX_FIB, config.num_width);
  }

  end_time = omp_get_wtime();
  printf("\nExecution time: %.4f seconds\n", calculate_execution_time(start_time, end_time));

  return 0;
}

bool is_prime(long long num)
{
  if (num <= 1)
    return false;
  if (num <= 3)
    return true;
  if (num % 2 == 0 || num % 3 == 0)
    return false;

  for (long long i = 5; i * i <= num; i += 6)
  {
    if (num % i == 0 || num % (i + 2) == 0)
      return false;
  }
  return true;
}

bool has_duplicate_digits(long long num)
{
  if (num < 10)
    return false;

  bool digits[10] = {false};
  while (num > 0)
  {
    int digit = num % 10;
    if (digits[digit])
      return true;
    digits[digit] = true;
    num /= 10;
  }
  return false;
}

void print_primes_up_to(long long limit, int threads, int row_size, int width)
{
  long long *primes = malloc(limit * sizeof(long long));
  if (!primes)
  {
    print_error("Memory allocation failed");
    exit(1);
  }

  int count = 0;
#pragma omp parallel for num_threads(threads) schedule(dynamic)
  for (long long i = 2; i <= limit; i++)
  {
    if (is_prime(i))
    {
#pragma omp critical
      {
        primes[count++] = i;
      }
    }
  }

  print_numbers_in_rows(primes, count, row_size, width);
  free(primes);
}

void print_fibonacci_divisible_by(int divisor, int max_fib, int width)
{
  long long fibs[1000];
  int count = 0;
  long long a = 0, b = 1, c;

  while (a <= max_fib)
  {
    if (a % divisor == 0)
    {
      fibs[count++] = a;
    }
    c = a + b;
    a = b;
    b = c;
  }

  print_numbers_in_rows(fibs, count, DEFAULT_ROW, width);
}

void print_numbers_in_rows(long long *numbers, int count, int row_size, int width)
{
  for (int i = 0; i < count; i++)
  {
    printf("%*lld", width, numbers[i]);
    if ((i + 1) % row_size == 0)
    {
      printf("\n");
    }
  }
  if (count % row_size != 0)
  {
    printf("\n");
  }
}

void parse_args(int argc, char *argv[], Config *config)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      print_help();
      exit(0);
    }
    else if (strcmp(argv[i], "--primes") == 0 || strcmp(argv[i], "-p") == 0)
    {
      config->show_primes = true;
    }
    else if (strcmp(argv[i], "--fibonacci") == 0 || strcmp(argv[i], "-f") == 0)
    {
      config->show_fibonacci = true;
    }
    else if (strcmp(argv[i], "--check-prime") == 0 || strcmp(argv[i], "-c") == 0)
    {
      if (i + 1 < argc)
      {
        config->check_prime = true;
        config->number = atoll(argv[++i]);
      }
    }
    else if (strcmp(argv[i], "--check-duplicates") == 0 || strcmp(argv[i], "-d") == 0)
    {
      if (i + 1 < argc)
      {
        config->check_duplicates = true;
        config->number = atoll(argv[++i]);
      }
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (i + 1 < argc)
      {
        config->threads = atoi(argv[++i]);
        if (config->threads < 1)
          config->threads = 1;
      }
    }
    else if (strcmp(argv[i], "--width") == 0 || strcmp(argv[i], "-w") == 0)
    {
      if (i + 1 < argc)
      {
        config->num_width = atoi(argv[++i]);
        if (config->num_width < 1)
          config->num_width = 1;
      }
    }
    else if (strcmp(argv[i], "--row") == 0 || strcmp(argv[i], "-r") == 0)
    {
      if (i + 1 < argc)
      {
        config->row_size = atoi(argv[++i]);
        if (config->row_size < 1)
          config->row_size = 1;
      }
    }
  }
}

void print_error(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
}

void print_help()
{
  printf("Advanced Number Operations Program\n");
  printf("Usage: start [options]\n\n");
  printf("Options:\n");
  printf("  -h, --help            Show this help message\n");
  printf("  -p, --primes          Print all primes up to 1000\n");
  printf("  -f, --fibonacci       Print Fibonacci numbers <= 10000 divisible by 5\n");
  printf("  -c, --check-prime N   Check if N is prime\n");
  printf("  -d, --check-duplicates N  Check if N has duplicate digits\n");
  printf("  -t, --threads N       Set number of threads (default: 4)\n");
  printf("  -w, --width N         Set output width for numbers (default: 5)\n");
  printf("  -r, --row N           Set numbers per row (default: 10)\n\n");
}

double calculate_execution_time(double start, double end)
{
  return end - start;
}