#pragma once

#include <cmath>

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