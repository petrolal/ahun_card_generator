#ifndef MAGICK_WAND_WRAPPER_HPP
#define MAGICK_WAND_WRAPPER_HPP

#include <MagickWand/MagickWand.h>
#include <stdexcept>
#include <string>

namespace ahun {

class MagickEngine {
public:
    MagickEngine() {
        MagickWandGenesis();
    }
    ~MagickEngine() {
        MagickWandTerminus();
    }
    
    // Non-copyable
    MagickEngine(const MagickEngine&) = delete;
    MagickEngine& operator=(const MagickEngine&) = delete;
};

class ImageWand {
public:
    ImageWand() : wand_(NewMagickWand()) {
        if (!wand_) throw std::runtime_error("Failed to create MagickWand");
    }
    ~ImageWand() {
        if (wand_) wand_ = DestroyMagickWand(wand_);
    }
    
    MagickWand* get() const { return wand_; }
    operator MagickWand*() const { return wand_; }

    void readImage(const std::string& path) {
        if (MagickReadImage(wand_, path.c_str()) == MagickFalse) {
            throw std::runtime_error("Could not read image: " + path);
        }
    }

    void writeImage(const std::string& path) {
        if (MagickWriteImage(wand_, path.c_str()) == MagickFalse) {
            throw std::runtime_error("Could not write image: " + path);
        }
    }

private:
    MagickWand* wand_;
    ImageWand(const ImageWand&) = delete;
    ImageWand& operator=(const ImageWand&) = delete;
};

class PixelWandWrapper {
public:
    PixelWandWrapper() : wand_(NewPixelWand()) {
        if (!wand_) throw std::runtime_error("Failed to create PixelWand");
    }
    ~PixelWandWrapper() {
        if (wand_) wand_ = DestroyPixelWand(wand_);
    }
    
    PixelWand* get() const { return wand_; }
    operator PixelWand*() const { return wand_; }

    void setColor(const std::string& color) {
        PixelSetColor(wand_, color.c_str());
    }

private:
    PixelWand* wand_;
    PixelWandWrapper(const PixelWandWrapper&) = delete;
    PixelWandWrapper& operator=(const PixelWandWrapper&) = delete;
};

class DrawingWandWrapper {
public:
    DrawingWandWrapper() : wand_(NewDrawingWand()) {
        if (!wand_) throw std::runtime_error("Failed to create DrawingWand");
    }
    ~DrawingWandWrapper() {
        if (wand_) wand_ = DestroyDrawingWand(wand_);
    }
    
    DrawingWand* get() const { return wand_; }
    operator DrawingWand*() const { return wand_; }

    void setFillColor(const PixelWandWrapper& pixel_wand) {
        DrawSetFillColor(wand_, pixel_wand.get());
    }

    void setFontSize(double size) {
        DrawSetFontSize(wand_, size);
    }

    void setFont(const std::string& font_path) {
        DrawSetFont(wand_, font_path.c_str());
    }

private:
    DrawingWand* wand_;
    DrawingWandWrapper(const DrawingWandWrapper&) = delete;
    DrawingWandWrapper& operator=(const DrawingWandWrapper&) = delete;
};

} // namespace ahun

#endif // MAGICK_WAND_WRAPPER_HPP
