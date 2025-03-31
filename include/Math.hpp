#pragma once

#include <cmath>
#include <vector>

/**
 * 2D Point/Vector representation
 */
class Vec2 {
public:
    double x, y;

    /**
     * Constructor for creating a 2D vector
     *
     * @param x X coordinate
     * @param y Y coordinate
     */
    Vec2(double x = 0, double y = 0);
};

/**
 * 3D Vector/Point representation with common vector operations
 */
class Vec3 {
public:
    double x, y, z;

    /**
     * Constructor for creating a 3D vector
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    Vec3(double x = 0, double y = 0, double z = 0);

    /**
     * Vector addition
     *
     * @param v Vector to add
     * @return Result of the addition
     */
    Vec3 operator+(const Vec3& v) const;

    /**
     * Vector subtraction
     *
     * @param v Vector to subtract
     * @return Result of the subtraction
     */
    Vec3 operator-(const Vec3& v) const;

    /**
     * Scalar multiplication
     *
     * @param s Scalar value
     * @return Scaled vector
     */
    Vec3 operator*(double s) const;

    /**
     * Dot product calculation
     *
     * @param v Second vector
     * @return Dot product result
     */
    double dot(const Vec3& v) const;

    /**
     * Cross product calculation
     *
     * @param v Second vector
     * @return Cross product result
     */
    Vec3 cross(const Vec3& v) const;

    /**
     * Calculate vector length
     *
     * @return The length of the vector
     */
    double length() const;

    /**
     * Normalize the vector
     *
     * @return Normalized vector with length 1
     */
    Vec3 normalize() const;
};

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

/**
 * Project a 3D point onto a 2D plane using Euclidean camera projection
 *
 * @param p 3D point to project
 * @param d Distance to projection plane
 * @param cx X coordinate of center point
 * @param cy Y coordinate of center point
 * @return Projected 2D point
 */
Vec2 projectPoint(const Vec3& p, double d, double cx, double cy);

/**
 * Compute homography matrix from four point correspondences
 *
 * @param src Source points in 2D
 * @param dst Destination points in 2D
 * @return Homography matrix (3x3)
 */
Mat3x3 computeHomography(const std::vector<Vec2>& src, const std::vector<Vec2>& dst);

/**
 * Check if a point is inside a convex quadrilateral
 *
 * @param p Point to check
 * @param quad Vector of four points defining the quadrilateral
 * @return True if the point is inside the quadrilateral
 */
bool isInsideQuad(const Vec2& p, const std::vector<Vec2>& quad);