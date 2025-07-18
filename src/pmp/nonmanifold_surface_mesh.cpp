// Copyright 2011-2021 the Polygon Mesh Processing Library developers.
// Copyright 2001-2005 by Computer Graphics Group, RWTH Aachen
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/nonmanifold_surface_mesh.h"

#include <ranges>

namespace pmp {

NonManifoldSurfaceMesh::NonManifoldSurfaceMesh()
{
    // allocate standard properties
    // same list is used in operator=() and assign()
    vpoint_ = add_vertex_property<Point>("v:point");
    vconn_ = add_vertex_property<VertexConnectivity>("v:connectivity");
    hconn_ = add_halfedge_property<HalfedgeConnectivity>("h:connectivity");
    fconn_ = add_face_property<FaceConnectivity>("f:connectivity");
    econn_ = add_edge_property<EdgeConnectivity>("e:connectivity");

    vdeleted_ = add_vertex_property<bool>("v:deleted", false);
    edeleted_ = add_edge_property<bool>("e:deleted", false);
    hdeleted_ = add_halfedge_property<bool>("h:deleted", false);
    fdeleted_ = add_face_property<bool>("f:deleted", false);
}

NonManifoldSurfaceMesh::~NonManifoldSurfaceMesh() = default;

NonManifoldSurfaceMesh& NonManifoldSurfaceMesh::operator=(const NonManifoldSurfaceMesh& rhs)
{
    if (this != &rhs)
    {
        // deep copy of property containers
        vprops_ = rhs.vprops_;
        hprops_ = rhs.hprops_;
        eprops_ = rhs.eprops_;
        fprops_ = rhs.fprops_;

        // property handles contain pointers, have to be reassigned
        vpoint_ = vertex_property<Point>("v:point");
        vconn_ = vertex_property<VertexConnectivity>("v:connectivity");
        hconn_ = halfedge_property<HalfedgeConnectivity>("h:connectivity");
        fconn_ = face_property<FaceConnectivity>("f:connectivity");
        econn_ = edge_property<EdgeConnectivity>("e:connectivity");

        vdeleted_ = vertex_property<bool>("v:deleted");
        edeleted_ = edge_property<bool>("e:deleted");
        hdeleted_ = halfedge_property<bool>("h:deleted");
        fdeleted_ = face_property<bool>("f:deleted");

        // how many elements are deleted?
        deleted_vertices_ = rhs.deleted_vertices_;
        deleted_edges_ = rhs.deleted_edges_;
        deleted_halfedges_ = rhs.deleted_halfedges_;
        deleted_faces_ = rhs.deleted_faces_;

        has_garbage_ = rhs.has_garbage_;
    }

    return *this;
}

NonManifoldSurfaceMesh& NonManifoldSurfaceMesh::assign(const NonManifoldSurfaceMesh& rhs)
{
    if (this != &rhs)
    {
        // clear properties
        vprops_.clear();
        hprops_.clear();
        eprops_.clear();
        fprops_.clear();

        // allocate standard properties
        vpoint_ = add_vertex_property<Point>("v:point");
        vconn_ = add_vertex_property<VertexConnectivity>("v:connectivity");
        hconn_ = add_halfedge_property<HalfedgeConnectivity>("h:connectivity");
        fconn_ = add_face_property<FaceConnectivity>("f:connectivity");
        econn_ = add_edge_property<EdgeConnectivity>("e:connectivity");

        vdeleted_ = add_vertex_property<bool>("v:deleted", false);
        edeleted_ = add_edge_property<bool>("e:deleted", false);
        hdeleted_ = add_halfedge_property<bool>("h:deleted", false);
        fdeleted_ = add_face_property<bool>("f:deleted", false);

        // copy properties from other mesh
        vpoint_.array() = rhs.vpoint_.array();
        vconn_.array() = rhs.vconn_.array();
        hconn_.array() = rhs.hconn_.array();
        fconn_.array() = rhs.fconn_.array();
        econn_.array() = rhs.econn_.array();

        vdeleted_.array() = rhs.vdeleted_.array();
        edeleted_.array() = rhs.edeleted_.array();
        hdeleted_.array() = rhs.hdeleted_.array();
        fdeleted_.array() = rhs.fdeleted_.array();

        // resize (needed by property containers)
        vprops_.resize(rhs.vertices_size());
        hprops_.resize(rhs.halfedges_size());
        eprops_.resize(rhs.edges_size());
        fprops_.resize(rhs.faces_size());

        // how many elements are deleted?
        deleted_vertices_ = rhs.deleted_vertices_;
        deleted_edges_ = rhs.deleted_edges_;
        deleted_halfedges_ = rhs.deleted_halfedges_;
        deleted_faces_ = rhs.deleted_faces_;
        has_garbage_ = rhs.has_garbage_;
    }

    return *this;
}

void NonManifoldSurfaceMesh::clear()
{
    // remove all properties
    vprops_.clear();
    hprops_.clear();
    eprops_.clear();
    fprops_.clear();

    // really free their memory
    free_memory();

    // add the standard properties back
    vpoint_ = add_vertex_property<Point>("v:point");
    vconn_ = add_vertex_property<VertexConnectivity>("v:connectivity");
    hconn_ = add_halfedge_property<HalfedgeConnectivity>("h:connectivity");
    fconn_ = add_face_property<FaceConnectivity>("f:connectivity");
    econn_ = add_edge_property<EdgeConnectivity>("e:connectivity");
    vdeleted_ = add_vertex_property<bool>("v:deleted", false);
    edeleted_ = add_edge_property<bool>("e:deleted", false);
    hdeleted_ = add_halfedge_property<bool>("h:deleted", false);
    fdeleted_ = add_face_property<bool>("f:deleted", false);

    // set initial status (as in constructor)
    deleted_vertices_ = 0;
    deleted_edges_ = 0;
    deleted_faces_ = 0;
    deleted_halfedges_ = 0;
    has_garbage_ = false;
}

void NonManifoldSurfaceMesh::free_memory()
{
    vprops_.free_memory();
    hprops_.free_memory();
    eprops_.free_memory();
    fprops_.free_memory();
}

void NonManifoldSurfaceMesh::reserve(size_t nvertices, size_t nedges, size_t nhedges, size_t nfaces)
{
    vprops_.reserve(nvertices);
    hprops_.reserve(nhedges);
    eprops_.reserve(nedges);
    fprops_.reserve(nfaces);
}

Halfedge NonManifoldSurfaceMesh::find_halfedge(Vertex start, Vertex end) const
{
    assert(is_valid(start) && is_valid(end));

    Halfedge h = halfedge(start);
    const Halfedge hh = h;

    if (h.is_valid())
    {
        do
        {
            if (to_vertex(h) == end)
                return h;
            h = next_neighbor_halfedge(h);
        } while (h.is_valid() && h != hh);
    }

    return {};
}

Edge NonManifoldSurfaceMesh::find_edge(Vertex a, Vertex b) const
{
    const Halfedge h = find_halfedge(a, b);
    if (h.is_valid())
        return edge(h);
    const Halfedge hh = find_halfedge(b, a);
    if (hh.is_valid())
        return edge(hh);
    return Edge();
}

Vertex NonManifoldSurfaceMesh::add_vertex(const Point& p)
{
    Vertex v = new_vertex();
    if (v.is_valid())
        vpoint_[v] = p;
    return v;
}

Face NonManifoldSurfaceMesh::add_triangle(Vertex v0, Vertex v1, Vertex v2)
{
    add_face_vertices_.resize(3);
    add_face_vertices_[0] = v0;
    add_face_vertices_[1] = v1;
    add_face_vertices_[2] = v2;
    return add_face(add_face_vertices_);
}

Face NonManifoldSurfaceMesh::add_quad(Vertex v0, Vertex v1, Vertex v2, Vertex v3)
{
    add_face_vertices_.resize(4);
    add_face_vertices_[0] = v0;
    add_face_vertices_[1] = v1;
    add_face_vertices_[2] = v2;
    add_face_vertices_[3] = v3;
    return add_face(add_face_vertices_);
}

Face NonManifoldSurfaceMesh::add_face(const std::vector<Vertex>& vertices)
{
    const size_t n(vertices.size());
    assert(n > 2);

    size_t i, ii;

    // use global arrays to avoid new/delete of local arrays!!!
    std::vector<Halfedge>& halfedges = add_face_halfedges_;
    std::vector<bool>& is_new = add_face_is_new_;
    halfedges.clear();
    halfedges.resize(n);
    is_new.clear();
    is_new.resize(n);

    // test for topological errors
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        auto e = find_edge(vertices[i], vertices[ii]);
        is_new[i] = !e.is_valid();
    }

