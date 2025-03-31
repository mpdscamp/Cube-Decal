#pragma once

#include <vector>
#include <string>

/**
 * RGB Color structure with 8-bit components
 */
class Color {
public:
    unsigned char r, g, b;

    /**
     * Constructor for creating a color
     *
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     */
    Color(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
};

/**
 * Simple image class with pixel manipulation and drawing capabilities
 */
class Image {
private:
    std::vector<Color> pixels;
    int width, height;

public:
    /**
     * Default constructor creates a 1x1 black image
     */
    Image();

    /**
     * Constructor with dimensions and background color
     *
     * @param width Width of the image in pixels
     * @param height Height of the image in pixels
     * @param background Background color for the image
     */
    Image(int width, int height, const Color& background = Color(0, 0, 0));

    /**
     * Constructor from pixel data (for stb_image loaded images)
     *
     * @param data Raw pixel data
     * @param width Width of the image in pixels
     * @param height Height of the image in pixels
     * @param channels Number of color channels
     */
    Image(unsigned char* data, int width, int height, int channels);

    /**
     * Set color of a specific pixel
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Color to set
     */
    void setPixel(int x, int y, const Color& color);

    /**
     * Get color of a specific pixel
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @return Color of the pixel
     */
    Color getPixel(int x, int y) const;

    /**
     * Draw a line between two points
     *
     * @param x0 Start X coordinate
     * @param y0 Start Y coordinate
     * @param x1 End X coordinate
     * @param y1 End Y coordinate
     * @param color Line color
     */
    void drawLine(int x0, int y0, int x1, int y1, const Color& color);

    /**
     * Fill a triangle defined by three points
     *
     * @param x1 First point X coordinate
     * @param y1 First point Y coordinate
     * @param x2 Second point X coordinate
     * @param y2 Second point Y coordinate
     * @param x3 Third point X coordinate
     * @param y3 Third point Y coordinate
     * @param color Fill color
     */
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color);

    /**
     * Helper method for triangle filling (flat bottom case)
     *
     * @param x1 First point X coordinate
     * @param y1 First point Y coordinate
     * @param x2 Second point X coordinate
     * @param y2 Second point Y coordinate
     * @param x3 Third point X coordinate
     * @param y3 Third point Y coordinate
     * @param color Fill color
     */
    void fillFlatBottomTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color);

    /**
     * Helper method for triangle filling (flat top case)
     *
     * @param x1 First point X coordinate
     * @param y1 First point Y coordinate
     * @param x2 Second point X coordinate
     * @param y2 Second point Y coordinate
     * @param x3 Third point X coordinate
     * @param y3 Third point Y coordinate
     * @param color Fill color
     */
    void fillFlatTopTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color);

    /**
     * Save image as PPM (P6 format)
     *
     * @param filename Output filename
     */
    void saveAsPPM(const std::string& filename) const;

    /**
     * Get image width
     *
     * @return Width in pixels
     */
    int getWidth() const;

    /**
     * Get image height
     *
     * @return Height in pixels
     */
    int getHeight() const;
};

/**
 * Load image using stb_image (PNG, JPG, etc.)
 *
 * @param filename Input image filename
 * @return Loaded image
 */
Image loadImage(const std::string& filename);