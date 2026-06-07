#include "cli.hpp"
#include "template.hpp"
#include "card.hpp"
#include "parse_calendar.hpp"
#include "logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <cstring>

#ifdef ARDUINO
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#endif

using namespace ahun;

#ifdef ARDUINO
void setup() {
    Serial.begin(115200);
    delay(1000);
    Logger::info("Ahun Card Generator - Embedded Start");

    if (!LittleFS.begin(true)) {
        Logger::error("LittleFS Mount Failed!");
        return;
    }
    Logger::success("LittleFS Mounted.");

    // Simple test logic for embedded
    CardTemplate tmpl;
    if (CardTemplate::load("/templates/template1.json", tmpl) == Status::OK) {
        Logger::success("Template loaded successfully on ESP32.");
    }
}

void loop() {
    CliParser::process_serial_command();
    delay(10); // Pequeno delay para estabilidade
}

#else
// Native Main for Double Targeting
int main(int argc, char *argv[]) {
    AppConfig opts = CliParser::parse_arguments(argc, argv);

    if (opts.error) {
        CliParser::show_help(argv[0]);
        return 1;
    }

    if (opts.show_help) {
        CliParser::show_help(argv[0]);
        return 0;
    }

    if (opts.list_templates) {
        CardTemplate::list_available("./templates");
        return 0;
    }

    if (opts.interactive) {
        CliParser::interactive_menu(opts);
    }

    if (!opts.generate_calendar.empty()) {
        Logger::info("Generating calendar (Native)...");
        if (CalendarParser::convert_to_json(opts.generate_calendar, opts.output_path) == Status::OK) {
            Logger::success("Calendar generated.");
            return 0;
        }
        return 1;
    }

    if (opts.template_path.empty() || opts.text.empty()) {
        Logger::error("Template path and text are required.");
        return 1;
    }

    CardTemplate tmpl;
    if (CardTemplate::load(opts.template_path, tmpl) != Status::OK) {
        return 1;
    }

    CardConfig config;
    std::strncpy(config.template_path.data(), tmpl.background_path.data(), config.template_path.size() - 1);
    std::strncpy(config.output_path.data(), opts.output_path.c_str(), config.output_path.size() - 1);
    config.elements = tmpl.elements;

    // Mapping text logic
    if (!config.elements.empty()) {
        if (opts.text.find('=') != std::string::npos) {
            std::stringstream ss(opts.text);
            std::string pair;
            while (std::getline(ss, pair, ';')) {
                size_t sep = pair.find('=');
                if (sep != std::string::npos) {
                    std::string key = pair.substr(0, sep);
                    std::string val = pair.substr(sep + 1);
                    for (auto& el : config.elements) {
                        if (std::string(el.id.data()) == key) {
                            std::strncpy(el.text.data(), val.c_str(), el.text.size() - 1);
                        }
                    }
                }
            }
        } else {
            std::strncpy(config.elements[0].text.data(), opts.text.c_str(), config.elements[0].text.size() - 1);
        }
    }

    Logger::info("Generating card (Native)...");
    if (CardGenerator::generate(config) == Status::OK) {
        Logger::success("Card generated successfully.");
        return 0;
    }

    return 1;
}
#endif
