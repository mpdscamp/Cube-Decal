#include "ViewCamera.hpp"

ViewCamera::ViewCamera(double scale, double x, double y)
    : scale(scale), centerX(x), centerY(y) {
}

Vec2 ViewCamera::projectPoint(const Vec3& point) const {
    if (point.z <= 0.1) {
        // Return a point far off-screen for points behind camera
        return Vec2(-10000, -10000);
    }

    // Standard perspective projection
    return Vec2(scale * point.x / point.z + centerX,
        scale * point.y / point.z + centerY);
}

double ViewCamera::getScale() const {
    return scale;
}

void ViewCamera::setScale(double s) {
    scale = s;
}

double ViewCamera::getCenterX() const {
    return centerX;
}

void ViewCamera::setCenterX(double x) {
    centerX = x;
}

double ViewCamera::getCenterY() const {
    return centerY;
}

void ViewCamera::setCenterY(double y) {
    centerY = y;
}