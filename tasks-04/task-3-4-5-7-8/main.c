#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <ctype.h>
#include <errno.h>

#define MAX_DIM 100
#define DEFAULT_THREADS 4
#define DEFAULT_WIDTH 8
#define DEFAULT_ROW 10

typedef struct
{
  enum
  {
    OP_NONE,
    OP_STRING_TO_INT,
    OP_MATRIX_TRANSPOSE,
    OP_SWAP_MIN_MAX,
    OP_MATRIX_MULTIPLY,
    OP_MERGE_SORTED_ARRAYS
  } operation;

  char *input_file1;
  char *input_file2;
  char *output_file;
  char *number_string;
  int threads;
  bool verbose;
  bool write_output;
} Config;

// Matrix structure
typedef struct
{
  int rows;
  int cols;
  double data[MAX_DIM * MAX_DIM];
} Matrix;

// Function prototypes
void parse_args(int argc, char *argv[], Config *config);
void print_help(void);
int string_to_int(const char *str);
void matrix_transpose(Matrix *mat, Matrix *result, int threads);
void swap_min_max(double *array, int size, int threads);
void matrix_multiply(Matrix *a, Matrix *b, Matrix *result, int threads);
void merge_sorted_arrays(double *a, int a_size, double *b, int b_size, double *result);
void print_matrix(Matrix *mat, const char *label);
void print_array(double *arr, int size, const char *label);
bool read_matrix_from_file(const char *filename, Matrix *mat);
bool write_matrix_to_file(const char *filename, Matrix *mat);
bool read_array_from_file(const char *filename, double **arr, int *size);
bool write_array_to_file(const char *filename, double *arr, int size);
void print_error(const char *msg);
void handle_file_error(const char *filename, const char *mode);

int main(int argc, char *argv[])
{
  Config config = {
      .operation = OP_NONE,
      .input_file1 = NULL,
      .input_file2 = NULL,
      .output_file = "out.txt",
      .number_string = NULL,
      .threads = DEFAULT_THREADS,
      .verbose = false,
      .write_output = false};
  double start_time, end_time;

  parse_args(argc, argv, &config);

  start_time = omp_get_wtime();

  switch (config.operation)
  {
  case OP_STRING_TO_INT:
  {
    if (config.number_string == NULL)
    {
      print_error("No number string provided");
      exit(1);
    }
    int result = string_to_int(config.number_string);
    printf("String '%s' converted to integer: %d\n", config.number_string, result);
    break;
  }

  case OP_MATRIX_TRANSPOSE:
  {
    if (config.input_file1 == NULL)
    {
      print_error("No input file provided for matrix transpose");
      exit(1);
    }

    Matrix mat, result;
    if (!read_matrix_from_file(config.input_file1, &mat))
    {
      exit(1);
    }

    result.rows = mat.cols;
    result.cols = mat.rows;
    matrix_transpose(&mat, &result, config.threads);

    print_matrix(&mat, "Original Matrix");
    print_matrix(&result, "Transposed Matrix");

    if (config.write_output)
    {
      write_matrix_to_file(config.output_file, &result);
    }
    break;
  }

  case OP_SWAP_MIN_MAX:
  {
    if (config.input_file1 == NULL)
    {
      print_error("No input file provided for array operation");
      exit(1);
    }

    double *array;
    int size;
    if (!read_array_from_file(config.input_file1, &array, &size))
    {
      exit(1);
    }

    print_array(array, size, "Original Array");
    swap_min_max(array, size, config.threads);
    print_array(array, size, "After Swapping Min/Max");

    if (config.write_output)
    {
      write_array_to_file(config.output_file, array, size);
    }
    free(array);
    break;
  }

  case OP_MATRIX_MULTIPLY:
  {
    if (config.input_file1 == NULL || config.input_file2 == NULL)
    {
      print_error("Two input files required for matrix multiplication");
      exit(1);
    }

    Matrix a, b, result;
    if (!read_matrix_from_file(config.input_file1, &a) ||
        !read_matrix_from_file(config.input_file2, &b))
    {
      exit(1);
    }

    if (a.cols != b.rows)
    {
      print_error("Matrix dimensions incompatible for multiplication");
      exit(1);
    }

    result.rows = a.rows;
    result.cols = b.cols;
    matrix_multiply(&a, &b, &result, config.threads);

    print_matrix(&a, "Matrix A");
    print_matrix(&b, "Matrix B");
    print_matrix(&result, "Product A×B");

    if (config.write_output)
    {
      write_matrix_to_file(config.output_file, &result);
    }
    break;
  }

  case OP_MERGE_SORTED_ARRAYS:
  {
    if (config.input_file1 == NULL || config.input_file2 == NULL)
    {
      print_error("Two input files required for merging arrays");
      exit(1);
    }

    double *a, *b;
    int a_size, b_size;
    if (!read_array_from_file(config.input_file1, &a, &a_size) ||
        !read_array_from_file(config.input_file2, &b, &b_size))
    {
      exit(1);
    }

    double *merged = malloc((a_size + b_size) * sizeof(double));
    if (merged == NULL)
    {
      print_error("Memory allocation failed");
      free(a);
      free(b);
      exit(1);
    }

    merge_sorted_arrays(a, a_size, b, b_size, merged);

    print_array(a, a_size, "Array A");
    print_array(b, b_size, "Array B");
    print_array(merged, a_size + b_size, "Merged Array");

    if (config.write_output)
    {
      write_array_to_file(config.output_file, merged, a_size + b_size);
    }
    free(a);
    free(b);
    free(merged);
    break;
  }

  default:
    print_help();
    break;
  }

  end_time = omp_get_wtime();
  if (config.verbose)
  {
    printf("\nExecution time: %.4f seconds\n", end_time - start_time);
  }

  return 0;
}

