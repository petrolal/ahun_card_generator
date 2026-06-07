#include "card.hpp"
#include "logger.hpp"

#ifdef ARDUINO
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LittleFS.h>

class LGFX_App : public lgfx::LGFX_Device {
public:
    LGFX_App() {}
};

static LGFX_App gfx;
#endif

namespace ahun {

#ifdef ARDUINO
uint32_t CardGenerator::get_color_from_string(const std::string& color) {
    if (color == "white") return TFT_WHITE;
    if (color == "black") return TFT_BLACK;
    if (color == "red") return TFT_RED;
    if (color == "green") return TFT_GREEN;
    if (color == "blue") return TFT_BLUE;
    if (color == "yellow") return TFT_YELLOW;
    if (color == "cyan") return TFT_CYAN;
    if (color == "magenta") return TFT_MAGENTA;
    if (color == "orange") return TFT_ORANGE;
    
    if (color.size() == 7 && color[0] == '#') {
        return strtol(color.c_str() + 1, nullptr, 16);
    }
    
    return TFT_WHITE;
}

static bool save_sprite_to_bmp(LGFX_Sprite& sprite, const char* path) {
    std::string local_path = path;
    if (!local_path.empty() && local_path[0] != '/') {
        local_path = "/" + local_path;
    }
    File file = LittleFS.open(local_path.c_str(), FILE_WRITE);
    if (!file) return false;

    uint32_t width = sprite.width();
    uint32_t height = sprite.height();
    uint32_t rowSize = (width * 3 + 3) & ~3;
    uint32_t fileSize = 54 + rowSize * height;

    unsigned char bmpFileHeader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpInfoHeader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};

    bmpFileHeader[2] = (unsigned char)(fileSize);
    bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
    bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
    bmpFileHeader[5] = (unsigned char)(fileSize >> 24);

    bmpInfoHeader[4] = (unsigned char)(width);
    bmpInfoHeader[5] = (unsigned char)(width >> 8);
    bmpInfoHeader[6] = (unsigned char)(width >> 16);
    bmpInfoHeader[7] = (unsigned char)(width >> 24);
    bmpInfoHeader[8] = (unsigned char)(height);
    bmpInfoHeader[9] = (unsigned char)(height >> 8);
    bmpInfoHeader[10] = (unsigned char)(height >> 16);
    bmpInfoHeader[11] = (unsigned char)(height >> 24);

    file.write(bmpFileHeader, 14);
    file.write(bmpInfoHeader, 40);

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < (int)width; x++) {
            lgfx::rgb888_t pixel = sprite.readPixel(x, y);
            uint8_t bgr[3] = { pixel.b, pixel.g, pixel.r };
            file.write(bgr, 3);
        }
        if (rowSize > width * 3) {
            uint8_t padding[3] = {0,0,0};
            file.write(padding, rowSize - width * 3);
        }
    }
    file.close();
    return true;
}
#endif

Status CardGenerator::generate(const CardConfig& config) {
#ifdef ARDUINO
    Logger::info("Gerando card via LovyanGFX...");

    std::string path = config.template_path.data();
    if (!path.empty() && path[0] != '/') {
        path = "/" + path;
    }
    uint32_t width = 800, height = 1200;
    
    File file = LittleFS.open(path.c_str());
    if (file) {
        if (path.find(".bmp") != std::string::npos) {
            file.seek(18);
            file.read((uint8_t*)&width, 4);
            file.read((uint8_t*)&height, 4);
        }
        file.close();
    }

    LGFX_Sprite canvas(&gfx);
    canvas.setPsram(true);
    canvas.setColorDepth(24); 

    if (!canvas.createSprite(width, height)) {
        Logger::error("Memória insuficiente na PSRAM para " + std::to_string(width) + "x" + std::to_string(height));
        return Status::ERROR_OUT_OF_MEMORY;
    }

    bool success = false;
    if (path.find(".png") != std::string::npos) {
        success = canvas.drawPngFile(path.c_str(), 0, 0);
    } else {
        success = canvas.drawBmpFile(path.c_str(), 0, 0);
    }

    if (!success) {
        Logger::warn("Falha ao carregar background: " + path);
        canvas.fillScreen(TFT_BLACK); 
    }

    for (const auto& el : config.elements) {
        if (el.text[0] == '\0') continue;
        canvas.setTextColor(get_color_from_string(el.color.data()));
        canvas.setTextSize(el.font_size / 12.0); 
        canvas.drawString(el.text.data(), el.x, el.y);
    }

    if (save_sprite_to_bmp(canvas, config.output_path.data())) {
        Logger::success("Card gerado: " + std::string(config.output_path.data()));
    } else {
        Logger::error("Erro ao gravar arquivo no LittleFS.");
    }

    canvas.deleteSprite();
    return Status::OK;
#else
    Logger::info("Native Mock: Card configurado para " + std::string(config.output_path.data()));
    return Status::OK;
#endif
}

} // namespace ahun
