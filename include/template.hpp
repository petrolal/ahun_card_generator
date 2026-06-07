#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "card.hpp"
#include <string>
#include <vector>

namespace ahun {

class CardTemplate {
public:
    std::string background_path;
    std::vector<TextElement> elements;

    static CardTemplate* load(const std::string& xml_path);
    static void list_available(const std::string& dir_path);
};

} // namespace ahun

#endif // TEMPLATE_H
