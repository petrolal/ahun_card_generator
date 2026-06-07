#ifndef PARSE_CALENDAR_H
#define PARSE_CALENDAR_H

#include <string>

namespace ahun {

class CalendarParser {
public:
    static void convert_to_json(const std::string& input_path, const std::string& output_path);
};

} // namespace ahun

#endif // !PARSER_CALENDAR_H
