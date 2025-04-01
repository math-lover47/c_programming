#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define MAX_ROWS 300
#define MAX_SYMBOLS 50
#define DEFAULT_SYMBOL 'A'

// ANSI color codes
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

typedef struct
{
  int rows;
  char symbols[MAX_SYMBOLS];
  bool random_symbols;
  bool use_color;
  char color[20];
  bool help;
  int threads;
} PyramidConfig;

void parse_args(int argc, char *argv[], PyramidConfig *config);
void print_help(void);
void print_pyramid(PyramidConfig *config);
void print_pyramid_parallel(PyramidConfig *config);
char get_symbol(const char *symbols, int symbols_length, bool random, int index);
const char *get_color_code(const char *color_name);
void print_error(const char *msg);
void set_default_config(PyramidConfig *config);

int main(int argc, char *argv[])
{
  PyramidConfig config;
  set_default_config(&config);
  parse_args(argc, argv, &config);

  if (config.help)
  {
    print_help();
    return 0;
  }

  if (config.rows < 1 || config.rows > MAX_ROWS)
  {
    print_error("Number of rows must be between 1 and 300");
    return 1;
  }

  double start_time = omp_get_wtime();

  if (config.threads > 1)
  {
    print_pyramid_parallel(&config);
  }
  else
  {
    print_pyramid(&config);
  }

  double end_time = omp_get_wtime();
  printf("\nPyramid generation completed in %.4f seconds\n", end_time - start_time);
  return 0;
}

void set_default_config(PyramidConfig *config)
{
  config->rows = 5;
  config->symbols[0] = DEFAULT_SYMBOL;
  config->symbols[1] = '\0';
  config->random_symbols = false;
  config->use_color = false;
  strcpy(config->color, "blue");
  config->help = false;
  config->threads = 1;
}

void print_pyramid(PyramidConfig *config)
{
  int symbols_len = strlen(config->symbols);
  const char *color_code = config->use_color ? get_color_code(config->color) : "";
  const char *reset = config->use_color ? COLOR_RESET : "";

  printf("\nPyramid:\n\n");

  for (int i = 0; i < config->rows; i++)
  {
    // Calculate spaces and symbols for this row
    int spaces = config->rows - i - 1;
    int symbols = 2 * i + 1;

    // Print leading spaces
    for (int s = 0; s < spaces; s++)
    {
      printf(" ");
    }

    // Print colored symbols
    printf("%s", color_code);
    for (int j = 0; j < symbols; j++)
    {
      char symbol = get_symbol(config->symbols, symbols_len, config->random_symbols, j);
      printf("%c", symbol);
    }
    printf("%s\n", reset);
  }
}

void print_pyramid_parallel(PyramidConfig *config)
{
  int symbols_len = strlen(config->symbols);
  char **rows = (char **)malloc(config->rows * sizeof(char *));
  int max_row_length = 2 * config->rows + 10; // Maximum width + buffer

#pragma omp parallel for num_threads(config->threads)
  for (int i = 0; i < config->rows; i++)
  {
    int spaces = config->rows - i - 1;
    int symbols = 2 * i + 1;

    rows[i] = (char *)malloc(max_row_length * sizeof(char));
    int pos = 0;

    // Add spaces
    for (int s = 0; s < spaces; s++)
    {
      rows[i][pos++] = ' ';
    }

    // Add symbols
    if (config->use_color)
    {
      pos += sprintf(rows[i] + pos, "%s", get_color_code(config->color));
    }
    for (int j = 0; j < symbols; j++)
    {
      rows[i][pos++] = get_symbol(config->symbols, symbols_len, config->random_symbols, j);
    }
    if (config->use_color)
    {
      pos += sprintf(rows[i] + pos, "%s", COLOR_RESET);
    }
    rows[i][pos] = '\0';
  }

  printf("\nPyramid:\n\n");
  for (int i = 0; i < config->rows; i++)
  {
    printf("%s\n", rows[i]);
    free(rows[i]);
  }
  free(rows);
}

char get_symbol(const char *symbols, int symbols_length, bool random, int index)
{
  if (symbols_length == 0)
    return ' ';
  if (random)
    return symbols[rand() % symbols_length];
  return symbols[index % symbols_length];
}

const char *get_color_code(const char *color_name)
{
  if (strcmp(color_name, "red") == 0)
    return COLOR_RED;
  if (strcmp(color_name, "green") == 0)
    return COLOR_GREEN;
  if (strcmp(color_name, "yellow") == 0)
    return COLOR_YELLOW;
  if (strcmp(color_name, "blue") == 0)
    return COLOR_BLUE;
  if (strcmp(color_name, "magenta") == 0)
    return COLOR_MAGENTA;
  if (strcmp(color_name, "cyan") == 0)
    return COLOR_CYAN;
  if (strcmp(color_name, "white") == 0)
    return COLOR_WHITE;
  return COLOR_RESET;
}

void parse_args(int argc, char *argv[], PyramidConfig *config)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      config->help = true;
    }
    else if (strcmp(argv[i], "--rows") == 0 || strcmp(argv[i], "-r") == 0)
    {
      if (++i < argc)
        config->rows = atoi(argv[i]);
    }
    else if (strcmp(argv[i], "--symbols") == 0 || strcmp(argv[i], "-s") == 0)
    {
      if (++i < argc)
        strncpy(config->symbols, argv[i], MAX_SYMBOLS - 1);
    }
    else if (strcmp(argv[i], "--random") == 0 || strcmp(argv[i], "-rand") == 0)
    {
      config->random_symbols = true;
    }
    else if (strcmp(argv[i], "--color") == 0 || strcmp(argv[i], "-c") == 0)
    {
      config->use_color = true;
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        strcpy(config->color, argv[++i]);
      }
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (++i < argc)
        config->threads = atoi(argv[i]);
    }
  }
}

void print_help()
{
  printf("\nSingle Pyramid Printer\n\n");
  printf("Usage: start [options]\n\n");
  printf("Options:\n");
  printf("  -h, --help        Show this help message\n");
  printf("  -r N, --rows N    Set number of rows (1-300, default: 5)\n");
  printf("  -s CHARS, --symbols CHARS  Set symbols to use (default: 'A')\n");
  printf("  -rand, --random   Use random symbols from the provided set\n");
  printf("  -c COLOR, --color COLOR  Apply color (red, green, blue, etc.)\n");
  printf("  -t N, --threads N Set number of threads for parallel processing\n\n");
}

void print_error(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
}