// Convert numeric string to integer without atoi()
int string_to_int(const char *str)
{
  int result = 0;
  int sign = 1;
  int i = 0;

  // Handle optional sign
  if (str[0] == '-')
  {
    sign = -1;
    i++;
  }
  else if (str[0] == '+')
  {
    i++;
  }

  // Process each character
  for (; str[i] != '\0'; i++)
  {
    if (!isdigit(str[i]))
    {
      print_error("Invalid character in number string");
      exit(1);
    }
    result = result * 10 + (str[i] - '0');
  }

  return sign * result;
}

// Transpose matrix stored in 1D array
void matrix_transpose(Matrix *mat, Matrix *result, int threads)
{
#pragma omp parallel for num_threads(threads)
  for (int i = 0; i < mat->rows; i++)
  {
    for (int j = 0; j < mat->cols; j++)
    {
      result->data[j * result->cols + i] = mat->data[i * mat->cols + j];
    }
  }
}

// Swap minimum and maximum in array
void swap_min_max(double *array, int size, int threads)
{
  if (size < 1)
    return;

  int min_idx = 0, max_idx = 0;
  double min_val = array[0], max_val = array[0];

#pragma omp parallel num_threads(threads)
  {
    double local_min = min_val;
    double local_max = max_val;
    int local_min_idx = min_idx;
    int local_max_idx = max_idx;

#pragma omp for nowait
    for (int i = 1; i < size; i++)
    {
      if (array[i] < local_min)
      {
        local_min = array[i];
        local_min_idx = i;
      }
      if (array[i] > local_max)
      {
        local_max = array[i];
        local_max_idx = i;
      }
    }

#pragma omp critical
    {
      if (local_min < min_val)
      {
        min_val = local_min;
        min_idx = local_min_idx;
      }
      if (local_max > max_val)
      {
        max_val = local_max;
        max_idx = local_max_idx;
      }
    }
  }

  double temp = array[min_idx];
  array[min_idx] = array[max_idx];
  array[max_idx] = temp;
}
// Matrix multiplication
void matrix_multiply(Matrix *a, Matrix *b, Matrix *result, int threads)
{
#pragma omp parallel for num_threads(threads)
  for (int i = 0; i < a->rows; i++)
  {
    for (int j = 0; j < b->cols; j++)
    {
      double sum = 0.0;
      for (int k = 0; k < a->cols; k++)
      {
        sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];
      }
      result->data[i * result->cols + j] = sum;
    }
  }
}

