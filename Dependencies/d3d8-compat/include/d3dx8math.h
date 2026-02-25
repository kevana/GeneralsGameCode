// Compatibility stub: d3dx8math.h for non-Windows builds.
// D3DX math types used by WW3D/WWMath are aliased to the base D3D8 types.
#pragma once
#include "d3d8.h"
#include <cstring>

// D3DX constants
#ifndef D3DX_PI
#define D3DX_PI  3.141592653589793238f
#endif

// D3DX filter flags (used with D3DXFilterTexture etc.)
#ifndef D3DX_FILTER_NONE
#define D3DX_FILTER_NONE        (1 << 0)
#define D3DX_FILTER_POINT       (2 << 0)
#define D3DX_FILTER_LINEAR      (3 << 0)
#define D3DX_FILTER_TRIANGLE    (4 << 0)
#define D3DX_FILTER_BOX         (5 << 0)
#define D3DX_DEFAULT            ((unsigned long)-1)
#define D3DX_DEFAULT_NONPOW2    ((unsigned long)-2)
#define D3DX_DEFAULT_FLOAT      3.402823466e+38f
#endif

// D3DXMATRIX is layout-compatible with D3DMATRIX.
// Declared as a struct so it is compatible with the forward declarations in matrix3d.h/matrix4.h.
struct D3DXMATRIX : public _D3DMATRIX {
    D3DXMATRIX() = default;
    D3DXMATRIX(float m11, float m12, float m13, float m14,
               float m21, float m22, float m23, float m24,
               float m31, float m32, float m33, float m34,
               float m41, float m42, float m43, float m44)
    {
        _11=m11; _12=m12; _13=m13; _14=m14;
        _21=m21; _22=m22; _23=m23; _24=m24;
        _31=m31; _32=m32; _33=m33; _34=m34;
        _41=m41; _42=m42; _43=m43; _44=m44;
    }
};

// D3DXVECTOR3 - 3-component float vector
struct D3DXVECTOR3 {
    float x, y, z;
    D3DXVECTOR3() = default;
    D3DXVECTOR3(float x, float y, float z) : x(x), y(y), z(z) {}
    float  operator[](int i) const { return (&x)[i]; }
    float& operator[](int i)       { return (&x)[i]; }
};

// D3DXVECTOR4 - 4-component float vector
struct D3DXVECTOR4 {
    float x, y, z, w;
    D3DXVECTOR4() = default;
    D3DXVECTOR4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    float  operator[](int i) const { return (&x)[i]; }
    float& operator[](int i)       { return (&x)[i]; }
};

// D3DXMATRIX operator*
inline D3DXMATRIX operator*(const D3DXMATRIX& a, const D3DXMATRIX& b)
{
    D3DXMATRIX out;
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++)
                sum += a.m[r][k] * b.m[k][c];
            out.m[r][c] = sum;
        }
    return out;
}

// D3DXVec4Transform: transform 4D vector by 4x4 matrix (row-major)
inline D3DXVECTOR4* D3DXVec4Transform(D3DXVECTOR4* pOut, const D3DXVECTOR4* pV, const D3DXMATRIX* pM)
{
    float x = pV->x * pM->m[0][0] + pV->y * pM->m[1][0] + pV->z * pM->m[2][0] + pV->w * pM->m[3][0];
    float y = pV->x * pM->m[0][1] + pV->y * pM->m[1][1] + pV->z * pM->m[2][1] + pV->w * pM->m[3][1];
    float z = pV->x * pM->m[0][2] + pV->y * pM->m[1][2] + pV->z * pM->m[2][2] + pV->w * pM->m[3][2];
    float w = pV->x * pM->m[0][3] + pV->y * pM->m[1][3] + pV->z * pM->m[2][3] + pV->w * pM->m[3][3];
    pOut->x = x; pOut->y = y; pOut->z = z; pOut->w = w;
    return pOut;
}

// D3DXVec3Transform: transform 3D vector (w=1) by 4x4 matrix, result is 4D
inline D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM)
{
    float x = pV->x * pM->m[0][0] + pV->y * pM->m[1][0] + pV->z * pM->m[2][0] + pM->m[3][0];
    float y = pV->x * pM->m[0][1] + pV->y * pM->m[1][1] + pV->z * pM->m[2][1] + pM->m[3][1];
    float z = pV->x * pM->m[0][2] + pV->y * pM->m[1][2] + pV->z * pM->m[2][2] + pM->m[3][2];
    float w = pV->x * pM->m[0][3] + pV->y * pM->m[1][3] + pV->z * pM->m[2][3] + pM->m[3][3];
    pOut->x = x; pOut->y = y; pOut->z = z; pOut->w = w;
    return pOut;
}

// D3DXVec4Dot: dot product of two 4D vectors
inline float D3DXVec4Dot(const D3DXVECTOR4* pV1, const D3DXVECTOR4* pV2)
{
    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z + pV1->w * pV2->w;
}

// D3DXMatrixTranspose: transpose a 4x4 matrix
inline D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* pOut, const D3DXMATRIX* pM)
{
    D3DXMATRIX tmp;
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            tmp.m[r][c] = pM->m[c][r];
    *pOut = tmp;
    return pOut;
}

// D3DXMatrixRotationZ: rotation matrix around Z axis
#include <cmath>
inline D3DXMATRIX* D3DXMatrixRotationZ(D3DXMATRIX* pOut, float angle)
{
    float s = sinf(angle), c = cosf(angle);
    D3DXMATRIX m;
    memset(&m, 0, sizeof(m));
    m._11 = c;  m._12 = s;
    m._21 = -s; m._22 = c;
    m._33 = 1.0f; m._44 = 1.0f;
    *pOut = m;
    return pOut;
}

// D3DXMatrixRotationX: rotation matrix around X axis
inline D3DXMATRIX* D3DXMatrixRotationX(D3DXMATRIX* pOut, float angle)
{
    float s = sinf(angle), c = cosf(angle);
    D3DXMATRIX m;
    memset(&m, 0, sizeof(m));
    m._11 = 1.0f; m._22 = c;  m._23 = s;
    m._32 = -s;   m._33 = c;  m._44 = 1.0f;
    *pOut = m;
    return pOut;
}

// D3DXMatrixRotationY: rotation matrix around Y axis
inline D3DXMATRIX* D3DXMatrixRotationY(D3DXMATRIX* pOut, float angle)
{
    float s = sinf(angle), c = cosf(angle);
    D3DXMATRIX m;
    memset(&m, 0, sizeof(m));
    m._11 = c;  m._13 = -s;
    m._22 = 1.0f;
    m._31 = s;  m._33 = c;  m._44 = 1.0f;
    *pOut = m;
    return pOut;
}

// D3DXMatrixIdentity: set matrix to identity
inline D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* pOut)
{
    memset(pOut, 0, sizeof(*pOut));
    pOut->_11 = pOut->_22 = pOut->_33 = pOut->_44 = 1.0f;
    return pOut;
}

// D3DXMatrixMultiply: multiply two matrices
inline D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pA, const D3DXMATRIX* pB)
{
    *pOut = (*pA) * (*pB);
    return pOut;
}
