#pragma once

#include "Geometry.hpp"

/**
 * 3x3 Matrix for homography and 2D transformations
 */
class Mat3x3 {
public:
    double m[3][3];

    /**
     * Default constructor creates an identity matrix
     */
    Mat3x3();

    /**
     * Multiply matrix by a 3D vector
     *
     * @param v Vector to transform
     * @return Transformed vector
     */
    Vec3 operator*(const Vec3& v) const;

    /**
     * Calculate the inverse of the matrix
     *
     * @return Inverse matrix
     */
    Mat3x3 inverse() const;
};

/**
 * 4x4 Matrix for 3D transformations
 */
class Mat4x4 {
public:
    double m[4][4];

    /**
     * Default constructor creates an identity matrix
     */
    Mat4x4();

    /**
     * Matrix multiplication
     *
     * @param other Right-hand matrix
     * @return Result of multiplication
     */
    Mat4x4 operator*(const Mat4x4& other) const;

    /**
     * Transform a 3D vector using this matrix
     *
     * @param v Vector to transform
     * @return Transformed vector
     */
    Vec3 transform(const Vec3& v) const;
};

/**
 * Create rotation matrix around X axis
 *
 * @param angle Rotation angle in radians
 * @return Rotation matrix
 */
Mat4x4 rotateX(double angle);

/**
 * Create rotation matrix around Y axis
 *
 * @param angle Rotation angle in radians
 * @return Rotation matrix
 */
Mat4x4 rotateY(double angle);

/**
 * Create rotation matrix around Z axis
 *
 * @param angle Rotation angle in radians
 * @return Rotation matrix
 */
Mat4x4 rotateZ(double angle);