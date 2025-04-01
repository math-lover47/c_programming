/**
 * @file armstrong_enhanced.c
 * @brief Enhanced Armstrong numbers calculator with multiple algorithms and multithreading
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define MAX_RANGE 1000000000000000000LL // 10^18
#define MAX_INPUT_LEN 256
#define MAX_THREADS 16
#define ALGO_BASIC 1
#define ALGO_OPTIMIZED 2
#define ALGO_MATH 3

typedef struct
{
  long long start;
  long long end;
  int algorithm;
  long long *results;
  int *result_count;
  pthread_mutex_t *mutex;
} ThreadData;

typedef struct
{
  long long limit;
  bool help;
  int algorithm;
  int threads;
} Armstrong;

// Function prototypes
void parse_args(int argc, char *argv[], Armstrong *arm);
void print_help(void);
bool parse_long_long(const char *str, long long *value);
void handle_input(Armstrong *arm);
void process_input(Armstrong *arm, const char *input);
void print_armstrong_numbers(const Armstrong *arm);
bool is_armstrong_number_basic(long long num);
bool is_armstrong_number_optimized(long long num);
bool is_armstrong_number_math(long long num);
int count_digits(long long num);
long long ipow(long long base, int exp);
void print_error(const char *msg);
double calculate_execution_time(clock_t start, clock_t end);
void *thread_function(void *arg);
void find_armstrong_numbers_threaded(long long limit, int algorithm, int thread_count);

int main(int argc, char *argv[])
{
  Armstrong arm = {.limit = 0, .help = false, .algorithm = ALGO_OPTIMIZED, .threads = 4};
  clock_t start_time, end_time;

  parse_args(argc, argv, &arm);

  if (arm.help)
  {
    print_help();
    return 0;
  }

  handle_input(&arm);

  start_time = clock();

  if (arm.threads > 1)
  {
    find_armstrong_numbers_threaded(arm.limit, arm.algorithm, arm.threads);
  }
  else
  {
    print_armstrong_numbers(&arm);
  }

  end_time = clock();

  printf("\nCalculation time: %.4f seconds\n",
         calculate_execution_time(start_time, end_time));

  return 0;
}

// Thread function for parallel processing
void *thread_function(void *arg)
{
  ThreadData *data = (ThreadData *)arg;
  int local_count = 0;
  long long local_results[1000]; // Buffer for local results

  for (long long i = data->start; i <= data->end; i++)
  {
    bool is_armstrong = false;

    switch (data->algorithm)
    {
    case ALGO_BASIC:
      is_armstrong = is_armstrong_number_basic(i);
      break;
    case ALGO_OPTIMIZED:
      is_armstrong = is_armstrong_number_optimized(i);
      break;
    case ALGO_MATH:
      is_armstrong = is_armstrong_number_math(i);
      break;
    }

    if (is_armstrong)
    {
      local_results[local_count++] = i;
      if (local_count >= 1000)
      {
        // Lock and add to global results
        pthread_mutex_lock(data->mutex);
        for (int j = 0; j < local_count; j++)
        {
          data->results[(*data->result_count)++] = local_results[j];
        }
        pthread_mutex_unlock(data->mutex);
        local_count = 0;
      }
    }
  }

  // Add remaining results
  if (local_count > 0)
  {
    pthread_mutex_lock(data->mutex);
    for (int j = 0; j < local_count; j++)
    {
      data->results[(*data->result_count)++] = local_results[j];
    }
    pthread_mutex_unlock(data->mutex);
  }

  return NULL;
}

// Threaded Armstrong number search
void find_armstrong_numbers_threaded(long long limit, int algorithm, int thread_count)
{
  pthread_t threads[MAX_THREADS];
  ThreadData thread_data[MAX_THREADS];
  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);

  long long *results = malloc(limit * sizeof(long long));
  if (results == NULL)
  {
    print_error("Memory allocation failed");
    exit(1);
  }

  int result_count = 0;

  long long chunk_size = limit / thread_count;

  printf("Armstrong numbers up to %lld (using %d threads):\n", limit, thread_count);

  for (int i = 0; i < thread_count; i++)
  {
    thread_data[i].start = i * chunk_size + 1;
    thread_data[i].end = (i == thread_count - 1) ? limit : (i + 1) * chunk_size;
    thread_data[i].algorithm = algorithm;
    thread_data[i].results = results;
    thread_data[i].result_count = &result_count;
    thread_data[i].mutex = &mutex;

    if (pthread_create(&threads[i], NULL, thread_function, &thread_data[i]) != 0)
    {
      print_error("Failed to create thread");
      exit(1);
    }
  }

  for (int i = 0; i < thread_count; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // Sort results (simple bubble sort for demonstration)
  for (int i = 0; i < result_count - 1; i++)
  {
    for (int j = 0; j < result_count - i - 1; j++)
    {
      if (results[j] > results[j + 1])
      {
        long long temp = results[j];
        results[j] = results[j + 1];
        results[j + 1] = temp;
      }
    }
  }

  // Print results
  for (int i = 0; i < result_count; i++)
  {
    printf("%lld ", results[i]);
  }

  free(results);
  pthread_mutex_destroy(&mutex);
}

// Basic algorithm (uses pow)
bool is_armstrong_number_basic(long long num)
{
  if (num < 10)
    return true;

  long long original = num;
  long long sum = 0;
  int digits = count_digits(num);

  while (num > 0)
  {
    int digit = num % 10;
    sum += (long long)pow(digit, digits);
    if (sum > original)
      break; // Early exit if sum exceeds original
    num /= 10;
  }

  return sum == original;
}

// Optimized algorithm (uses custom power function)
bool is_armstrong_number_optimized(long long num)
{
  if (num < 10)
    return true;

  long long original = num;
  long long sum = 0;
  int digits = count_digits(num);

  while (num > 0)
  {
    int digit = num % 10;
    sum += ipow(digit, digits);
    if (sum > original)
      break; // Early exit if sum exceeds original
    num /= 10;
  }

  return sum == original;
}

// Mathematical optimization (pre-calculates digit powers)
bool is_armstrong_number_math(long long num)
{
  if (num < 10)
    return true;

  long long original = num;
  long long sum = 0;
  int digits = count_digits(num);
  long long digit_powers[10];

  // Pre-calculate digit powers
  for (int i = 0; i < 10; i++)
  {
    digit_powers[i] = ipow(i, digits);
  }

  while (num > 0)
  {
    int digit = num % 10;
    sum += digit_powers[digit];
    if (sum > original)
      break; // Early exit if sum exceeds original
    num /= 10;
  }

  return sum == original;
}

// Integer power function (faster than pow for integers)
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

// Count digits using log10 (fast for large numbers)
int count_digits(long long num)
{
  if (num == 0)
    return 1;
  return (int)floor(log10(llabs(num))) + 1;
}

/* ===================== ARMSTRONG CALCULATIONS ================== */
/**
 * @brief Prints all Armstrong numbers up to given limit
 * @param arm Pointer to Armstrong structure containing the limit
 */
