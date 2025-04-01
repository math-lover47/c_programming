#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define MAX_RANGE 1000000000000000000LL // 10^18
#define MAX_INPUT_LEN 256
#define MAX_THREADS 16
#define DEFAULT_ROW 10
#define DEFAULT_WIDTH 8

typedef struct
{
  long long limit;
  bool help;
  int threads;
  int sum_target;
  bool reverse_flag;
  long long reverse_num;
  int row_size;
  int num_width;
} Config;

// Function prototypes
void parse_args(int argc, char *argv[], Config *config);
void print_help(void);
bool parse_long_long(const char *str, long long *value);
void handle_input(Config *config);
void reverse_number(long long num);
int sum_digits(long long num);
void print_numbers_with_sum(Config *config);
void print_error(const char *msg);
void print_numbers_in_rows(long long *numbers, int count, int row_size, int width);

int main(int argc, char *argv[])
{
  Config config = {
      .limit = 0,
      .help = false,
      .threads = 1,
      .sum_target = 25,
      .reverse_flag = false,
      .reverse_num = 0,
      .row_size = DEFAULT_ROW,
      .num_width = DEFAULT_WIDTH};
  double start_time, end_time;

  parse_args(argc, argv, &config);

  if (config.help)
  {
    print_help();
    return 0;
  }

  start_time = omp_get_wtime();
  if (config.reverse_flag)
  {
    if (config.reverse_num != 0)
    {
      reverse_number(config.reverse_num);
    }
    else if (config.limit > 0)
    {
      // Reverse all numbers from 1 to limit
      for (long long i = 1; i <= config.limit; i++)
      {
        reverse_number(i);
      }
      printf("\n");
    }
    end_time = omp_get_wtime();
    printf("\nTime: %.4f seconds\n", end_time - start_time);
    return 0;
  }

  if (config.sum_target > 0 && config.limit > 0)
  {
    print_numbers_with_sum(&config);
    end_time = omp_get_wtime();
    printf("\nTime: %.4f seconds\n", end_time - start_time);
    return 0;
  }

  handle_input(&config);
  end_time = omp_get_wtime();
  printf("\nTime: %.4f seconds\n", end_time - start_time);
  return 0;
}

void reverse_number(long long num)
{
  long long reversed = 0;
  while (num != 0)
  {
    reversed = reversed * 10 + num % 10;
    num /= 10;
  }
  printf("%lld ", reversed);
}

int sum_digits(long long num)
{
  int sum = 0;
  while (num != 0)
  {
    sum += num % 10;
    num /= 10;
  }
  return sum;
}

void print_numbers_with_sum(Config *config)
{
  long long *numbers = malloc(config->limit * sizeof(long long));
  if (!numbers)
  {
    print_error("Memory allocation failed");
    exit(1);
  }

  int count = 0;
#pragma omp parallel for num_threads(config->threads)
  for (long long i = 1; i <= config->limit; i++)
  {
    if (sum_digits(i) == config->sum_target)
    {
#pragma omp critical
      {
        numbers[count++] = i;
      }
    }
  }

  print_numbers_in_rows(numbers, count, config->row_size, config->num_width);
  free(numbers);
}

void print_numbers_in_rows(long long *numbers, int count, int row_size, int width)
{
  for (int i = 0; i < count; i++)
  {
    printf("%*lld ", width, numbers[i]);
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
      config->help = true;
    }
    else if (strcmp(argv[i], "--limit") == 0 || strcmp(argv[i], "-l") == 0)
    {
      if (i + 1 < argc && parse_long_long(argv[i + 1], &config->limit))
      {
        i++;
      }
      else
      {
        print_error("Missing or invalid number after -l/--limit");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--reverse") == 0 || strcmp(argv[i], "-r") == 0)
    {
      config->reverse_flag = true;
      if (i + 1 < argc && parse_long_long(argv[i + 1], &config->reverse_num))
      {
        i++;
      }
    }
    else if (strcmp(argv[i], "--sum") == 0 || strcmp(argv[i], "-s") == 0)
    {
      if (i + 1 < argc && parse_long_long(argv[i + 1], (long long *)&config->sum_target))
      {
        i++;
      }
      else
      {
        print_error("Missing or invalid number after -s/--sum");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--row") == 0)
    {
      if (i + 1 < argc && parse_long_long(argv[i + 1], (long long *)&config->row_size))
      {
        i++;
      }
      else
      {
        print_error("Missing or invalid number after --row");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--width") == 0 || strcmp(argv[i], "-w") == 0)
    {
      if (i + 1 < argc && parse_long_long(argv[i + 1], (long long *)&config->num_width))
      {
        i++;
      }
      else
      {
        print_error("Missing or invalid number after -w/--width");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (i + 1 < argc && parse_long_long(argv[i + 1], (long long *)&config->threads))
      {
        if (config->threads < 1 || config->threads > MAX_THREADS)
        {
          print_error("Thread count must be between 1 and MAX_THREADS");
          exit(1);
        }
        i++;
      }
      else
      {
        print_error("Missing or invalid thread count after -t/--threads");
        exit(1);
      }
    }
  }
}

bool parse_long_long(const char *str, long long *value)
{
  char *endptr;
  long long num = strtoll(str, &endptr, 10);
  if (endptr == str || *endptr != '\0')
  {
    return false;
  }
  *value = num;
  return true;
}

void handle_input(Config *config)
{
  if (config->limit == 0)
  {
    char input[MAX_INPUT_LEN];
    printf("Enter a number to reverse: ");

    if (fgets(input, sizeof(input), stdin) == NULL)
    {
      print_error("Failed to read input");
      exit(1);
    }

    long long num;
    if (!parse_long_long(input, &num))
    {
      print_error("Invalid input. Please enter a valid number");
      exit(1);
    }

    reverse_number(num);
    printf("\n");
  }
}

void print_error(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
}

void print_help()
{
  printf("Number Operations Program\n");
  printf("Usage: start [options]\n\n");
  printf("Options:\n");
  printf("  -h, --help        Show this help message\n");
  printf("  -l, --limit N     Generate numbers from 1 to N\n");
  printf("  -r, --reverse N   Reverse the digits of N (0 means reverse all numbers from -l)\n");
  printf("  -s, --sum N       Print numbers whose digits sum to N (requires -l)\n");
  printf("  --row N           Set how many numbers to print per line (default: 10)\n");
  printf("  -w, --width N     Set the width for number output (default: 8)\n");
  printf("  -t, --threads N   Set number of threads to use (default: 1)\n\n");
}