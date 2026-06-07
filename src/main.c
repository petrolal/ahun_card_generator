#include "cli.h"
#include "template.h"
#include "card.h"
#include "parse_calendar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

int main(int argc, char *argv[]) {
    CliOptions opts = parse_arguments(argc, argv);
    Template *tmpl = NULL;
    int exit_code = 0;

    // Global Initializations
    init_card_engine();
    xmlInitParser();

    if (opts.error) {
        show_help(argv[0]);
        exit_code = 1;
        goto cleanup;
    }

    if (opts.show_help) {
        show_help(argv[0]);
        goto cleanup;
    }

    if (opts.list_templates) {
        list_templates_xml("./templates");
        goto cleanup;
    }

    if (opts.interactive) {
        interactive_menu(&opts);
    }

    if (opts.generate_calendar) {
        const char *input_path = opts.generate_calendar;
        const char *output_path = opts.output_path;
        
        printf("\nGenerating calendar...\n");
        printf("  Input:  %s\n", input_path);
        printf("  Output: %s\n", output_path);

        if (convert_calendar_to_json(input_path, output_path) == 0) {
            printf("\x1b[32m" "Success! Calendar generated at: %s" "\x1b[0m" "\n", output_path);
            goto cleanup;
        } else {
            fprintf(stderr, "Failed to generate calendar.\n");
            exit_code = 1;
            goto cleanup;
        }
    }

    if (opts.template_path == NULL) {
        fprintf(stderr, "Error: Template path is required. Use -t or -i.\n");
        exit_code = 1;
        goto cleanup;
    }

    if (opts.text == NULL) {
        fprintf(stderr, "Error: Text is required. Use -x or -i.\n");
        exit_code = 1;
        goto cleanup;
    }

    tmpl = load_template(opts.template_path);
    if (!tmpl) {
        exit_code = 1;
        goto cleanup;
    }

    printf("\n" "\x1b[32m" "Configuration Loaded:" "\x1b[0m" "\n");
    printf("  Template: %s\n", opts.template_path);
    printf("  Background: %s\n", tmpl->background_path);
    printf("  Text: %s\n", opts.text);
    printf("  Output: %s\n", opts.output_path);

    CardConfig config;
    char full_bg_path[1024];
    memset(full_bg_path, 0, sizeof(full_bg_path));

    if (tmpl->background_path[0] != '/' && tmpl->background_path[0] != '.') {
        snprintf(full_bg_path, sizeof(full_bg_path), "templates/%s", tmpl->background_path);
    } else {
        strncpy(full_bg_path, tmpl->background_path, sizeof(full_bg_path) - 1);
    }
    
    config.template_path = full_bg_path;
    config.output_path = opts.output_path;
    config.elements = tmpl->elements;
    config.element_count = tmpl->element_count;
    config.font_path = NULL;

    if (config.element_count > 0) {
        config.elements[0].text = opts.text;
    }

    printf("\nGenerating card...\n");
    if (generate_card(&config) != 0) {
        fprintf(stderr, "Failed to generate card.\n");
        exit_code = 1;
        goto cleanup;
    }

    printf("\x1b[32m" "\x1b[1m" "Success! Card generated at: %s" "\x1b[0m" "\n", opts.output_path);

cleanup:
    if (tmpl) free_template(tmpl);
    free_cli_options(&opts);
    
    // Global Cleanups
    xmlCleanupParser();
    cleanup_card_engine();

    return exit_code;
}
