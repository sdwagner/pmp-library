// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// SPDX-License-Identifier: MIT

#pragma once

#include "pmp/surface_mesh.h"

namespace pmp {

//! \brief Fill the hole specified by halfedge \p h
//! \details Close simple holes (boundary loops of manifold vertices) by first
//! filling the hole with an angle/area-minimizing triangulation, followed
//! by isometric remeshing, and finished by curvature-minimizing fairing of the
//! filled-in patch.
//! See \cite liepa_2003_filling for details.
//! \pre The specified halfedge is valid.
//! \pre The specified halfedge is a boundary halfedge.
//! \pre The specified halfedge is not adjacent to a non-manifold hole.
//! \throw InvalidInputException in case on of the input preconditions is violated
//! \note This algorithm works on general polygon meshes.
//! \ingroup algorithms
// When enabled, adaptive refinement uses local boundary-edge lengths near the
// loop and transitions to the original mean-length target in the interior.
void fill_hole(SurfaceMesh& mesh, Halfedge h, bool adaptive_refinement = false);

} // namespace pmp
