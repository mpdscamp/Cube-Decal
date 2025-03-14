#pragma once
#include "Geometry.hpp"

/**
 * Handles 3D to 2D projection for rendering
 */
class ViewCamera {
private:
    double scale;      // Projection scale factor
    double centerX;    // Screen center X
    double centerY;    // Screen center Y

public:
    /**
     * Constructor with default values for camera parameters
     */
    ViewCamera(double scale = 500.0, double x = 0.0, double y = 0.0);

    /**
     * Projects a 3D point to 2D screen coordinates
     */
    Vec2 projectPoint(const Vec3& point) const;

    // Getters/setters
    double getScale() const;
    void setScale(double s);

    double getCenterX() const;
    void setCenterX(double x);

    double getCenterY() const;
    void setCenterY(double y);
};