    // create missing edges
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        if (is_new[i])
        {
            halfedges[i] = new_edge(vertices[i], vertices[ii]);
        }
        else
        {
            halfedges[i] = new_halfedge(vertices[i], vertices[ii]);
        }
    }

    // create the face
    Face f(new_face());
    set_halfedge(f, halfedges[n - 1]);

    // setup halfedges
    for (i = 0, ii = 1; i < n; ++i, ++ii, ii %= n)
    {
        set_next_halfedge(halfedges[i], halfedges[ii]);
        // set face handle
        set_face(halfedges[i], f);

        if (!halfedge(vertices[ii]).is_valid())
            set_halfedge(vertices[ii], halfedges[ii]);
        else
            insert_neighbor_halfedge(halfedge(vertices[ii]), halfedges[ii]);
    }

    return f;
}

size_t NonManifoldSurfaceMesh::valence(Vertex v) const
{
    auto vv = vertices(v);
    return std::distance(vv.begin(), vv.end());
}

size_t NonManifoldSurfaceMesh::valence(Face f) const
{
    auto vv = vertices(f);
    return std::distance(vv.begin(), vv.end());
}

size_t NonManifoldSurfaceMesh::valence(Edge e) const
{
    auto h = halfedges(e);
    return std::distance(h.begin(), h.end());
}

