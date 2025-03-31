#include "Image.hpp"
#include "Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Color implementation
Color::Color(unsigned char r, unsigned char g, unsigned char b)
    : r(r), g(g), b(b) {
}

// Default constructor - creates a 1x1 black image
Image::Image() : width(1), height(1), pixels(1, Color(0, 0, 0)) {}

Image::Image(int width, int height, const Color& background)
    : width(width), height(height), pixels(width* height, background) {
}

void Image::setPixel(int x, int y, const Color& color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixels[y * width + x] = color;
    }
}

Color Image::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return pixels[y * width + x];
    }
    return Color(); // Default black
}

void Image::drawLine(int x0, int y0, int x1, int y1, const Color& color) {
    // Bresenham's line algorithm
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        setPixel(x0, y0, color);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Image::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color) {
    // Sort vertices by y-coordinate (y1 <= y2 <= y3)
    if (y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    if (y1 > y3) {
        std::swap(x1, x3);
        std::swap(y1, y3);
    }
    if (y2 > y3) {
        std::swap(x2, x3);
        std::swap(y2, y3);
    }

    if (y2 == y3) {
        // Flat bottom triangle
        fillFlatBottomTriangle(x1, y1, x2, y2, x3, y3, color);
    }
    else if (y1 == y2) {
        // Flat top triangle
        fillFlatTopTriangle(x1, y1, x2, y2, x3, y3, color);
    }
    else {
        // General triangle - split into flat-bottom and flat-top
        int x4 = x1 + ((y2 - y1) * (x3 - x1)) / (y3 - y1);
        int y4 = y2;

        fillFlatBottomTriangle(x1, y1, x2, y2, x4, y4, color);
        fillFlatTopTriangle(x2, y2, x4, y4, x3, y3, color);
    }
}

void Image::fillFlatBottomTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color) {
    if (y2 == y1) return; // Prevent division by zero

    double slope1 = static_cast<double>(x2 - x1) / (y2 - y1);
    double slope2 = static_cast<double>(x3 - x1) / (y3 - y1);

    double x_start = x1;
    double x_end = x1;

    for (int y = y1; y <= y2; y++) {
        drawLine((int)x_start, y, (int)x_end, y, color);
        x_start += slope1;
        x_end += slope2;
    }
}

void Image::fillFlatTopTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color) {
    if (y3 == y1) return; // Prevent division by zero

    double slope1 = static_cast<double>(x3 - x1) / (y3 - y1);
    double slope2 = static_cast<double>(x3 - x2) / (y3 - y2);

    double x_start = x3;
    double x_end = x3;

    for (int y = y3; y >= y1; y--) {
        drawLine((int)x_start, y, (int)x_end, y, color);
        x_start -= slope1;
        x_end -= slope2;
    }
}

void Image::saveAsPPM(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        LOG_ERROR << "Could not open file for writing: " << filename;
        return;
    }

    // PPM header
    file << "P6\n" << width << " " << height << "\n255\n";

    // Write pixel data
    for (const auto& pixel : pixels) {
        file.write(reinterpret_cast<const char*>(&pixel.r), 1);
        file.write(reinterpret_cast<const char*>(&pixel.g), 1);
        file.write(reinterpret_cast<const char*>(&pixel.b), 1);
    }

    file.close();
}

int Image::getWidth() const { return width; }
int Image::getHeight() const { return height; }

// Load image (PNG, JPG, etc.)
Image loadImage(const std::string& filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    if (!data) {
        LOG_ERROR << "Failed to load image: " << filename;
        LOG_ERROR << "STB Error: " << stbi_failure_reason();
        return Image(1, 1); // Return a 1x1 black image
    }

    // Create image from loaded data
    Image img(data, width, height, channels);

    // Free the loaded data as we've copied it
    stbi_image_free(data);

    return img;
}

// Constructor from pixel data
Image::Image(unsigned char* data, int width, int height, int channels)
    : width(width), height(height), pixels(width* height, Color(0, 0, 0)) {

    // Process the raw pixel data based on the number of channels
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * channels;

            if (channels >= 3) {
                // RGB or RGBA
                setPixel(x, y, Color(data[index], data[index + 1], data[index + 2]));
            }
            else if (channels == 1) {
                // Grayscale
                setPixel(x, y, Color(data[index], data[index], data[index]));
            }
        }
    }
}