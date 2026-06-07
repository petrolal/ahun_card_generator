#ifndef CARD_H
#define CARD_H

#include <string>
#include <vector>

namespace ahun {

struct TextElement {
    std::string id;
    std::string text;
    double x = 0;
    double y = 0;
    double font_size = 0;
    std::string color;
    std::string font_path;
};

struct CardConfig {
    std::string template_path;
    std::string font_path;
    std::string output_path;
    std::vector<TextElement> elements;
};

class CardGenerator {
public:
    static void generate(const CardConfig& config);
};

void init_card_engine(void);
void cleanup_card_engine(void);

} // namespace ahun

#endif // CARD_H
