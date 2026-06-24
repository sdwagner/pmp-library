// Copyright 2023 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

// TODO: is not really relevant anymore

#include <pmp/types.h>

// clang-format off
int main()
{
{
//! [eigen-point]
// pmp::Point is vec3 — use Eigen API directly
pmp::Point p(1.0f, 2.0f, 3.0f);
float len = p.norm();
pmp::Point normalized = p.normalized();
//! [eigen-point]
(void)len;
(void)normalized;
}

{
//! [eigen-interop]
// pmp types are Eigen types — no conversion needed
pmp::vec3 eigen_vec(1.0f, 2.0f, 3.0f);
pmp::Point pmp_point = eigen_vec.cast<pmp::Scalar>();
pmp::vec3 back = pmp_point.cast<float>();
//! [eigen-interop]
(void)back;
}
}
// clang-format on
