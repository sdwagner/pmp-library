// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#pragma once

#include <cassert>
#include <cstddef>
#include <compare>
#include <filesystem>
#include <iterator>
#include <ostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "pmp/types.h"
#include "pmp/properties.h"
#include "pmp/exceptions.h"
#include "pmp/surface_mesh.h"

namespace pmp {

struct IOFlags;

//! \addtogroup core
//!@{

//! \brief A class for representing polygon surface meshes.
//! \details This class implements a half-edge data structure for surface meshes.
//! See \cite sieger_2011_design for details on the design and implementation.
//! \note This class only supports 2-manifold surface meshes with boundary.
class NonManifoldSurfaceMesh
{
public:
    //! \name Iterator Types
    //!@{

    //! An iterator class to iterate linearly over all vertices
    class VertexIterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using reference = Vertex&;
        using pointer = Vertex*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! Default constructor
        VertexIterator(Vertex v = Vertex(), const NonManifoldSurfaceMesh* m = nullptr)
            : handle_(v), mesh_(m)
        {
            if (mesh_ && mesh_->has_garbage())
                while (mesh_->is_valid(handle_) && mesh_->is_deleted(handle_))
                    ++handle_.idx_;
        }

        //! get the vertex the iterator refers to
        Vertex operator*() const { return handle_; }

        //! Three-way comparison operator.
        auto operator<=>(const VertexIterator& rhs) const = default;

        //! pre-increment iterator
        VertexIterator& operator++()
        {
            ++handle_.idx_;
            assert(mesh_);
            while (mesh_->has_garbage() && mesh_->is_valid(handle_) &&
                   mesh_->is_deleted(handle_))
                ++handle_.idx_;
            return *this;
        }

        //! post-increment iterator
        VertexIterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement iterator
        VertexIterator& operator--()
        {
            --handle_.idx_;
            assert(mesh_);
            while (mesh_->has_garbage() && mesh_->is_valid(handle_) &&
                   mesh_->is_deleted(handle_))
                --handle_.idx_;
            return *this;
        }

        //! post-decrement iterator
        VertexIterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

    private:
        Vertex handle_;
        const NonManifoldSurfaceMesh* mesh_;
    };

    //! this class iterates linearly over all halfedges
    //! \sa halfedges_begin(), halfedges_end()
    //! \sa VertexIterator, EdgeIterator, FaceIterator
    class HalfedgeIterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Halfedge;
        using reference = Halfedge&;
        using pointer = Halfedge*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! Default constructor
        HalfedgeIterator(Halfedge h = Halfedge(),
                         const NonManifoldSurfaceMesh* mesh = nullptr)
            : handle_(h), mesh_(mesh)
        {
            if (mesh_ && mesh_->has_garbage())
                while (mesh_->is_valid(handle_) && mesh_->is_deleted(handle_))
                    ++handle_.idx_;
        }

        //! get the halfedge the iterator refers to
        Halfedge operator*() const { return handle_; }

        //! Three-way comparison operator.
        auto operator<=>(const HalfedgeIterator& rhs) const = default;

        //! pre-increment iterator
        HalfedgeIterator& operator++()
        {
            ++handle_.idx_;
            assert(mesh_);
            while (mesh_->has_garbage() && mesh_->is_valid(handle_) &&
                   mesh_->is_deleted(handle_))
                ++handle_.idx_;
            return *this;
        }

        //! post-increment iterator
        HalfedgeIterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement iterator
        HalfedgeIterator& operator--()
        {
            --handle_.idx_;
            assert(mesh_);
            while (mesh_->has_garbage() && mesh_->is_valid(handle_) &&
                   mesh_->is_deleted(handle_))
                --handle_.idx_;
            return *this;
        }

        //! post-decrement iterator
        HalfedgeIterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

