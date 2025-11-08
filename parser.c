#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sntchk.h"
#include "errors.h"

static char *trim_ws(char *s) {
    if (!s) return s;
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (*start == '\0') return start;
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return start;
}

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
    bool has_rcl = false;
    bool has_end = false;
    bool in_column = false;
    int key_count = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *p = trim_ws(line);
        if (*p == '\0') continue;
        if (p[0] == '/' && p[1] == '/') continue;
        
        if (strncmp(p, "&rcl", 4) == 0) {
            if (has_rcl) { fclose(fp); return error_invalid_syntax(filepath, "multiple &rcl tags"); }
            has_rcl = true;
            continue;
        }
        if (strncmp(p, "&c[", 3) == 0) {
            if (!has_rcl) { fclose(fp); return error_no_rcl_tag(filepath); }
            if (in_column) { fclose(fp); return error_invalid_column(filepath, p); }
            char *br = strchr(p, '[');
            char *br2 = strchr(p, ']');
            if (!br || !br2 || br2 <= br) { fclose(fp); return error_invalid_column(filepath, p); }
            in_column = true;
            key_count = 0;
            continue;
        }
        if (strncmp(p, "&e", 2) == 0) {
            if (!in_column) { fclose(fp); return error_invalid_syntax(filepath, "&e without &c[]"); }
            in_column = false;
            has_end = true;
            continue;
        }
        if (!in_column) { fclose(fp); return error_invalid_syntax(filepath, p); }
    }
    fclose(fp);
    if (!has_rcl) return error_no_rcl_tag(filepath);
    if (!has_end) return error_no_end_tag(filepath);
    return 0;
}