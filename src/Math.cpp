#include "Math.hpp"
#include "Logger.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

// Vec2 Implementation
Vec2::Vec2(double x, double y) : x(x), y(y) {}

// Vec3 Implementation
Vec3::Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

Vec3 Vec3::operator+(const Vec3& v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator-(const Vec3& v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator*(double s) const {
    return Vec3(x * s, y * s, z * s);
}

double Vec3::dot(const Vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

Vec3 Vec3::cross(const Vec3& v) const {
    return Vec3(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    );
}

double Vec3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::normalize() const {
    double len = length();
    if (len > 0) {
        return Vec3(x / len, y / len, z / len);
    }
    return *this;
}

// Mat3x3 Implementation
Mat3x3::Mat3x3() {
    // Initialize as identity matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
}

Vec3 Mat3x3::operator*(const Vec3& v) const {
    return Vec3(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
    );
}

Mat3x3 Mat3x3::inverse() const {
    // Calculate determinant
    double det =
        m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    const double EPSILON = 1e-10;
    if (std::abs(det) < EPSILON) {
        LOG_ERROR << "Matrix is singular, can't invert! Determinant: " << det;
        Mat3x3 identity;
        // Add a small stabilizing factor to avoid pure zeros
        identity.m[0][0] = 1.0001;
        identity.m[1][1] = 1.0001;
        identity.m[2][2] = 1.0001;
        return identity;
    }

    double invDet = 1.0 / det;

    Mat3x3 inv;
    inv.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
    inv.m[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invDet;
    inv.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
    inv.m[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invDet;
    inv.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
    inv.m[1][2] = (m[0][2] * m[1][0] - m[0][0] * m[1][2]) * invDet;
    inv.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;
    inv.m[2][1] = (m[0][1] * m[2][0] - m[0][0] * m[2][1]) * invDet;
    inv.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;

    // Check for NaNs or infinities in the result
    bool hasInvalid = false;
    for (int i = 0; i < 3 && !hasInvalid; i++) {
        for (int j = 0; j < 3; j++) {
            if (std::isnan(inv.m[i][j]) || std::isinf(inv.m[i][j])) {
                hasInvalid = true;
                LOG_ERROR << "Warning: Invalid value in inverse matrix at [" << i << "][" << j << "]: " << inv.m[i][j];
                break;
            }
        }
    }

    if (hasInvalid) {
        // Return identity as a fallback
        Mat3x3 identity;
        identity.m[0][0] = 1.0001;
        identity.m[1][1] = 1.0001;
        identity.m[2][2] = 1.0001;
        return identity;
    }

    return inv;
}

// Mat4x4 Implementation
Mat4x4::Mat4x4() {
    // Initialize as identity matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
}

Mat4x4 Mat4x4::operator*(const Mat4x4& other) const {
    Mat4x4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

Vec3 Mat4x4::transform(const Vec3& v) const {
    double x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
    double y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
    double z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];
    double w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];

    if (std::abs(w) < 1e-10) {
        // Avoid division by very small w
        LOG_WARNING << "Very small w component in 3D transformation";
        w = 1e-10; // Use a small non-zero value instead of zero
    }
    return Vec3(x / w, y / w, z / w);
}

// Rotation matrices
Mat4x4 rotateX(double angle) {
    double c = cos(angle);
    double s = sin(angle);

    Mat4x4 rot;
    rot.m[1][1] = c;
    rot.m[1][2] = -s;
    rot.m[2][1] = s;
    rot.m[2][2] = c;

    return rot;
}

Mat4x4 rotateY(double angle) {
    double c = cos(angle);
    double s = sin(angle);

    Mat4x4 rot;
    rot.m[0][0] = c;
    rot.m[0][2] = s;
    rot.m[2][0] = -s;
    rot.m[2][2] = c;

    return rot;
}

Mat4x4 rotateZ(double angle) {
    double c = cos(angle);
    double s = sin(angle);

    Mat4x4 rot;
    rot.m[0][0] = c;
    rot.m[0][1] = -s;
    rot.m[1][0] = s;
    rot.m[1][1] = c;

    return rot;
}

// Projection and Homography functions
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