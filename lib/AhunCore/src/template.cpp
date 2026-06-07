#include "template.hpp"
#include "logger.hpp"
#include "wrappers/JsonWrapper.hpp"
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>

#ifdef ARDUINO
#include <FS.h>
#include <SD.h>
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

namespace ahun {

void CardTemplate::list_available(const std::string& dir_path) {
#ifdef ARDUINO
    File root = SD.open(dir_path.c_str());
    if (!root || !root.isDirectory()) {
        Logger::error("Templates directory does not exist on SD: " + dir_path);
        return;
    }
    Logger::info("Available Templates (SD):");
    File file = root.openNextFile();
    while (file) {
        std::string fileName = file.name();
        if (fileName.find(".json") != std::string::npos) {
             Serial.print("  » ");
             Serial.println(fileName.c_str());
        }
        file = root.openNextFile();
    }
#else
    if (!fs::exists(dir_path)) {
        Logger::error("Templates directory does not exist: " + dir_path);
        return;
    }

    Logger::info("Available Templates:");

    int count = 0;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::cout << "  \x1b[1m»\x1b[0m " << entry.path().filename().string() << std::endl;
            count++;
        }
    }

    if (count == 0) {
        Logger::warn("No .json templates found in '" + dir_path + "'");
    }
#endif
}

Status CardTemplate::load(const std::string& json_path, CardTemplate& tmpl) {
    std::string content;
#ifdef ARDUINO
    File file = SD.open(json_path.c_str());
    if (!file) {
        Logger::error("Could not open template file: " + json_path);
        return Status::ERROR_FILE_NOT_FOUND;
    }
    while (file.available()) {
        content += (char)file.read();
    }
    file.close();
#else
    std::ifstream in(json_path);
    if (!in.is_open()) {
        Logger::error("Could not open template file: " + json_path);
        return Status::ERROR_FILE_NOT_FOUND;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    content = buffer.str();
#endif

    auto root = JsonDocument::parse(content);
    if (!root) {
        Logger::error("Failed to parse JSON template: " + json_path);
        return Status::ERROR_JSON_PARSE;
    }

    cJSON* bg = cJSON_GetObjectItem(root.get(), "background");
    if (cJSON_IsString(bg)) {
        std::strncpy(tmpl.background_path.data(), bg->valuestring, tmpl.background_path.size() - 1);
    }

    cJSON* layers = cJSON_GetObjectItem(root.get(), "text_layers");
    if (cJSON_IsArray(layers)) {
        int size = cJSON_GetArraySize(layers);
        tmpl.elements.clear();
        tmpl.elements.reserve(size);

        for (int i = 0; i < size; i++) {
            cJSON* item = cJSON_GetArrayItem(layers, i);
            TextElement el = {};

            cJSON* id = cJSON_GetObjectItem(item, "id");
            if (cJSON_IsString(id)) std::strncpy(el.id.data(), id->valuestring, el.id.size() - 1);

            cJSON* x = cJSON_GetObjectItem(item, "x");
            if (cJSON_IsNumber(x)) el.x = x->valuedouble;

            cJSON* y = cJSON_GetObjectItem(item, "y");
            if (cJSON_IsNumber(y)) el.y = y->valuedouble;

            cJSON* size_node = cJSON_GetObjectItem(item, "font_size");
            if (cJSON_IsNumber(size_node)) el.font_size = size_node->valuedouble;

            cJSON* color = cJSON_GetObjectItem(item, "color");
            if (cJSON_IsString(color)) std::strncpy(el.color.data(), color->valuestring, el.color.size() - 1);

            cJSON* font = cJSON_GetObjectItem(item, "font");
            if (cJSON_IsString(font)) std::strncpy(el.font_path.data(), font->valuestring, el.font_path.size() - 1);

            tmpl.elements.push_back(el);
        }
    }

    return Status::OK;
}

} // namespace ahun
