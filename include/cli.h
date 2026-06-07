#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

typedef struct {
  char *template_path;
  char *output_path;
  char *text;
  bool list_templates;
  char *generate_calendar;
  bool interactive;
  bool show_help;
  int error;
} CliOptions;

CliOptions parse_arguments(int argc, char *argv[]);
void print_banner(void);
void show_help(const char *prog_name);
void interactive_menu(CliOptions *opts);
void free_cli_options(CliOptions *opts);

#endif // CLI_H
