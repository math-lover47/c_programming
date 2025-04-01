#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <omp.h>

#define MAX_INPUT_LEN 4096
#define MAX_FILENAME_LEN 256
#define MORSE_CODE_SIZE 36

typedef struct
{
  char input_files[MAX_FILENAME_LEN][MAX_FILENAME_LEN];
  char output_files[MAX_FILENAME_LEN][MAX_FILENAME_LEN];
  int file_count;
  bool invert_case;
  bool caesar_cipher;
  bool morse_code;
  bool decode_caesar;
  bool decode_morse;
  int caesar_shift;
  int threads;
  bool help;
} TextConverterConfig;

// Morse code lookup table
const char *morse_code_table[MORSE_CODE_SIZE] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
    "-----", ".----", "..---", "...--", "....-", ".....",
    "-....", "--...", "---..", "----."};

const char morse_chars[MORSE_CODE_SIZE] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

void parse_args(int argc, char *argv[], TextConverterConfig *config);
void print_help(void);
void process_text(TextConverterConfig *config);
void process_files(TextConverterConfig *config);
void invert_case(char *str);
void caesar_cipher(char *str, int shift, bool decode);
void text_to_morse(char *str);
void morse_to_text(char *str);
char *find_morse_char(char morse);
char find_char_morse(const char *morse);

int main(int argc, char *argv[])
{
  TextConverterConfig config = {
      .file_count = 0,
      .invert_case = false,
      .caesar_cipher = false,
      .morse_code = false,
      .decode_caesar = false,
      .decode_morse = false,
      .caesar_shift = 13,
      .threads = 1,
      .help = false};

  parse_args(argc, argv, &config);

  if (config.help)
  {
    print_help();
    return 0;
  }

  double start_time = omp_get_wtime();

  if (config.file_count > 0)
  {
    process_files(&config);
  }
  else
  {
    process_text(&config);
  }

  double end_time = omp_get_wtime();
  printf("\nProcessing completed in %.4f seconds\n", end_time - start_time);

  return 0;
}

void parse_args(int argc, char *argv[], TextConverterConfig *config)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      config->help = true;
    }
    else if (strcmp(argv[i], "--invert") == 0 || strcmp(argv[i], "-i") == 0)
    {
      config->invert_case = true;
    }
    else if (strcmp(argv[i], "--caesar") == 0 || strcmp(argv[i], "-c") == 0)
    {
      config->caesar_cipher = true;
      if (i + 1 < argc && isdigit(argv[i + 1][0]))
      {
        config->caesar_shift = atoi(argv[++i]);
      }
    }
    else if (strcmp(argv[i], "--decode-caesar") == 0 || strcmp(argv[i], "-dc") == 0)
    {
      config->decode_caesar = true;
      if (i + 1 < argc && isdigit(argv[i + 1][0]))
      {
        config->caesar_shift = atoi(argv[++i]);
      }
    }
    else if (strcmp(argv[i], "--morse") == 0 || strcmp(argv[i], "-m") == 0)
    {
      config->morse_code = true;
    }
    else if (strcmp(argv[i], "--decode-morse") == 0 || strcmp(argv[i], "-dm") == 0)
    {
      config->decode_morse = true;
    }
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
    {
      if (++i < argc)
      {
        config->threads = atoi(argv[i]);
        if (config->threads < 1)
          config->threads = 1;
      }
    }
    else if (strcmp(argv[i], "--input") == 0 || strcmp(argv[i], "-in") == 0)
    {
      while (i + 1 < argc && argv[i + 1][0] != '-')
      {
        strncpy(config->input_files[config->file_count], argv[++i], MAX_FILENAME_LEN);
        config->file_count++;
      }
    }
    else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-out") == 0)
    {
      int out_idx = 0;
      while (i + 1 < argc && argv[i + 1][0] != '-')
      {
        strncpy(config->output_files[out_idx++], argv[++i], MAX_FILENAME_LEN);
      }
    }
  }
}

void print_help()
{
  printf("\nText Converter\n\n");
  printf("Usage: start [options]\n\n");
  printf("Options:\n");
  printf("  -h, --help              Show this help message\n");
  printf("  -i, --invert            Invert character cases\n");
  printf("  -c [N], --caesar [N]    Apply Caesar cipher (default shift: 13)\n");
  printf("  -dc [N], --decode-caesar [N]  Decode Caesar cipher (default shift: 13)\n");
  printf("  -m, --morse             Convert text to Morse code\n");
  printf("  -dm, --decode-morse     Convert Morse code to text\n");
  printf("  -in FILE, --input FILE  Input file(s) (multiple files allowed)\n");
  printf("  -out FILE, --output FILE Output file(s) (must match input files count)\n");
  printf("  -t N, --threads N       Number of threads for parallel processing\n\n");
}

