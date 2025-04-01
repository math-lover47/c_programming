#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define MAX_ROWS 300
#define MAX_COLS 300
#define MAX_SYMBOLS 50
#define DEFAULT_LEFT_SYMBOL 'A'
#define DEFAULT_RIGHT_SYMBOL 'B'

// ANSI color codes
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

/**
 * @brief Program configuration structure
 */
typedef struct
{
  int rows;                        ///< Number of rows in pyramids
  int columns;                     ///< Number of columns in pyramids
  char left_symbols[MAX_SYMBOLS];  ///< Symbols for left pyramid
  char right_symbols[MAX_SYMBOLS]; ///< Symbols for right pyramid
  bool random_symbols;             ///< Use random symbols from the provided set
  bool use_color_left;             ///< Apply color to left pyramid
  bool use_color_right;            ///< Apply color to right pyramid
  char left_color[20];             ///< Color for left pyramid
  char right_color[20];            ///< Color for right pyramid
  bool help;                       ///< Show help message
  int threads;                     ///< Number of threads for parallel processing
} PyramidConfig;

/* Function prototypes */
void parse_args(int argc, char *argv[], PyramidConfig *config);
void print_help(void);
void print_pyramids(PyramidConfig *config);
void print_pyramid_parallel(PyramidConfig *config);
char get_symbol(const char *symbols, int symbols_length, bool random, int index);
const char *get_color_code(const char *color_name);
void print_error(const char *msg);
void set_default_config(PyramidConfig *config);

/**
 * @brief Main program entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status (0 for success)
 */
int main(int argc, char *argv[])
{
  PyramidConfig config;
  set_default_config(&config);

  srand((unsigned int)time(NULL)); // Initialize random number generator

  parse_args(argc, argv, &config);

  if (config.help)
  {
    print_help();
    return 0;
  }

  // Validate configuration
  if (config.rows < 1 || config.rows > MAX_ROWS)
  {
    print_error("Number of rows must be between 1 and 300");
    return 1;
  }

  if (config.columns < 1 || config.columns > MAX_COLS)
  {
    print_error("Number of columns must be between 1 and 300");
    return 1;
  }

  double start_time = omp_get_wtime();

  if (config.threads > 1 && (config.rows * config.columns > 5000))
  {
    print_pyramid_parallel(&config);
  }
  else
  {
    print_pyramids(&config);
  }

  double end_time = omp_get_wtime();
  printf("\nPyramid generation completed in %.4f seconds\n", end_time - start_time);

  return 0;
}

/**
 * @brief Set default configuration values
 * @param config Pointer to configuration structure
 */
void set_default_config(PyramidConfig *config)
{
  config->rows = 5;
  config->columns = 5;
  config->left_symbols[0] = DEFAULT_LEFT_SYMBOL;
  config->left_symbols[1] = '\0';
  config->right_symbols[0] = DEFAULT_RIGHT_SYMBOL;
  config->right_symbols[1] = '\0';
  config->random_symbols = false;
  config->use_color_left = false;
  config->use_color_right = false;
  strcpy(config->left_color, "blue");
  strcpy(config->right_color, "green");
  config->help = false;
  config->threads = 1;
}

/**
 * @brief Print pyramids side by side with center spacing
 * @param config Pointer to configuration structure
 */
void print_pyramids(PyramidConfig *config)
{
  int left_symbols_len = strlen(config->left_symbols);
  int right_symbols_len = strlen(config->right_symbols);
  int max_width = config->columns * 2 + 2; // Maximum width of both pyramids plus spacing

  printf("\nCharacter Pyramids:\n\n");

  for (int i = 0; i < config->rows; i++)
  {
    // Calculate the actual width for this row
    int left_width = (i < config->columns) ? i + 1 : config->columns;
    int right_width = (i < config->columns) ? i + 1 : config->columns;
    int total_width = left_width + right_width;

    // Calculate spacing needed between pyramids
    int spacing = max_width - total_width;

    // Print left pyramid
    const char *left_color = config->use_color_left ? get_color_code(config->left_color) : "";
    const char *reset = config->use_color_left ? COLOR_RESET : "";

    printf("%s", left_color);
    for (int j = 0; j < left_width; j++)
    {
      char symbol = get_symbol(config->left_symbols, left_symbols_len, config->random_symbols, j);
      printf("%c", symbol);
    }
    printf("%s", reset);

    // Print spacing between pyramids
    for (int s = 0; s < spacing; s++)
    {
      printf(" ");
    }

    // Print right pyramid
    const char *right_color = config->use_color_right ? get_color_code(config->right_color) : "";
    reset = config->use_color_right ? COLOR_RESET : "";

    printf("%s", right_color);
    for (int j = 0; j < right_width; j++)
    {
      char symbol = get_symbol(config->right_symbols, right_symbols_len, config->random_symbols, j);
      printf("%c", symbol);
    }
    printf("%s", reset);

    printf("\n");
  }
}

