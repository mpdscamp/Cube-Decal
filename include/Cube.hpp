#pragma once

#include <vector>
#include "Geometry.hpp"
#include "Matrix.hpp"

/**
 * 3D Cube representation with vertices and faces
 */
class Cube {
public:
    std::vector<Vec3> vertices;
    std::vector<std::vector<int>> faces;

    /**
     * Constructor for creating a cube
     *
     * @param size The side length of the cube
     */
    Cube(double size = 1.0);

    /**
     * Apply a 4x4 transformation matrix to all vertices
     *
     * @param matrix The transformation matrix to apply
     */
    void transform(const Mat4x4& matrix);
};
