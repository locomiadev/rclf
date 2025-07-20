#ifndef RCLF_ERRORS_H
#define RCLF_ERRORS_H

#include <stdio.h>

// return (error) codes (very important)
// 1 2 4 returns from MAIN
// 3 returns from PARSER
// 4 5 6 7 8 9 10 11 returns from SNTCHK
enum RclfErrorCode {
    RCLF_ERR_INVALID_ARGS = 1,		 // (main) when there is invalid tag | example: rclf out -s
    RCLF_ERR_FILE_NOT_FOUND = 2,	 // (main) when file from filepath isn't file
    RCLF_ERR_PARSING_FAILED = 3,	 // (pars) parser sends to main that something is not ok
    RCLF_ERR_EMPTY_FILE = 4,		 // (sntc) when file is empty
    RCLF_ERR_NO_RCL_TAG = 5,		 // (sntc) no $rcl tag in beginning
    RCLF_ERR_NO_END_TAG = 6,		 // (sntc) no &e tag in end
    RCLF_ERR_INVALID_COLUMN = 7,	 // (sntc) invalid &c using
    RCLF_ERR_INVALID_KEY_COUNT = 8,	 // (sntc) when keys is > or < than values
    RCLF_ERR_INVALID_VALUE_COUNT = 9,	 // (sntc) same as return 8 but with values
    RCLF_ERR_MEMORY_ALLOC = 10,		 // (sntc) error with memory
    RCLF_ERR_INVALID_SYNTAX = 11	 // (sntc) invalid syntax
};
int error_invalid_args(const char *arg);
int error_file_not_found(const char *filepath);
int error_parsing_failed(void);
int error_empty_file(const char *filepath);
int error_no_rcl_tag(const char *filepath);
int error_no_end_tag(const char *filepath);
int error_invalid_column(const char *filepath, const char *line);
int error_invalid_key_count(const char *filepath, int expected, int actual);
int error_invalid_value_count(const char *filepath, int key_index, int expected, int actual);
int error_memory_alloc(const char *filepath);
int error_invalid_syntax(const char *filepath, const char *line);
#endif