/**
 * @brief Print pyramids in parallel for large configurations
 * @param config Pointer to configuration structure
 */
void print_pyramid_parallel(PyramidConfig *config)
{
  int left_symbols_len = strlen(config->left_symbols);
  int right_symbols_len = strlen(config->right_symbols);
  int max_width = config->columns * 2 + 2;

  char **rows = (char **)malloc(config->rows * sizeof(char *));
  int max_row_length = max_width + 20; // Extra space for row number and colors

  for (int i = 0; i < config->rows; i++)
  {
    rows[i] = (char *)malloc(max_row_length * sizeof(char));
  }

  printf("\nGenerating pyramids in parallel with %d threads...\n", config->threads);

#pragma omp parallel num_threads(config->threads)
  {
#pragma omp for schedule(dynamic, 10)
    for (int i = 0; i < config->rows; i++)
    {
      int pos = 0;
      int left_width = (i < config->columns) ? i + 1 : config->columns;
      int right_width = (i < config->columns) ? i + 1 : config->columns;
      int spacing = max_width - (left_width + right_width);

      // Left pyramid
      if (config->use_color_left)
      {
        pos += sprintf(rows[i] + pos, "%s", get_color_code(config->left_color));
      }
      for (int j = 0; j < left_width; j++)
      {
        char symbol = get_symbol(config->left_symbols, left_symbols_len, config->random_symbols, j);
        rows[i][pos++] = symbol;
      }
      if (config->use_color_left)
      {
        pos += sprintf(rows[i] + pos, "%s", COLOR_RESET);
      }

      // Spacing
      for (int s = 0; s < spacing; s++)
      {
        rows[i][pos++] = ' ';
      }

      // Right pyramid
      if (config->use_color_right)
      {
        pos += sprintf(rows[i] + pos, "%s", get_color_code(config->right_color));
      }
      for (int j = 0; j < right_width; j++)
      {
        char symbol = get_symbol(config->right_symbols, right_symbols_len, config->random_symbols, j);
        rows[i][pos++] = symbol;
      }
      if (config->use_color_right)
      {
        pos += sprintf(rows[i] + pos, "%s", COLOR_RESET);
      }

      rows[i][pos] = '\0';
    }
  }

  printf("\nCharacter Pyramids:\n\n");
  for (int i = 0; i < config->rows; i++)
  {
    printf("%s\n", rows[i]);
    free(rows[i]);
  }
  free(rows);
}

/**
 * @brief Get a symbol from the symbol set
 * @param symbols Array of available symbols
 * @param symbols_length Length of the symbols array
 * @param random Whether to choose a random symbol
 * @param index Position index in the pyramid
 * @return Selected symbol
 */
char get_symbol(const char *symbols, int symbols_length, bool random, int index)
{
  if (symbols_length == 0)
    return ' ';

  if (random)
  {
    return symbols[rand() % symbols_length];
  }
  else
  {
    // Use the index to cycle through the available symbols
    return symbols[index % symbols_length];
  }
}

/**
 * @brief Get ANSI color code from color name
 * @param color_name Name of the color
 * @return ANSI color code string
 */