void print_armstrong_numbers(const Armstrong *arm)
{
  printf("Armstrong numbers up to %lld:\n", arm->limit);
  for (long long i = 1; i <= arm->limit; i++)
  {
    bool is_armstrong = false;

    switch (arm->algorithm)
    {
    case ALGO_BASIC:
      is_armstrong = is_armstrong_number_basic(i);
      break;
    case ALGO_OPTIMIZED:
      is_armstrong = is_armstrong_number_optimized(i);
      break;
    case ALGO_MATH:
      is_armstrong = is_armstrong_number_math(i);
      break;
    }

    if (is_armstrong)
    {
      printf("%lld ", i);
    }
  }
}

/* ====================== INPUT HANDLING ======================== */
void handle_input(Armstrong *arm)
{
  if (arm->limit == 0)
  {
    char input[MAX_INPUT_LEN];
    printf("Enter upper limit (1-%lld): ", MAX_RANGE);

    if (fgets(input, sizeof(input), stdin) == NULL)
    {
      print_error("Failed to read input");
      exit(1);
    }

    process_input(arm, input);
  }
}

void process_input(Armstrong *arm, const char *input)
{
  if (!parse_long_long(input, &arm->limit) || arm->limit < 1 || arm->limit > MAX_RANGE)
  {
    print_error("Invalid input. Please enter a number between 1 and 1000000000000000000");
    exit(1);
  }
}

/* ====================== UTILITY FUNCTIONS ===================== */
bool parse_long_long(const char *str, long long *value)
{
  char *endptr;
  long long num = strtoll(str, &endptr, 10);
  if (endptr == str || (*endptr != '\n' && *endptr != '\0'))
  {
    return false;
  }
  *value = num;
  return true;
}

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
      if (i + 1 < argc && parse_long_long(argv[i + 1], &arm->limit))
      {
        i++;
      }
      else
      {
        print_error("Missing or invalid number after -n/--num");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--algo") == 0 || strcmp(argv[i], "-a") == 0)
    {
      if (i + 1 < argc)
      {
        if (strcmp(argv[i + 1], "basic") == 0)
        {
          arm->algorithm = ALGO_BASIC;
        }
        else if (strcmp(argv[i + 1], "optimized") == 0)
        {
          arm->algorithm = ALGO_OPTIMIZED;
        }
        else if (strcmp(argv[i + 1], "math") == 0)
        {
          arm->algorithm = ALGO_MATH;
        }
        else
        {
          print_error("Invalid algorithm choice. Use basic, optimized, or math");
          exit(1);
        }
        i++;
      }
      else
      {
        print_error("Missing algorithm specification after -a/--algo");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (i + 1 < argc && parse_long_long(argv[i + 1], (long long *)&arm->threads))
      {
        if (arm->threads < 1 || arm->threads > MAX_THREADS)
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

double calculate_execution_time(clock_t start, clock_t end)
{
  return (double)(end - start) / CLOCKS_PER_SEC;
}

void print_error(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
}

void print_help()
{
  printf("Armstrong Numbers Calculator\n");
  printf("Usage: armstrong [options]\n\n");
  printf("Options:\n");
  printf("  -h, --help        Show this help message\n");
  printf("  -n, --num LIMIT   Set upper limit for Armstrong number search (1-1000000000000000000)\n");
  printf("  -a, --algo ALG    Set algorithm (basic, optimized, math)\n");
  printf("  -t, --threads N   Set number of threads (1-%d)\n\n", MAX_THREADS);
  printf("An Armstrong number is a number that is equal to the sum of its own\n");
  printf("digits each raised to the power of the number of digits.\n");
  printf("Example: 153 = 1^3 + 5^3 + 3^3 = 1 + 125 + 27 = 153\n");
}