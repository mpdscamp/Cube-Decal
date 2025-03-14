#include "Matrix.hpp"
#include "Logger.hpp"
#include <iostream>
#include <cmath>

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