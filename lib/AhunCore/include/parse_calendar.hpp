#ifndef PARSE_CALENDAR_HPP
#define PARSE_CALENDAR_HPP

#include "status.hpp"
#include <string>

namespace ahun {

class CalendarParser {
public:
    static Status convert_to_json(const std::string& input_path, const std::string& output_path);
};

} // namespace ahun

#endif // PARSE_CALENDAR_HPP
