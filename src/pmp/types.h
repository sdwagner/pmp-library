// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>

#include <Eigen/Dense>

//! The pmp-library namespace
namespace pmp {

//! \addtogroup core
//! @{

//! Scalar type
#ifdef PMP_SCALAR_TYPE_64
using Scalar = double;
#else
using Scalar = float;
#endif

// define index type to be used
#ifdef PMP_INDEX_TYPE_64
using IndexType = std::uint_least64_t;
#define PMP_MAX_INDEX UINT_LEAST64_MAX
#else
using IndexType = std::uint_least32_t;
#define PMP_MAX_INDEX UINT_LEAST32_MAX
#endif

//! @}

//! \defgroup core core
//! \brief Core data structure and utilities.
//! \details The central class of the library is SurfaceMesh: \copybrief SurfaceMesh
//!
//! The Matrix class provides basic matrix and vector operations.
//!
//! Utility classes:
//!
//!   * BoundingBox: \copybrief BoundingBox
//!   * StopWatch: \copybrief StopWatch
//!   * MemoryUsage: \copybrief MemoryUsage

//! \defgroup algorithms algorithms
//! \brief Mesh processing algorithms.

//! \defgroup viewers viewers
//! \brief Viewers using OpenGL.

//! \defgroup io io
//! Read and write common mesh formats.

} // namespace pmp

namespace Eigen {
    typedef Matrix<pmp::Scalar, Dynamic, Dynamic> MatrixXs;
    typedef Matrix<pmp::Scalar, Dynamic, 1> VectorXs;
    typedef Matrix<pmp::Scalar, 1, 1> Vector1s;
    typedef Matrix<pmp::Scalar, 2, 1> Vector2s;
    typedef Matrix<pmp::Scalar, 3, 1> Vector3s;
    typedef Matrix<pmp::Scalar, 4, 1> Vector4s;
    typedef Matrix<pmp::Scalar, 2, 2> Matrix2s;
    typedef Matrix<pmp::Scalar, 3, 3> Matrix3s;
    typedef Matrix<pmp::Scalar, 4, 4> Matrix4s;
    typedef Transform<pmp::Scalar, 2, Isometry> Isometry2s;
    typedef Transform<pmp::Scalar, 3, Isometry> Isometry3s;
    typedef Transform<pmp::Scalar, 3, Affine> Affine3s;
    typedef Quaternion<pmp::Scalar> Quaternion_s;
    typedef AngleAxis<pmp::Scalar> AngleAxis_s;
    typedef Translation<pmp::Scalar, 3> Translation3s;
}

namespace pmp {

    //! Point type
    using Point = Eigen::Vector3s;

    //! Normal type
    using Normal = Eigen::Vector3s;

    //! Color type
    //! \details RGB values in the range of [0,1]
    using Color = Eigen::Vector3s;

    //! Texture coordinate type
    using TexCoord = Eigen::Vector2s;

    //! template specialization for a vector of two float values
    using vec2 = Eigen::Vector2f;
    //! template specialization for a vector of two double values
    using dvec2 = Eigen::Vector2d;
    //! template specialization for a vector of two bool values
    using bvec2 = Eigen::Vector2<bool>;
    //! template specialization for a vector of two int values
    using ivec2 = Eigen::Vector2i;
    //! template specialization for a vector of two unsigned int values
    using uvec2 = Eigen::Vector2<unsigned int>;

    //! template specialization for a vector of three float values
    using vec3 = Eigen::Vector3f;
    //! template specialization for a vector of three double values
    using dvec3 = Eigen::Vector3d;
    //! template specialization for a vector of three bool values
    using bvec3 = Eigen::Vector3<bool>;
    //! template specialization for a vector of three int values
    using ivec3 = Eigen::Vector3i;
    //! template specialization for a vector of three unsigned int values
    using uvec3 = Eigen::Vector3<unsigned int>;

    //! template specialization for a vector of four float values
    using vec4 = Eigen::Vector4f;
    //! template specialization for a vector of four double values
    using dvec4 = Eigen::Vector4d;
    //! template specialization for a vector of four bool values
    using bvec4 = Eigen::Vector4<bool>;
    //! template specialization for a vector of four int values
    using ivec4 = Eigen::Vector4i;
    //! template specialization for a vector of four unsigned int values
    using uvec4 = Eigen::Vector4<unsigned int>;

    //! template specialization for a 2x2 matrix of float values
    using mat2 = Eigen::Matrix2f;
    //! template specialization for a 2x2 matrix of double values
    using dmat2 = Eigen::Matrix2d;
    //! template specialization for a 3x3 matrix of float values
    using mat3 = Eigen::Matrix3f;
    //! template specialization for a 3x3 matrix of double values
    using dmat3 = Eigen::Matrix3d;
    //! template specialization for a 4x4 matrix of float values
    using mat4 = Eigen::Matrix4f;
    //! template specialization for a 4x4 matrix of double values
    using dmat4 = Eigen::Matrix4d;
}