bool NonManifoldSurfaceMesh::is_triangle_mesh() const
{
    for (auto f : faces())
        if (valence(f) != 3)
            return false;

    return true;
}

bool NonManifoldSurfaceMesh::is_quad_mesh() const
{
    for (auto f : faces())
        if (valence(f) != 4)
            return false;

    return true;
}

void NonManifoldSurfaceMesh::split(Face f, Vertex v)
{
    // Split an arbitrary face into triangles by connecting each vertex of face
    // f to vertex v . Face f will remain valid (it will become one of the
    // triangles). The halfedge handles of the new triangles will point to the
    // old halfedges.

    const Halfedge hend = halfedge(f);
    Halfedge h = next_halfedge(hend);

    Halfedge hold = new_edge(to_vertex(hend), v);

    set_next_halfedge(hend, hold);
    set_face(hold, f);
    insert_neighbor_halfedge(h, hold);

    hold = new_halfedge(v, to_vertex(hend));

    if (halfedge(v).is_valid())
        insert_neighbor_halfedge(halfedge(v), hold);
    else
        set_halfedge(v, hold);

    while (h != hend)
    {
        const Halfedge hnext = next_halfedge(h);

        const Face fnew = new_face();
        set_halfedge(fnew, h);

        const Halfedge hnew = new_edge(to_vertex(h), v);
        insert_neighbor_halfedge(hnext, hnew);

        set_next_halfedge(hnew, hold);
        set_next_halfedge(hold, h);
        set_next_halfedge(h, hnew);

        set_face(hnew, fnew);
        set_face(hold, fnew);
        set_face(h, fnew);

        hold = new_halfedge(v, to_vertex(h));
        insert_neighbor_halfedge(halfedge(v), hold);

        h = hnext;
    }

    set_next_halfedge(hold, hend);
    set_next_halfedge(next_halfedge(hend), hold);

    set_face(hold, f);
}

