#include "AnimationManager.hpp"
#include "Logger.hpp"
#include <sstream>
#include <cmath>

AnimationManager::AnimationManager(const ConfigManager& config) {
    configure(config);
}

void AnimationManager::configure(const ConfigManager& config) {
    outputDirectory = config.outputDirectory;
    numFrames = config.numFrames;
    frameRate = config.frameRate;
    outputFilename = config.outputFilename;

    rotationSpeedX = config.rotationSpeedX;
    rotationSpeedY = config.rotationSpeedY;
    rotationSpeedZ = config.rotationSpeedZ;
    rotateX = config.rotateX;
    rotateY = config.rotateY;
    rotateZ = config.rotateZ;
    totalRotation = config.totalRotation;
}

void AnimationManager::renderAnimation(Renderer& renderer, Cube& cube, const Image* decalImage) {
    // Prepare output directory
    prepareOutputDirectory();

    // Render each frame
    for (int frame = 0; frame < numFrames; frame++) {
        // Calculate angle based on frame number and rotation settings
        Mat4x4 rotation = calculateRotation(frame);

        // Render the frame with the calculated rotation
        double angle = 2.0 * M_PI * frame / numFrames;
        Image frameImage = renderer.renderFrame(cube, angle, decalImage, &rotation);

        // Save the frame
        saveFrame(frameImage, frame);

        LOG_INFO << "Frame " << frame + 1 << "/" << numFrames << " rendered";
    }

    // Create video from the frames
    createVideo();
}

Mat4x4 AnimationManager::calculateRotation(int frame) const {
    // Base angle calculation - proportion of total rotation
    double baseAngle = totalRotation * frame / numFrames;

    // Calculate rotation around each axis
    double angleX = rotateX ? baseAngle * rotationSpeedX : 0.0;
    double angleY = rotateY ? baseAngle * rotationSpeedY : 0.0;
    double angleZ = rotateZ ? baseAngle * rotationSpeedZ : 0.0;

    // Create rotation matrices
    Mat4x4 rotMatX = ::rotateX(angleX);
    Mat4x4 rotMatY = ::rotateY(angleY);
    Mat4x4 rotMatZ = ::rotateZ(angleZ);

    // Combine rotations: first X, then Y, then Z
    return rotMatZ * rotMatY * rotMatX;
}

void AnimationManager::prepareOutputDirectory() const {
    LOG_INFO << "Preparing output directory: " << outputDirectory;

#if defined(_WIN32)
    // On Windows
    std::string rmDir = "if exist \"" + outputDirectory + "\" rmdir /S /Q \"" + outputDirectory + "\"";
    std::string mkDir = "mkdir \"" + outputDirectory + "\"";
#else
    // On Unix-like systems
    std::string rmDir = "rm -rf \"" + outputDirectory + "\"";
    std::string mkDir = "mkdir -p \"" + outputDirectory + "\"";
#endif

    // Execute the commands
    int rmResult = system(rmDir.c_str());
    int mkResult = system(mkDir.c_str());

    if (rmResult != 0 || mkResult != 0) {
        LOG_WARNING << "Directory cleanup may have failed. Check permissions.";
    }
}

void AnimationManager::saveFrame(const Image& frame, int frameNumber) const {
    std::stringstream ss;
    ss << outputDirectory << "/frame_" << frameNumber << ".ppm";
    frame.saveAsPPM(ss.str());
}

bool AnimationManager::createVideo() const {
    LOG_INFO << "Creating video with ffmpeg...";

#ifdef HAVE_FFMPEG
    // Simple ffmpeg command
    std::string ffmpegCmd = "ffmpeg -y -framerate " + std::to_string(frameRate) + " -i " +
        outputDirectory + "/frame_%d.ppm -c:v libx264 -pix_fmt yuv420p " +
        outputFilename;

    LOG_INFO << "Running command: " << ffmpegCmd;
    int result = system(ffmpegCmd.c_str());

    if (result == 0) {
        LOG_INFO << "Video created successfully: " << outputFilename;
        return true;
    }
    else {
        LOG_ERROR << "Failed to create video. There might be an issue with ffmpeg.";
        return false;
    }
#else
    LOG_WARNING << "FFmpeg not found during build configuration. Video creation skipped.";
    return false;
#endif
}

std::string AnimationManager::getOutputDirectory() const {
    return outputDirectory;
}

void AnimationManager::setOutputDirectory(const std::string& directory) {
    outputDirectory = directory;
}

int AnimationManager::getNumFrames() const {
    return numFrames;
}

void AnimationManager::setNumFrames(int frames) {
    if (frames > 0) {
        numFrames = frames;
    }
    else {
        LOG_ERROR << "Number of frames must be positive";
    }
}

int AnimationManager::getFrameRate() const {
    return frameRate;
}

void AnimationManager::setFrameRate(int rate) {
    if (rate > 0) {
        frameRate = rate;
    }
    else {
        LOG_ERROR << "Frame rate must be positive";
    }
}

std::string AnimationManager::getOutputFilename() const {
    return outputFilename;
}

void AnimationManager::setOutputFilename(const std::string& filename) {
    outputFilename = filename;
}

double AnimationManager::getTotalRotation() const {
    return totalRotation;
}

void AnimationManager::setTotalRotation(double radians) {
    totalRotation = radians;
}