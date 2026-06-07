#include "parse_calendar.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

static char *trim_whitespace(char *str) {
    if (!str) return NULL;
    char *end;

    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    end[1] = '\0';

    return str;
}

int convert_calendar_to_json(const char *input_path, const char *output_path) {
    FILE *in = fopen(input_path, "r");
    if (!in) {
        perror("Error opening input file");
        return 1;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *calendar_array = cJSON_AddArrayToObject(root, "calendar");
    
    char line[512];
    cJSON *current_month_obj = NULL;
    cJSON *current_giras_array = NULL;

    while (fgets(line, sizeof(line), in)) {
        char *trimmed = trim_whitespace(line);

        if (*trimmed == '\0' || *trimmed == '#')
            continue;

        char *colon = strchr(trimmed, ':');

        // New month detection
        if (strstr(trimmed, "🔸") || (!colon && (!current_month_obj || current_giras_array))) {
            current_month_obj = cJSON_CreateObject();
            cJSON_AddItemToArray(calendar_array, current_month_obj);
            cJSON_AddStringToObject(current_month_obj, "month", trimmed);
            current_giras_array = NULL; // Reset for new month
        } else if (!colon) {
            // Theme line
            if (current_month_obj) {
                cJSON_AddStringToObject(current_month_obj, "theme", trimmed);
            }
        } else {
            // Gira entry
            if (!current_month_obj) {
                // Should not happen with valid input, but for safety:
                current_month_obj = cJSON_CreateObject();
                cJSON_AddItemToArray(calendar_array, current_month_obj);
                cJSON_AddStringToObject(current_month_obj, "month", "Unknown");
            }

            if (!current_giras_array) {
                current_giras_array = cJSON_AddArrayToObject(current_month_obj, "giras");
            }

            *colon = '\0';
            char *type = trim_whitespace(trimmed);
            char *date = trim_whitespace(colon + 1);

            cJSON *gira_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(gira_obj, "type", type);
            cJSON_AddStringToObject(gira_obj, "date", date);
            cJSON_AddItemToArray(current_giras_array, gira_obj);
        }
    }

    // Ensure all months have at least an empty giras array if not present
    cJSON *month_item;
    cJSON_ArrayForEach(month_item, calendar_array) {
        if (!cJSON_GetObjectItem(month_item, "giras")) {
            cJSON_AddArrayToObject(month_item, "giras");
        }
    }

    fclose(in);

    char *json_string = cJSON_Print(root);
    FILE *out = fopen(output_path, "w");
    if (out) {
        fputs(json_string, out);
        fclose(out);
    } else {
        perror("Error opening output file");
    }

    free(json_string);
    cJSON_Delete(root);
    
    return 0;
}