Halfedge NonManifoldSurfaceMesh::split(Edge e, Vertex v)
{
    bool edge_exists = false;
    const Halfedge h = halfedge(e);
    Halfedge iter_h = h;
    Halfedge new_h;
    const Vertex v1 = to_vertex(iter_h);
    assert(next_halfedge(next_halfedge(h)) == prev_halfedge(h) && "face needs to be triangle");

    do
    {
        const bool correct_orient = to_vertex(iter_h) == v1;
        if (edge_exists)
        {
            if (correct_orient)
            {
                new_h = new_halfedge(v, v1);
                insert_neighbor_halfedge(halfedge(v), new_h);
            }
            else
            {
                new_h = new_halfedge(v1, v);
                insert_neighbor_halfedge(halfedge(v1), new_h);
                set_halfedge(v1, new_h);
            }
        }
        else
        {
            new_h = new_edge(v, v1);
            if (halfedge(v).is_valid())
                insert_neighbor_halfedge(halfedge(v), new_h);
            else
                set_halfedge(v, new_h);
            edge_exists = true;
        }

        const Face f0 = face(iter_h);
        const Face f1 = new_face();

        const Halfedge next_h = next_halfedge(iter_h);
        const Halfedge prev_h = prev_halfedge(iter_h);

        const Vertex opp_v = to_vertex(next_h);
        const Halfedge split_h1 = new_edge(v, opp_v);
        insert_neighbor_halfedge(halfedge(v), split_h1);
        const Halfedge split_h2 = new_halfedge(opp_v, v);
        insert_neighbor_halfedge(prev_h, split_h2);

        if (correct_orient)
        {
            set_vertex(iter_h, v);

            // Relink first triangle
            set_next_halfedge(iter_h, split_h1);
            set_next_halfedge(split_h1, prev_h);

            // Relink second triangle
            set_next_halfedge(new_h, next_h);
            set_next_halfedge(split_h2, new_h);
            set_next_halfedge(next_h, split_h2);

            set_halfedge(f0, iter_h);
            set_halfedge(f1, new_h);

            set_face(split_h1, f0);
            set_face(split_h2, f1);
            set_face(next_h, f1);
            set_face(new_h, f1);
        }
        else
        {
            insert_neighbor_halfedge(split_h1, iter_h);

            // Relink first triangle
            set_next_halfedge(split_h2, iter_h);
            set_next_halfedge(next_h, split_h2);

            // Relink second triangle
            set_next_halfedge(prev_h, new_h);
            set_next_halfedge(new_h, split_h1);
            set_next_halfedge(split_h1, prev_h);

            set_halfedge(f0, iter_h);
            set_halfedge(f1, new_h);

            set_face(split_h2, f0);
            set_face(split_h1, f1);
            set_face(prev_h, f1);
            set_face(new_h, f1);
        }


        iter_h = next_sibling_halfedge(iter_h);
    } while (iter_h.is_valid() && h != iter_h);
    return h;
}

Halfedge NonManifoldSurfaceMesh::insert_vertex(Halfedge h0, Vertex v)
{
    bool edge_exists = false;
    Halfedge iter_h = h0;
    Halfedge new_h;
    const Vertex v1 = to_vertex(iter_h);

    do
    {
        const bool same_orientation = v1 == to_vertex(iter_h);
        if (edge_exists)
            new_h = same_orientation ? new_halfedge(v, v1) : new_halfedge(v1, v);
        else
        {
            new_h = new_edge(v, v1);
            edge_exists = true;
        }

        const Face f0 = face(iter_h);
        set_face(new_h, f0);

        if (same_orientation)
        {
            if (halfedge(v).is_valid())
                insert_neighbor_halfedge(halfedge(v), new_h);
            else
                set_halfedge(v, new_h);
            const Halfedge next_h = next_halfedge(iter_h);
            set_next_halfedge(new_h, next_h);
            set_next_halfedge(iter_h, new_h);

            set_vertex(iter_h, v);
        }
        else
        {
            if (halfedge(v1).is_valid())
                insert_neighbor_halfedge(halfedge(v1), new_h);
            set_halfedge(v1, new_h);

            const Halfedge prev_h = prev_halfedge(iter_h);
            set_next_halfedge(prev_h, new_h);
            set_next_halfedge(new_h, iter_h);

            insert_neighbor_halfedge(halfedge(v), iter_h);
        }

        iter_h = next_sibling_halfedge(iter_h);
    } while (iter_h.is_valid() && h0 != iter_h);
    return h0;
}


Halfedge NonManifoldSurfaceMesh::insert_edge(Halfedge h0, Halfedge h1)
{
    assert(face(h0) == face(h1));
    assert(face(h0).is_valid());

    const Vertex v0 = to_vertex(h0);
    const Vertex v1 = to_vertex(h1);

    const Halfedge h2 = next_halfedge(h0);
    const Halfedge h3 = next_halfedge(h1);

    Halfedge h4 = new_edge(v0, v1);
    insert_neighbor_halfedge(h2, h4);
    Halfedge h5 = new_halfedge(v1, v0);
    insert_neighbor_halfedge(h3, h5);

    const Face f0 = face(h0);
    const Face f1 = new_face();

    set_halfedge(f0, h0);
    set_halfedge(f1, h1);

    set_next_halfedge(h0, h4);
    set_next_halfedge(h4, h3);
    set_face(h4, f0);

    set_next_halfedge(h1, h5);
    set_next_halfedge(h5, h2);
    Halfedge h = h2;
    do
    {
        set_face(h, f1);
        h = next_halfedge(h);
    } while (h != h2);

    return h4;
}

