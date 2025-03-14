#include "Homography.hpp"
#include "Logger.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

Vec2 projectPoint(const Vec3& p, double d, double cx, double cy) {
    if (p.z <= 0.1) {
        // Return a point far off-screen
        return Vec2(-10000, -10000);
    }

    // Standard perspective projection
    return Vec2(d * p.x / p.z + cx, d * p.y / p.z + cy);
}

Mat3x3 computeHomography(const std::vector<Vec2>& src, const std::vector<Vec2>& dst) {
    if (src.size() != 4 || dst.size() != 4) {
        LOG_ERROR << "Need exactly 4 corresponding points!";
        return Mat3x3();
    }

    // Check if points make a proper quadrilateral
    bool tooSmall = false;

    // Check for minimum area and size
    for (int i = 0; i < 4; i++) {
        int j = (i + 1) % 4;
        // Check if any side is too small
        double dx = dst[j].x - dst[i].x;
        double dy = dst[j].y - dst[i].y;
        if (dx * dx + dy * dy < 1.0) { // If any side is < 1 pixel long
            tooSmall = true;
            break;
        }
    }

    if (tooSmall) {
        LOG_WARNING << "Quadrilateral is too small or degenerate";
        return Mat3x3();
    }

    // Solve the system of equations Ax = b using Gaussian elimination
    // with partial pivoting for numerical stability
    double A[8][8] = { 0 };
    double b[8] = { 0 };
    double x[8] = { 0 };

    // Set up the system of equations
    for (int i = 0; i < 4; i++) {
        A[i * 2][0] = src[i].x;
        A[i * 2][1] = src[i].y;
        A[i * 2][2] = 1;
        A[i * 2][3] = 0;
        A[i * 2][4] = 0;
        A[i * 2][5] = 0;
        A[i * 2][6] = -dst[i].x * src[i].x;
        A[i * 2][7] = -dst[i].x * src[i].y;

        A[i * 2 + 1][0] = 0;
        A[i * 2 + 1][1] = 0;
        A[i * 2 + 1][2] = 0;
        A[i * 2 + 1][3] = src[i].x;
        A[i * 2 + 1][4] = src[i].y;
        A[i * 2 + 1][5] = 1;
        A[i * 2 + 1][6] = -dst[i].y * src[i].x;
        A[i * 2 + 1][7] = -dst[i].y * src[i].y;

        b[i * 2] = dst[i].x;
        b[i * 2 + 1] = dst[i].y;
    }

    // Scaling to improve numerical stability
    double maxA = 0.0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            maxA = std::max(maxA, std::abs(A[i][j]));
        }
    }

    if (maxA > 1e-8) {
        double scale = 1.0 / maxA;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                A[i][j] *= scale;
            }
            b[i] *= scale;
        }
    }

    // Gaussian elimination with partial pivoting
    for (int i = 0; i < 8; i++) {
        // Find the pivot
        int max_row = i;
        double max_val = std::abs(A[i][i]);

        for (int j = i + 1; j < 8; j++) {
            if (std::abs(A[j][i]) > max_val) {
                max_val = std::abs(A[j][i]);
                max_row = j;
            }
        }

        // Check for singularity
        if (max_val < 1e-10) {
            LOG_ERROR << "Matrix appears singular during Gaussian elimination";
            return Mat3x3(); // Return identity if system is singular
        }

        // Swap rows if needed
        if (max_row != i) {
            for (int j = i; j < 8; j++) {
                std::swap(A[i][j], A[max_row][j]);
            }
            std::swap(b[i], b[max_row]);
        }

        // Eliminate below
        for (int j = i + 1; j < 8; j++) {
            double factor = A[j][i] / A[i][i];

            for (int k = i; k < 8; k++) {
                A[j][k] -= factor * A[i][k];
            }

            b[j] -= factor * b[i];
        }
    }

    // Back substitution
    for (int i = 7; i >= 0; i--) {
        x[i] = b[i];

        for (int j = i + 1; j < 8; j++) {
            x[i] -= A[i][j] * x[j];
        }

        // Check for numerical issues
        if (std::abs(A[i][i]) < 1e-10) {
            LOG_WARNING << "Potential division by very small number during back substitution";
            return Mat3x3(); // Return identity if division would be unstable
        }

        x[i] /= A[i][i];
    }

    // Construct the homography matrix
    Mat3x3 H;
    H.m[0][0] = x[0];
    H.m[0][1] = x[1];
    H.m[0][2] = x[2];
    H.m[1][0] = x[3];
    H.m[1][1] = x[4];
    H.m[1][2] = x[5];
    H.m[2][0] = x[6];
    H.m[2][1] = x[7];
    H.m[2][2] = 1.0;

    return H;
}

bool isInsideQuad(const Vec2& p, const std::vector<Vec2>& quad) {
    if (quad.size() != 4) return false;

    // Check if point is on the same side of all edges
    bool allPositive = true;
    bool allNegative = true;

    for (int i = 0; i < 4; i++) {
        int j = (i + 1) % 4;
        double edge_x = quad[j].x - quad[i].x;
        double edge_y = quad[j].y - quad[i].y;
        double to_p_x = p.x - quad[i].x;
        double to_p_y = p.y - quad[i].y;

        double cross = edge_x * to_p_y - edge_y * to_p_x;

        // Add a small epsilon for numerical stability
        const double EPSILON = 1e-6;
        if (cross > EPSILON) allNegative = false;
        if (cross < -EPSILON) allPositive = false;

        // Break early if we know the point can't be inside
        if (!allPositive && !allNegative) return false;
    }

    return allPositive || allNegative;
}