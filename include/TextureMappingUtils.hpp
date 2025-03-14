#pragma once
#include "Image.hpp"
#include "Geometry.hpp"
#include "Homography.hpp"
#include <vector>

/**
 * Utility class for texture mapping operations
 * Implemented as static methods for simplicity
 */
class TextureMappingUtils {
public:
    /**
     * Maps a texture onto a quadrilateral in the target image
     *
     * @param targetImage The image to draw the texture onto
     * @param textureImage The texture image to map
     * @param quadVertices The four vertices of the target quadrilateral
     * @param fallbackColor Color to use if mapping fails
     */
    static void mapTextureToQuad(
        Image& targetImage,
        const Image& textureImage,
        const std::vector<Vec2>& quadVertices,
        const Color& fallbackColor = Color(150, 150, 150)
    );

    /**
     * Checks if a point is inside a quadrilateral
     * This is a wrapper around the global isInsideQuad function
     *
     * @param p The point to check
     * @param quad The quadrilateral vertices (4 points)
     * @return true if the point is inside the quadrilateral
     */
    static bool isInsideQuad(const Vec2& p, const std::vector<Vec2>& quad);
};