bool NonManifoldSurfaceMesh::is_flip_ok(Edge e) const
{
    // boundary edges cannot be flipped
    if (is_boundary(e))
        return false;

    // Non-Manifold Edges cannot be flipped
    if (valence(e) > 2)
        return false;

    // Only triangles
    for (auto f : faces(e))
        if (valence(f) > 3)
            return false;

    // check if the flipped edge is already present in the mesh
    const Halfedge h0 = halfedge(e, 0);
    const Halfedge h1 = halfedge(e, 1);

    const Vertex v0 = to_vertex(next_halfedge(h0));
    const Vertex v1 = to_vertex(next_halfedge(h1));

    if (v0 == v1) // this is generally a bad sign !!!
        return false;

    if (find_edge(v0, v1).is_valid())
        return false;

    return true;
}

void NonManifoldSurfaceMesh::flip(Edge e)
{
    //let's make it sure it is actually checked
    assert(is_flip_ok(e));

    const Halfedge a0 = halfedge(e, 0);
    const Halfedge b0 = halfedge(e, 1);

    const bool same_orientation = have_same_orientation(a0, b0);

    const Halfedge a1 = next_halfedge(a0);
    const Halfedge a2 = next_halfedge(a1);

    const Halfedge b1 = next_halfedge(b0);
    const Halfedge b2 = next_halfedge(b1);

    const Vertex va0 = to_vertex(a0);
    const Vertex va1 = to_vertex(a1);

    const Vertex vb0 = from_vertex(a0);
    const Vertex vb1 = to_vertex(b1);

    const Face fa = face(a0);
    const Face fb = face(b0);

    if (same_orientation)
    {
        set_vertex(b1, va0);

        set_vertex(b2, vb1);

        set_vertex(a0, vb1);

        set_vertex(b0, va1);

        insert_neighbor_halfedge(b2, b0);
        insert_neighbor_halfedge(b2, b1);
        insert_neighbor_halfedge(a0, b2);
        insert_neighbor_halfedge(a2, a0);

        set_next_halfedge(a0, b1);
        set_next_halfedge(b1, a1);
        set_next_halfedge(a1, a0);

        set_next_halfedge(a2, b2);
        set_next_halfedge(b2, b0);
        set_next_halfedge(b0, a2);

        set_face(a1, fa);
        set_face(b1, fa);

        set_face(a2, fb);
        set_face(b2, fb);

        set_halfedge(fa, a0);
        set_halfedge(fb, b0);

        set_halfedge(va0, a1);
        set_halfedge(vb0, b2);
        set_halfedge(va1, a0);
        set_halfedge(vb1, b1);

    }
    else
    {

        set_vertex(a0, vb1);

        set_vertex(b0, va1);

        insert_neighbor_halfedge(b2, b0);
        insert_neighbor_halfedge(a2, a0);

        set_next_halfedge(a0, b2);
        set_next_halfedge(b2, a1);
        set_next_halfedge(a1, a0);

        set_next_halfedge(a2, b1);
        set_next_halfedge(b1, b0);
        set_next_halfedge(b0, a2);

        set_face(a1, fa);
        set_face(b2, fa);

        set_face(a2, fb);
        set_face(b1, fb);

        set_halfedge(fa, a0);
        set_halfedge(fb, b0);

        set_halfedge(va0, a1);
        set_halfedge(vb0, b1);
        set_halfedge(va1, a0);
        set_halfedge(vb1, b0);

    }
}


bool NonManifoldSurfaceMesh::is_collapse_ok(Halfedge v0v1) const
{
    Halfedge h1, h2;

    std::vector<Vertex> vertices;

    auto iter_h = v0v1;

    vertices.push_back(to_vertex(v0v1));
    vertices.push_back(from_vertex(v0v1));

    do
    {
        h1 = next_halfedge(iter_h);
        vertices.push_back(to_vertex(h1));
        h2 = next_halfedge(h1);
        if (next_halfedge(h2) == iter_h && is_boundary(edge(h1)) && is_boundary(edge(h2)))
            return false;
        iter_h = next_sibling_halfedge(iter_h);
    } while (iter_h != v0v1);

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        for (size_t j = i + 1; j < vertices.size(); ++j)
        {
            if (vertices[i] == vertices[j])
                return false;
        }
    }

    // passed all tests
    return true;
}



