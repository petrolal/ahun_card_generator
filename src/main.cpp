#include "cli.hpp"
#include "template.hpp"
#include "card.hpp"
#include "parse_calendar.hpp"
#include "exceptions.hpp"
#include "logger.hpp"
#include "wrappers/MagickWandWrapper.hpp"
#include "wrappers/XmlWrapper.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include <memory>

using namespace ahun;
namespace fs = std::filesystem;

static void ensure_directory_exists(const std::string& path) {
    if (path.empty()) return;
    fs::path p(path);
    if (p.has_parent_path()) {
        if (fs::create_directories(p.parent_path())) {
            Logger::info("Created directory: " + p.parent_path().string());
        }
    }
}

int main(int argc, char *argv[]) {
    try {
        // Global Initializations (RAII)
        MagickEngine magick_engine;
        XmlEngine xml_engine;

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
            ensure_directory_exists(opts.output_path);
            Logger::info("Generating calendar...");
            Logger::info("  Input:  " + opts.generate_calendar);
            Logger::info("  Output: " + opts.output_path);

            CalendarParser::convert_to_json(opts.generate_calendar, opts.output_path);
            Logger::success("Calendar generated at: " + opts.output_path);
            return 0;
        }

        if (opts.template_path.empty()) {
            throw ConfigException("Template path is required. Use -t or -i.");
        }

        if (opts.text.empty()) {
            throw ConfigException("Text is required. Use -x or -i.");
        }

        std::unique_ptr<CardTemplate> tmpl(CardTemplate::load(opts.template_path));
        
        Logger::info("Configuration Loaded:");
        Logger::info("  Template: " + opts.template_path);
        Logger::info("  Background: " + tmpl->background_path);
        Logger::info("  Text: " + opts.text);
        Logger::info("  Output: " + opts.output_path);

        CardConfig config;
        if (!tmpl->background_path.empty() && tmpl->background_path[0] != '/' && tmpl->background_path[0] != '.') {
            config.template_path = "templates/" + tmpl->background_path;
        } else {
            config.template_path = tmpl->background_path;
        }
        
        config.output_path = opts.output_path;
        config.elements = tmpl->elements;

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
                            if (el.id == key) {
                                el.text = val;
                            }
                        }
                    }
                }
            } else {
                config.elements[0].text = opts.text;
            }
        }

        ensure_directory_exists(opts.output_path);
        Logger::info("Generating card...");
        CardGenerator::generate(config);

        Logger::success("Card generated at: " + opts.output_path);
        return 0;

    } catch (const AhunException& e) {
        Logger::error(e.what());
        return 1;
    } catch (const std::exception& e) {
        Logger::error(std::string("Unhandled exception: ") + e.what());
        return 1;
    }
}
