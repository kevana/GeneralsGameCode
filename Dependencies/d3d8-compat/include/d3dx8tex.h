// Compatibility stub: d3dx8tex.h for non-Windows builds.
#pragma once
#include "d3d8.h"
#include "d3dx8math.h"

// D3DXLoadSurfaceFromSurface - stub (returns D3DERR_INVALIDCALL on non-Windows)
inline HRESULT D3DXLoadSurfaceFromSurface(
    IDirect3DSurface8* pDestSurface, const void* pDestPalette, const RECT* pDestRect,
    IDirect3DSurface8* pSrcSurface, const void* pSrcPalette, const RECT* pSrcRect,
    unsigned long Filter, unsigned long ColorKey)
{
    return D3DERR_INVALIDCALL;
}

// D3DXLoadSurfaceFromMemory - stub
inline HRESULT D3DXLoadSurfaceFromMemory(
    IDirect3DSurface8* pDestSurface, const void* pDestPalette, const RECT* pDestRect,
    const void* pSrcMemory, D3DFORMAT SrcFormat, unsigned int SrcPitch,
    const void* pSrcPalette, const RECT* pSrcRect,
    unsigned long Filter, unsigned long ColorKey)
{
    return D3DERR_INVALIDCALL;
}

// D3DXFilterTexture - stub
inline HRESULT D3DXFilterTexture(IDirect3DBaseTexture8* pBaseTexture, const void* pPalette,
    unsigned int SrcLevel, unsigned long MipFilter)
{
    return D3DERR_INVALIDCALL;
}

// D3DX_FILTER and image info structs
struct D3DXIMAGE_INFO {
    unsigned int Width;
    unsigned int Height;
    unsigned int Depth;
    unsigned int MipLevels;
    D3DFORMAT Format;
    unsigned int ResourceType;
    unsigned int ImageFileFormat;
};

inline HRESULT D3DXGetImageInfoFromFileInMemory(const void* pSrcData, unsigned int SrcDataSize,
    D3DXIMAGE_INFO* pSrcInfo)
{
    return D3DERR_INVALIDCALL;
}

inline HRESULT D3DXCreateTextureFromFileInMemoryEx(
    IDirect3DDevice8* pDevice, const void* pSrcData, unsigned int SrcDataSize,
    unsigned int Width, unsigned int Height, unsigned int MipLevels, unsigned long Usage,
    D3DFORMAT Format, D3DPOOL Pool, unsigned long Filter, unsigned long MipFilter,
    unsigned long ColorKey, D3DXIMAGE_INFO* pSrcInfo, void* pPalette,
    IDirect3DTexture8** ppTexture)
{
    return D3DERR_INVALIDCALL;
}