bool NonManifoldSurfaceMesh::is_removal_ok(Edge e) const
{
    if (valence(e) != 2)
        return false;

    const Halfedge h0 = halfedge(e, 0);
    const Halfedge h1 = halfedge(e, 1);
    const Vertex v0 = to_vertex(h0);
    const Vertex v1 = from_vertex(h0);
    const Face f0 = face(h0);
    const Face f1 = face(h1);

    if (have_same_orientation(h0, h1))
        return false;

    // same face?
    if (f0 == f1)
        return false;

    // are the two faces connect through another vertex?
    for (auto v : vertices(f0))
        if (v != v0 && v != v1)
            for (auto f : faces(v))
                if (f == f1)
                    return false;

    return true;
}

bool NonManifoldSurfaceMesh::remove_edge(Edge e)
{
    if (!is_removal_ok(e))
        return false;

    const Halfedge h0 = halfedge(e, 0);
    const Halfedge h1 = halfedge(e, 1);


    const Vertex v0 = to_vertex(h0);
    const Vertex v1 = from_vertex(h0);

    const Face f0 = face(h0);
    const Face f1 = face(h1);

    const Halfedge h0_prev = prev_halfedge(h0);
    const Halfedge h0_next = next_halfedge(h0);
    const Halfedge h1_prev = prev_halfedge(h1);
    const Halfedge h1_next = next_halfedge(h1);

    remove_halfedge_from_neighbors(h0);
    remove_halfedge_from_neighbors(h1);

    // adjust vertex->halfedge
    if (halfedge(v0) == h1)
        set_halfedge(v0, h0_next);
    if (halfedge(v1) == h0)
        set_halfedge(v1, h1_next);

    // adjust halfedge->face
    for (auto h : halfedges(f0))
        set_face(h, f1);

    // adjust halfedge->halfedge
    set_next_halfedge(h1_prev, h0_next);
    set_next_halfedge(h0_prev, h1_next);

    // adjust face->halfedge
    if (halfedge(f1) == h1)
        set_halfedge(f1, h1_next);

    // delete face f0 and edge e
    mark_deleted(f0);
    mark_deleted(e);
    mark_deleted(h0);
    mark_deleted(h1);

    has_garbage_ = true;

    return true;
}


void NonManifoldSurfaceMesh::collapse(Halfedge h)
{

    const Edge e = edge(h);

    const Vertex vh = to_vertex(h);
    const Vertex vo = from_vertex(h);


    // map all halfedges from vo -> vh
    auto iter = edges(vo);
    std::vector const edge_cache(iter.begin(), iter.end());
    for (const Edge el : edge_cache)
    {

        for (auto hl : halfedges(el))
        {
            if (to_vertex(hl) == vo)
                set_vertex(hl, vh);
            else
            {
                if (halfedge(vh).is_valid())
                    insert_neighbor_halfedge(halfedge(vh), hl);
                else
                    set_halfedge(vh, hl);
            }
        }
    }

    for (const Halfedge hl : halfedges(e))
    {
        Halfedge const prev_h = prev_halfedge(hl);
        Halfedge const next_h = next_halfedge(hl);
        assert(prev_h != next_h && prev_h != hl);

        Face const f = face(hl);

        // remove triangle and one edge
        if (next_halfedge(next_h) == prev_h)
        {
            Edge const prev_e = edge(prev_h);
            Edge const next_e = edge(next_h);



            remove_sibling_halfedge(prev_h);
            remove_sibling_halfedge(next_h);

            // Combine sibling structure
            if (halfedge(prev_e) != prev_h) // prev_e was not boundary
            {
                if (halfedge(next_e) != next_h) // next_e was not boundary
                {
                    Halfedge const out_prev_h = halfedge(prev_e);
                    auto const hedge_cache = std::vector(halfedges(next_e).begin(), halfedges(next_e).end());
                    for (const auto next_hedge : hedge_cache)
                    {
                        set_edge(next_hedge, prev_e);
                        insert_sibling(out_prev_h, next_hedge);
                    }
                }
                mark_deleted(next_e);
            }
            else
            {
                assert(halfedge(next_e) != next_h && "We dont want to get rid of both edges");
                mark_deleted(prev_e);
            }

            remove_halfedge(prev_h);
            remove_halfedge(next_h);

            mark_deleted(f);
            set_halfedge(f, Halfedge());

        }
        else
        {
            set_halfedge(f, next_h);
            set_next_halfedge(prev_h, next_h);
        }

        remove_halfedge(hl);
    }

    // delete stuff
    mark_deleted(e);
    set_halfedge(e, Halfedge());
    mark_deleted(vo);
    set_halfedge(vo, Halfedge());


    // Non-elegant solution for triangular holes adjacent to edge
    for (auto e1 : edges(vh))
    {
        auto vi = vertex(e1, 0) == vh ? vertex(e1, 1) : vertex(e1, 0);
        for (auto e2 : edges(vi))
        {
            if (e1 == e2)
                continue;
            auto h1 = halfedge(e1);
            auto h2 = halfedge(e2);
            if ((to_vertex(h2) == to_vertex(h1) && from_vertex(h2) == from_vertex(h1))
                || (to_vertex(h2) == from_vertex(h1) && from_vertex(h2) == to_vertex(h1)))
            {
                // merge sibling lists
                auto const hedge_cache = std::vector(halfedges(e2).begin(), halfedges(e2).end());
                for (const auto next_hedge : hedge_cache)
                {
                    set_edge(next_hedge, e1);
                    insert_sibling(h1, next_hedge);
                }
                mark_deleted(e2);
                set_halfedge(e2, Halfedge());
                break;
            }
        }
    }

    has_garbage_ = true;
}