    private:
        Halfedge handle_;
        const NonManifoldSurfaceMesh* mesh_;
    };

    //! this class iterates linearly over all edges
    //! \sa edges_begin(), edges_end()
    //! \sa VertexIterator, HalfedgeIterator, FaceIterator
    class EdgeIterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Edge;
        using reference = Edge&;
        using pointer = Edge*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! Default constructor
        EdgeIterator(Edge e = Edge(), const NonManifoldSurfaceMesh* mesh = nullptr)
            : handle_(e), mesh_(mesh)
        {
            if (mesh_ && mesh_->has_garbage())
                while (mesh_->is_valid(handle_) && mesh_->is_deleted(handle_))
                    ++handle_.idx_;
        }

        //! get the edge the iterator refers to
        Edge operator*() const { return handle_; }

        //! Three-way comparison operator.
        auto operator<=>(const EdgeIterator& rhs) const = default;

        //! pre-increment iterator
        EdgeIterator& operator++()
        {
            ++handle_.idx_;
            assert(mesh_);
            while (mesh_->has_garbage() && mesh_->is_valid(handle_) &&
                   mesh_->is_deleted(handle_))
                ++handle_.idx_;
            return *this;
        }

        //! post-increment iterator
        EdgeIterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement iterator
        EdgeIterator& operator--()
        {
            --handle_.idx_;
            assert(mesh_);
            while (mesh_->has_garbage() && mesh_->is_valid(handle_) &&
                   mesh_->is_deleted(handle_))
                --handle_.idx_;
            return *this;
        }

        //! post-decrement iterator
        EdgeIterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

    private:
        Edge handle_;
        const NonManifoldSurfaceMesh* mesh_;
    };

    //! this class iterates linearly over all faces
    //! \sa faces_begin(), faces_end()
    //! \sa VertexIterator, HalfedgeIterator, EdgeIterator
    class FaceIterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Face;
        using reference = Face&;
        using pointer = Face*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! Default constructor
        FaceIterator(Face f = Face(), const NonManifoldSurfaceMesh* m = nullptr)
            : handle_(f), mesh_(m)
        {
            if (mesh_ && mesh_->has_garbage())
                while (mesh_->is_valid(handle_) && mesh_->is_deleted(handle_))
                    ++handle_.idx_;
        }

        //! get the face the iterator refers to
        Face operator*() const { return handle_; }

        //! Three-way comparison operator
        auto operator<=>(const FaceIterator& rhs) const = default;

        //! pre-increment iterator
        FaceIterator& operator++()
        {
            ++handle_.idx_;
            assert(mesh_);
            while (mesh_->has_garbage() && mesh_->is_valid(handle_) &&
                   mesh_->is_deleted(handle_))
                ++handle_.idx_;
            return *this;
        }

        //! post-increment iterator
        FaceIterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement iterator
        FaceIterator& operator--()
        {
            --handle_.idx_;
            assert(mesh_);
            while (mesh_->has_garbage() && mesh_->is_valid(handle_) &&
                   mesh_->is_deleted(handle_))
                --handle_.idx_;
            return *this;
        }

        //! post-decrement iterator
        FaceIterator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

    private:
        Face handle_;
        const NonManifoldSurfaceMesh* mesh_;
    };

    //!@}
    //! \name Container Types
    //!@{

    //! helper class for iterating through all vertices using range-based
    //! for-loops.
    class VertexContainer
    {
    public:
        VertexContainer(VertexIterator begin, VertexIterator end)
            : begin_(begin), end_(end)
        {
        }
        VertexIterator begin() const { return begin_; }
        VertexIterator end() const { return end_; }

    private:
        VertexIterator begin_;
        VertexIterator end_;
    };

    //! helper class for iterating through all halfedges using range-based
    //! for-loops. \sa halfedges()
    class HalfedgeContainer
    {
    public:
        HalfedgeContainer(HalfedgeIterator begin, HalfedgeIterator end)
            : begin_(begin), end_(end)
        {
        }
        HalfedgeIterator begin() const { return begin_; }
        HalfedgeIterator end() const { return end_; }

    private:
        HalfedgeIterator begin_;
        HalfedgeIterator end_;
    };

    //! helper class for iterating through all edges using range-based
    //! for-loops. \sa edges()
    class EdgeContainer
    {
    public:
        EdgeContainer(EdgeIterator begin, EdgeIterator end)
            : begin_(begin), end_(end)
        {
        }
        EdgeIterator begin() const { return begin_; }
        EdgeIterator end() const { return end_; }

    private:
        EdgeIterator begin_;
        EdgeIterator end_;
    };

    //! helper class for iterating through all faces using range-based
    //! for-loops. \sa faces()
    class FaceContainer
    {
    public:
        FaceContainer(FaceIterator begin, FaceIterator end)
            : begin_(begin), end_(end)
        {
        }
        FaceIterator begin() const { return begin_; }
        FaceIterator end() const { return end_; }

    private:
        FaceIterator begin_;
        FaceIterator end_;
    };

    //!@}
    //! \name Circulator Types
    //!@{

    //! this class circulates through all one-ring neighbors of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa HalfedgeAroundVertexCirculator, vertices(Vertex)
    class VertexAroundVertexCirculator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using reference = Vertex&;
        using pointer = Vertex*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        VertexAroundVertexCirculator(const NonManifoldSurfaceMesh* mesh = nullptr,
                                     Vertex v = Vertex())
            : mesh_(mesh)
        {
            if (mesh_)
                halfedge_ = mesh_->halfedge(v);
            first_ = halfedge_;
            visited_.emplace((*(*this)).idx());
        }

        //! are two circulators equal?
        bool operator==(const VertexAroundVertexCirculator& rhs) const
        {
            assert(mesh_);
            assert(mesh_ == rhs.mesh_);
            return (is_active_ && (halfedge_ == rhs.halfedge_));
        }

        //! are two circulators different?
        bool operator!=(const VertexAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotate counter-clockwise)
        VertexAroundVertexCirculator& operator++()
        {
            assert(mesh_);
            if (loaded)
            {
                halfedge_ = cache_;
                halfedge_ = mesh_->hconn_[halfedge_].next_neighbor_from_;
                cache_ = Halfedge();
                loaded = false;
            }
            else
            {
                if (!mesh_->opposite_halfedge(mesh_->prev_halfedge(halfedge_)).is_valid())
                {
                    cache_ = halfedge_;
                    halfedge_ = mesh_->prev_halfedge(halfedge_);
                    loaded = true;
                }
                else
                {
                    halfedge_ = mesh_->hconn_[halfedge_].next_neighbor_from_;
                }
            }
            if (halfedge_ != first_ && visited_.contains((*(*this)).idx()))
            {
                ++(*this);
            }
            is_active_ = true;
            visited_.emplace((*(*this)).idx());
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        VertexAroundVertexCirculator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! get the vertex the circulator refers to
        Vertex operator*() const
        {
            assert(mesh_);
            return (cache_.is_valid()) ? mesh_->from_vertex(halfedge_) : mesh_->to_vertex(halfedge_);
        }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.is_valid(); }

        //! \return the current halfedge
        Halfedge halfedge() const { return halfedge_; }

        // helper for C++11 range-based for-loops
        VertexAroundVertexCirculator& begin()
        {
            is_active_ = !halfedge_.is_valid();
            return *this;
        }
        // helper for C++11 range-based for-loops
        VertexAroundVertexCirculator& end()
        {
            is_active_ = true;
            return *this;
        }

    private:
        const NonManifoldSurfaceMesh* mesh_;
        Halfedge halfedge_, first_, cache_;
        bool is_active_{true}; // helper for C++11 range-based for-loops
        bool loaded{false};
        std::unordered_set<int> visited_;

    };

    //! this class circulates through all outgoing halfedges of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, halfedges(Vertex)
    class HalfedgeAroundVertexCirculator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Halfedge;
        using reference = Halfedge&;
        using pointer = Halfedge*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        HalfedgeAroundVertexCirculator(const NonManifoldSurfaceMesh* mesh = nullptr,
                                       Vertex v = Vertex())
            : mesh_(mesh)
        {
            if (mesh_)
                halfedge_ = mesh_->halfedge(v);
        }

        //! are two circulators equal?
        bool operator==(const HalfedgeAroundVertexCirculator& rhs) const
        {
            assert(mesh_);
            assert(mesh_ == rhs.mesh_);
            return (is_active_ && (halfedge_ == rhs.halfedge_));
        }

        //! are two circulators different?
        bool operator!=(const HalfedgeAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotate counter-clockwise)
        HalfedgeAroundVertexCirculator& operator++()
        {
            assert(mesh_);
            halfedge_ = mesh_->hconn_[halfedge_].next_neighbor_from_;
            is_active_ = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        HalfedgeAroundVertexCirculator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotate clockwise)
        HalfedgeAroundVertexCirculator& operator--()
        {
            assert(mesh_);
            halfedge_ = mesh_->hconn_[halfedge_].prev_neighbor_from_;
            return *this;
        }

        //! post-decrement (rotate clockwise)
        HalfedgeAroundVertexCirculator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        //! get the halfedge the circulator refers to
        Halfedge operator*() const { return halfedge_; }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.is_valid(); }

        // helper for C++11 range-based for-loops
        HalfedgeAroundVertexCirculator& begin()
        {
            is_active_ = !halfedge_.is_valid();
            return *this;
        }
        // helper for C++11 range-based for-loops
        HalfedgeAroundVertexCirculator& end()
        {
            is_active_ = true;
            return *this;
        }

    private:
        const NonManifoldSurfaceMesh* mesh_;
        Halfedge halfedge_;
        bool is_active_{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through all edges incident to a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, edges(Vertex)
    class EdgeAroundVertexCirculator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Edge;
        using reference = Edge&;
        using pointer = Edge*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        EdgeAroundVertexCirculator(const NonManifoldSurfaceMesh* mesh = nullptr,
                                   Vertex v = Vertex())
            : mesh_(mesh)
        {
            if (mesh_)
                halfedge_ = mesh_->halfedge(v);
            first_ = halfedge_;
            visited_.emplace((*(*this)).idx());
        }

        //! are two circulators equal?
        bool operator==(const EdgeAroundVertexCirculator& rhs) const
        {
            assert(mesh_);
            assert(mesh_ == rhs.mesh_);
            return (is_active_ && (halfedge_ == rhs.halfedge_));
        }

        //! are two circulators different?
        bool operator!=(const EdgeAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotate counter-clockwise)
        EdgeAroundVertexCirculator& operator++()
        {
            assert(mesh_);
            if (loaded)
            {
                halfedge_ = cache_;
                halfedge_ = mesh_->hconn_[halfedge_].next_neighbor_from_;
                cache_ = Halfedge();
                loaded = false;
            }
            else
            {
                if (!mesh_->opposite_halfedge(mesh_->prev_halfedge(halfedge_)).is_valid())
                {
                    cache_ = halfedge_;
                    halfedge_ = mesh_->prev_halfedge(halfedge_);
                    loaded = true;
                }
                else
                {
                    halfedge_ = mesh_->hconn_[halfedge_].next_neighbor_from_;
                }
            }
            if (halfedge_ != first_ && visited_.contains((*(*this)).idx()))
            {
                ++(*this);
            }
            is_active_ = true;
            visited_.emplace((*(*this)).idx());
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        EdgeAroundVertexCirculator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! get the halfedge the circulator refers to
        Edge operator*() const { return mesh_->edge(halfedge_); }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.is_valid(); }

        // helper for C++11 range-based for-loops
        EdgeAroundVertexCirculator& begin()
        {
            is_active_ = !halfedge_.is_valid();
            return *this;
        }
        // helper for C++11 range-based for-loops
        EdgeAroundVertexCirculator& end()
        {
            is_active_ = true;
            return *this;
        }

    private:
        const NonManifoldSurfaceMesh* mesh_;
        Halfedge halfedge_, first_, cache_;
        bool is_active_{true}; // helper for C++11 range-based for-loops
        bool loaded{false};
        std::unordered_set<int> visited_;
    };

    //! this class circulates through all incident faces of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, HalfedgeAroundVertexCirculator, faces(Vertex)
    class FaceAroundVertexCirculator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Face;
        using reference = Face&;
        using pointer = Face*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! construct with mesh and vertex (vertex should not be isolated!)
        FaceAroundVertexCirculator(const NonManifoldSurfaceMesh* m = nullptr,
                                   Vertex v = Vertex())
            : mesh_(m)
        {
            if (mesh_)
            {
                halfedge_ = mesh_->halfedge(v);
            }
        }

        //! are two circulators equal?
        bool operator==(const FaceAroundVertexCirculator& rhs) const
        {
            assert(mesh_);
            assert(mesh_ == rhs.mesh_);
            return (is_active_ && (halfedge_ == rhs.halfedge_));
        }

        //! are two circulators different?
        bool operator!=(const FaceAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        FaceAroundVertexCirculator& operator++()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->hconn_[halfedge_].next_neighbor_from_;
            is_active_ = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        FaceAroundVertexCirculator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotate clockwise)
        FaceAroundVertexCirculator& operator--()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->hconn_[halfedge_].prev_neighbor_from_;
            return *this;
        }

        //! post-decrement (rotate clockwise)
        FaceAroundVertexCirculator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        //! get the face the circulator refers to
        Face operator*() const
        {
            assert(mesh_ && halfedge_.is_valid());
            return mesh_->face(halfedge_);
        }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.is_valid(); }

        // helper for C++11 range-based for-loops
        FaceAroundVertexCirculator& begin()
        {
            is_active_ = !halfedge_.is_valid();
            return *this;
        }
        // helper for C++11 range-based for-loops
        FaceAroundVertexCirculator& end()
        {
            is_active_ = true;
            return *this;
        }

    private:
        const NonManifoldSurfaceMesh* mesh_;
        Halfedge halfedge_;
        bool is_active_{true}; // helper for C++11 range-based for-loops
    };


    //! this class circulates through all incident faces of a vertex.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundVertexCirculator, HalfedgeAroundVertexCirculator, faces(Vertex)
    class FaceAroundEdgeCirculator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Face;
        using reference = Face&;
        using pointer = Face*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! construct with mesh and vertex (vertex should not be isolated!)
        FaceAroundEdgeCirculator(const NonManifoldSurfaceMesh* m = nullptr,
                                   Edge e = Edge())
            : mesh_(m)
        {
            if (mesh_)
            {
                halfedge_ = mesh_->halfedge(e, 0);
            }
        }

        //! are two circulators equal?
        bool operator==(const FaceAroundEdgeCirculator& rhs) const
        {
            assert(mesh_);
            assert(mesh_ == rhs.mesh_);
            return (is_active_ && (halfedge_ == rhs.halfedge_));
        }

        //! are two circulators different?
        bool operator!=(const FaceAroundEdgeCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        FaceAroundEdgeCirculator& operator++()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->next_sibling_halfedge(halfedge_);
            is_active_ = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        FaceAroundEdgeCirculator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! get the face the circulator refers to
        Face operator*() const
        {
            assert(mesh_ && halfedge_.is_valid());
            return mesh_->face(halfedge_);
        }

        //! cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.is_valid(); }

        // helper for C++11 range-based for-loops
        FaceAroundEdgeCirculator& begin()
        {
            is_active_ = !halfedge_.is_valid();
            return *this;
        }
        // helper for C++11 range-based for-loops
        FaceAroundEdgeCirculator& end()
        {
            is_active_ = true;
            return *this;
        }

    private:
        const NonManifoldSurfaceMesh* mesh_;
        Halfedge halfedge_;
        bool is_active_{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through the vertices of a face.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa HalfedgeAroundFaceCirculator, vertices(Face)
    class VertexAroundFaceCirculator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using reference = Vertex&;
        using pointer = Vertex*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        VertexAroundFaceCirculator(const NonManifoldSurfaceMesh* m = nullptr,
                                   Face f = Face())
            : mesh_(m)
        {
            if (mesh_)
                halfedge_ = mesh_->halfedge(f);
        }

        //! are two circulators equal?
        bool operator==(const VertexAroundFaceCirculator& rhs) const
        {
            assert(mesh_);
            assert(mesh_ == rhs.mesh_);
            return (is_active_ && (halfedge_ == rhs.halfedge_));
        }

        //! are two circulators different?
        bool operator!=(const VertexAroundFaceCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        VertexAroundFaceCirculator& operator++()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->next_halfedge(halfedge_);
            is_active_ = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        VertexAroundFaceCirculator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotates clockwise)
        VertexAroundFaceCirculator& operator--()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->prev_halfedge(halfedge_);
            return *this;
        }

        //! post-decrement (rotate clockwise)
        VertexAroundFaceCirculator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        //! get the vertex the circulator refers to
        Vertex operator*() const
        {
            assert(mesh_ && halfedge_.is_valid());
            return mesh_->to_vertex(halfedge_);
        }

        // helper for C++11 range-based for-loops
        VertexAroundFaceCirculator& begin()
        {
            is_active_ = false;
            return *this;
        }
        // helper for C++11 range-based for-loops
        VertexAroundFaceCirculator& end()
        {
            is_active_ = true;
            return *this;
        }

    private:
        const NonManifoldSurfaceMesh* mesh_;
        Halfedge halfedge_;
        bool is_active_{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through the vertices of a face.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa HalfedgeAroundFaceCirculator, vertices(Face)
    class HalfedgeAroundEdgeCirculator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Halfedge;
        using reference = Halfedge&;
        using pointer = Halfedge*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        HalfedgeAroundEdgeCirculator(const NonManifoldSurfaceMesh* m = nullptr,
                                   Edge e = Edge())
            : mesh_(m)
        {
            if (mesh_)
                halfedge_ = mesh_->halfedge(e);
        }

        //! are two circulators equal?
        bool operator==(const HalfedgeAroundEdgeCirculator& rhs) const
        {
            assert(mesh_);
            assert(mesh_ == rhs.mesh_);
            return (is_active_ && (halfedge_ == rhs.halfedge_));
        }

        //! are two circulators different?
        bool operator!=(const HalfedgeAroundEdgeCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        HalfedgeAroundEdgeCirculator& operator++()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->next_sibling_halfedge(halfedge_);
            is_active_ = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        HalfedgeAroundEdgeCirculator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-increment (rotates counter-clockwise)
        HalfedgeAroundEdgeCirculator& operator--()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->prev_sibling_halfedge(halfedge_);
            is_active_ = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        HalfedgeAroundEdgeCirculator operator--(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! get the vertex the circulator refers to
        Halfedge operator*() const
        {
            assert(mesh_ && halfedge_.is_valid());
            return halfedge_;
        }

        // helper for C++11 range-based for-loops
        HalfedgeAroundEdgeCirculator& begin()
        {
            is_active_ = false;
            return *this;
        }
        // helper for C++11 range-based for-loops
        HalfedgeAroundEdgeCirculator& end()
        {
            is_active_ = true;
            return *this;
        }

    private:
        const NonManifoldSurfaceMesh* mesh_;
        Halfedge halfedge_;
        bool is_active_{true}; // helper for C++11 range-based for-loops
    };

    //! this class circulates through all halfedges of a face.
    //! it also acts as a container-concept for C++11 range-based for loops.
    //! \sa VertexAroundFaceCirculator, halfedges(Face)
    class HalfedgeAroundFaceCirculator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Halfedge;
        using reference = Halfedge&;
        using pointer = Halfedge*;
        using iterator_category = std::bidirectional_iterator_tag;

        //! default constructor
        HalfedgeAroundFaceCirculator(const NonManifoldSurfaceMesh* m = nullptr,
                                     Face f = Face())
            : mesh_(m)
        {
            if (mesh_)
                halfedge_ = mesh_->halfedge(f);
        }

        //! are two circulators equal?
        bool operator==(const HalfedgeAroundFaceCirculator& rhs) const
        {
            assert(mesh_);
            assert(mesh_ == rhs.mesh_);
            return (is_active_ && (halfedge_ == rhs.halfedge_));
        }

        //! are two circulators different?
        bool operator!=(const HalfedgeAroundFaceCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        //! pre-increment (rotates counter-clockwise)
        HalfedgeAroundFaceCirculator& operator++()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->next_halfedge(halfedge_);
            is_active_ = true;
            return *this;
        }

        //! post-increment (rotate counter-clockwise)
        HalfedgeAroundFaceCirculator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        //! pre-decrement (rotates clockwise)
        HalfedgeAroundFaceCirculator& operator--()
        {
            assert(mesh_ && halfedge_.is_valid());
            halfedge_ = mesh_->prev_halfedge(halfedge_);
            return *this;
        }

        //! post-decrement (rotate clockwise)
        HalfedgeAroundFaceCirculator operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        //! get the halfedge the circulator refers to
        Halfedge operator*() const { return halfedge_; }

        // helper for C++11 range-based for-loops
        HalfedgeAroundFaceCirculator& begin()
        {
            is_active_ = false;
            return *this;
        }
        // helper for C++11 range-based for-loops
        HalfedgeAroundFaceCirculator& end()
        {
            is_active_ = true;
            return *this;
        }

    private:
        const NonManifoldSurfaceMesh* mesh_;
        Halfedge halfedge_;
        bool is_active_{true}; // helper for C++11 range-based for-loops
    };

    //!@}
    //! \name Construction, destruction, assignment
    //!@{

    //! default constructor
    NonManifoldSurfaceMesh();

    //! destructor
    virtual ~NonManifoldSurfaceMesh();

    //! copy constructor: copies \p rhs to \p *this. performs a deep copy of all
    //! properties.
    NonManifoldSurfaceMesh(const NonManifoldSurfaceMesh& rhs) { operator=(rhs); }

    //! assign \p rhs to \p *this. performs a deep copy of all properties.
    NonManifoldSurfaceMesh& operator=(const NonManifoldSurfaceMesh& rhs);

    //! assign \p rhs to \p *this. does not copy custom properties.
    NonManifoldSurfaceMesh& assign(const NonManifoldSurfaceMesh& rhs);

    //!@}
    //! \name Add new elements by hand
    //!@{

    //! add a new vertex with position \p p
    Vertex add_vertex(const Point& p);

    //! \brief Add a new face with vertex list \p vertices
    //! \throw TopologyException in case a topological error occurs.
    //! \sa add_triangle, add_quad
    Face add_face(const std::vector<Vertex>& vertices);

    //! add a new triangle connecting vertices \p v0, \p v1, \p v2
    //! \sa add_face, add_quad
    Face add_triangle(Vertex v0, Vertex v1, Vertex v2);

    //! add a new quad connecting vertices \p v0, \p v1, \p v2, \p v3
    //! \sa add_triangle, add_face
    Face add_quad(Vertex v0, Vertex v1, Vertex v2, Vertex v3);

    //!@}
    //! \name Memory Management
    //!@{

    //! \return number of (deleted and valid) vertices in the mesh
    size_t vertices_size() const { return vprops_.size(); }

    //! \return number of (deleted and valid) halfedges in the mesh
    size_t halfedges_size() const { return hprops_.size(); }

    //! \return number of (deleted and valid) edges in the mesh
    size_t edges_size() const { return eprops_.size(); }

    //! \return number of (deleted and valid) faces in the mesh
    size_t faces_size() const { return fprops_.size(); }

    //! \return number of vertices in the mesh
    size_t n_vertices() const { return vertices_size() - deleted_vertices_; }

    //! \return number of halfedge in the mesh
    size_t n_halfedges() const { return halfedges_size() - deleted_halfedges_; }

    //! \return number of edges in the mesh
    size_t n_edges() const { return edges_size() - deleted_edges_; }

    //! \return number of faces in the mesh
    size_t n_faces() const { return faces_size() - deleted_faces_; }

    //! \return true if the mesh is empty, i.e., has no vertices
    bool is_empty() const { return n_vertices() == 0; }

    //! clear mesh: remove all vertices, edges, faces
    virtual void clear();

    //! remove unused memory from vectors
    void free_memory();

    //! reserve memory (mainly used in file readers)
    void reserve(size_t nvertices, size_t nedges, size_t nhedges, size_t nfaces);

    //! remove deleted elements
    void garbage_collection();

    //! \return whether vertex \p v is deleted
    //! \sa garbage_collection()
    bool is_deleted(Vertex v) const { return vdeleted_[v]; }

    //! \return whether halfedge \p h is deleted
    //! \sa garbage_collection()
    bool is_deleted(Halfedge h) const { return hdeleted_[h]; }

    //! \return whether edge \p e is deleted
    //! \sa garbage_collection()
    bool is_deleted(Edge e) const { return edeleted_[e]; }

    //! \return whether face \p f is deleted
    //! \sa garbage_collection()
    bool is_deleted(Face f) const { return fdeleted_[f]; }


    bool mark_deleted(Vertex v)
    {
        if (!vdeleted_[v])
        {
            vdeleted_[v] = true;
            ++deleted_vertices_;
            return true;
        }
        return false;
    }

    bool mark_deleted(Halfedge h)
    {
        if (!hdeleted_[h])
        {
            hdeleted_[h] = true;
            ++deleted_halfedges_;
            return true;
        }
        return false;
    }

    bool mark_deleted(Edge e)
    {
        if (!edeleted_[e])
        {
            edeleted_[e] = true;
            ++deleted_edges_;
            return true;
        }
        return false;
    }

    bool mark_deleted(Face f)
    {
        if (!fdeleted_[f])
        {
            fdeleted_[f] = true;
            ++deleted_faces_;
            return true;
        }
        return false;
    }

    //! \return whether vertex \p v is valid.
    bool is_valid(Vertex v) const { return v.idx() < vertices_size(); }

    //! \return whether halfedge \p h is valid.
    bool is_valid(Halfedge h) const { return h.idx() < halfedges_size(); }

    //! \return whether edge \p e is valid.
    bool is_valid(Edge e) const { return e.idx() < edges_size(); }

    //! \return whether the face \p f is valid.
    bool is_valid(Face f) const { return f.idx() < faces_size(); }

    //!@}
    //! \name Low-level connectivity
    //!@{

    //! \return an outgoing halfedge of vertex \p v.
    //! if \p v is a boundary vertex this will be a boundary halfedge.
    Halfedge halfedge(Vertex v) const { return vconn_[v].halfedge_; }

    //! set the outgoing halfedge of vertex \p v to \p h
    void set_halfedge(Vertex v, Halfedge h) { vconn_[v].halfedge_ = h; }

    void set_halfedge(Edge e, Halfedge h) { econn_[e].halfedge_ = h; }

    //! \return whether \p v is a boundary vertex
    bool is_boundary(Vertex v) const
    {
        const Halfedge h(halfedge(v));
        return (!(h.is_valid() && face(h).is_valid()));
    }

    //! \return whether \p v is isolated, i.e., not incident to any edge
    bool is_isolated(Vertex v) const { return !halfedge(v).is_valid(); }

    //! \return whether \p v is a manifold vertex (not incident to several patches)
    bool is_manifold(Vertex v) const
    {
        // The vertex is non-manifold if more than one gap exists, i.e.
        // more than one outgoing boundary halfedge.
        int n(0);
        auto hit = halfedges(v);
        auto hend = hit;
        if (hit)
            do
            {
                if (is_boundary(edge(*hit)))
                    ++n;
            } while (++hit != hend);
        return n < 2;
    }

    //! \return the vertex the halfedge \p h points to
    inline Vertex to_vertex(Halfedge h) const { return hconn_[h].to_vertex_; }

    //! \return the vertex the halfedge \p h emanates from
    inline Vertex from_vertex(Halfedge h) const { return hconn_[hconn_[h].prev_halfedge_].to_vertex_; }

    //! sets the vertex the halfedge \p h points to to \p v
    inline void set_to_vertex(Halfedge h, Vertex v) { hconn_[h].to_vertex_ = v; }


    //! \return the face incident to halfedge \p h
    Face face(Halfedge h) const { return hconn_[h].face_; }

    //! sets the incident face to halfedge \p h to \p f
    void set_face(Halfedge h, Face f) { hconn_[h].face_ = f; }

    //! \return the next halfedge within the incident face
    inline Halfedge next_halfedge(Halfedge h) const
    {
        return hconn_[h].next_halfedge_;
    }

    //! sets the next halfedge of \p h within the face to \p nh
    inline void set_next_halfedge(Halfedge h, Halfedge nh)
    {
        hconn_[h].next_halfedge_ = nh;
        hconn_[nh].prev_halfedge_ = h;
    }

    //! sets the previous halfedge of \p h and the next halfedge of \p ph to \p nh
    inline void set_prev_halfedge(Halfedge h, Halfedge ph)
    {
        hconn_[h].prev_halfedge_ = ph;
        hconn_[ph].next_halfedge_ = h;
    }

    //! \return the previous halfedge within the incident face
    inline Halfedge prev_halfedge(Halfedge h) const
    {
        return hconn_[h].prev_halfedge_;
    }

    //! \return the opposite halfedge of \p h
    inline Halfedge next_sibling_halfedge(Halfedge h) const
    {
        return hconn_[h].next_sibling_;
    }
    inline Halfedge prev_sibling_halfedge(Halfedge h) const
    {
        return hconn_[h].prev_sibling_;
    }

    inline Halfedge opposite_halfedge(Halfedge h) const
    {
        Halfedge opp_h = next_sibling_halfedge(h);
        while (opp_h.is_valid() && opp_h != h && halfedge_sibling_same_orientation(opp_h, h))
            opp_h = next_sibling_halfedge(opp_h);
        if (opp_h == h)
            return Halfedge();
        return opp_h;
    }

    //! \return the edge that contains halfedge \p h as one of its two
    //! halfedges.
    inline Edge edge(Halfedge h) const { return hconn_[h].edge_; }

    //! \return the \p i'th halfedge of edge \p e. \p i has to be 0 or 1.
    inline Halfedge halfedge(Edge e, unsigned int i) const
    {
        auto h = econn_[e].halfedge_;
        for (unsigned int j = 0; j < i; j++)
            h = next_sibling_halfedge(h);
        return h;
    }

    //! \return the \p i'th vertex of edge \p e. \p i has to be 0 or 1.
    inline Vertex vertex(Edge e, unsigned int i) const
    {
        assert(i <= 1);
        return (i == 0) ? from_vertex(halfedge(e, 0)) : to_vertex(halfedge(e, 0));
    }

    //! \return the face incident to the \p i'th halfedge of edge \p e. \p i has to be 0 or 1.
    Face face(Edge e, unsigned int i) const
    {
        return face(halfedge(e, i));
    }

    //! \return whether \p e is a boundary edge, i.e., if one of its
    //! halfedges is a boundary halfedge.
    bool is_boundary(Edge e) const
    {
        return next_sibling_halfedge(econn_[e].halfedge_) == econn_[e].halfedge_;
    }

    //! \return a halfedge of face \p f
    Halfedge halfedge(Face f) const { return fconn_[f].halfedge_; }

    //! \return a halfedge of edge \p e
    Halfedge halfedge(Edge e) const { return econn_[e].halfedge_; }

    //! sets the halfedge of face \p f to \p h
    void set_halfedge(Face f, Halfedge h) { fconn_[f].halfedge_ = h; }

    //! \return whether \p f is a boundary face, i.e., it one of its edges is a boundary edge.
    bool is_boundary(Face f) const
    {
        Halfedge h = halfedge(f);
        const Halfedge hh = h;
        do
        {
            if (is_boundary(edge(h)))
                return true;
            h = next_halfedge(h);
        } while (h != hh);
        return false;
    }

    void set_edge(Halfedge h, Edge e) { hconn_[h].edge_ = e; }

    //!@}
    //! \name Property handling
    //!@{

    //! add a vertex property of type \p T with name \p name and default
    //! value \p t. fails if a property named \p name exists already,
    //! since the name has to be unique. in this case it returns an
    //! invalid property
    template <class T>
    VertexProperty<T> add_vertex_property(const std::string& name,
                                          const T t = T())
    {
        return VertexProperty<T>(vprops_.add<T>(name, t));
    }

    //! get the vertex property named \p name of type \p T. returns an
    //! invalid VertexProperty if the property does not exist or if the
    //! type does not match.
    template <class T>
    VertexProperty<T> get_vertex_property(const std::string& name) const
    {
        return VertexProperty<T>(vprops_.get<T>(name));
    }

    //! if a vertex property of type \p T with name \p name exists, it is
    //! returned. otherwise this property is added (with default value \c
    //! t)
    template <class T>
    VertexProperty<T> vertex_property(const std::string& name, const T t = T())
    {
        return VertexProperty<T>(vprops_.get_or_add<T>(name, t));
    }

    //! remove the vertex property \p p
    template <class T>
    void remove_vertex_property(VertexProperty<T>& p)
    {
        vprops_.remove(p);
    }

    //! does the mesh have a vertex property with name \p name?
    bool has_vertex_property(const std::string& name) const
    {
        return vprops_.exists(name);
    }

    //! add a halfedge property of type \p T with name \p name and default
    //! value \p t.  fails if a property named \p name exists already,
    //! since the name has to be unique. in this case it returns an
    //! invalid property.
    template <class T>
    HalfedgeProperty<T> add_halfedge_property(const std::string& name,
                                              const T t = T())
    {
        return HalfedgeProperty<T>(hprops_.add<T>(name, t));
    }

    //! add a edge property of type \p T with name \p name and default
    //! value \p t.  fails if a property named \p name exists already,
    //! since the name has to be unique.  in this case it returns an
    //! invalid property.
    template <class T>
    EdgeProperty<T> add_edge_property(const std::string& name, const T t = T())
    {
        return EdgeProperty<T>(eprops_.add<T>(name, t));
    }

    //! get the halfedge property named \p name of type \p T. returns an
    //! invalid VertexProperty if the property does not exist or if the
    //! type does not match.
    template <class T>
    HalfedgeProperty<T> get_halfedge_property(const std::string& name) const
    {
        return HalfedgeProperty<T>(hprops_.get<T>(name));
    }

    //! get the edge property named \p name of type \p T. returns an
    //! invalid VertexProperty if the property does not exist or if the
    //! type does not match.
    template <class T>
    EdgeProperty<T> get_edge_property(const std::string& name) const
    {
        return EdgeProperty<T>(eprops_.get<T>(name));
    }

    //! if a halfedge property of type \p T with name \p name exists, it is
    //! returned.  otherwise this property is added (with default value \c
    //! t)
    template <class T>
    HalfedgeProperty<T> halfedge_property(const std::string& name,
                                          const T t = T())
    {
        return HalfedgeProperty<T>(hprops_.get_or_add<T>(name, t));
    }

    //! if an edge property of type \p T with name \p name exists, it is
    //! returned.  otherwise this property is added (with default value \c
    //! t)
    template <class T>
    EdgeProperty<T> edge_property(const std::string& name, const T t = T())
    {
        return EdgeProperty<T>(eprops_.get_or_add<T>(name, t));
    }

    //! remove the halfedge property \p p
    template <class T>
    void remove_halfedge_property(HalfedgeProperty<T>& p)
    {
        hprops_.remove(p);
    }

    //! does the mesh have a halfedge property with name \p name?
    bool has_halfedge_property(const std::string& name) const
    {
        return hprops_.exists(name);
    }

    //! remove the edge property \p p
    template <class T>
    void remove_edge_property(EdgeProperty<T>& p)
    {
        eprops_.remove(p);
    }

    //! does the mesh have an edge property with name \p name?
    bool has_edge_property(const std::string& name) const
    {
        return eprops_.exists(name);
    }

    //! \return the names of all vertex properties
    std::vector<std::string> vertex_properties() const
    {
        return vprops_.properties();
    }

    //! \return the names of all halfedge properties
    std::vector<std::string> halfedge_properties() const
    {
        return hprops_.properties();
    }

    //! \return the names of all edge properties
    std::vector<std::string> edge_properties() const
    {
        return eprops_.properties();
    }

    //! add a face property of type \p T with name \p name and default value \c
    //! t.  fails if a property named \p name exists already, since the name has
    //! to be unique.  in this case it returns an invalid property
    template <class T>
    FaceProperty<T> add_face_property(const std::string& name, const T t = T())
    {
        return FaceProperty<T>(fprops_.add<T>(name, t));
    }

    //! get the face property named \p name of type \p T. returns an invalid
    //! VertexProperty if the property does not exist or if the type does not
    //! match.
    template <class T>
    FaceProperty<T> get_face_property(const std::string& name) const
    {
        return FaceProperty<T>(fprops_.get<T>(name));
    }

    //! if a face property of type \p T with name \p name exists, it is
    //! returned.  otherwise this property is added (with default value \p t)
    template <class T>
    FaceProperty<T> face_property(const std::string& name, const T t = T())
    {
        return FaceProperty<T>(fprops_.get_or_add<T>(name, t));
    }

    //! remove the face property \p p
    template <class T>
    void remove_face_property(FaceProperty<T>& p)
    {
        fprops_.remove(p);
    }

    //! does the mesh have a face property with name \p name?
    bool has_face_property(const std::string& name) const
    {
        return fprops_.exists(name);
    }

    //! \return the names of all face properties
    std::vector<std::string> face_properties() const
    {
        return fprops_.properties();
    }

    //!@}
    //! \name Iterators and circulators
    //!@{

    //! \return start iterator for vertices
    VertexIterator vertices_begin() const
    {
        return VertexIterator(Vertex(0), this);
    }

    //! \return end iterator for vertices
    VertexIterator vertices_end() const
    {
        return VertexIterator(Vertex(static_cast<IndexType>(vertices_size())),
                              this);
    }

    //! \return vertex container for C++11 range-based for-loops
    VertexContainer vertices() const
    {
        return VertexContainer(vertices_begin(), vertices_end());
    }

    //! \return start iterator for halfedges
    HalfedgeIterator halfedges_begin() const
    {
        return HalfedgeIterator(Halfedge(0), this);
    }

    //! \return end iterator for halfedges
    HalfedgeIterator halfedges_end() const
    {
        return HalfedgeIterator(
            Halfedge(static_cast<IndexType>(halfedges_size())), this);
    }

    //! \return halfedge container for C++11 range-based for-loops
    HalfedgeContainer halfedges() const
    {
        return HalfedgeContainer(halfedges_begin(), halfedges_end());
    }

    //! \return start iterator for edges
    EdgeIterator edges_begin() const { return EdgeIterator(Edge(0), this); }

    //! \return end iterator for edges
    EdgeIterator edges_end() const
    {
        return EdgeIterator(Edge(static_cast<IndexType>(edges_size())), this);
    }

    //! \return edge container for C++11 range-based for-loops
    EdgeContainer edges() const
    {
        return EdgeContainer(edges_begin(), edges_end());
    }

    //! \return circulator for vertices around vertex \p v
    VertexAroundVertexCirculator vertices(Vertex v) const
    {
        return VertexAroundVertexCirculator(this, v);
    }

    //! \return circulator for edges around vertex \p v
    EdgeAroundVertexCirculator edges(Vertex v) const
    {
        return EdgeAroundVertexCirculator(this, v);
    }

    //! \return circulator for outgoing halfedges around vertex \p v
    HalfedgeAroundVertexCirculator halfedges(Vertex v) const
    {
        return HalfedgeAroundVertexCirculator(this, v);
    }

    //! \return start iterator for faces
    FaceIterator faces_begin() const { return FaceIterator(Face(0), this); }

    //! \return end iterator for faces
    FaceIterator faces_end() const
    {
        return FaceIterator(Face(static_cast<IndexType>(faces_size())), this);
    }

    //! \return face container for C++11 range-based for-loops
    FaceContainer faces() const
    {
        return FaceContainer(faces_begin(), faces_end());
    }

    //! \return circulator for faces around vertex \p v
    FaceAroundVertexCirculator faces(Vertex v) const
    {
        return FaceAroundVertexCirculator(this, v);
    }

    //! \return circulator for faces around edge \p e
    FaceAroundEdgeCirculator faces(Edge e) const
    {
        return FaceAroundEdgeCirculator(this, e);
    }

    //! \return circulator for vertices of face \p f
    VertexAroundFaceCirculator vertices(Face f) const
    {
        return VertexAroundFaceCirculator(this, f);
    }

    //! \return circulator for halfedges of face \p f
    HalfedgeAroundFaceCirculator halfedges(Face f) const
    {
        return HalfedgeAroundFaceCirculator(this, f);
    }

    //! \return circulator for halfedges of face \p f
    HalfedgeAroundEdgeCirculator halfedges(Edge e) const
    {
        return HalfedgeAroundEdgeCirculator(this, e);
    }

    //!@}
    //! \name Higher-level Topological Operations
    //!@{

    //! Subdivide the edge \p e = (v0,v1) by splitting it into the two edge
    //! (v0,p) and (p,v1). Note that this function does not introduce any
    //! other edge or faces. It simply splits the edge. Returns halfedge that
    //! points to \p p.
    //! \sa insert_vertex(Edge, Vertex)
    //! \sa insert_vertex(Halfedge, Vertex)
    Halfedge insert_vertex(Edge e, const Point& p)
    {
        return insert_vertex(halfedge(e, 0), add_vertex(p));
    }

    //! Subdivide the edge \p e = (v0,v1) by splitting it into the two edge
    //! (v0,v) and (v,v1). Note that this function does not introduce any
    //! other edge or faces. It simply splits the edge. Returns halfedge
    //! that points to \p p. \sa insert_vertex(Edge, Point) \sa
    //! insert_vertex(Halfedge, Vertex)
    Halfedge insert_vertex(Edge e, Vertex v)
    {
        return insert_vertex(halfedge(e, 0), v);
    }

    //! Subdivide the halfedge \p h = (v0,v1) by splitting it into the two halfedges
    //! (v0,v) and (v,v1). Note that this function does not introduce any
    //! other edge or faces. It simply splits the edge. Returns the halfedge
    //! that points from v1 to \p v.
    //! \sa insert_vertex(Edge, Point)
    //! \sa insert_vertex(Edge, Vertex)
    Halfedge insert_vertex(Halfedge h0, Vertex v);

    //! find the halfedge from start to end
    Halfedge find_halfedge(Vertex start, Vertex end) const;

    //! find the edge (a,b)
    Edge find_edge(Vertex a, Vertex b) const;

    //! \return whether the mesh a triangle mesh. this function simply tests
    //! each face, and therefore is not very efficient.
    bool is_triangle_mesh() const;

    //! \return whether the mesh a quad mesh. this function simply tests
    //! each face, and therefore is not very efficient.
    bool is_quad_mesh() const;

    //! \return whether collapsing the halfedge \p v0v1 is topologically legal.
    //! \attention This function is only valid for triangle meshes.
    bool is_collapse_ok(Halfedge v0v1) const;

    //! Collapse the halfedge \p h by moving its start vertex into its target
    //! vertex. For non-boundary halfedges this function removes one vertex, three
    //! edges, and two faces. For boundary halfedges it removes one vertex, two
    //! edges and one face.
    //! \attention This function is only valid for triangle meshes.
    //! \attention Halfedge collapses might lead to invalid faces. Call
    //! is_collapse_ok(Halfedge) to be sure the collapse is legal.
    //! \attention The removed items are only marked as deleted. You have
    //! to call garbage_collection() to finally remove them.
    void collapse(Halfedge h);

    //! \return whether removing the edge \p e is topologically legal.
    bool is_removal_ok(Edge e) const;

    //! Remove edge and merge its two incident faces into one.
    //! This operation requires that the edge has two incident faces
    //! and that these two are not equal.
    //! \sa is_removal_ok(Edge)
    bool remove_edge(Edge e);

    //! Split the face \p f by first adding point \p p to the mesh and then
    //! inserting edges between \p p and the vertices of \p f. For a triangle
    //! this is a standard one-to-three split.
    //! \sa split(Face, Vertex)
    Vertex split(Face f, const Point& p)
    {
        Vertex v = add_vertex(p);
        split(f, v);
        return v;
    }

    //! Split the face \p f by inserting edges between \p v and the vertices
    //! of \p f. For a triangle this is a standard one-to-three split.
    //! \sa split(Face, const Point&)
    void split(Face f, Vertex v);

    //! Split the edge \p e by first adding point \p p to the mesh and then
    //! connecting it to the two vertices of the adjacent triangles that are
    //! opposite to edge \p e. Returns the halfedge pointing to \p p that is
    //! created by splitting the existing edge \p e.
    //!
    //! \attention This function is only valid for triangle meshes.
    //! \sa split(Edge, Vertex)
    Halfedge split(Edge e, const Point& p) { return split(e, add_vertex(p)); }

    //! Split the edge \p e by connecting vertex \p v it to the two
    //! vertices of the adjacent triangles that are opposite to edge \c
    //! e. Returns the halfedge pointing to \p v that is created by splitting
    //! the existing edge \p e.
    //!
    //! \attention This function is only valid for triangle meshes.
    //! \sa split(Edge, const Point&)
    Halfedge split(Edge e, Vertex v);

    //! Insert edge between the to-vertices of \p h0 and \p h1.
    //! \return The new halfedge from v0 to v1.
    //! \attention \p h0 and \p h1 have to belong to the same face.
    Halfedge insert_edge(Halfedge h0, Halfedge h1);

    //! Check whether flipping edge \p e is topologically OK.
    //! \attention This function is only valid for triangle meshes.
    //! \sa flip(Edge)
    bool is_flip_ok(Edge e) const;

    //! Flip the edge \p e . Removes the edge \p e and add an edge between the
    //! two vertices opposite to edge \p e of the two incident triangles.
    //! \attention This function is only valid for triangle meshes.
    //! \attention Flipping an edge may result in a non-manifold mesh, hence check
    //! for yourself whether this operation is allowed or not!
    //! \sa is_flip_ok()
    void flip(Edge e);

    //! Compute the valence of vertex \p v (number of incident edges).
    size_t valence(Vertex v) const;

    //! Compute the valence of face \p f (its number of vertices).
    size_t valence(Face f) const;

    //! Compute the valence of edge \p e (its number of halfedges).
    size_t valence(Edge e) const;

    //! Delete vertex \p v from the mesh.
    //! \note Only marks the vertex as deleted. Call garbage_collection() to finally remove deleted entities.
    void delete_vertex(Vertex v);

    //! Delete edge \p e from the mesh.
    //! \note Only marks the edge as deleted. Call garbage_collection() to finally remove deleted entities.
    void delete_edge(Edge e);

    //! Deletes face \p f from the mesh.
    //! \note Only marks the face as deleted. Call garbage_collection() to finally remove deleted entities.
    void delete_face(Face f);

    //!@}
    //! \name Geometry-related Functions
    //!@{

    //! position of a vertex (read only)
    const Point& position(Vertex v) const { return vpoint_[v]; }

    //! position of a vertex
    Point& position(Vertex v) { return vpoint_[v]; }

    //! \return vector of point positions
    std::vector<Point>& positions() { return vpoint_.vector(); }

    //!@}

    //! \name Allocate new elements
    //!@{

    //! \brief Allocate a new vertex, resize vertex properties accordingly.
    //! \throw AllocationException in case of failure to allocate a new vertex.
    Vertex new_vertex()
    {
        if (vertices_size() == PMP_MAX_INDEX - 1)
        {
            auto what =
                "NonManifoldSurfaceMesh: cannot allocate vertex, max. index reached";
            throw AllocationException(what);
        }
        vprops_.push_back();
        return Vertex(static_cast<IndexType>(vertices_size()) - 1);
    }

    bool halfedge_sibling_same_orientation(const Halfedge h0, const Halfedge h1) const
    {
        return hconn_[h0].to_vertex_ == hconn_[h1].to_vertex_;
    }

    //! \brief Allocate a new edge, resize edge and halfedge properties accordingly.
    //! \throw AllocationException in case of failure to allocate a new edge.
    Halfedge new_edge()
    {
        if (halfedges_size() == PMP_MAX_INDEX - 1)
        {
            auto what = "NonManifoldSurfaceMesh: cannot allocate edge, max. index reached";
            throw AllocationException(what);
        }

        eprops_.push_back();
        hprops_.push_back();
        hprops_.push_back();

        Halfedge h0(static_cast<IndexType>(halfedges_size()) - 2);
        const Halfedge h1(static_cast<IndexType>(halfedges_size()) - 1);

        return h0;
    }

    //! \brief Allocate a new edge, resize edge and halfedge properties accordingly.
    //! \throw AllocationException in case of failure to allocate a new edge.
    //! \param start starting Vertex of the new edge
    //! \param end end Vertex of the new edge
    Halfedge new_edge(Vertex start, Vertex end)
    {
        if (start == end)
        {
            auto what = "NonManifoldSurfaceMesh: No self-edges allowed";
            throw TopologyException(what);
        }

        if (halfedges_size() == PMP_MAX_INDEX - 1)
        {
            auto what = "NonManifoldSurfaceMesh: cannot allocate edge, max. index reached";
            throw AllocationException(what);
        }

        eprops_.push_back();
        hprops_.push_back();

        const Edge e(static_cast<IndexType>(edges_size()) - 1);
        const Halfedge h(static_cast<IndexType>(halfedges_size()) - 1);

        hconn_[h].edge_ = e;

        hconn_[h].next_neighbor_from_ = h;
        hconn_[h].prev_neighbor_from_ = h;
        hconn_[h].next_sibling_ = h;
        hconn_[h].prev_sibling_ = h;

        econn_[e].halfedge_ = h;

        set_to_vertex(h, end);

        return h;
    }

    Halfedge new_halfedge(Vertex start, Vertex end)
    {
        assert(start != end);

        if (halfedges_size() == PMP_MAX_INDEX - 1)
        {
            auto what = "NonManifoldSurfaceMesh: cannot allocate edge, max. index reached";
            throw AllocationException(what);
        }

        hprops_.push_back();

        auto e = find_edge(start, end);
        const Halfedge h(static_cast<IndexType>(halfedges_size()) - 1);

        const auto first_h = econn_[e].halfedge_;
        insert_sibling(first_h, h);

        hconn_[h].next_neighbor_from_ = h;
        hconn_[h].prev_neighbor_from_ = h;

        hconn_[h].edge_ = e;

        set_to_vertex(h, end);


        return h;
    }

    // Insert h_sibling into doubly-linked sibling list after h
    void insert_sibling(Halfedge h, Halfedge h_sibling)
    {
        if (!h.is_valid())
        {
            hconn_[h_sibling].next_sibling_ = h_sibling;
            hconn_[h_sibling].prev_sibling_ = h_sibling;
            return;
        }
        auto next = next_sibling_halfedge(h);
        hconn_[next].prev_sibling_ = h_sibling;
        hconn_[h_sibling].next_sibling_ = hconn_[h].next_sibling_;
        hconn_[h].next_sibling_ = h_sibling;
        hconn_[h_sibling].prev_sibling_ = h;
    }

    //! \brief Allocate a new face, resize face properties accordingly.
    //! \throw AllocationException in case of failure to allocate a new face.
    Face new_face()
    {
        if (faces_size() == PMP_MAX_INDEX - 1)
        {
            auto what = "NonManifoldSurfaceMesh: cannot allocate face, max. index reached";
            throw AllocationException(what);
        }

        fprops_.push_back();
        return Face(static_cast<IndexType>(faces_size()) - 1);
    }

    //!@}

private:
    struct VertexConnectivity
    {
        // an outgoing halfedge per vertex (it will be a boundary halfedge
        // for boundary vertices)
        Halfedge halfedge_;
    };

    struct HalfedgeConnectivity
    {
        Face face_;              // face incident to halfedge
        Vertex to_vertex_;          // vertex the halfedge points to
        Edge edge_;              // edge incident to halfedge
        Halfedge next_halfedge_; // next halfedge
        Halfedge prev_halfedge_; // previous halfedge
        Halfedge prev_sibling_;       // Sibling Halfedge (might be opposing)
        Halfedge next_sibling_;       // Sibling Halfedge (might be opposing)
        Halfedge next_neighbor_from_; // next neighbor from vertex
        Halfedge prev_neighbor_from_; // previous neighbor from vertex
    };

    struct FaceConnectivity
    {
        Halfedge halfedge_; // a halfedge that is part of the face
    };

    struct EdgeConnectivity
    {
        Halfedge halfedge_;
    };

    // make sure that the outgoing halfedge of vertex \p v is a boundary
    // halfedge if \p v is a boundary vertex.
    void adjust_outgoing_halfedge(Vertex v);


    void remove_halfedge_from_neighbors(Halfedge h)
    {
        if (!h.is_valid())
            return;
        if (hconn_[h].prev_neighbor_from_.is_valid() && hconn_[h].prev_neighbor_from_ != h)
        {
            auto prev_temp = hconn_[h].prev_neighbor_from_;
            auto next_temp = hconn_[h].next_neighbor_from_;
            hconn_[prev_temp].next_neighbor_from_ = next_temp;
            hconn_[next_temp].prev_neighbor_from_ = prev_temp;
        }
    }

    void insert_neighbor_halfedge_from(Halfedge h, Halfedge new_h)
    {
        if (!h.is_valid() || !new_h.is_valid())
            return;
        remove_halfedge_from_neighbors(new_h);
        if (hconn_[h].prev_neighbor_from_.is_valid())
        {
            auto h_prev = hconn_[h].prev_neighbor_from_;
            hconn_[h_prev].next_neighbor_from_ = new_h;
            hconn_[new_h].prev_neighbor_from_ = h_prev;
        }
        else
        {
            hconn_[h].next_neighbor_from_ = new_h;
            hconn_[new_h].prev_neighbor_from_ = h;
        }
        hconn_[new_h].next_neighbor_from_ = h;
        hconn_[h].prev_neighbor_from_ = new_h;
    }

    // Helper for halfedge collapse
    void remove_edge_helper(Halfedge h);

    // Helper for halfedge collapse
    void remove_loop_helper(Halfedge h);

    // Remove Halfedge from sibling doubly-linked list
    // Set edge -> halfedge reference if necessary
    void remove_sibling_halfedge(Halfedge h)
    {
        auto prev_sib = prev_sibling_halfedge(h);
        auto next_sib = next_sibling_halfedge(h);
        hconn_[prev_sib].next_sibling_ = next_sib;
        hconn_[next_sib].prev_sibling_ = prev_sib;
        if (econn_[edge(h)].halfedge_ == h)
            econn_[edge(h)].halfedge_ = next_sib;
    }


    void remove_halfedge(Halfedge h)
    {

        // remove halfedge
        Vertex const v1 = from_vertex(h);
        if (halfedge(v1) == h)
            set_halfedge(v1, hconn_[h].next_neighbor_from_);
        remove_halfedge_from_neighbors(h);
        mark_deleted(h);
    }

    // are there any deleted entities?
    inline bool has_garbage() const { return has_garbage_; }

    // io functions that need access to internal details
    friend void read_pmp(NonManifoldSurfaceMesh&, const std::filesystem::path&);
    friend void write_pmp(const NonManifoldSurfaceMesh&, const std::filesystem::path&,
                          const IOFlags&);

    // property containers for each entity type and object
    PropertyContainer vprops_;
    PropertyContainer hprops_;
    PropertyContainer eprops_;
    PropertyContainer fprops_;

    // point coordinates
    VertexProperty<Point> vpoint_;

    // connectivity information
    VertexProperty<VertexConnectivity> vconn_;
    HalfedgeProperty<HalfedgeConnectivity> hconn_;
    FaceProperty<FaceConnectivity> fconn_;
    EdgeProperty<EdgeConnectivity> econn_;

    // markers for deleted entities
    VertexProperty<bool> vdeleted_;
    EdgeProperty<bool> edeleted_;
    HalfedgeProperty<bool> hdeleted_;
    FaceProperty<bool> fdeleted_;

    // numbers of deleted entities
    IndexType deleted_vertices_{0};
    IndexType deleted_edges_{0};
    IndexType deleted_halfedges_{0};
    IndexType deleted_faces_{0};

    // indicate garbage present
    bool has_garbage_{false};

    // helper data for add_face()
    using NextCacheEntry = std::pair<Halfedge, Halfedge>;
    using NextCache = std::vector<NextCacheEntry>;
    std::vector<Vertex> add_face_vertices_;
    std::vector<Halfedge> add_face_halfedges_;
    std::vector<bool> add_face_is_new_;
    std::vector<bool> add_face_needs_adjust_;
    NextCache add_face_next_cache_;
};

//!@}

} // namespace pmp
