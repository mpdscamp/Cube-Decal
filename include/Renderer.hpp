#pragma once
#include "Cube.hpp"
#include "Image.hpp"
#include "ViewCamera.hpp"
#include "ConfigManager.hpp"
#include <vector>
#include <array>

/**
 * Simplified Renderer class - focuses on rendering a cube with optional texture
 */
class Renderer {
private:
    int width;
    int height;
    Color backgroundColor;
    ViewCamera camera;
    int decalFaceIndex;  // Which face gets the texture (0-5)

    // Face colors (configurable)
    std::array<Color, 6> faceColors;

public:
    /**
     * Constructor
     *
     * @param width The width of the rendered image
     * @param height The height of the rendered image
     */
    Renderer(int width, int height);

    /**
     * Constructor with configuration
     *
     * @param config Configuration settings
     */
    Renderer(const ConfigManager& config);

    /**
     * Configure the renderer with settings
     *
     * @param config Configuration settings
     */
    void configure(const ConfigManager& config);

    /**
     * Renders a single frame of the cube
     *
     * @param cube The cube to render
     * @param angle Rotation angle (used for legacy compatibility)
     * @param decalImage Optional texture for the specified face
     * @param rotationMatrix Optional custom rotation matrix (if null, uses angle)
     * @return The rendered image
     */
    Image renderFrame(
        const Cube& cube,
        double angle,
        const Image* decalImage = nullptr,
        const Mat4x4* rotationMatrix = nullptr
    );

    // Getters/setters
    void setBackgroundColor(const Color& color);
    Color getBackgroundColor() const;

    void setFaceColor(int faceIndex, const Color& color);

    void setDecalFaceIndex(int index);
    int getDecalFaceIndex() const;

    ViewCamera& getCamera();

    int getWidth() const;
    int getHeight() const;
};