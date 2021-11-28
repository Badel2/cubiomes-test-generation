#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Parsed arguments. You need to manually call `free_args` in order to free the memory.
 */
typedef struct Args {
  char *mc_version;
  char *input_zip;
  bool save_img;
} Args;

/**
 * Parse args into `parsed_args`. Returns 0 on success, non-0 on error, and exits the program if
 * the arguments are not valid showing a small help in the process.
 *
 * # Safety
 *
 * `argc` and `argv` should follow the usual conventions.
 * `parsed_args` must be a valid pointer.
 */
int32_t parse_args(uintptr_t argc, const char *const *argv, struct Args *parsed_args);

/**
 * Free memory allocated by `parse_args()`.
 */
void free_args(struct Args parsed_args);
