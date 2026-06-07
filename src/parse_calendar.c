#include "parse_calendar.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *trim_whitespace(char *str) {
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

  FILE *out = fopen(output_path, "w");
  if (!out) {
    perror("Error opening output file");
    fclose(in);
    return 1;
  }

  char line[512];
  int month_started = 0;
  int giras_started = 0;

  fprintf(out, "{\n  \"calendar\": [\n");

  while (fgets(line, sizeof(line), in)) {
    char *trimmed = trim_whitespace(line);

    if (*trimmed == '\0' || *trimmed == '#') // Skip empty lines and comments
      continue;

    char *colon = strchr(trimmed, ':');

    // Check if it's a new month (starts with emoji OR is a line without colon
    // after giras have started)
    if (strstr(trimmed, "🔸") ||
        (!colon && (!month_started || giras_started))) {
      if (month_started) {
        if (giras_started) {
          fprintf(out, "\n      ]\n");
        }
        fprintf(out, "    },\n");
      }
      fprintf(out, "    {\n      \"month\": \"%s\"", trimmed);
      month_started = 1;
      giras_started = 0;
    } else if (!colon) {
      // It's a theme line (no colon and we are already inside a month but
      // haven't started giras)
      fprintf(out, ",\n      \"theme\": \"%s\"", trimmed);
    } else {
      // It's a gira (type: date)
      if (!giras_started) {
        fprintf(out, ",\n      \"giras\": [");
        giras_started = 1;
      } else {
        fprintf(out, ",");
      }

      *colon = '\0';
      char *type = trim_whitespace(trimmed);
      char *date = trim_whitespace(colon + 1);

      fprintf(out, "\n        { \"type\": \"%s\", \"date\": \"%s\" }", type,
              date);
    }
  }

  if (month_started) {
    if (giras_started) {
      fprintf(out, "\n      ]\n");
    } else {
      fprintf(out, ",\n      \"giras\": []\n");
    }
    fprintf(out, "    }\n");
  }

  fprintf(out, "  ]\n}\n");
  fclose(in);
  fclose(out);
  return 0;
}
