#include "cli.hpp"
#include "template.hpp"
#include <getopt.h>
#include <iostream>
#include <vector>
#include <cstring>

namespace ahun {

#define COLOR_RESET "\x1b[0m"
#define COLOR_BOLD "\x1b[1m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RED "\x1b[31m"

void CliParser::print_banner() {
    std::cout << COLOR_CYAN << COLOR_BOLD
              << "========================================\n"
              << "     AHUN CARD GENERATOR v1.0\n"
              << "========================================\n"
              << COLOR_RESET;
}

void CliParser::show_help(const char* prog_name) {
    print_banner();
    std::cout << COLOR_BOLD << "Usage:" << COLOR_RESET << " " << prog_name << " [OPTIONS]\n\n"
              << COLOR_BOLD << "Options:\n" << COLOR_RESET
              << "  -l, --list           List available templates\n"
              << "  -t, --template <xml> Specify the template XML file\n"
              << "  -o, --output <png>   Specify the output image path\n"
              << "  -x, --text <string>  Specify the text to write on the card\n"
              << "  -g, --generate-calendar <file> Generate a JSON calendar from a text file\n"
              << "  -i, --interactive    Start in interactive mode\n"
              << "  -h, --help           Show this help message\n\n";
}

AppConfig CliParser::parse_arguments(int argc, char* argv[]) {
    AppConfig opts;
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

    while ((opt = getopt_long(argc, argv, "lt:o:x:ihg:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'l': opts.list_templates = true; break;
            case 't': opts.template_path = optarg; break;
            case 'g': opts.generate_calendar = optarg; break;
            case 'o': opts.output_path = optarg; break;
            case 'x': opts.text = optarg; break;
            case 'i': opts.interactive = true; break;
            case 'h': opts.show_help = true; break;
            default: opts.error = 1; return opts;
        }
    }

    if (argc == 1) {
        opts.interactive = true;
    }

    return opts;
}

void CliParser::interactive_menu(AppConfig& opts) {
    print_banner();
    std::cout << COLOR_YELLOW << "Starting interactive mode...\n" << COLOR_RESET;

    std::cout << "\n" << COLOR_BOLD << "Choose an action:\n" << COLOR_RESET
              << "  1. Generate a Card\n"
              << "  2. Generate a Calendar JSON from text file\n"
              << "\n" << COLOR_BOLD << "Option: " << COLOR_RESET;

    int choice = 1;
    std::string buffer;
    if (std::getline(std::cin, buffer)) {
        try {
            choice = std::stoi(buffer);
        } catch (...) {
            choice = 1;
        }
    }

    if (choice == 2) {
        std::cout << COLOR_BOLD << "Enter calendar text file path: " << COLOR_RESET;
        if (std::getline(std::cin, buffer)) {
            opts.generate_calendar = buffer;
        }
        std::cout << COLOR_BOLD << "Enter output JSON path (default: calendar.json): " << COLOR_RESET;
        if (std::getline(std::cin, buffer)) {
            if (!buffer.empty()) {
                opts.output_path = buffer;
            } else {
                opts.output_path = "calendar.json";
            }
        }
        std::cout << "\n" << COLOR_GREEN << "Configuration complete!" << COLOR_RESET << "\n";
        return;
    }

    std::cout << "\n" << COLOR_BOLD << "1. Template List:\n" << COLOR_RESET;
    CardTemplate::list_available("./templates");

    std::cout << "\n" << COLOR_BOLD << "Enter template name (from above): " << COLOR_RESET;
    if (std::getline(std::cin, buffer)) {
        opts.template_path = "./templates/" + buffer;
    }

    std::cout << COLOR_BOLD << "Enter text for the card: " << COLOR_RESET;
    if (std::getline(std::cin, buffer)) {
        opts.text = buffer;
    }

    std::cout << COLOR_BOLD << "Enter output path (default: output.png): " << COLOR_RESET;
    if (std::getline(std::cin, buffer)) {
        if (!buffer.empty()) {
            opts.output_path = buffer;
        }
    }

    std::cout << "\n" << COLOR_GREEN << "Configuration complete!" << COLOR_RESET << "\n";
}

} // namespace ahun
