#include "TextureMappingUtils.hpp"
#include "Logger.hpp"
#include <algorithm>

void TextureMappingUtils::mapTextureToQuad(
    Image& targetImage,
    const Image& textureImage,
    const std::vector<Vec2>& quadVertices,
    const Color& fallbackColor
) {
    if (quadVertices.size() != 4) {
        LOG_ERROR << "Texture mapping requires exactly 4 vertices";
        return;
    }

    // Define the corners of the texture in texture space
    std::vector<Vec2> textureCorners = {
        Vec2(0, 0),                                                      // Top-left
        Vec2(textureImage.getWidth() - 1, 0),                            // Top-right
        Vec2(textureImage.getWidth() - 1, textureImage.getHeight() - 1), // Bottom-right
        Vec2(0, textureImage.getHeight() - 1)                            // Bottom-left
    };

    // Compute homography from texture to quad
    Mat3x3 H = computeHomography(textureCorners, quadVertices);
    Mat3x3 Hinv = H.inverse();

    // Check if homography is valid
    bool validHomography = true;
    for (int i = 0; i < 3 && validHomography; i++) {
        for (int j = 0; j < 3; j++) {
            if (std::isnan(Hinv.m[i][j]) || std::isinf(Hinv.m[i][j])) {
                validHomography = false;
                break;
            }
        }
    }

    if (!validHomography) {
        LOG_WARNING << "Invalid homography for texture mapping, using fallback";

        // Fill with solid color using triangulation
        targetImage.fillTriangle(
            static_cast<int>(quadVertices[0].x), static_cast<int>(quadVertices[0].y),
            static_cast<int>(quadVertices[1].x), static_cast<int>(quadVertices[1].y),
            static_cast<int>(quadVertices[2].x), static_cast<int>(quadVertices[2].y),
            fallbackColor
        );

        targetImage.fillTriangle(
            static_cast<int>(quadVertices[0].x), static_cast<int>(quadVertices[0].y),
            static_cast<int>(quadVertices[2].x), static_cast<int>(quadVertices[2].y),
            static_cast<int>(quadVertices[3].x), static_cast<int>(quadVertices[3].y),
            fallbackColor
        );

        return;
    }

    // Find bounding box of the quad
    int minX = targetImage.getWidth(), minY = targetImage.getHeight();
    int maxX = 0, maxY = 0;

    for (const auto& v : quadVertices) {
        minX = std::min(minX, static_cast<int>(v.x));
        minY = std::min(minY, static_cast<int>(v.y));
        maxX = std::max(maxX, static_cast<int>(v.x));
        maxY = std::max(maxY, static_cast<int>(v.y));
    }

    // Clamp to image bounds
    minX = std::max(0, minX);
    minY = std::max(0, minY);
    maxX = std::min(targetImage.getWidth() - 1, maxX);
    maxY = std::min(targetImage.getHeight() - 1, maxY);

    // For each pixel in the bounding box
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            // Check if point is inside the quad
            Vec2 p(x, y);
            if (isInsideQuad(p, quadVertices)) {
                // Apply inverse homography
                Vec3 p_hom = Hinv * Vec3(p.x, p.y, 1.0);
                if (std::abs(p_hom.z) > 1e-8) {
                    p_hom.x /= p_hom.z;
                    p_hom.y /= p_hom.z;
                }

                // Check if point is within texture bounds
                if (p_hom.x >= 0 && p_hom.x < textureImage.getWidth() &&
                    p_hom.y >= 0 && p_hom.y < textureImage.getHeight()) {
                    // Bilinear interpolation
                    int x0 = static_cast<int>(p_hom.x);
                    int y0 = static_cast<int>(p_hom.y);
                    int x1 = std::min(x0 + 1, textureImage.getWidth() - 1);
                    int y1 = std::min(y0 + 1, textureImage.getHeight() - 1);

                    double fx = p_hom.x - x0;
                    double fy = p_hom.y - y0;

                    Color c00 = textureImage.getPixel(x0, y0);
                    Color c10 = textureImage.getPixel(x1, y0);
                    Color c01 = textureImage.getPixel(x0, y1);
                    Color c11 = textureImage.getPixel(x1, y1);

                    unsigned char r = static_cast<unsigned char>(
                        (1 - fx) * (1 - fy) * c00.r + fx * (1 - fy) * c10.r +
                        (1 - fx) * fy * c01.r + fx * fy * c11.r);
                    unsigned char g = static_cast<unsigned char>(
                        (1 - fx) * (1 - fy) * c00.g + fx * (1 - fy) * c10.g +
                        (1 - fx) * fy * c01.g + fx * fy * c11.g);
                    unsigned char b = static_cast<unsigned char>(
                        (1 - fx) * (1 - fy) * c00.b + fx * (1 - fy) * c10.b +
                        (1 - fx) * fy * c01.b + fx * fy * c11.b);

                    targetImage.setPixel(x, y, Color(r, g, b));
                }
                else {
                    // Outside texture bounds, use fallback color
                    targetImage.setPixel(x, y, fallbackColor);
                }
            }
        }
    }
}

bool TextureMappingUtils::isInsideQuad(const Vec2& p, const std::vector<Vec2>& quad) {
    // Defer to the existing implementation from global scope
    return ::isInsideQuad(p, quad);
}