#include "card.h"
#include <stdio.h>
#include <MagickWand/MagickWand.h>

void init_card_engine(void) {
    MagickWandGenesis();
}

void cleanup_card_engine(void) {
    MagickWandTerminus();
}

int generate_card(CardConfig *config) {
    MagickWand *wand;
    DrawingWand *d_wand;
    PixelWand *p_wand;
    MagickBooleanType status;

    wand = NewMagickWand();

    // 1. Load background
    status = MagickReadImage(wand, config->template_path);
    if (status == MagickFalse) {
        fprintf(stderr, "Error: Could not read template image '%s'\n", config->template_path);
        wand = DestroyMagickWand(wand);
        return -1;
    }

    // 2. Configure drawing
    d_wand = NewDrawingWand();
    p_wand = NewPixelWand();

    PixelSetColor(p_wand, "black"); // Default color
    DrawSetFillColor(d_wand, p_wand);
    
    if (config->font_path) {
        DrawSetFont(d_wand, config->font_path);
    }

    // 3. Draw text elements
    for (int i = 0; i < config->element_count; i++) {
        if (config->elements[i].text == NULL) {
            continue;
        }
        DrawSetFontSize(d_wand, config->elements[i].font_size);
        MagickAnnotateImage(wand, d_wand, config->elements[i].x, config->elements[i].y, 0, (const char*)config->elements[i].text);
    }

    // 4. Save result
    status = MagickWriteImage(wand, config->output_path);
    if (status == MagickFalse) {
        fprintf(stderr, "Error: Could not write output image '%s'\n", config->output_path);
    }

    // Cleanup
    d_wand = DestroyDrawingWand(d_wand);
    p_wand = DestroyPixelWand(p_wand);
    wand = DestroyMagickWand(wand);

    return (status == MagickTrue) ? 0 : -1;
}
