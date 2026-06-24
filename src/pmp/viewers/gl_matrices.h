// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include "pmp/types.h"
#include <numbers>
#include <cmath>

namespace pmp {

//! OpenGL perspective projection matrix
inline Eigen::Projective3f perspective_matrix(float fovy, float aspect, float zNear,
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
    return Eigen::Projective3f(m);
}

//! OpenGL orthographic projection matrix
inline Eigen::Projective3f ortho_matrix(float left, float right, float bottom,
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
    return Eigen::Projective3f(m);
}


} // namespace pmp
