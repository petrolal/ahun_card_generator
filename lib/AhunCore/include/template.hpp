#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#include "card.hpp"
#include "status.hpp"
#include <vector>
#include <string>

namespace ahun {

class CardTemplate {
public:
    std::array<char, 128> background_path;
    std::vector<TextElement> elements;

    // Static loader that returns a pointer (managed by unique_ptr externally)
    // or a Status code via an out-parameter. 
    // To follow the "no exceptions" rule, we return Status and use unique_ptr.
    static Status load(const std::string& json_path, CardTemplate& tmpl);
    static void list_available(const std::string& dir_path);
};

} // namespace ahun

#endif // TEMPLATE_HPP
