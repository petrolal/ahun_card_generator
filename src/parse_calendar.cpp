#include "parse_calendar.hpp"
#include "exceptions.hpp"
#include "wrappers/JsonWrapper.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace ahun {

static std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

void CalendarParser::convert_to_json(const std::string& input_path, const std::string& output_path) {
    std::ifstream in(input_path);
    if (!in.is_open()) {
        throw CalendarException("Error opening input file: " + input_path);
    }

    auto root = JsonDocument::createObject();
    cJSON* calendar_array = cJSON_AddArrayToObject(root.get(), "calendar");
    
    std::string line;
    cJSON* current_month_obj = nullptr;
    cJSON* current_giras_array = nullptr;

    while (std::getline(in, line)) {
        std::string trimmed = trim(line);

        if (trimmed.empty() || trimmed[0] == '#')
            continue;

        size_t colon_pos = trimmed.find(':');

        // New month detection
        if (trimmed.find("🔸") != std::string::npos || (colon_pos == std::string::npos && (!current_month_obj || current_giras_array))) {
            current_month_obj = cJSON_CreateObject();
            cJSON_AddItemToArray(calendar_array, current_month_obj);
            cJSON_AddStringToObject(current_month_obj, "month", trimmed.c_str());
            current_giras_array = nullptr; // Reset for new month
        } else if (colon_pos == std::string::npos) {
            // Theme line
            if (current_month_obj) {
                cJSON_AddStringToObject(current_month_obj, "theme", trimmed.c_str());
            }
        } else {
            // Gira entry
            if (!current_month_obj) {
                current_month_obj = cJSON_CreateObject();
                cJSON_AddItemToArray(calendar_array, current_month_obj);
                cJSON_AddStringToObject(current_month_obj, "month", "Unknown");
            }

            if (!current_giras_array) {
                current_giras_array = cJSON_AddArrayToObject(current_month_obj, "giras");
            }

            std::string type = trim(trimmed.substr(0, colon_pos));
            std::string date = trim(trimmed.substr(colon_pos + 1));

            cJSON* gira_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(gira_obj, "type", type.c_str());
            cJSON_AddStringToObject(gira_obj, "date", date.c_str());
            cJSON_AddItemToArray(current_giras_array, gira_obj);
        }
    }

    // Ensure all months have at least an empty giras array
    cJSON* month_item;
    cJSON_ArrayForEach(month_item, calendar_array) {
        if (!cJSON_GetObjectItem(month_item, "giras")) {
            cJSON_AddArrayToObject(month_item, "giras");
        }
    }

    char* json_string = cJSON_Print(root.get());
    if (json_string) {
        std::ofstream out(output_path);
        if (out.is_open()) {
            out << json_string;
        } else {
            free(json_string);
            throw CalendarException("Error opening output file: " + output_path);
        }
        free(json_string);
    }
}

} // namespace ahun
