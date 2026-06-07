#ifndef CARD_HPP
#define CARD_HPP

#include <array>
#include <vector>
#include "status.hpp"

namespace ahun {

struct TextElement {
    std::array<char, 32> id;
    std::array<char, 128> text; // Fixed size for embedded to prevent fragmentation
    double x = 0;
    double y = 0;
    double font_size = 0;
    std::array<char, 32> color;
    std::array<char, 128> font_path;
};

struct CardConfig {
    std::array<char, 128> template_path;
    std::array<char, 128> font_path;
    std::array<char, 128> output_path;
    std::vector<TextElement> elements;
};

class CardGenerator {
public:
    static Status generate(const CardConfig& config);

private:
#ifdef ARDUINO
    static uint32_t get_color_from_string(const std::string& color);
#endif
};

} // namespace ahun

#endif // CARD_HPP
