/**
 * @file pointer_operations.c
 * @brief Tree and linked list operations with parallel processing
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <stdbool.h>

#define MAX_TREE_DEPTH 10
#define MAX_CHILDREN 5
#define MAX_LIST_LENGTH 26
#define DEFAULT_THREADS 4

typedef struct ListNode
{
  char data;
  struct ListNode *next;
} ListNode;

typedef struct TreeNode
{
  int data;
  struct TreeNode **children;
  int child_count;
} TreeNode;

typedef struct
{
  enum
  {
    OP_NONE,
    OP_REVERSE_LIST,
    OP_BINARY_TREE_DEPTH,
    OP_MBRANCH_TREE_DEPTH,
    OP_GENERATE_TREE,
    OP_LIST_OPERATIONS
  } operation;

  int list_length;
  int m_branches;
  int threads;
  bool verbose;
  bool write_output;
  char *output_file;
  bool use_parallel;
  int tree_depth;
  bool show_original;
} Config;

// Function prototypes
void parse_args(int argc, char *argv[], Config *config);
void print_help();
ListNode *create_list(int length);
void reverse_list(ListNode **head);
void print_list(ListNode *head, FILE *stream);
void free_list(ListNode *head);
TreeNode *create_binary_tree(int depth, int threads);
TreeNode *create_mbranch_tree(int depth, int branches, int threads);
int binary_tree_depth(TreeNode *root);
int mbranch_tree_depth(TreeNode *root);
void print_tree(TreeNode *root, int level, FILE *stream);
void free_tree(TreeNode *root);
double measure_time(double start, double end);
void print_error(const char *msg);
ListNode *copy_list(ListNode *head);

int main(int argc, char *argv[])
{
  // Seed random number generator
  srand(time(NULL));

  Config config = {
      .operation = OP_NONE,
      .list_length = 0,
      .m_branches = 2,
      .threads = DEFAULT_THREADS,
      .verbose = false,
      .write_output = false,
      .output_file = "out.txt",
      .use_parallel = true,
      .tree_depth = 4,
      .show_original = false};

  parse_args(argc, argv, &config);

  // Set OpenMP thread count globally
  omp_set_num_threads(config.threads);

  double start_time = omp_get_wtime();
  FILE *output_stream = config.write_output ? fopen(config.output_file, "w") : stdout;

  if (output_stream == NULL && config.write_output)
  {
    print_error("Could not open output file");
    return 1;
  }

  switch (config.operation)
  {
  case OP_REVERSE_LIST:
  case OP_LIST_OPERATIONS:
  {
    if (config.list_length < 1 || config.list_length > MAX_LIST_LENGTH)
    {
      print_error("List length must be between 1 and 26");
      return 1;
    }

    ListNode *head = create_list(config.list_length);
    fprintf(output_stream, "Original list: ");
    print_list(head, output_stream);

    if (config.operation == OP_LIST_OPERATIONS && config.show_original)
    {
      // Make a copy to keep original for comparison
      ListNode *head_copy = copy_list(head);
      reverse_list(&head);
      fprintf(output_stream, "Reversed list: ");
      print_list(head, output_stream);

      // Reset to original for demonstration
      free_list(head);
      fprintf(output_stream, "Original list preserved: ");
      print_list(head_copy, output_stream);
      free_list(head_copy);
    }
    else
    {
      reverse_list(&head);
      fprintf(output_stream, "Reversed list: ");
      print_list(head, output_stream);
      free_list(head);
    }
    break;
  }

  case OP_BINARY_TREE_DEPTH:
  {
    int depth = (config.tree_depth > 0) ? config.tree_depth : (rand() % MAX_TREE_DEPTH + 1);
    TreeNode *root = create_binary_tree(depth, config.use_parallel ? config.threads : 1);

    if (config.verbose)
    {
      fprintf(output_stream, "Binary tree structure (depth=%d):\n", depth);
      print_tree(root, 0, output_stream);
    }

    int calculated_depth = binary_tree_depth(root);
    fprintf(output_stream, "Binary tree depth: %d\n", calculated_depth);

    free_tree(root);
    break;
  }

  case OP_MBRANCH_TREE_DEPTH:
  {
    if (config.m_branches < 1)
    {
      print_error("Number of branches must be at least 1");
      return 1;
    }

    int depth = (config.tree_depth > 0) ? config.tree_depth : (rand() % MAX_TREE_DEPTH + 1);
    TreeNode *root = create_mbranch_tree(depth, config.m_branches, config.use_parallel ? config.threads : 1);

    if (config.verbose)
    {
      fprintf(output_stream, "M-branch tree structure (M=%d, depth=%d):\n", config.m_branches, depth);
      print_tree(root, 0, output_stream);
    }

    int calculated_depth = mbranch_tree_depth(root);
    fprintf(output_stream, "M-branch tree depth (M=%d): %d\n", config.m_branches, calculated_depth);

    free_tree(root);
    break;
  }

  case OP_GENERATE_TREE:
  {
    int depth = (config.tree_depth > 0) ? config.tree_depth : (rand() % MAX_TREE_DEPTH + 1);
    int branches = (config.m_branches > 0) ? config.m_branches : (rand() % MAX_CHILDREN + 1);

    TreeNode *root = create_mbranch_tree(depth, branches, config.use_parallel ? config.threads : 1);

    fprintf(output_stream, "Generated tree (branches=%d, depth=%d):\n", branches, depth);
    print_tree(root, 0, output_stream);

    free_tree(root);
    break;
  }

  default:
    print_help();
    break;
  }

  if (config.verbose)
  {
    double end_time = omp_get_wtime();
    fprintf(output_stream, "Execution time: %.4f seconds\n", measure_time(start_time, end_time));
    fprintf(output_stream, "Thread count: %d\n", config.threads);
  }

  if (config.write_output)
  {
    fclose(output_stream);
    if (config.verbose)
    {
      printf("Output written to %s\n", config.output_file);
    }
  }

  return 0;
}

// Create a copy of a linked list
ListNode *copy_list(ListNode *head)
{
  if (head == NULL)
    return NULL;

  ListNode *new_head = malloc(sizeof(ListNode));
  ListNode *current_new = new_head;
  ListNode *current_old = head;

  while (current_old != NULL)
  {
    current_new->data = current_old->data;

    if (current_old->next != NULL)
    {
      current_new->next = malloc(sizeof(ListNode));
      current_new = current_new->next;
    }
    else
    {
      current_new->next = NULL;
    }

    current_old = current_old->next;
  }

  return new_head;
}

// Create a singly linked list with uppercase letters
ListNode *create_list(int length)
{
  if (length < 1 || length > MAX_LIST_LENGTH)
    return NULL;

  ListNode *head = malloc(sizeof(ListNode));
  ListNode *current = head;

  for (int i = 0; i < length; i++)
  {
    current->data = 'A' + i;
    if (i < length - 1)
    {
      current->next = malloc(sizeof(ListNode));
      current = current->next;
    }
    else
    {
      current->next = NULL;
    }
  }

  return head;
}

// Reverse a singly linked list
void reverse_list(ListNode **head)
{
  ListNode *prev = NULL;
  ListNode *current = *head;
  ListNode *next = NULL;

  while (current != NULL)
  {
    next = current->next;
    current->next = prev;
    prev = current;
    current = next;
  }

  *head = prev;
}

// Print the linked list
void print_list(ListNode *head, FILE *stream)
{
  ListNode *current = head;
  while (current != NULL)
  {
    fprintf(stream, "%c", current->data);
    if (current->next != NULL)
    {
      fprintf(stream, "->");
    }
    current = current->next;
  }
  fprintf(stream, "->NIL\n");
}

// Free the linked list memory
void free_list(ListNode *head)
{
  ListNode *current = head;
  while (current != NULL)
  {
    ListNode *next = current->next;
    free(current);
    current = next;
  }
}

// Create a random binary tree
TreeNode *create_binary_tree(int depth, int threads)
{
  if (depth <= 0)
    return NULL;

  TreeNode *node = malloc(sizeof(TreeNode));
  node->data = rand() % 100;
  node->child_count = 2;
  node->children = malloc(2 * sizeof(TreeNode *));

  // Initialize children to NULL for safety
  node->children[0] = NULL;
  node->children[1] = NULL;

  if (threads > 1)
  {
#pragma omp parallel sections num_threads(2)
    {
#pragma omp section
      {
        node->children[0] = create_binary_tree(depth - 1, threads / 2);
      }

#pragma omp section
      {
        node->children[1] = create_binary_tree(depth - 1, threads / 2);
      }
    }
  }
  else
  {
    node->children[0] = create_binary_tree(depth - 1, 1);
    node->children[1] = create_binary_tree(depth - 1, 1);
  }

  return node;
}

// Create a random M-branch tree
TreeNode *create_mbranch_tree(int depth, int branches, int threads)
{
  if (depth <= 0)
    return NULL;

  TreeNode *node = malloc(sizeof(TreeNode));
  node->data = rand() % 100;
  node->child_count = branches;
  node->children = malloc(branches * sizeof(TreeNode *));

  // Initialize all children to NULL for safety
  for (int i = 0; i < branches; i++)
  {
    node->children[i] = NULL;
  }

  if (threads > 1 && branches > 1)
  {
    int threads_per_branch = threads / branches;
    threads_per_branch = (threads_per_branch < 1) ? 1 : threads_per_branch;

#pragma omp parallel for num_threads(threads > branches ? branches : threads)
    for (int i = 0; i < branches; i++)
    {
      node->children[i] = create_mbranch_tree(depth - 1, branches, threads_per_branch);
    }
  }
  else
  {
    for (int i = 0; i < branches; i++)
    {
      node->children[i] = create_mbranch_tree(depth - 1, branches, 1);
    }
  }

  return node;
}

// Calculate binary tree depth recursively
int binary_tree_depth(TreeNode *root)
{
  if (root == NULL)
    return 0;

  int left_depth = 0;
  int right_depth = 0;

  if (root->child_count > 0 && root->children[0] != NULL)
    left_depth = binary_tree_depth(root->children[0]);

  if (root->child_count > 1 && root->children[1] != NULL)
    right_depth = binary_tree_depth(root->children[1]);

  return 1 + (left_depth > right_depth ? left_depth : right_depth);
}

// Calculate M-branch tree depth recursively
int mbranch_tree_depth(TreeNode *root)
{
  if (root == NULL)
    return 0;

  int max_depth = 0;
  for (int i = 0; i < root->child_count; i++)
  {
    if (root->children[i] != NULL)
    {
      int current_depth = mbranch_tree_depth(root->children[i]);
      if (current_depth > max_depth)
      {
        max_depth = current_depth;
      }
    }
  }

  return 1 + max_depth;
}

// Improved tree visualization function
void print_tree(TreeNode *root, int level, FILE *stream)
{
  if (root == NULL)
    return;

  // Print indentation and node value
  for (int i = 0; i < level; i++)
  {
    if (i == level - 1)
    {
      fprintf(stream, "|-- ");
    }
    else
    {
      fprintf(stream, "|   ");
    }
  }
  fprintf(stream, "%d\n", root->data);

  // Print children
  for (int i = 0; i < root->child_count; i++)
  {
    if (root->children[i] != NULL)
    {
      print_tree(root->children[i], level + 1, stream);
    }
  }

  // Print empty line after the complete tree
  if (level == 0)
  {
    fprintf(stream, "\n");
  }
}

// Free tree memory
void free_tree(TreeNode *root)
{
  if (root == NULL)
    return;

  for (int i = 0; i < root->child_count; i++)
  {
    if (root->children[i] != NULL)
    {
      free_tree(root->children[i]);
    }
  }

  free(root->children);
  free(root);
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
    else if (strcmp(argv[i], "--reverse-list") == 0 || strcmp(argv[i], "-r") == 0)
    {
      if (i + 1 < argc)
      {
        config->operation = OP_REVERSE_LIST;
        config->list_length = atoi(argv[++i]);
      }
      else
      {
        print_error("Missing list length");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--list-operations") == 0 || strcmp(argv[i], "-l") == 0)
    {
      if (i + 1 < argc)
      {
        config->operation = OP_LIST_OPERATIONS;
        config->list_length = atoi(argv[++i]);
      }
      else
      {
        print_error("Missing list length");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--binary-depth") == 0 || strcmp(argv[i], "-b") == 0)
    {
      config->operation = OP_BINARY_TREE_DEPTH;
    }
    else if (strcmp(argv[i], "--mbranch-depth") == 0 || strcmp(argv[i], "-m") == 0)
    {
      if (i + 1 < argc)
      {
        config->operation = OP_MBRANCH_TREE_DEPTH;
        config->m_branches = atoi(argv[++i]);
      }
      else
      {
        print_error("Missing branch count");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--generate-tree") == 0 || strcmp(argv[i], "-g") == 0)
    {
      config->operation = OP_GENERATE_TREE;
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
    else if (strcmp(argv[i], "--threads") == 0 || strcmp(argv[i], "-t") == 0)
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
    else if (strcmp(argv[i], "--depth") == 0 || strcmp(argv[i], "-d") == 0)
    {
      if (i + 1 < argc)
      {
        config->tree_depth = atoi(argv[++i]);
        if (config->tree_depth < 1 || config->tree_depth > MAX_TREE_DEPTH)
        {
          print_error("Tree depth must be between 1 and MAX_TREE_DEPTH");
          exit(1);
        }
      }
      else
      {
        print_error("Missing tree depth");
        exit(1);
      }
    }
    else if (strcmp(argv[i], "--no-parallel") == 0 || strcmp(argv[i], "-np") == 0)
    {
      config->use_parallel = false;
    }
    else if (strcmp(argv[i], "--show-original") == 0 || strcmp(argv[i], "-s") == 0)
    {
      config->show_original = true;
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
  printf("Tree and Linked List Operations\n");
  printf("Usage: program [OPTIONS]\n\n");
  printf("Options:\n");
  printf("  -h, --help               Show this help message\n");
  printf("  -r, --reverse-list N     Create and reverse a list of length N (1-26)\n");
  printf("  -l, --list-operations N  Perform operations on a list of length N (1-26)\n");
  printf("  -b, --binary-depth       Calculate depth of a binary tree\n");
  printf("  -m, --mbranch-depth M    Calculate depth of M-branch tree\n");
  printf("  -g, --generate-tree      Generate and visualize a random tree\n");
  printf("  -d, --depth D            Specify tree depth (1-%d)\n", MAX_TREE_DEPTH);
  printf("  -o, --output FILE        Output file (default: out.txt)\n");
  printf("  -v, --verbose            Show verbose output including timing\n");
  printf("  -t, --threads N          Number of threads (default: 4)\n");
  printf("  -np, --no-parallel       Disable parallel processing\n");
  printf("  -s, --show-original      For list operations, preserve original list\n\n");
  printf("Examples:\n");
  printf("  ./program -r 5 -o list.txt      # Reverse a 5-element list\n");
  printf("  ./program -b -v -d 6            # Show binary tree depth with timing\n");
  printf("  ./program -m 3 -t 8             # Calculate 3-branch tree depth with 8 threads\n");
  printf("  ./program -g -d 5 -m 3          # Generate a 3-branch tree with depth 5\n");
}

// Measure execution time
double measure_time(double start, double end)
{
  return end - start;
}

// Print error message
void print_error(const char *msg)
{
  fprintf(stderr, "Error: %s\n", msg);
}