#include "cli.h"
#include "template.h"
#include "card.h"
#include "parse_calendar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    CliOptions opts = parse_arguments(argc, argv);

    if (opts.error) {
        show_help(argv[0]);
        return 1;
    }

    if (opts.show_help) {
        show_help(argv[0]);
        return 0;
    }

    if (opts.list_templates) {
        list_templates_xml("./templates");
        return 0;
    }

    if (opts.interactive) {
        interactive_menu(&opts);
    }

    if (opts.generate_calendar) {
        const char *input_path = opts.generate_calendar;
        const char *output_path = opts.output_path;
        
        // If output is default and we are generating a calendar, change it to .json
        if (strcmp(output_path, "output.png") == 0) {
            output_path = "calendar.json";
        }
        
        printf("\nGenerating calendar...\n");
        printf("  Input:  %s\n", input_path);
        printf("  Output: %s\n", output_path);

        if (convert_calendar_to_json(input_path, output_path) == 0) {
            printf("\x1b[32m" "Success! Calendar generated at: %s" "\x1b[0m" "\n", output_path);
            return 0;
        } else {
            fprintf(stderr, "Failed to generate calendar.\n");
            return 1;
        }
    }

    if (opts.template_path == NULL) {
        fprintf(stderr, "Error: Template path is required. Use -t or -i.\n");
        return 1;
    }

    if (opts.text == NULL) {
        fprintf(stderr, "Error: Text is required. Use -x or -i.\n");
        return 1;
    }

    Template *tmpl = load_template(opts.template_path);
    if (!tmpl) {
        return 1;
    }

    printf("\n" "\x1b[32m" "Configuration Loaded:" "\x1b[0m" "\n");
    printf("  Template: %s\n", opts.template_path);
    printf("  Background: %s\n", tmpl->background_path);
    printf("  Text: %s\n", opts.text);
    printf("  Output: %s\n", opts.output_path);

    CardConfig config;
    // Map background path if it's relative
    char full_bg_path[512];
    if (tmpl->background_path[0] != '/' && tmpl->background_path[0] != '.') {
        // Assume it's in the same directory as the template or in templates/
        snprintf(full_bg_path, sizeof(full_bg_path), "templates/%s", tmpl->background_path);
    } else {
        strncpy(full_bg_path, tmpl->background_path, sizeof(full_bg_path));
    }
    
    config.template_path = full_bg_path;
    config.output_path = opts.output_path;
    config.elements = tmpl->elements;
    config.element_count = tmpl->element_count;
    config.font_path = NULL; // Can be improved later

    // For simplicity, we apply the user text to ALL elements for now, 
    // or just the first one. Let's do the first one as it's most common.
    if (config.element_count > 0) {
        config.elements[0].text = opts.text;
    }

    printf("\nGenerating card...\n");
    if (generate_card(&config) != 0) {
        fprintf(stderr, "Failed to generate card.\n");
        free_template(tmpl);
        return 1;
    }

    printf("\x1b[32m" "\x1b[1m" "Success! Card generated at: %s" "\x1b[0m" "\n", opts.output_path);

    free_template(tmpl);
    return 0;
}
