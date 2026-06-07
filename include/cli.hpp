#ifndef CLI_H
#define CLI_H

#include <string>

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
};

} // namespace ahun

#endif // CLI_H
