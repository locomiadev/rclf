#include "errors.h"

int error_invalid_args(const char *arg) {
    fprintf(stderr, "[rclf] return %d: unknown arg %s\n", RCLF_ERR_INVALID_ARGS, arg);
    return RCLF_ERR_INVALID_ARGS;
}
int error_file_not_found(const char *filepath) {
    fprintf(stderr, "[rclf] return %d: can't open file %s\n", RCLF_ERR_FILE_NOT_FOUND, filepath);
    return RCLF_ERR_FILE_NOT_FOUND;
}
int error_parsing_failed(void) {
    fprintf(stderr, "\x1b[1;31m[rclf] return %d: parsing failed oh no\x1b[0m\n", RCLF_ERR_PARSING_FAILED);
    return RCLF_ERR_PARSING_FAILED;
}
int error_empty_file(const char *filepath) {
    fprintf(stderr, "[rclf] return %d: file %s is empty\n", RCLF_ERR_EMPTY_FILE, filepath);
    return RCLF_ERR_EMPTY_FILE;
}
int error_no_rcl_tag(const char *filepath) {
    fprintf(stderr, "[rclf] return %d: missing &rcl tag in %s\n", RCLF_ERR_NO_RCL_TAG, filepath);
    return RCLF_ERR_NO_RCL_TAG;
}
int error_no_end_tag(const char *filepath) {
    fprintf(stderr, "[rclf] return %d: missing &e tag in %s\n", RCLF_ERR_NO_END_TAG, filepath);
    return RCLF_ERR_NO_END_TAG;
}
int error_invalid_column(const char *filepath, const char *line) {
    fprintf(stderr, "[rclf] return %d: invalid column syntax in %s: %s\n", RCLF_ERR_INVALID_COLUMN, filepath, line);
    return RCLF_ERR_INVALID_COLUMN;
}

int error_invalid_key_count(const char *filepath, int expected, int actual) {
    fprintf(stderr, "[rclf] return %d: invalid key count in %s: expected %d, got %d\n", 
            RCLF_ERR_INVALID_KEY_COUNT, filepath, expected, actual);
    return RCLF_ERR_INVALID_KEY_COUNT;
}
int error_invalid_value_count(const char *filepath, int key_index, int expected, int actual) {
    fprintf(stderr, "[rclf] return %d: invalid value count for key %d in %s: expected %d, got %d\n", 
            RCLF_ERR_INVALID_VALUE_COUNT, key_index, filepath, expected, actual);
    return RCLF_ERR_INVALID_VALUE_COUNT;
}
int error_memory_alloc(const char *filepath) {
    fprintf(stderr, "[rclf] return %d: memory allocation failed in %s\n", RCLF_ERR_MEMORY_ALLOC, filepath);
    return RCLF_ERR_MEMORY_ALLOC;
}
int error_invalid_syntax(const char *filepath, const char *line) {
    fprintf(stderr, "[rclf] return %d: invalid syntax in %s: %s\n", RCLF_ERR_INVALID_SYNTAX, filepath, line);
    return RCLF_ERR_INVALID_SYNTAX;
}