void process_text(TextConverterConfig *config)
{
  char input[MAX_INPUT_LEN];
  printf("Enter text to convert: ");
  fgets(input, MAX_INPUT_LEN, stdin);
  input[strcspn(input, "\n")] = '\0'; // Remove newline

  if (config->invert_case)
    invert_case(input);
  if (config->caesar_cipher)
    caesar_cipher(input, config->caesar_shift, false);
  if (config->decode_caesar)
    caesar_cipher(input, config->caesar_shift, true);
  if (config->morse_code)
    text_to_morse(input);
  if (config->decode_morse)
    morse_to_text(input);

  printf("Converted text: %s\n", input);
}

void process_files(TextConverterConfig *config)
{
  if (config->file_count == 0)
    return;

#pragma omp parallel for num_threads(config->threads)
  for (int i = 0; i < config->file_count; i++)
  {
    FILE *input_file = fopen(config->input_files[i], "r");
    if (!input_file)
    {
      printf("Error opening input file: %s\n", config->input_files[i]);
      continue;
    }

    char output_filename[MAX_FILENAME_LEN];
    if (config->output_files[i][0] == '\0')
    {
      snprintf(output_filename, MAX_FILENAME_LEN, "output_%d.txt", i);
    }
    else
    {
      strncpy(output_filename, config->output_files[i], MAX_FILENAME_LEN);
    }

    FILE *output_file = fopen(output_filename, "w");
    if (!output_file)
    {
      printf("Error opening output file: %s\n", output_filename);
      fclose(input_file);
      continue;
    }

    char buffer[MAX_INPUT_LEN];
    while (fgets(buffer, MAX_INPUT_LEN, input_file))
    {
      buffer[strcspn(buffer, "\n")] = '\0';

      if (config->invert_case)
        invert_case(buffer);
      if (config->caesar_cipher)
        caesar_cipher(buffer, config->caesar_shift, false);
      if (config->decode_caesar)
        caesar_cipher(buffer, config->caesar_shift, true);
      if (config->morse_code)
        text_to_morse(buffer);
      if (config->decode_morse)
        morse_to_text(buffer);

      fprintf(output_file, "%s\n", buffer);
    }

    fclose(input_file);
    fclose(output_file);
    printf("Processed file: %s -> %s\n", config->input_files[i], output_filename);
  }
}

void invert_case(char *str)
{
  for (int i = 0; str[i]; i++)
  {
    if (isupper(str[i]))
    {
      str[i] = tolower(str[i]);
    }
    else if (islower(str[i]))
    {
      str[i] = toupper(str[i]);
    }
  }
}

void caesar_cipher(char *str, int shift, bool decode)
{
  if (decode)
    shift = -shift;

  for (int i = 0; str[i]; i++)
  {
    if (isalpha(str[i]))
    {
      char base = isupper(str[i]) ? 'A' : 'a';
      str[i] = (str[i] - base + shift + 26) % 26 + base;
    }
  }
}

void text_to_morse(char *str)
{
  char result[MAX_INPUT_LEN * 6] = ""; // Morse can be up to 6 chars per character
  for (int i = 0; str[i]; i++)
  {
    char c = toupper(str[i]);
    if (c == ' ')
    {
      strcat(result, "/ ");
    }
    else if (isalnum(c))
    {
      int index;
      if (isalpha(c))
      {
        index = c - 'A';
      }
      else
      {
        index = 26 + (c - '0');
      }
      strcat(result, morse_code_table[index]);
      strcat(result, " ");
    }
  }
  strcpy(str, result);
}

void morse_to_text(char *str)
{
  char result[MAX_INPUT_LEN] = "";
  char *token = strtok(str, " ");
  while (token != NULL)
  {
    if (strcmp(token, "/") == 0)
    {
      strcat(result, " ");
    }
    else
    {
      char c = find_char_morse(token);
      if (c != '\0')
      {
        char temp[2] = {c, '\0'};
        strcat(result, temp);
      }
    }
    token = strtok(NULL, " ");
  }
  strcpy(str, result);
}

char find_char_morse(const char *morse)
{
  for (int i = 0; i < MORSE_CODE_SIZE; i++)
  {
    if (strcmp(morse, morse_code_table[i]) == 0)
    {
      return morse_chars[i];
    }
  }
  return '\0';
}