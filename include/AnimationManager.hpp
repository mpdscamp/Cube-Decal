#pragma once
#include "Renderer.hpp"
#include "ConfigManager.hpp"
#include <string>

/**
 * Handles animation creation and video export
 */
class AnimationManager {
private:
    std::string outputDirectory;
    int numFrames;
    int frameRate;
    std::string outputFilename;

    // Rotation settings
    double rotationSpeedX;
    double rotationSpeedY;
    double rotationSpeedZ;
    bool rotateX;
    bool rotateY;
    bool rotateZ;
    double totalRotation;  // Total rotation in degrees

public:
    /**
     * Constructor
     *
     * @param config Configuration settings
     */
    AnimationManager(const ConfigManager& config);

    /**
     * Configure the animation manager
     *
     * @param config Configuration settings
     */
    void configure(const ConfigManager& config);

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

    // Getters/setters
    std::string getOutputDirectory() const;
    void setOutputDirectory(const std::string& directory);

    int getNumFrames() const;
    void setNumFrames(int frames);

    int getFrameRate() const;
    void setFrameRate(int rate);

    std::string getOutputFilename() const;
    void setOutputFilename(const std::string& filename);

    double getTotalRotation() const;
    void setTotalRotation(double degrees);
};