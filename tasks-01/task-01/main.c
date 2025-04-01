/**
 * @file main.c
 * @brief A comprehensive date calculator with various date operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_INPUT_LEN 256 ///< Maximum length for input strings

/**
 * @struct Date
 * @brief Structure representing a date with year, month, and day
 */
typedef struct
{
  int year;  ///< Year component of the date
  int month; ///< Month component (1-12)
  int day;   ///< Day component (1-31)
} Date;

/**
 * @struct Flags
 * @brief Structure containing program flags and options
 */
typedef struct
{
  bool help;        ///< Help flag
  char *month_str;  ///< Month string from -m flag
  bool day_of_week; ///< Calculate day of week flag
  bool day_of_year; ///< Calculate day of year flag
  bool date_diff;   ///< Calculate date difference flag
  Date date;        ///< Date structure
} Flags;

/* Main execution handlers */
void parse_args(int argc, char *argv[], Flags *flags); // CLI argument parser
void handle_input(Flags *flags);                       // Top-level input processor
void validate_and_process(Flags *flags);               // Validates and runs calculations

/* Interactive input handling */
void handle_prompt(Flags *flags, char *input, size_t size); // Shows input prompt
void process_input(Flags *flags, const char *input);        // Processes user input
void parse_year_day_input(Flags *flags, const char *input); // Parses YYYY DD format
void convert_month_string(Flags *flags);                    // Converts month names to numbers

/* Date parsing utilities */
bool parse_data(const char *str, Date *date);      // Parses YYYY-MM-DD or YYYY MM DD
bool parse_int(const char *str, int *value);       // Safe string-to-int conversion
void trim_whitespace(char *str);                   // Trims leading/trailing whitespace
void month_sti(const char *month_str, int *month); // Month name to number (e.g., "january" → 1)

/* Validation */
bool Date_is_valid(const Date *date);          // Checks date validity
static bool is_out_of_range(const Date *date); // Validates year/month/day ranges
static bool is_gregorian(const Date *date);    // Checks month-specific day rules
bool Date_is_leap_year(const Date *date);      // Leap year checker

/* Core calculations */
int Date_calc_day_of_year(const Date *date);           // Returns day number (1-366)
int Date_calc_day_of_week(const Date *date);           // Returns weekday (0=Sun, 6=Sat)
void Date_calc_diff(const Date *date, char *diff_str); // Calculates difference from today

/* Calculation formatters */
void format_difference_string(int diff_days, char *diff_str); // Formats time delta string

/* Result printers */
void print_day_of_year(const Date *date); // Prints day-of-year result
void print_day_of_week(const Date *date); // Prints weekday name
void print_date_diff(const Date *date);   // Prints human-readable date difference
void Date_print_error(const char *msg);   // Standardized error printer
void print_help(void);                    // Displays CLI usage help

/* Command-line flag processors */
bool handle_help_flag(const char *arg, Flags *flags);                 // Processes -h/--help
bool handle_month_flag(int argc, char *argv[], int *i, Flags *flags); // Handles -m/--m
bool handle_operation_flags(const char *arg, Flags *flags);           // Processes --dw/--dy/--df
void handle_date_argument(const char *arg, Flags *flags);             // Fallback for date args

/* ======================== MAIN FUNCTION ======================== */

/**
 * @brief Main program entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return Program exit status
 */
int main(int argc, char *argv[])
{
  Flags flags = {0};
  parse_args(argc, argv, &flags);

  if (flags.help)
  {
    print_help();
    return 0;
  }

  handle_input(&flags);
  validate_and_process(&flags);

  return 0;
}

/* ======================= CORE FUNCTIONS ======================= */

/**
 * @brief Handles interactive date input
 * @param flags Pointer to Flags structure
 */
void handle_input(Flags *flags)
{
  if (flags->date.year == 0 && flags->date.month == 0 && flags->date.day == 0)
  {
    char input[MAX_INPUT_LEN];
    handle_prompt(flags, input, sizeof(input));
    process_input(flags, input);
  }
}

/**
 * @brief Validates and processes the date based on flags
 * @param flags Pointer to Flags structure
 */
