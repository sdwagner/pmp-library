// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include "pmp/types.h"
#include <numbers>
#include <cmath>

namespace pmp {

//! OpenGL perspective projection matrix
inline mat4 perspective_matrix(float fovy, float aspect, float zNear,
                                          float zFar)
{
    const float t = zNear * std::tan(fovy * (float)std::numbers::pi / 360.0f);
    const float b = -t;
    const float l = b * aspect;
    const float r = t * aspect;
    const float nn = zNear + zNear;
    const float fn = std::fabs(zFar - zNear);

    mat4 m = mat4::Zero();
    m(0, 0) = nn / (r - l);
    m(0, 2) = (r + l) / (r - l);
    m(1, 1) = nn / (t - b);
    m(1, 2) = (t + b) / (t - b);
    m(2, 2) = -(zFar + zNear) / fn;
    m(2, 3) = -zFar * nn / fn;
    m(3, 2) = -1.0f;
    return m;
}

//! OpenGL orthographic projection matrix
inline mat4 ortho_matrix(float left, float right, float bottom,
                                    float top, float zNear = -1.0f,
                                    float zFar = 1.0f)
{
    mat4 m = mat4::Zero();
    m(0, 0) = 2.0f / (right - left);
    m(1, 1) = 2.0f / (top - bottom);
    m(2, 2) = -2.0f / (zFar - zNear);
    m(0, 3) = -(right + left) / (right - left);
    m(1, 3) = -(top + bottom) / (top - bottom);
    m(2, 3) = -(zFar + zNear) / (zFar - zNear);
    m(3, 3) = 1.0f;
    return m;
}

//! OpenGL translation matrix
inline mat4 translation_matrix(const vec3& t)
{
    mat4 m = mat4::Identity();
    m(0, 3) = t[0];
    m(1, 3) = t[1];
    m(2, 3) = t[2];
    return m;
}

//! OpenGL rotation matrix (axis-angle, angle in degrees)
inline mat4 rotation_matrix(const vec3& axis, float angle)
{
    mat4 m = mat4::Identity();
    m.topLeftCorner<3, 3>() =
        Eigen::AngleAxisf(angle * (float)std::numbers::pi / 180.0f,
                          axis.normalized())
            .toRotationMatrix();
    return m;
}

} // namespace pmp