const char *get_color_code(const char *color_name)
{
  if (strcmp(color_name, "red") == 0)
    return COLOR_RED;
  else if (strcmp(color_name, "green") == 0)
    return COLOR_GREEN;
  else if (strcmp(color_name, "yellow") == 0)
    return COLOR_YELLOW;
  else if (strcmp(color_name, "blue") == 0)
    return COLOR_BLUE;
  else if (strcmp(color_name, "magenta") == 0)
    return COLOR_MAGENTA;
  else if (strcmp(color_name, "cyan") == 0)
    return COLOR_CYAN;
  else if (strcmp(color_name, "white") == 0)
    return COLOR_WHITE;
  else
    return COLOR_RESET;
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
 * @brief Parse command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @param config Configuration structure to populate
 */
void parse_args(int argc, char *argv[], PyramidConfig *config)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      config->help = true;
    }
    else if (strcmp(argv[i], "--rows") == 0 || strcmp(argv[i], "-row") == 0)
    {
      if (++i < argc)
      {
        config->rows = atoi(argv[i]);
      }
      else
      {
        print_error("Missing value for rows parameter");
      }
    }
    else if (strcmp(argv[i], "--columns") == 0 || strcmp(argv[i], "-col") == 0)
    {
      if (++i < argc)
      {
        config->columns = atoi(argv[i]);
      }
      else
      {
        print_error("Missing value for columns parameter");
      }
    }
    else if (strcmp(argv[i], "--symbols") == 0 || strcmp(argv[i], "-s") == 0)
    {
      if (++i < argc)
      {
        // Split the argument into left and right symbols
        char *delimiter = strchr(argv[i], ',');
        if (delimiter)
        {
          int left_len = delimiter - argv[i];
          if (left_len > 0 && left_len < MAX_SYMBOLS - 1)
          {
            strncpy(config->left_symbols, argv[i], left_len);
            config->left_symbols[left_len] = '\0';
          }

          if (strlen(delimiter + 1) > 0 && strlen(delimiter + 1) < MAX_SYMBOLS - 1)
          {
            strcpy(config->right_symbols, delimiter + 1);
          }
        }
        else
        {
          // Use the same symbols for both pyramids
          if (strlen(argv[i]) > 0 && strlen(argv[i]) < MAX_SYMBOLS - 1)
          {
            strcpy(config->left_symbols, argv[i]);
            strcpy(config->right_symbols, argv[i]);
          }
        }
      }
      else
      {
        print_error("Missing value for symbols parameter");
      }
    }
    else if (strcmp(argv[i], "--random") == 0 || strcmp(argv[i], "-r") == 0)
    {
      config->random_symbols = true;
    }
    else if (strcmp(argv[i], "--color-left") == 0 || strcmp(argv[i], "-cl") == 0)
    {
      config->use_color_left = true;
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        strcpy(config->left_color, argv[++i]);
      }
    }
    else if (strcmp(argv[i], "--color-right") == 0 || strcmp(argv[i], "-cr") == 0)
    {
      config->use_color_right = true;
      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
        strcpy(config->right_color, argv[++i]);
      }
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (++i < argc)
      {
        config->threads = atoi(argv[i]);
        if (config->threads < 1)
        {
          config->threads = 1;
        }
      }
      else
      {
        print_error("Missing value for threads parameter");
      }
    }
  }
}

/**
 * @brief Display program usage information
 */
void print_help()
{
  printf("\nCharacter Pyramid Printer\n\n");
  printf("This program prints two pyramids of characters side by side with various customization options.\n\n");
  printf("Usage: start [options]\n\n");
  printf("Options:\n");
  printf("  -h, --help              Show this help message\n");
  printf("  -row, --rows N          Set number of rows in pyramids (1-300, default: 5)\n");
  printf("  -col, --columns N       Set number of columns in pyramids (1-300, default: 5)\n");
  printf("  -s, --symbols CHARS     Set symbols to use (format: 'LEFT,RIGHT' or 'BOTH')\n");
  printf("  -r, --random            Use random symbols from the provided set\n");
  printf("  -cl, --color-left COLOR Apply color to left pyramid (red, green, blue, etc.)\n");
  printf("  -cr, --color-right COLOR Apply color to right pyramid\n");
  printf("  -t, --threads N         Set number of threads for parallel processing\n\n");
  printf("Available colors: red, green, yellow, blue, magenta, cyan, white\n");
}