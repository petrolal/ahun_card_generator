#include "card.hpp"
#include "exceptions.hpp"
#include "wrappers/MagickWandWrapper.hpp"
#include <iostream>

namespace ahun {

void CardGenerator::generate(const CardConfig& config) {
    try {
        ImageWand wand;
        wand.readImage(config.template_path);

        DrawingWandWrapper d_wand;
        PixelWandWrapper p_wand;

        p_wand.setColor("black");
        d_wand.setFillColor(p_wand);
        
        if (!config.font_path.empty()) {
            d_wand.setFont(config.font_path);
        }

        for (const auto& el : config.elements) {
            if (el.text.empty()) continue;

            if (!el.color.empty()) {
                p_wand.setColor(el.color);
            } else {
                p_wand.setColor("black");
            }
            d_wand.setFillColor(p_wand);

            if (!el.font_path.empty()) {
                d_wand.setFont(el.font_path);
            } else if (!config.font_path.empty()) {
                d_wand.setFont(config.font_path);
            }

            d_wand.setFontSize(el.font_size);
            if (MagickAnnotateImage(wand.get(), d_wand.get(), el.x, el.y, 0, el.text.c_str()) == MagickFalse) {
                throw RenderingException("Failed to annotate image with text: " + el.text);
            }
        }

        wand.writeImage(config.output_path);
    } catch (const std::exception& e) {
        throw RenderingException(std::string("Error generating card: ") + e.what());
    }
}

// Deprecated in favor of MagickEngine RAII, but keeping for compatibility if needed during Phase 3
void init_card_engine(void) {
    MagickWandGenesis();
}

void cleanup_card_engine(void) {
    MagickWandTerminus();
}

} // namespace ahun
