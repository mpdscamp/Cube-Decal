#include "ConfigManager.hpp"
#include "Logger.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <regex>
#include <iomanip>
#include <sstream>

#include "json.hpp"

// Using the namespace for convenience
using json = nlohmann::json;

ConfigManager::ConfigManager() {
    // Set default values
    setDefaults();
}

void ConfigManager::setDefaults() {
    // Animation settings
    numFrames = 240;
    frameRate = 60;
    outputDirectory = "frames";
    outputFilename = "rotating_cube.mp4";

    // Rendering settings
    width = 800;
    height = 600;
    backgroundColor = Color(10, 20, 30);

    // Camera settings
    cameraScale = 500.0;

    // Cube settings
    cubeSize = 2.0;
    decalFaceIndex = 1;
    decalImagePath = "resources/textures/shrek.png";

    // Rotation settings
    rotationSpeedX = 0.5;
    rotationSpeedY = 1.0;
    rotationSpeedZ = 0.0;
    rotateX = true;
    rotateY = true;
    rotateZ = false;
    totalRotation = 2.0 * M_PI;

    // Face colors
    faceColors = {
        Color(100, 100, 100),  // back face
        Color(150, 150, 150),  // front face
        Color(180, 100, 100),  // bottom face
        Color(100, 180, 100),  // top face
        Color(100, 100, 180),  // left face
        Color(180, 180, 100)   // right face
    };
}

bool ConfigManager::loadFromFile(const std::string& filename) {
    try {
        // Open the file
        std::ifstream file(filename);
        if (!file.is_open()) {
            LOG_ERROR << "Could not open configuration file: " << filename;
            return false;
        }

        // Parse the JSON
        json config;
        file >> config;

        // Animation settings
        if (config.contains("animation")) {
            auto& animation = config["animation"];
            numFrames = animation.value("numFrames", numFrames);
            frameRate = animation.value("frameRate", frameRate);
            outputDirectory = animation.value("outputDirectory", outputDirectory);
            outputFilename = animation.value("outputFilename", outputFilename);
        }

        // Rendering settings
        if (config.contains("rendering")) {
            auto& rendering = config["rendering"];
            width = rendering.value("width", width);
            height = rendering.value("height", height);

            if (rendering.contains("backgroundColor")) {
                auto& bg = rendering["backgroundColor"];
                backgroundColor.r = bg.value("r", backgroundColor.r);
                backgroundColor.g = bg.value("g", backgroundColor.g);
                backgroundColor.b = bg.value("b", backgroundColor.b);
            }
        }

        // Camera settings
        if (config.contains("camera")) {
            auto& camera = config["camera"];
            cameraScale = camera.value("scale", cameraScale);
        }

        // Cube settings
        if (config.contains("cube")) {
            auto& cube = config["cube"];
            cubeSize = cube.value("size", cubeSize);
            decalFaceIndex = cube.value("decalFaceIndex", decalFaceIndex);
            decalImagePath = cube.value("decalImagePath", decalImagePath);

            // Face colors
            if (cube.contains("faceColors")) {
                auto& colors = cube["faceColors"];
                for (size_t i = 0; i < std::min(colors.size(), faceColors.size()); i++) {
                    if (colors[i].is_object()) {
                        faceColors[i].r = colors[i].value("r", faceColors[i].r);
                        faceColors[i].g = colors[i].value("g", faceColors[i].g);
                        faceColors[i].b = colors[i].value("b", faceColors[i].b);
                    }
                }
            }
        }

        // Rotation settings
        if (config.contains("rotation")) {
            auto& rotation = config["rotation"];
            rotationSpeedX = rotation.value("speedX", rotationSpeedX);
            rotationSpeedY = rotation.value("speedY", rotationSpeedY);
            rotationSpeedZ = rotation.value("speedZ", rotationSpeedZ);
            rotateX = rotation.value("enableX", rotateX);
            rotateY = rotation.value("enableY", rotateY);
            rotateZ = rotation.value("enableZ", rotateZ);

            // Handle totalRotation in various formats
            if (rotation.contains("totalRotation")) {
                if (rotation["totalRotation"].is_string()) {
                    // If it's a string, try to parse it as Xpi
                    totalRotation = parseRotation(rotation["totalRotation"].get<std::string>());
                }
                else if (rotation["totalRotation"].is_number()) {
                    // If it's a number, use it directly as radians
                    totalRotation = rotation["totalRotation"].get<double>();
                }
            }
        }

        LOG_INFO << "Successfully loaded configuration from: " << filename;
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR << "Error loading configuration: " << e.what();
        return false;
    }
}

bool ConfigManager::saveToFile(const std::string& filename) const {
    try {
        // Create the JSON object
        json config;

        // Animation settings
        config["animation"] = {
            {"numFrames", numFrames},
            {"frameRate", frameRate},
            {"outputDirectory", outputDirectory},
            {"outputFilename", outputFilename}
        };

        // Rendering settings
        config["rendering"] = {
            {"width", width},
            {"height", height},
            {"backgroundColor", {
                {"r", backgroundColor.r},
                {"g", backgroundColor.g},
                {"b", backgroundColor.b}
            }}
        };

        // Camera settings
        config["camera"] = {
            {"scale", cameraScale}
        };

        // Cube settings
        json faceColorsJson = json::array();
        for (const auto& color : faceColors) {
            faceColorsJson.push_back({
                {"r", color.r},
                {"g", color.g},
                {"b", color.b}
                });
        }

        config["cube"] = {
            {"size", cubeSize},
            {"decalFaceIndex", decalFaceIndex},
            {"decalImagePath", decalImagePath},
            {"faceColors", faceColorsJson}
        };

        // Rotation settings
        config["rotation"] = {
            {"speedX", rotationSpeedX},
            {"speedY", rotationSpeedY},
            {"speedZ", rotationSpeedZ},
            {"enableX", rotateX},
            {"enableY", rotateY},
            {"enableZ", rotateZ},
            {"totalRotation", totalRotation}
        };

        // Write to file
        std::ofstream file(filename);
        if (!file.is_open()) {
            LOG_ERROR << "Could not open file for writing: " << filename;
            return false;
        }

        file << config.dump(4);
        LOG_INFO << "Successfully saved configuration to: " << filename;
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR << "Error saving configuration: " << e.what();
        return false;
    }
}

bool ConfigManager::createDefaultConfigFile(const std::string& filename) {
    ConfigManager defaultConfig;
    return defaultConfig.saveToFile(filename);
}

double ConfigManager::parseRotation(const std::string& piString) {
    try {
        // Regex pattern to match strings like "2pi", "2.5pi", etc.
        std::regex pattern(R"(^(\d*\.?\d+|\.?\d+)?(pi)$)", std::regex::icase);
        std::smatch matches;

        if (std::regex_match(piString, matches, pattern)) {
            // Extract the multiplier
            std::string multiplierStr = matches[1].str();

            // If no multiplier before pi, assume 1
            double multiplier = multiplierStr.empty() ? 1.0 : std::stod(multiplierStr);

            // Return as radians
            return multiplier * M_PI;
        }

        // If it doesn't match the pattern, try parsing as a direct number
        try {
            return std::stod(piString);
        }
        catch (...) {
            LOG_WARNING << "Failed to parse rotation value: " << piString << ". Using default (2π).";
            return 2.0 * M_PI;
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR << "Error parsing rotation: " << e.what();
        return 2.0 * M_PI;  // Default
    }
}