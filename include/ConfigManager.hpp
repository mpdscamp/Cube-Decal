#pragma once

#include <string>
#include <vector>
#include "Image.hpp"
#include "Math.hpp"
#include "Cube.hpp"

// Forward declaration
class Renderer;

/**
 * Configuration manager class to load, save and provide access to application settings
 * Also handles animation and video export
 */
class ConfigManager {
public:
    // Animation settings
    int numFrames;
    int frameRate;
    std::string outputDirectory;
    std::string outputFilename;

    // Rendering settings
    int width;
    int height;
    Color backgroundColor;

    // Camera settings
    double cameraScale;

    // Cube settings
    double cubeSize;
    int decalFaceIndex;  // Which face gets the texture (0-5)
    std::string decalImagePath;

    // Rotation settings
    double rotationSpeedX;     // Rotation speed multiplier for X axis
    double rotationSpeedY;     // Rotation speed multiplier for Y axis
    double rotationSpeedZ;     // Rotation speed multiplier for Z axis
    bool rotateX;              // Whether to rotate around X axis
    bool rotateY;              // Whether to rotate around Y axis 
    bool rotateZ;              // Whether to rotate around Z axis
    double totalRotation;      // Total rotation in radians

    // Face colors (0: back, 1: front, 2: bottom, 3: top, 4: left, 5: right)
    std::vector<Color> faceColors;

    /**
     * Constructor - initializes with default values
     */
    ConfigManager();

    /**
     * Load configuration from a JSON file
     *
     * @param filename The JSON file to load
     * @return true if successful, false otherwise
     */
    bool loadFromFile(const std::string& filename);

    /**
     * Save the current configuration to a JSON file
     *
     * @param filename The filename to save to
     * @return true if successful, false otherwise
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * Create a default configuration file if one doesn't exist
     *
     * @param filename The filename to create
     * @return true if successful, false otherwise
     */
    static bool createDefaultConfigFile(const std::string& filename);

    /**
     * Convert a rotation string in format "Xpi" to radians
     *
     * @param piString String in format "2.0pi", "1.0pi", etc.
     * @return Value in radians, or 0 if parsing failed
     */
    static double parseRotation(const std::string& piString);

    /**
     * Configure the animation manager with current settings
     */
    void configureAnimation();

    /**
     * Render an animation of a rotating cube
     *
     * @param renderer The renderer to use
     * @param cube The cube to animate
     * @param decalImage Optional texture to apply to front face
     */
    void renderAnimation(Renderer& renderer, Cube& cube, const Image* decalImage = nullptr);

    /**
     * Create output directory and clean any existing files
     */
    void prepareOutputDirectory() const;

    /**
     * Save a frame to the output directory
     *
     * @param frame The image to save
     * @param frameNumber The frame number (used in filename)
     */
    void saveFrame(const Image& frame, int frameNumber) const;

    /**
     * Create a video from the rendered frames
     *
     * @return true if video creation was successful
     */
    bool createVideo() const;

    /**
     * Calculate rotation matrix for a given frame
     *
     * @param frame Current frame number
     * @return Combined rotation matrix
     */
    Mat4x4 calculateRotation(int frame) const;

private:
    /**
     * Initialize with default values
     */
    void setDefaults();
};