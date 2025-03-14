#include "Cube.hpp"

Cube::Cube(double size) {
    // Define the 8 vertices of the cube
    double h = size / 2.0;
    vertices = {
        Vec3(-h, -h, -h), // 0: bottom-left-back
        Vec3(h, -h, -h),  // 1: bottom-right-back
        Vec3(h, h, -h),   // 2: top-right-back
        Vec3(-h, h, -h),  // 3: top-left-back
        Vec3(-h, -h, h),  // 4: bottom-left-front
        Vec3(h, -h, h),   // 5: bottom-right-front
        Vec3(h, h, h),    // 6: top-right-front
        Vec3(-h, h, h)    // 7: top-left-front
    };

    // Define the 6 faces of the cube (each face is a list of vertex indices)
    faces = {
        {4, 5, 6, 7}, // back face
        {0, 3, 2, 1}, // front face
        {0, 1, 5, 4}, // bottom face
        {2, 3, 7, 6}, // top face
        {0, 4, 7, 3}, // left face
        {1, 2, 6, 5}  // right face
    };
}

void Cube::transform(const Mat4x4& matrix) {
    for (auto& vertex : vertices) {
        vertex = matrix.transform(vertex);
    }
}