#pragma once

#include <string>
#include <vector>
#include "Color.hpp"

/**
 * Configuration manager class to load and provide access to application settings
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

private:
    /**
     * Initialize with default values
     */
    void setDefaults();
};