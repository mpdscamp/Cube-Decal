#pragma once

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