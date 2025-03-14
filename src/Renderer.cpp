#include "Renderer.hpp"
#include "TextureMappingUtils.hpp"
#include "Logger.hpp"
#include <algorithm>

Renderer::Renderer(int width, int height)
    : width(width), height(height), backgroundColor(10, 20, 30), decalFaceIndex(1) {

    // Set up camera at the center with appropriate scale
    camera = ViewCamera(500, width / 2.0, height / 2.0);

    // Set default face colors
    faceColors = {
        Color(100, 100, 100),   // back face
        Color(150, 150, 150),   // front face
        Color(180, 100, 100),   // bottom face
        Color(100, 180, 100),   // top face
        Color(100, 100, 180),   // left face
        Color(180, 180, 100)    // right face
    };
}

Renderer::Renderer(const ConfigManager& config) {
    configure(config);
}

void Renderer::configure(const ConfigManager& config) {
    width = config.width;
    height = config.height;
    backgroundColor = config.backgroundColor;

    // Make sure decalFaceIndex is within valid range
    decalFaceIndex = (config.decalFaceIndex >= 0 && config.decalFaceIndex < 6) ?
        config.decalFaceIndex : 1;

    // Set up camera
    camera = ViewCamera(config.cameraScale, width / 2.0, height / 2.0);

    // Copy face colors
    for (size_t i = 0; i < std::min(config.faceColors.size(), faceColors.size()); i++) {
        faceColors[i] = config.faceColors[i];
    }
}

Image Renderer::renderFrame(
    const Cube& cube,
    double angle,
    const Image* decalImage,
    const Mat4x4* rotationMatrix
) {
    // Create a copy of the cube for transformation
    Cube transformedCube = cube;

    // Set up transformation matrices
    Mat4x4 translateZ;
    translateZ.m[2][3] = 10.0;

    // Create rotation matrix based on the angle or use provided matrix
    Mat4x4 rotation;
    if (rotationMatrix) {
        rotation = *rotationMatrix;
    }
    else {
        rotation = rotateY(angle) * rotateX(angle * 0.5);
    }

    // Apply transformations
    transformedCube.transform(translateZ * rotation);

    // Create frame with background color
    Image frameImage(width, height, backgroundColor);

    // Project the cube vertices to 2D
    std::vector<Vec2> projectedVertices;
    for (const auto& v : transformedCube.vertices) {
        projectedVertices.push_back(camera.projectPoint(v));
    }

    // Calculate face visibility
    std::vector<bool> faceVisible(transformedCube.faces.size(), false);

    // Calculate which faces are visible
    for (size_t i = 0; i < transformedCube.faces.size(); i++) {
        const auto& face = transformedCube.faces[i];

        // Get three vertices of the face to calculate normal
        Vec3 v0 = transformedCube.vertices[face[0]];
        Vec3 v1 = transformedCube.vertices[face[1]];
        Vec3 v2 = transformedCube.vertices[face[2]];

        // Calculate face normal using cross product
        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;
        Vec3 normal = edge1.cross(edge2).normalize();

        // Face is visible if normal points towards camera (negative z)
        double dotProduct = -normal.z;

        // Threshold for visibility
        faceVisible[i] = (dotProduct > 0.001);
    }

    // Check if decal face is visible (with proper bounds checking)
    const size_t numFaces = transformedCube.faces.size();
    const size_t safeDecalFaceIndex = static_cast<size_t>(decalFaceIndex);
    bool decalFaceVisible = (safeDecalFaceIndex < numFaces &&
        faceVisible[safeDecalFaceIndex] &&
        decalImage != nullptr);

    // Sort faces by z-depth for correct rendering order (back-to-front)
    std::vector<size_t> faceIndices(numFaces);
    for (size_t i = 0; i < faceIndices.size(); i++) {
        faceIndices[i] = i;
    }

    std::sort(faceIndices.begin(), faceIndices.end(), [&](size_t a, size_t b) {
        const auto& faceA = transformedCube.faces[a];
        const auto& faceB = transformedCube.faces[b];

        // Calculate center Z consistently
        Vec3 centerA(0, 0, 0);
        Vec3 centerB(0, 0, 0);

        for (int idx : faceA) centerA = centerA + transformedCube.vertices[idx];
        for (int idx : faceB) centerB = centerB + transformedCube.vertices[idx];

        centerA = centerA * (1.0 / faceA.size());
        centerB = centerB * (1.0 / faceB.size());

        const double EPSILON = 1e-10;
        if (std::abs(centerA.z - centerB.z) < EPSILON)
            return a < b;

        return centerA.z > centerB.z; // Render back-to-front
        });

    // Draw each visible face
    for (size_t idx : faceIndices) {
        if (!faceVisible[idx]) continue;

        const auto& face = transformedCube.faces[idx];

        // Project face vertices
        std::vector<Vec2> quadVertices;
        for (int vertIdx : face) {
            quadVertices.push_back(projectedVertices[vertIdx]);
        }

        // Apply decal texture to specified face if needed (with proper bounds checking)
        if (idx == safeDecalFaceIndex && decalFaceVisible) {
            TextureMappingUtils::mapTextureToQuad(
                frameImage,
                *decalImage,
                quadVertices,
                faceColors[idx]
            );
        }
        else {
            // Fill with solid color using triangulation
            for (size_t i = 0; i < face.size() - 2; i++) {
                frameImage.fillTriangle(
                    static_cast<int>(quadVertices[0].x), static_cast<int>(quadVertices[0].y),
                    static_cast<int>(quadVertices[i + 1].x), static_cast<int>(quadVertices[i + 1].y),
                    static_cast<int>(quadVertices[i + 2].x), static_cast<int>(quadVertices[i + 2].y),
                    faceColors[idx]
                );
            }
        }

        // Draw face outlines
        for (size_t i = 0; i < quadVertices.size(); i++) {
            size_t j = (i + 1) % quadVertices.size();
            frameImage.drawLine(
                static_cast<int>(quadVertices[i].x), static_cast<int>(quadVertices[i].y),
                static_cast<int>(quadVertices[j].x), static_cast<int>(quadVertices[j].y),
                Color(255, 255, 255) // White outlines
            );
        }
    }

    return frameImage;
}

void Renderer::setBackgroundColor(const Color& color) {
    backgroundColor = color;
}

Color Renderer::getBackgroundColor() const {
    return backgroundColor;
}

void Renderer::setFaceColor(int faceIndex, const Color& color) {
    if (faceIndex >= 0 && faceIndex < static_cast<int>(faceColors.size())) {
        faceColors[faceIndex] = color;
    }
    else {
        LOG_ERROR << "Invalid face index: " << faceIndex;
    }
}

void Renderer::setDecalFaceIndex(int index) {
    if (index >= 0 && index < static_cast<int>(faceColors.size())) {
        decalFaceIndex = index;
    }
    else {
        LOG_ERROR << "Invalid decal face index: " << index;
        decalFaceIndex = 1;
    }
}

int Renderer::getDecalFaceIndex() const {
    return decalFaceIndex;
}

ViewCamera& Renderer::getCamera() {
    return camera;
}

int Renderer::getWidth() const {
    return width;
}

int Renderer::getHeight() const {
    return height;
}