void NonManifoldSurfaceMesh::delete_vertex(Vertex v)
{
    if (is_deleted(v))
        return;

    // collect incident faces
    std::vector<Face> incident_faces;
    incident_faces.reserve(6);

    for (auto f : faces(v))
        incident_faces.push_back(f);

    // delete incident faces
    for (auto f : incident_faces)
        delete_face(f);

    // mark v as deleted if not yet done by delete_face()
    mark_deleted(v);
    has_garbage_ = true;
}

void NonManifoldSurfaceMesh::delete_edge(Edge e)
{
    if (is_deleted(e))
        return;

    const std::vector face_vec(faces(e).begin(), faces(e).end());

    for (auto f : face_vec)
    {
        if (f.is_valid())
            delete_face(f);
    }
}

void NonManifoldSurfaceMesh::delete_face(Face f)
{
    if (fdeleted_[f])
        return;

    // mark face deleted
    mark_deleted(f);

    // boundary edges of face f to be deleted
    std::vector<Edge> deleted_edges;
    deleted_edges.reserve(3);

    std::vector<Halfedge> deleted_halfedges;
    deleted_halfedges.reserve(3);

    // for all halfedges of face f do:
    //   1) invalidate face handle.
    //   2) collect all boundary halfedges, set them deleted
    //   3) store vertex handles
    for (auto hc : halfedges(f))
    {
        if (is_boundary(edge(hc)))
            deleted_edges.push_back(edge(hc));
        else
            deleted_halfedges.push_back(hc);

    }

    // delete all collected (half)edges
    // delete isolated vertices
    for (const auto& e : deleted_edges)
    {

        auto h0 = halfedge(e);
        const auto v1 = from_vertex(h0);

        mark_deleted(e);
        mark_deleted(h0);

        // update v1
        if (halfedge(v1) == h0)
        {
            if (next_neighbor_halfedge(h0) == h0)
            {
                mark_deleted(v1);
            }
            else
                set_halfedge(v1, next_neighbor_halfedge(h0));
        }
        remove_halfedge_from_neighbors(h0);
    }

    for (auto &h : deleted_halfedges)
    {

        remove_sibling_halfedge(h);
        remove_halfedge(h);
    }

    has_garbage_ = true;
}


