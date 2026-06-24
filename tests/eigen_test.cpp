
// Copyright 2017-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

// TODO: is not really relevant anymore
#include "gtest/gtest.h"

#include <pmp/surface_mesh.h>
#include <vector>

using namespace pmp;

TEST(EigenTest, point_is_eigen_vector3f)
{
    Point p(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(p[0], 1.0f);
    EXPECT_EQ(p[1], 2.0f);
    EXPECT_EQ(p[2], 3.0f);
    EXPECT_FLOAT_EQ(p.norm(), std::sqrt(14.0f));
}

TEST(EigenTest, point_eigen_operations)
{
    Point a(1.0f, 0.0f, 0.0f);
    Point b(0.0f, 1.0f, 0.0f);
    EXPECT_FLOAT_EQ(a.dot(b), 0.0f);
    Point c = a.cross(b);
    EXPECT_FLOAT_EQ(c[2], 1.0f);
    EXPECT_FLOAT_EQ((a - b).norm(), std::sqrt(2.0f));
}

TEST(EigenTest, normal_is_eigen_vector3f)
{
    Normal n(0.0f, 0.0f, 1.0f);
    EXPECT_FLOAT_EQ(n.norm(), 1.0f);
    EXPECT_EQ(n, n.normalized());
}

TEST(EigenTest, texcoord_is_eigen_vector2f)
{
    TexCoord t(0.5f, 0.5f);
    EXPECT_EQ(t[0], 0.5f);
    EXPECT_EQ(t[1], 0.5f);
}

TEST(EigenTest, assign_from_eigen)
{
    vec3 ev(1.0f, 2.0f, 3.0f);
    Point p = ev.cast<Scalar>();
    EXPECT_EQ(p[1], 2.0f);
}
