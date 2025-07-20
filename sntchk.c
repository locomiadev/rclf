#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sntchk.h"
int rclf_check_syntax(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return error_file_not_found(filepath);
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0) {
        fclose(fp);
        return error_empty_file(filepath);
    }
    fseek(fp, 0, SEEK_SET);
    char line[2048];
    int line_count = 0;
    bool has_rcl = false;
    bool has_end = false;
    bool in_column = false;
    int key_count = 0;
    int value_line_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        line_count++;
        if (strncmp(line, "//", 2) == 0 || strlen(line) < 2)
            continue;
        if (strncmp(line, "&rcl", 4) == 0) {
            if (has_rcl) return error_invalid_syntax(filepath, "multiple &rcl tags");
            has_rcl = true;
        } else if (strncmp(line, "&e", 2) == 0) {
            if (!in_column) return error_invalid_syntax(filepath, "&e without &c[]");
            has_end = true;
            in_column = false;
        } else if (strncmp(line, "&c[", 3) == 0) {
            if (!has_rcl) return error_no_rcl_tag(filepath);
            if (in_column) return error_invalid_column(filepath, line);
            in_column = true;
            char *endptr;
            strtol(line + 3, &endptr, 10);
            if (*endptr != ']') return error_invalid_column(filepath, line);
            key_count = 0;
            value_line_count = 0;
        } else if (in_column && strchr(line, ';')) {
            if (key_count > 0) return error_invalid_syntax(filepath, "multiple key lines in column");
            char *token = strtok(line, ";");
            while (token) {
                while (isspace(*token)) token++;
                if (strlen(token) == 0) return error_invalid_syntax(filepath, "empty key");
                key_count++;
                token = strtok(NULL, ";");
            }
        } else if (in_column && strchr(line, ':')) {
            value_line_count++;
            char *token = strtok(line, ":");
            int value_count = 0;
            while (token) {
                while (isspace(*token)) token++;
                if (strlen(token) == 0) return error_invalid_syntax(filepath, "empty value");
                value_count++;
                token = strtok(NULL, ":");
            }
            if (value_count != key_count) {
                return error_invalid_value_count(filepath, value_line_count - 1, key_count, value_count);
            }
        } else {
            return error_invalid_syntax(filepath, line);
        }
    }
    fclose(fp);
    if (!has_rcl) return error_no_rcl_tag(filepath);
    if (!has_end) return error_no_end_tag(filepath);
    return 0;
}
















// Здравствуйте, я великий уникальный, не такой как все Mr.Sugoma.
// Я поселился в данную программу потому-что в Говниково-1 отключили интернет.
// Пожалуйста, купите мне нутеллу.
// NOOOCOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORD!