void NonManifoldSurfaceMesh::garbage_collection()
{
    if (!has_garbage_)
        return;

    auto nv = vertices_size();
    auto ne = edges_size();
    auto nh = halfedges_size();
    auto nf = faces_size();

    // setup handle mapping
    VertexProperty<Vertex> vmap =
        add_vertex_property<Vertex>("v:garbage-collection");
    HalfedgeProperty<Halfedge> hmap =
        add_halfedge_property<Halfedge>("h:garbage-collection");
    FaceProperty<Face> fmap = add_face_property<Face>("f:garbage-collection");
    EdgeProperty<Edge> emap = add_edge_property<Edge>("e:garbage-collection");

    for (size_t i = 0; i < nv; ++i)
        vmap[Vertex(i)] = Vertex(i);
    for (size_t i = 0; i < nh; ++i)
        hmap[Halfedge(i)] = Halfedge(i);
    for (size_t i = 0; i < nf; ++i)
        fmap[Face(i)] = Face(i);
    for (size_t i = 0; i < ne; ++i)
        emap[Edge(i)] = Edge(i);

    // remove deleted vertices
    if (nv > 0)
    {
        size_t i0 = 0;
        size_t i1 = nv - 1;

        while (true)
        {
            // find first deleted and last un-deleted
            while (!vdeleted_[Vertex(i0)] && i0 < i1)
                ++i0;
            while (vdeleted_[Vertex(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            vprops_.swap(i0, i1);
        }

        // remember new size
        nv = vdeleted_[Vertex(i0)] ? i0 : i0 + 1;
    }

    // remove deleted edges
    if (ne > 0)
    {
        size_t i0 = 0;
        size_t i1 = ne - 1;

        while (true)
        {
            // find first deleted and last un-deleted
            while (!edeleted_[Edge(i0)] && i0 < i1)
                ++i0;
            while (edeleted_[Edge(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            eprops_.swap(i0, i1);
        }

        // remember new size
        ne = edeleted_[Edge(i0)] ? i0 : i0 + 1;
    }

    // remove deleted edges
    if (nh > 0)
    {
        size_t i0 = 0;
        size_t i1 = nh - 1;

        while (true)
        {
            // find first deleted and last un-deleted
            while (!hdeleted_[Halfedge(i0)] && i0 < i1)
                ++i0;
            while (hdeleted_[Halfedge(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            hprops_.swap(i0, i1);
        }

        // remember new size
        nh = hdeleted_[Halfedge(i0)] ? i0 : i0 + 1;
    }

    // remove deleted faces
    if (nf > 0)
    {
        size_t i0 = 0;
        size_t i1 = nf - 1;

        while (true)
        {
            // find 1st deleted and last un-deleted
            while (!fdeleted_[Face(i0)] && i0 < i1)
                ++i0;
            while (fdeleted_[Face(i1)] && i0 < i1)
                --i1;
            if (i0 >= i1)
                break;

            // swap
            fprops_.swap(i0, i1);
        }

        // remember new size
        nf = fdeleted_[Face(i0)] ? i0 : i0 + 1;
    }

    // update vertex connectivity
    for (size_t i = 0; i < nv; ++i)
    {
        auto v = Vertex(i);
        if (!is_isolated(v))
            set_halfedge(v, hmap[halfedge(v)]);
    }

    // update halfedge connectivity
    for (size_t i = 0; i < nh; ++i)
    {
        auto h = Halfedge(i);
        set_vertex(h, vmap[to_vertex(h)]);
        set_face(h, fmap[face(h)]);
        set_edge(h, emap[edge(h)]);
        set_next_halfedge(h, hmap[next_halfedge(h)]);
        set_next_neighbor_halfedge(h, hmap[next_neighbor_halfedge(h)]);
        set_next_sibling_halfedge(h, hmap[next_sibling_halfedge(h)]);

    }

    for (size_t i = 0; i < ne; ++i)
    {
        const auto e = Edge(i);
        set_halfedge(e, hmap[halfedge(e)]);

    }

    // update handles of faces
    for (size_t i = 0; i < nf; ++i)
    {
        auto f = Face(i);
        set_halfedge(f, hmap[halfedge(f)]);
    }

    // remove handle maps
    remove_vertex_property(vmap);
    remove_halfedge_property(hmap);
    remove_face_property(fmap);
    remove_edge_property(emap);

    // finally resize arrays
    vprops_.resize(nv);
    vprops_.free_memory();
    hprops_.resize(nh);
    hprops_.free_memory();
    eprops_.resize(ne);
    eprops_.free_memory();
    fprops_.resize(nf);
    fprops_.free_memory();

    deleted_vertices_ = deleted_edges_ = deleted_faces_ = deleted_halfedges_ = 0;
    has_garbage_ = false;
}


} // namespace pmp
