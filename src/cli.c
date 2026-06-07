#include "cli.h"
#include "template.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_RESET "\x1b[0m"
#define COLOR_BOLD "\x1b[1m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RED "\x1b[31m"

void print_banner(void) {
  printf(COLOR_CYAN COLOR_BOLD);
  printf("========================================\n");
  printf("     AHUN CARD GENERATOR v1.0\n");
  printf("========================================\n");
  printf(COLOR_RESET);
}

void show_help(const char *prog_name) {
  print_banner();
  printf(COLOR_BOLD "Usage:" COLOR_RESET " %s [OPTIONS]\n\n", prog_name);
  printf(COLOR_BOLD "Options:\n" COLOR_RESET);
  printf("  %-20s %s\n", "-l, --list", "List available templates");
  printf("  %-20s %s\n", "-t, --template <xml>",
         "Specify the template XML file");
  printf("  %-20s %s\n", "-o, --output <png>", "Specify the output image path");
  printf("  %-20s %s\n", "-x, --text <string>",
         "Specify the text to write on the card");
  printf("  %-20s %s\n", "-g, --generate-calendar <file>",
         "Generate a JSON calendar from a text file");
  printf("  %-20s %s\n", "-i, --interactive", "Start in interactive mode");
  printf("  %-20s %s\n", "-h, --help", "Show this help message");
  printf("\n");
}

void free_cli_options(CliOptions *opts) {
  if (opts->template_path) free(opts->template_path);
  if (opts->output_path) free(opts->output_path);
  if (opts->text) free(opts->text);
  if (opts->generate_calendar) free(opts->generate_calendar);
}

CliOptions parse_arguments(int argc, char *argv[]) {
  CliOptions opts = {NULL, strdup("output.png"), NULL, false, NULL, false, false, 0};
  int opt;

  static struct option long_options[] = {
      {"list", no_argument, 0, 'l'},
      {"template", required_argument, 0, 't'},
      {"generate-calendar", required_argument, 0, 'g'},
      {"output", required_argument, 0, 'o'},
      {"text", required_argument, 0, 'x'},
      {"interactive", no_argument, 0, 'i'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "lt:o:x:ihg:", long_options, NULL)) !=
         -1) {
    switch (opt) {
    case 'l':
      opts.list_templates = true;
      break;
    case 't':
      if (opts.template_path) free(opts.template_path);
      opts.template_path = strdup(optarg);
      break;
    case 'g':
      if (opts.generate_calendar) free(opts.generate_calendar);
      opts.generate_calendar = strdup(optarg);
      break;
    case 'o':
      if (opts.output_path) free(opts.output_path);
      opts.output_path = strdup(optarg);
      break;
    case 'x':
      if (opts.text) free(opts.text);
      opts.text = strdup(optarg);
      break;
    case 'i':
      opts.interactive = true;
      break;
    case 'h':
      opts.show_help = true;
      break;
    default:
      opts.error = 1;
      return opts;
    }
  }

  if (argc == 1) {
    opts.interactive = true;
  }

  return opts;
}

void interactive_menu(CliOptions *opts) {
  print_banner();
  printf(COLOR_YELLOW "Starting interactive mode...\n" COLOR_RESET);

  printf("\n" COLOR_BOLD "Choose an action:\n" COLOR_RESET);
  printf("  1. Generate a Card\n");
  printf("  2. Generate a Calendar JSON from text file\n");
  printf("\n" COLOR_BOLD "Option: " COLOR_RESET);

  int choice = 1;
  char buffer[512];
  if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    choice = atoi(buffer);
  }

  if (choice == 2) {
    printf(COLOR_BOLD "Enter calendar text file path: " COLOR_RESET);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
      buffer[strcspn(buffer, "\n")] = 0;
      if (opts->generate_calendar) free(opts->generate_calendar);
      opts->generate_calendar = strdup(buffer);
    }
    printf(COLOR_BOLD "Enter output JSON path (default: calendar.json): " COLOR_RESET);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
      buffer[strcspn(buffer, "\n")] = 0;
      if (strlen(buffer) > 0) {
        if (opts->output_path) free(opts->output_path);
        opts->output_path = strdup(buffer);
      } else {
        if (opts->output_path) free(opts->output_path);
        opts->output_path = strdup("calendar.json");
      }
    }
    printf("\n" COLOR_GREEN "Configuration complete!" COLOR_RESET "\n");
    return;
  }

  printf("\n" COLOR_BOLD "1. Template List:\n" COLOR_RESET);
  list_templates_xml("./templates");

  printf("\n" COLOR_BOLD "Enter template name (from above): " COLOR_RESET);
  if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    buffer[strcspn(buffer, "\n")] = 0;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "./templates/%s", buffer);
    if (opts->template_path) free(opts->template_path);
    opts->template_path = strdup(full_path);
  }

  printf(COLOR_BOLD "Enter text for the card: " COLOR_RESET);
  if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    buffer[strcspn(buffer, "\n")] = 0;
    if (opts->text) free(opts->text);
    opts->text = strdup(buffer);
  }

  printf(COLOR_BOLD "Enter output path (default: output.png): " COLOR_RESET);
  if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) > 0) {
      if (opts->output_path) free(opts->output_path);
      opts->output_path = strdup(buffer);
    }
  }

  printf("\n" COLOR_GREEN "Configuration complete!" COLOR_RESET "\n");
}
