#include "cli.hpp"
#include "template.hpp"
#include "logger.hpp"
#include <getopt.h>
#include <iostream>
#include <vector>
#include <cstring>

#ifdef ARDUINO
#include <Arduino.h>
#include <sstream>
#include "card.hpp"
#include "parse_calendar.hpp"
#endif

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
    Logger::warn("Starting interactive mode...");

    std::cout << "\n" << COLOR_BOLD << "Choose an action:\n" << COLOR_RESET
              << "  1. Generate a Card\n"
              << "  2. Generate a Calendar JSON from text file\n"
              << "\n" << COLOR_BOLD << "Option: " << COLOR_RESET;

    int choice = 1;
    std::string buffer;
    if (std::getline(std::cin, buffer)) {
        choice = std::atoi(buffer.c_str());
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
        Logger::success("Configuration complete!");
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

    Logger::success("Configuration complete!");
}

#ifdef ARDUINO
void CliParser::process_serial_command() {
    if (!Serial.available()) return;

    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) return;

    Logger::info("Comando recebido: " + std::string(input.c_str()));

    std::stringstream ss(input.c_str());
    std::string cmd;
    ss >> cmd;

    if (cmd == "help") {
        Serial.println("\n--- Ahun Card Generator Help ---");
        Serial.println("list                      - Lista os templates no SD");
        Serial.println("calendar <in> <out>       - Converte texto para JSON");
        Serial.println("gen <template> <text>     - Gera um card");
        Serial.println("   Ex: gen cura_exu.json cura=Gira;data=Hoje");
        Serial.println("--------------------------------\n");
    } 
    else if (cmd == "list") {
        CardTemplate::list_available("/templates");
    } 
    else if (cmd == "calendar") {
        std::string input_path, output_path;
        ss >> input_path >> output_path;
        if (input_path.empty() || output_path.empty()) {
            Logger::error("Uso: calendar <arquivo_txt> <arquivo_json>");
        } else {
            CalendarParser::convert_to_json(input_path, output_path);
        }
    } 
    else if (cmd == "gen") {
        std::string template_name, text;
        ss >> template_name;
        std::getline(ss, text); // Pega o resto da linha como texto
        
        if (template_name.empty() || text.empty()) {
            Logger::error("Uso: gen <template.json> <texto>");
            return;
        }

        std::string template_path = "/templates/" + template_name;
        CardTemplate tmpl;
        if (CardTemplate::load(template_path, tmpl) == Status::OK) {
            CardConfig config;
            std::strncpy(config.template_path.data(), tmpl.background_path.data(), config.template_path.size() - 1);
            std::string out_path = "/out/" + template_name + ".bmp";
            std::strncpy(config.output_path.data(), out_path.c_str(), config.output_path.size() - 1);
            config.elements = tmpl.elements;

            if (!config.elements.empty()) {
                if (text.find('=') != std::string::npos) {
                    std::stringstream ss_text(text);
                    std::string pair;
                    while (std::getline(ss_text, pair, ';')) {
                        size_t sep = pair.find('=');
                        if (sep != std::string::npos) {
                            std::string key = pair.substr(0, sep);
                            // Trim spaces
                            key.erase(0, key.find_first_not_of(" "));
                            std::string val = pair.substr(sep + 1);
                            for (auto& el : config.elements) {
                                if (std::string(el.id.data()) == key) {
                                    std::strncpy(el.text.data(), val.c_str(), el.text.size() - 1);
                                }
                            }
                        }
                    }
                } else {
                    std::strncpy(config.elements[0].text.data(), text.c_str(), config.elements[0].text.size() - 1);
                }
            }
            CardGenerator::generate(config);
        }
    } 
    else {
        Logger::error("Comando desconhecido: " + cmd + ". Digite 'help'.");
    }
}
#else
void CliParser::process_serial_command() {}
#endif

} // namespace ahun
