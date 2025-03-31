#include <iostream>
#include <string>
#include "Cube.hpp"
#include "Renderer.hpp"
#include "Image.hpp"
#include "Logger.hpp"
#include "ConfigManager.hpp"

// Function to print command-line usage
void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help            Show this help message" << std::endl;
    std::cout << "  -c, --config FILE     Load configuration from FILE (default: config.json)" << std::endl;
}

int main(int argc, char* argv[]) {
    // Default configuration file
    std::string configFile = "../../../../config.json";

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
            else {
                LOG_ERROR << "Missing argument for " << arg;
                return 1;
            }
        }
        else {
            LOG_WARNING << "Unknown argument: " << arg;
        }
    }

    // Load configuration
    ConfigManager config;
    if (!config.loadFromFile(configFile)) {
        LOG_WARNING << "Failed to load configuration file: " << configFile;
        LOG_INFO << "Using default settings...";
    }

    // Create a cube with configurable size
    Cube cube(config.cubeSize);

    // Initialize the renderer with configuration
    Renderer renderer(config);

    // Load the texture from the configured path
    Image decalImage = loadImage(config.decalImagePath);

    // Check if the image loaded successfully
    if (decalImage.getWidth() <= 1 || decalImage.getHeight() <= 1) {
        LOG_FATAL << "Failed to load required texture: " << config.decalImagePath;
        return 1;
    }

    LOG_INFO << "Successfully loaded image: " << config.decalImagePath;

    // Render animation
    LOG_INFO << "Rendering animation with " << config.numFrames << " frames...";
    LOG_INFO << "This will create a " << (config.numFrames / static_cast<double>(config.frameRate))
        << " second video at " << config.frameRate << " fps.";

    config.renderAnimation(renderer, cube, &decalImage);

    LOG_INFO << "Animation complete!";
    return 0;
}