void validate_and_process(Flags *flags)
{
  if (!Date_is_valid(&flags->date))
  {
    Date_print_error("Invalid date");
    exit(EXIT_FAILURE);
  }

  if (flags->day_of_year)
  {
    print_day_of_year(&flags->date);
  }

  if (flags->day_of_week)
  {
    print_day_of_week(&flags->date);
  }

  if (flags->date_diff)
  {
    print_date_diff(&flags->date);
  }

  if (!flags->day_of_year && !flags->day_of_week && !flags->date_diff)
  {
    printf("Date is valid\n");
  }
}

/* ==================== DATE INPUT FUNCTIONS ==================== */

/**
 * @brief Prompts user for date input
 * @param flags Pointer to Flags structure
 * @param input Buffer to store input
 * @param size Size of input buffer
 */
void handle_prompt(Flags *flags, char *input, size_t size)
{
  printf("Enter a date (YYYY %s): ",
         flags->month_str ? "DD" : "MM DD");

  if (fgets(input, size, stdin) == NULL)
  {
    Date_print_error("Failed to read input");
    exit(EXIT_FAILURE);
  }
  trim_whitespace(input);
}

/**
 * @brief Processes user input based on flags
 * @param flags Pointer to Flags structure
 * @param input User input string
 */
void process_input(Flags *flags, const char *input)
{
  if (flags->month_str)
  {
    parse_year_day_input(flags, input);
    convert_month_string(flags);
  }
  else
  {
    if (!parse_data(input, &flags->date))
    {
      Date_print_error("Invalid date format");
      exit(EXIT_FAILURE);
    }
  }
}

/**
 * @brief Parses year and day from input when month is provided
 * @param flags Pointer to Flags structure
 * @param input User input string
 */
