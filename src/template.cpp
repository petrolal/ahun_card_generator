#include "template.hpp"
#include "exceptions.hpp"
#include "logger.hpp"
#include "wrappers/XmlWrapper.hpp"
#include <iostream>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

namespace ahun {

void CardTemplate::list_available(const std::string& dir_path) {
    if (!fs::exists(dir_path)) {
        throw ConfigException("Templates directory does not exist: " + dir_path);
    }

    Logger::info("Available Templates:");

    int count = 0;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".xml") {
            std::cout << "  \x1b[1m»\x1b[0m " << entry.path().filename().string() << std::endl;
            count++;
        }
    }

    if (count == 0) {
        Logger::warn("No .xml templates found in '" + dir_path + "'");
    }
}

CardTemplate* CardTemplate::load(const std::string& xml_path) {
    try {
        XmlDocument doc(xml_path);
        xmlNodePtr root = doc.getRootElement();
        
        if (root == nullptr || std::strcmp((const char*)root->name, "template") != 0) {
            throw TemplateException("Invalid XML root in template: " + xml_path);
        }

        auto tmpl = std::make_unique<CardTemplate>();

        for (xmlNodePtr cur = root->children; cur; cur = cur->next) {
            if (cur->type == XML_ELEMENT_NODE) {
                if (std::strcmp((const char*)cur->name, "background") == 0) {
                    xmlChar* bg = xmlNodeGetContent(cur);
                    if (bg) {
                        tmpl->background_path = (const char*)bg;
                        xmlFree(bg);
                    }
                } else if (std::strcmp((const char*)cur->name, "text_layer") == 0) {
                    TextElement el = {}; // Initialize with zeros
                    for (xmlNodePtr layer_cur = cur->children; layer_cur; layer_cur = layer_cur->next) {
                        if (layer_cur->type == XML_ELEMENT_NODE) {
                            xmlChar* content = xmlNodeGetContent(layer_cur);
                            if (content) {
                                if (std::strcmp((const char*)layer_cur->name, "id") == 0) {
                                    el.id = (const char*)content;
                                } else if (std::strcmp((const char*)layer_cur->name, "x") == 0) {
                                    el.x = std::atof((const char*)content);
                                } else if (std::strcmp((const char*)layer_cur->name, "y") == 0) {
                                    el.y = std::atof((const char*)content);
                                } else if (std::strcmp((const char*)layer_cur->name, "font_size") == 0) {
                                    el.font_size = std::atof((const char*)content);
                                } else if (std::strcmp((const char*)layer_cur->name, "color") == 0) {
                                    el.color = (const char*)content;
                                } else if (std::strcmp((const char*)layer_cur->name, "font") == 0) {
                                    el.font_path = (const char*)content;
                                }
                                xmlFree(content);
                            }
                        }
                    }
                    tmpl->elements.push_back(el);
                }
            }
        }
        return tmpl.release();
    } catch (const std::exception& e) {
        throw TemplateException(std::string("Error loading template: ") + e.what());
    }
}

} // namespace ahun
