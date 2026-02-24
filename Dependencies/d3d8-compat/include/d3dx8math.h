// Compatibility stub: d3dx8math.h for non-Windows builds.
// D3DX math types used by WW3D/WWMath are aliased to the base D3D8 types.
#pragma once
#include "d3d8.h"

// D3DXMATRIX is layout-compatible with D3DMATRIX.
// Declared as a struct so it is compatible with the forward declarations in matrix3d.h/matrix4.h.
struct D3DXMATRIX : public _D3DMATRIX {};