void parse_year_day_input(Flags *flags, const char *input)
{
  char copy[MAX_INPUT_LEN];
  strncpy(copy, input, sizeof(copy));

  char *token = strtok(copy, " -");
  if (!token || !parse_int(token, &flags->date.year))
  {
    Date_print_error("Invalid year");
    exit(EXIT_FAILURE);
  }

  token = strtok(NULL, " -");
  if (!token || !parse_int(token, &flags->date.day))
  {
    Date_print_error("Invalid day");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Converts month string to numeric value
 * @param flags Pointer to Flags structure
 */
void convert_month_string(Flags *flags)
{
  month_sti(flags->month_str, &flags->date.month);
  if (flags->date.month == 0)
  {
    Date_print_error("Invalid month name");
    exit(EXIT_FAILURE);
  }
}

/* ================== COMMAND LINE FUNCTIONS ==================== */

/**
 * @brief Parses command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @param flags Pointer to Flags structure
 */
void parse_args(int argc, char *argv[], Flags *flags)
{
  for (int i = 1; i < argc; i++)
  {
    if (handle_help_flag(argv[i], flags))
      continue;
    if (handle_month_flag(argc, argv, &i, flags))
      continue;
    if (handle_operation_flags(argv[i], flags))
      continue;
    handle_date_argument(argv[i], flags);
  }
}

/* ====================== DATE VALIDATION ======================= */

/**
 * @brief Validates a Date structure
 * @param date Pointer to Date structure
 * @return true if date is valid, false otherwise
 */
bool Date_is_valid(const Date *date)
{
  return is_out_of_range(date) && is_gregorian(date);
}

/* ====================== UTILITY FUNCTIONS ===================== */

/**
 * @brief Trims whitespace from a string
 * @param str String to trim (modified in place)
 */
void trim_whitespace(char *str)
{
  char *end;
  while (isspace((unsigned char)*str))
    str++;
  if (*str == 0)
    return;

  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  *(end + 1) = 0;
}

/* ==================== DATE CALCULATION FUNCTIONS =============== */

/**
 * @brief Calculates the day of year for a given date
 * @param date Pointer to Date structure
 * @return Day of year (1-366)
 */
int Date_calc_day_of_year(const Date *date)
{
  static const int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int dayOfYear = date->day;

  for (int m = 1; m < date->month; m++)
  {
    dayOfYear += daysInMonth[m];
  }

  if (date->month > 2 && Date_is_leap_year(date))
  {
    dayOfYear += 1;
  }

  return dayOfYear;
}

/**
 * @brief Prints the day of year in human-readable format
 * @param date Pointer to Date structure
 */
void print_day_of_year(const Date *date)
{
  printf("Day of year: %d\n", Date_calc_day_of_year(date));
}

/**
 * @brief Calculates the day of week for a given date
 * @param date Pointer to Date structure
 * @return Day of week (0=Sunday, 6=Saturday)
 */
int Date_calc_day_of_week(const Date *date)
{
  struct tm tm = {0};
  tm.tm_year = date->year - 1900;
  tm.tm_mon = date->month - 1;
  tm.tm_mday = date->day;
  tm.tm_hour = 0;
  mktime(&tm);
  return tm.tm_wday;
}

/**
 * @brief Prints the day of week in human-readable format
 * @param date Pointer to Date structure
 */
void print_day_of_week(const Date *date)
{
  const char *days[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                        "Thursday", "Friday", "Saturday"};
  int dow = Date_calc_day_of_week(date);
  printf("Day of week: %s\n", days[dow]);
}

/**
 * @brief Calculates and prints the difference between date and today
 * @param date Pointer to Date structure
 */
void print_date_diff(const Date *date)
{
  char diff_str[100];
  Date_calc_diff(date, diff_str);
  printf("Date difference: %s\n", diff_str);
}

/**
 * @brief Calculates the difference between a date and today
 * @param date Pointer to Date structure
 * @param diff_str Buffer to store the difference string
 */
void Date_calc_diff(const Date *date, char *diff_str)
{
  time_t now = time(NULL);
  struct tm tm_date = {0};

  tm_date.tm_year = date->year - 1900;
  tm_date.tm_mon = date->month - 1;
  tm_date.tm_mday = date->day;
  time_t target = mktime(&tm_date);

  double diff_sec = difftime(target, now);
  int diff_days = (int)(diff_sec / (60 * 60 * 24));

  format_difference_string(diff_days, diff_str);
}

/**
 * @brief Formats the date difference into human-readable string
 * @param diff_days Number of days difference
 * @param diff_str Buffer to store the formatted string
 */
void format_difference_string(int diff_days, char *diff_str)
{
  char *direction = (diff_days < 0) ? "ago" : "after";
  diff_days = abs(diff_days);

  int years = diff_days / 365;
  int months = (diff_days % 365) / 30;
  int days = (diff_days % 365) % 30;

  if (years > 0)
  {
    sprintf(diff_str, "%d years, %d months, %d days %s",
            years, months, days, direction);
  }
  else if (months > 0)
  {
    sprintf(diff_str, "%d months, %d days %s", months, days, direction);
  }
  else if (days > 0)
  {
    sprintf(diff_str, "%d days %s", days, direction);
  }
  else
  {
    sprintf(diff_str, "its today");
  }
}

/* ====================== DATE PARSING FUNCTIONS ================ */

/**
 * @brief Parses a date string into Date structure
 * @param str Input date string (YYYY MM DD or YYYY-MM-DD)
 * @param date Pointer to Date structure to populate
 * @return true if parsing succeeded, false otherwise
 */
bool parse_data(const char *str, Date *date)
{
  char copy[MAX_INPUT_LEN];
  strncpy(copy, str, sizeof(copy));
  copy[sizeof(copy) - 1] = '\0';

  char *token = strtok(copy, " -");
  if (!token)
    return false;
  date->year = atoi(token);

  token = strtok(NULL, " -");
  if (!token)
    return false;
  date->month = atoi(token);

  token = strtok(NULL, " -");
  if (!token)
    return false;
  date->day = atoi(token);

  return true;
}

/**
 * @brief Parses an integer from string with validation
 * @param str Input string containing integer
 * @param value Pointer to store parsed value
 * @return true if parsing succeeded, false otherwise
 */
bool parse_int(const char *str, int *value)
{
  char *endptr;
  long num = strtol(str, &endptr, 10);
  if (endptr == str || *endptr != '\0')
  {
    return false;
  }
  *value = (int)num;
  return true;
}

/**
 * @brief Converts month name string to month number (1-12)
 * @param month_str Month name string (case insensitive)
 * @param month Pointer to store month number
 */
void month_sti(const char *month_str, int *month)
{
  const char *months[] = {"january", "february", "march", "april", "may", "june",
                          "july", "august", "september", "october", "november", "december"};

  char lower[20];
  strncpy(lower, month_str, sizeof(lower));
  for (int i = 0; lower[i]; i++)
  {
    lower[i] = tolower(lower[i]);
  }

  for (int i = 0; i < 12; i++)
  {
    if (strstr(months[i], lower) == months[i])
    {
      *month = i + 1;
      return;
    }
  }
  *month = 0;
}

/* ====================== VALIDATION FUNCTIONS ================== */

/**
 * @brief Checks if a year is a leap year
 * @param date Pointer to Date structure
 * @return true if leap year, false otherwise
 */
bool Date_is_leap_year(const Date *date)
{
  int year = date->year;
  if (year % 400 == 0)
    return true;
  if (year % 100 == 0)
    return false;
  if (year % 4 == 0)
    return true;
  return false;
}

/**
 * @brief Checks if date components are within valid ranges
 * @param date Pointer to Date structure
 * @return true if components are in range, false otherwise
 */
static bool is_out_of_range(const Date *date)
{
  if (date->year < 1)
  {
    Date_print_error("Year must be positive");
    return false;
  }
  if (date->month < 1 || date->month > 12)
  {
    Date_print_error("Month must be 1-12");
    return false;
  }
  if (date->day < 1 || date->day > 31)
  {
    Date_print_error("Day must be 1-31");
    return false;
  }
  return true;
}

/**
 * @brief Validates date according to Gregorian calendar rules
 * @param date Pointer to Date structure
 * @return true if date is valid, false otherwise
 */
static bool is_gregorian(const Date *date)
{
  switch (date->month)
  {
  case 4:
  case 6:
  case 9:
  case 11:
    if (date->day > 30)
    {
      Date_print_error("This month has maximum 30 days");
      return false;
    }
    break;

  case 2:
    if (Date_is_leap_year(date))
    {
      if (date->day > 29)
      {
        Date_print_error("February has 29 days in a leap year");
        return false;
      }
    }
    else
    {
      if (date->day > 28)
      {
        Date_print_error("February has 28 days in a not leap year");
        return false;
      }
    }
    break;
  }
  return true;
}

/* ====================== HELPER FUNCTIONS ====================== */

/**
 * @brief Prints an error message to stderr
 * @param msg Error message to print
 */
void Date_print_error(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
}

/**
 * @brief Handles the --help/-h command line flag
 * @param arg Current argument being processed
 * @param flags Pointer to Flags structure
 * @return true if help flag was processed
 */
bool handle_help_flag(const char *arg, Flags *flags)
{
  if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0)
  {
    flags->help = true;
    return true;
  }
  return false;
}

/**
 * @brief Handles the --m/-m month command line flag
 * @param argc Argument count
 * @param argv Argument vector
 * @param i Pointer to current argument index
 * @param flags Pointer to Flags structure
 * @return true if month flag was processed
 */
bool handle_month_flag(int argc, char *argv[], int *i, Flags *flags)
{
  if (strcmp(argv[*i], "--m") == 0 || strcmp(argv[*i], "-m") == 0)
  {
    if (*i + 1 < argc)
    {
      flags->month_str = argv[++*i];
      return true;
    }
    Date_print_error("Missing month argument");
    exit(EXIT_FAILURE);
  }
  return false;
}

/**
 * @brief Handles operation flags (--dw, --dy, --df)
 * @param arg Current argument being processed
 * @param flags Pointer to Flags structure
 * @return true if operation flag was processed
 */
bool handle_operation_flags(const char *arg, Flags *flags)
{
  if (strcmp(arg, "--dw") == 0 || strcmp(arg, "-dw") == 0)
  {
    flags->day_of_week = true;
    return true;
  }
  if (strcmp(arg, "--dy") == 0 || strcmp(arg, "-dy") == 0)
  {
    flags->day_of_year = true;
    return true;
  }
  if (strcmp(arg, "--df") == 0 || strcmp(arg, "-df") == 0)
  {
    flags->date_diff = true;
    return true;
  }
  return false;
}

/**
 * @brief Handles date argument parsing
 * @param arg Current argument being processed
 * @param flags Pointer to Flags structure
 */
void handle_date_argument(const char *arg, Flags *flags)
{
  if (!parse_data(arg, &flags->date))
  {
    Date_print_error("Invalid argument");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Prints help message
 */
void print_help()
{
  printf("Date Calculator\n");
  printf("Usage: start [options] [date]\n");
  printf("Options:\n");
  printf("  -h, --help     Show this help message\n");
  printf("  -m, --m MONTH  Set month by name (e.g. december)\n");
  printf("  -dw, --dw      Calculate day of week\n");
  printf("  -dy, --dy      Calculate day of year\n");
  printf("  -df, --df      Calculate difference from today\n");
  printf("\nDate format: YYYY MM DD or YYYY-MM-DD\n");
}
