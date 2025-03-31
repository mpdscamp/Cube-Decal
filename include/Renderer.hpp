#pragma once
#include "Cube.hpp"
#include "Image.hpp"
#include "Math.hpp"
#include <vector>
#include <array>

// Forward declaration for ConfigManager
class ConfigManager;

/**
 * Camera class for 3D to 2D projection
 */
class ViewCamera {
private:
    double scale;      // Projection scale factor
    double centerX;    // Screen center X
    double centerY;    // Screen center Y

public:
    /**
     * Constructor with default values for camera parameters
     */
    ViewCamera(double scale = 500.0, double x = 0.0, double y = 0.0);

    /**
     * Projects a 3D point to 2D screen coordinates
     */
    Vec2 projectPoint(const Vec3& point) const;

    // Getters/setters
    double getScale() const;
    void setScale(double s);

    double getCenterX() const;
    void setCenterX(double x);

    double getCenterY() const;
    void setCenterY(double y);
};

/**
 * Renderer class that combines rendering, camera and texture mapping
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

    /**
     * Maps a texture onto a quadrilateral in the target image
     *
     * @param targetImage The image to draw the texture onto
     * @param textureImage The texture image to map
     * @param quadVertices The four vertices of the target quadrilateral
     * @param fallbackColor Color to use if mapping fails
     */
    void mapTextureToQuad(
        Image& targetImage,
        const Image& textureImage,
        const std::vector<Vec2>& quadVertices,
        const Color& fallbackColor
    );

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