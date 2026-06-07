#include "parse_calendar.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
  int first_month = 1;
  int first_gira = 1;

  fprintf(out, "{\n  \"calendar\": [\n");

  while (fgets(line, sizeof(line), in)) {
    char *trimmed = trim_whitespace(line);

    if (*trimmed == '\0' || *trimmed == '#') // Skip empty lines and comments
      continue;

    char *colon = strchr(trimmed, ':');
    if (!colon) {
      // It's a month name
      if (!first_month) {
        fprintf(out, "\n      ]\n    },\n");
      }
      fprintf(out, "    {\n      \"mes\": \"%s\",\n      \"giras\": [", trimmed);
      first_month = 0;
      first_gira = 1;
    } else {
      // It's a gira (type: date)
      *colon = '\0';
      char *type = trim_whitespace(trimmed);
      char *date = trim_whitespace(colon + 1);

      // Filter: ignore 'fechada' and 'desenvolvimento'
      char *type_lower = strdup(type);
      for (int i = 0; type_lower[i]; i++) {
        type_lower[i] = tolower((unsigned char)type_lower[i]);
      }

      if (strstr(type_lower, "fechada") || strstr(type_lower, "desenvolvimento")) {
        free(type_lower);
        continue;
      }
      free(type_lower);

      if (!first_gira) {
        fprintf(out, ",");
      }
      fprintf(out, "\n        { \"tipo\": \"%s\", \"data\": \"%s\" }", type, date);
      first_gira = 0;
    }
  }

  if (!first_month) {
    fprintf(out, "\n      ]\n    }\n");
  }

  fprintf(out, "  ]\n}\n");
  fclose(in);
  fclose(out);
  return 0;
}
