#include "parse_calendar.hpp"
#include "logger.hpp"
#include "wrappers/JsonWrapper.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>

#ifdef ARDUINO
#include <FS.h>
#include <LittleFS.h>
#endif

namespace ahun {

static std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
        start++;
    }
    auto end = s.end();
    if (start == end) return "";
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

Status CalendarParser::convert_to_json(const std::string& input_path, const std::string& output_path) {
    std::string content;
#ifdef ARDUINO
    std::string local_input = input_path;
    if (!local_input.empty() && local_input[0] != '/') {
        local_input = "/" + local_input;
    }
    File in_file = LittleFS.open(local_input.c_str());
    if (!in_file) {
        Logger::error("Error opening input file on LittleFS: " + local_input);
        return Status::ERROR_FILE_NOT_FOUND;
    }
    while (in_file.available()) {
        content += (char)in_file.read();
    }
    in_file.close();
#else
    std::ifstream in(input_path);
    if (!in.is_open()) {
        Logger::error("Error opening input file: " + input_path);
        return Status::ERROR_FILE_NOT_FOUND;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    content = buffer.str();
#endif

    auto root = JsonDocument::createObject();
    cJSON* calendar_array = cJSON_AddArrayToObject(root.get(), "calendar");
    
    std::stringstream ss(content);
    std::string line;
    cJSON* current_month_obj = nullptr;
    cJSON* current_giras_array = nullptr;

    while (std::getline(ss, line)) {
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

    char* json_string = cJSON_Print(root.get());
    if (json_string) {
#ifdef ARDUINO
        std::string local_output = output_path;
        if (!local_output.empty() && local_output[0] != '/') {
            local_output = "/" + local_output;
        }
        File out_file = LittleFS.open(local_output.c_str(), FILE_WRITE);
        if (out_file) {
            out_file.print(json_string);
            out_file.close();
        } else {
            free(json_string);
            Logger::error("Error opening output file on LittleFS: " + local_output);
            return Status::ERROR_GENERAL;
        }
#else
        std::ofstream out(output_path);
        if (out.is_open()) {
            out << json_string;
        } else {
            free(json_string);
            Logger::error("Error opening output file: " + output_path);
            return Status::ERROR_GENERAL;
        }
#endif
        free(json_string);
    }
    
    return Status::OK;
}

} // namespace ahun
