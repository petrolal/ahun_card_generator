#ifndef CLI_HPP
#define CLI_HPP

#include <string>
#include <array>

namespace ahun {

struct AppConfig {
    std::string template_path;
    std::string output_path = "output.png";
    std::string text;
    std::string generate_calendar;
    bool list_templates = false;
    bool interactive = false;
    bool show_help = false;
    int error = 0;
};

class CliParser {
public:
    static AppConfig parse_arguments(int argc, char* argv[]);
    static void print_banner();
    static void show_help(const char* prog_name);
    static void interactive_menu(AppConfig& opts);

    // Métodos para o Console Serial no ESP32
    static void process_serial_command();
};

} // namespace ahun

#endif // CLI_HPP