// Merge two sorted arrays
void merge_sorted_arrays(double *a, int a_size, double *b, int b_size, double *result)
{
  int i = 0, j = 0, k = 0;

  while (i < a_size && j < b_size)
  {
    if (a[i] < b[j])
    {
      result[k++] = a[i++];
    }
    else
    {
      result[k++] = b[j++];
    }
  }

  while (i < a_size)
  {
    result[k++] = a[i++];
  }

  while (j < b_size)
  {
    result[k++] = b[j++];
  }
}

// Print matrix with label
void print_matrix(Matrix *mat, const char *label)
{
  printf("%s (%dx%d):\n", label, mat->rows, mat->cols);
  for (int i = 0; i < mat->rows; i++)
  {
    for (int j = 0; j < mat->cols; j++)
    {
      printf("%8.2f ", mat->data[i * mat->cols + j]);
    }
    printf("\n");
  }
  printf("\n");
}

// Print array with label
void print_array(double *arr, int size, const char *label)
{
  printf("%s (%d elements):\n", label, size);
  for (int i = 0; i < size; i++)
  {
    printf("%8.2f ", arr[i]);
    if ((i + 1) % DEFAULT_ROW == 0)
      printf("\n");
  }
  if (size % DEFAULT_ROW != 0)
    printf("\n");
  printf("\n");
}

// Read matrix from file
bool read_matrix_from_file(const char *filename, Matrix *mat)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    handle_file_error(filename, "read");
    return false;
  }

  int rows = 0, cols = 0;
  char line[1024];

  // First pass to determine dimensions
  while (fgets(line, sizeof(line), file))
  {
    if (strlen(line) < 2)
      continue; // Skip empty lines

    int current_cols = 0;
    char *ptr = line;
    while (*ptr != '\0')
    {
      if (isdigit(*ptr) || *ptr == '-' || *ptr == '.')
      {
        current_cols++;
        while (isdigit(*ptr) || *ptr == '-' || *ptr == '.')
          ptr++;
      }
      ptr++;
    }

    if (cols == 0)
    {
      cols = current_cols;
    }
    else if (current_cols != cols)
    {
      print_error("Inconsistent column count in matrix file");
      fclose(file);
      return false;
    }
    rows++;
  }

  // Reset file pointer
  rewind(file);

  // Check dimensions
  if (rows > MAX_DIM || cols > MAX_DIM)
  {
    print_error("Matrix dimensions exceed maximum allowed size");
    fclose(file);
    return false;
  }

  // Read data
  mat->rows = rows;
  mat->cols = cols;
  int index = 0;

  while (fgets(line, sizeof(line), file))
  {
    if (strlen(line) < 2)
      continue;

    char *token = strtok(line, " \t\n");
    while (token != NULL)
    {
      if (index >= MAX_DIM * MAX_DIM)
      {
        print_error("Matrix data exceeds maximum capacity");
        fclose(file);
        return false;
      }
      if (sscanf(token, "%lf", &mat->data[index]) != 1)
      {
        print_error("Invalid matrix data format");
        fclose(file);
        return false;
      }
      index++;
      token = strtok(NULL, " \t\n");
    }
  }

  fclose(file);
  return true;
}

// Write matrix to file
bool write_matrix_to_file(const char *filename, Matrix *mat)
{
  FILE *file = fopen(filename, "w");
  if (file == NULL)
  {
    handle_file_error(filename, "write");
    return false;
  }

  for (int i = 0; i < mat->rows; i++)
  {
    for (int j = 0; j < mat->cols; j++)
    {
      fprintf(file, "%.2f", mat->data[i * mat->cols + j]);
      if (j < mat->cols - 1)
        fprintf(file, " ");
    }
    fprintf(file, "\n");
  }

  fclose(file);
  return true;
}

// Read array from file
bool read_array_from_file(const char *filename, double **arr, int *size)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    handle_file_error(filename, "read");
    return false;
  }

  // Count elements first
  *size = 0;
  char line[1024];
  while (fgets(line, sizeof(line), file))
  {
    char *ptr = line;
    while (*ptr != '\0')
    {
      if (isdigit(*ptr) || *ptr == '-' || *ptr == '.')
      {
        (*size)++;
        while (isdigit(*ptr) || *ptr == '-' || *ptr == '.')
          ptr++;
      }
      ptr++;
    }
  }

  // Allocate memory
  *arr = malloc(*size * sizeof(double));
  if (*arr == NULL)
  {
    print_error("Memory allocation failed");
    fclose(file);
    return false;
  }

  // Read data
  rewind(file);
  int index = 0;
  while (fgets(line, sizeof(line), file))
  {
    char *token = strtok(line, " \t\n");
    while (token != NULL)
    {
      if (sscanf(token, "%lf", &(*arr)[index]) != 1)
      {
        print_error("Invalid array data format");
        free(*arr);
        fclose(file);
        return false;
      }
      index++;
      token = strtok(NULL, " \t\n");
    }
  }

  fclose(file);
  return true;
}

