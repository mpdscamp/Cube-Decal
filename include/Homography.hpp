#pragma once

#include <vector>
#include "Geometry.hpp"
#include "Matrix.hpp"

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