// Write array to file
bool write_array_to_file(const char *filename, double *arr, int size)
{
  FILE *file = fopen(filename, "w");
  if (file == NULL)
  {
    handle_file_error(filename, "write");
    return false;
  }

  for (int i = 0; i < size; i++)
  {
    fprintf(file, "%.2f", arr[i]);
    if (i < size - 1)
      fprintf(file, " ");
  }
  fprintf(file, "\n");

  fclose(file);
  return true;
}

// Handle file errors
void handle_file_error(const char *filename, const char *mode)
{
  fprintf(stderr, "Error: Could not %s file '%s' (%s)\n",
          mode, filename, strerror(errno));
}

// Print error message
void print_error(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
}

// Parse command line arguments
void parse_args(int argc, char *argv[], Config *config)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      print_help();
      exit(0);
    }
    else if (strcmp(argv[i], "--string") == 0 || strcmp(argv[i], "-s") == 0)
    {
      if (i + 1 < argc)
      {
        config->operation = OP_STRING_TO_INT;
        config->number_string = argv[++i];
      }
      else
      {
        print_error("Missing string argument");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--transpose") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (i + 1 < argc)
      {
        config->operation = OP_MATRIX_TRANSPOSE;
        config->input_file1 = argv[++i];
        config->write_output = true;
      }
      else
      {
        print_error("Missing input file for transpose");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--multiply") == 0 || strcmp(argv[i], "-m") == 0)
    {
      if (i + 2 < argc)
      {
        config->operation = OP_MATRIX_MULTIPLY;
        config->input_file1 = argv[++i];
        config->input_file2 = argv[++i];
        config->write_output = true;
      }
      else
      {
        print_error("Missing input files for multiplication");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--swap") == 0 || strcmp(argv[i], "-x") == 0)
    {
      if (i + 1 < argc)
      {
        config->operation = OP_SWAP_MIN_MAX;
        config->input_file1 = argv[++i];
        config->write_output = true;
      }
      else
      {
        print_error("Missing input file for swap operation");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--merge") == 0 || strcmp(argv[i], "-g") == 0)
    {
      if (i + 2 < argc)
      {
        config->operation = OP_MERGE_SORTED_ARRAYS;
        config->input_file1 = argv[++i];
        config->input_file2 = argv[++i];
        config->write_output = true;
      }
      else
      {
        print_error("Missing input files for merge operation");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0)
    {
      if (i + 1 < argc)
      {
        config->output_file = argv[++i];
        config->write_output = true;
      }
      else
      {
        print_error("Missing output filename");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0)
    {
      config->verbose = true;
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-p") == 0)
    {
      if (i + 1 < argc)
      {
        config->threads = atoi(argv[++i]);
        if (config->threads < 1)
        {
          print_error("Thread count must be positive");
          exit(1);
        }
      }
      else
      {
        print_error("Missing thread count");
        exit(1);
      }
    }
    else
    {
      fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
      print_help();
      exit(1);
    }
  }
}

// Print help message
void print_help()
{
  printf("Advanced Matrix and Array Operations\n");
  printf("Usage: start [OPTIONS]\n\n");
  printf("Options:\n");
  printf("  -h, --help            Show this help message\n");
  printf("  -s, --string STR      Convert string to integer\n");
  printf("  -t, --transpose FILE  Transpose matrix from file\n");
  printf("  -m, --multiply F1 F2  Multiply two matrices\n");
  printf("  -x, --swap FILE       Swap min/max in array\n");
  printf("  -g, --merge F1 F2     Merge sorted arrays\n");
  printf("  -o, --output FILE     Output file (default: out.txt)\n");
  printf("  -v, --verbose         Show verbose output\n");
  printf("  -p, --threads N       Number of threads (default: 4)\n\n");
}