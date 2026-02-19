/*
**	Command & Conquer Generals / Zero Hour
**	D3D8 API compatibility header for non-Windows platforms
**
**	This header provides the Direct3D 8 type definitions, enums, structs,
**	and stub COM interface declarations needed to compile the WW3D2 rendering
**	code on non-Windows platforms. On Windows the real d3d8.h from the
**	min-dx8-sdk is used instead.
*/

#pragma once

#ifdef _WIN32
#error "This d3d8 compatibility header should only be used on non-Windows platforms"
#endif

#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// Windows types not provided by compat.h
// ---------------------------------------------------------------------------

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef long HRESULT;
#endif

#ifndef SUCCEEDED
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif
#ifndef FAILED
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif

#ifndef S_OK
#define S_OK ((HRESULT)0L)
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef LPVOID
typedef void* LPVOID;
#endif

#ifndef WINAPI
#define WINAPI
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
    ((unsigned long)(unsigned char)(ch0) | ((unsigned long)(unsigned char)(ch1) << 8) | \
    ((unsigned long)(unsigned char)(ch2) << 16) | ((unsigned long)(unsigned char)(ch3) << 24))
#endif

#ifndef HWND
typedef void* HWND;
#endif

#ifndef HANDLE
typedef void* HANDLE;
#endif

#ifndef HMONITOR
typedef void* HMONITOR;
#endif

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
#endif

#ifndef RECT_DEFINED
#define RECT_DEFINED
typedef struct tagRECT {
    long left;
    long top;
    long right;
    long bottom;
} RECT;
#endif

#ifndef POINT_DEFINED
#define POINT_DEFINED
typedef struct tagPOINT {
    long x;
    long y;
} POINT;
#endif

#ifndef PALETTEENTRY_DEFINED
#define PALETTEENTRY_DEFINED
typedef struct tagPALETTEENTRY {
    unsigned char peRed;
    unsigned char peGreen;
    unsigned char peBlue;
    unsigned char peFlags;
} PALETTEENTRY;
#endif

// ---------------------------------------------------------------------------
// D3D8 return codes
// ---------------------------------------------------------------------------

#define D3D_OK                          S_OK
#define D3DERR_WRONGTEXTUREFORMAT       ((HRESULT)0x88760818L)
#define D3DERR_UNSUPPORTEDCOLOROPERATION ((HRESULT)0x88760819L)
#define D3DERR_UNSUPPORTEDCOLORARG      ((HRESULT)0x8876081AL)
#define D3DERR_UNSUPPORTEDALPHAOPERATION ((HRESULT)0x8876081BL)
#define D3DERR_UNSUPPORTEDALPHAARG      ((HRESULT)0x8876081CL)
#define D3DERR_TOOMANYOPERATIONS        ((HRESULT)0x8876081DL)
#define D3DERR_CONFLICTINGTEXTUREFILTER ((HRESULT)0x8876081EL)
#define D3DERR_UNSUPPORTEDFACTORVALUE   ((HRESULT)0x8876081FL)
#define D3DERR_CONFLICTINGRENDERSTATE   ((HRESULT)0x88760821L)
#define D3DERR_UNSUPPORTEDTEXTUREFILTER ((HRESULT)0x88760822L)
#define D3DERR_CONFLICTINGTEXTUREPALETTE ((HRESULT)0x88760826L)
#define D3DERR_DRIVERINTERNALERROR      ((HRESULT)0x88760827L)
#define D3DERR_NOTFOUND                 ((HRESULT)0x88760866L)
#define D3DERR_MOREDATA                 ((HRESULT)0x88760867L)
#define D3DERR_DEVICELOST               ((HRESULT)0x88760868L)
#define D3DERR_DEVICENOTRESET           ((HRESULT)0x88760869L)
#define D3DERR_NOTAVAILABLE             ((HRESULT)0x8876086AL)
#define D3DERR_OUTOFVIDEOMEMORY         ((HRESULT)0x8876017CL)
#define D3DERR_INVALIDDEVICE            ((HRESULT)0x8876086BL)
#define D3DERR_INVALIDCALL              ((HRESULT)0x8876086CL)

// ---------------------------------------------------------------------------
// D3D color macros
// ---------------------------------------------------------------------------

typedef unsigned long D3DCOLOR;

#ifndef D3DCOLOR_ARGB
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#endif
#ifndef D3DCOLOR_RGBA
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
#endif
#ifndef D3DCOLOR_XRGB
#define D3DCOLOR_XRGB(r,g,b) D3DCOLOR_ARGB(0xff,r,g,b)
#endif

// ---------------------------------------------------------------------------
// D3DCOLORVALUE
// ---------------------------------------------------------------------------

typedef struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;

// ---------------------------------------------------------------------------
// D3DMATRIX
// ---------------------------------------------------------------------------

typedef struct _D3DMATRIX {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DMATRIX;

// ---------------------------------------------------------------------------
// D3DVECTOR
// ---------------------------------------------------------------------------

typedef struct _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

typedef enum _D3DFORMAT {
    D3DFMT_UNKNOWN      = 0,
    D3DFMT_R8G8B8       = 20,
    D3DFMT_A8R8G8B8     = 21,
    D3DFMT_X8R8G8B8     = 22,
    D3DFMT_R5G6B5       = 23,
    D3DFMT_X1R5G5B5     = 24,
    D3DFMT_A1R5G5B5     = 25,
    D3DFMT_A4R4G4B4     = 26,
    D3DFMT_R3G3B2       = 27,
    D3DFMT_A8           = 28,
    D3DFMT_A8R3G3B2     = 29,
    D3DFMT_X4R4G4B4     = 30,
    D3DFMT_A2B10G10R10  = 31,
    D3DFMT_G16R16       = 34,
    D3DFMT_A8P8         = 40,
    D3DFMT_P8           = 41,
    D3DFMT_L8           = 50,
    D3DFMT_A8L8         = 51,
    D3DFMT_A4L4         = 52,
    D3DFMT_V8U8         = 60,
    D3DFMT_L6V5U5       = 61,
    D3DFMT_X8L8V8U8     = 62,
    D3DFMT_Q8W8V8U8     = 63,
    D3DFMT_V16U16       = 64,
    D3DFMT_W11V11U10    = 65,
    D3DFMT_UYVY         = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DFMT_YUY2         = MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DFMT_DXT1         = MAKEFOURCC('D', 'X', 'T', '1'),
    D3DFMT_DXT2         = MAKEFOURCC('D', 'X', 'T', '2'),
    D3DFMT_DXT3         = MAKEFOURCC('D', 'X', 'T', '3'),
    D3DFMT_DXT4         = MAKEFOURCC('D', 'X', 'T', '4'),
    D3DFMT_DXT5         = MAKEFOURCC('D', 'X', 'T', '5'),
    D3DFMT_D16_LOCKABLE = 70,
    D3DFMT_D32          = 71,
    D3DFMT_D15S1        = 73,
    D3DFMT_D24S8        = 75,
    D3DFMT_D16          = 80,
    D3DFMT_D24X8        = 77,
    D3DFMT_D24X4S4      = 79,
    D3DFMT_VERTEXDATA   = 100,
    D3DFMT_INDEX16      = 101,
    D3DFMT_INDEX32      = 102,
    D3DFMT_FORCE_DWORD  = 0x7fffffff
} D3DFORMAT;

typedef enum _D3DPRIMITIVETYPE {
    D3DPT_POINTLIST     = 1,
    D3DPT_LINELIST      = 2,
    D3DPT_LINESTRIP     = 3,
    D3DPT_TRIANGLELIST  = 4,
    D3DPT_TRIANGLESTRIP = 5,
    D3DPT_TRIANGLEFAN   = 6,
    D3DPT_FORCE_DWORD   = 0x7fffffff
} D3DPRIMITIVETYPE;

typedef enum _D3DPOOL {
    D3DPOOL_DEFAULT     = 0,
    D3DPOOL_MANAGED     = 1,
    D3DPOOL_SYSTEMMEM   = 2,
    D3DPOOL_SCRATCH     = 3,
    D3DPOOL_FORCE_DWORD = 0x7fffffff
} D3DPOOL;

typedef enum _D3DMULTISAMPLE_TYPE {
    D3DMULTISAMPLE_NONE         = 0,
    D3DMULTISAMPLE_2_SAMPLES    = 2,
    D3DMULTISAMPLE_FORCE_DWORD  = 0x7fffffff
} D3DMULTISAMPLE_TYPE;

typedef enum _D3DSWAPEFFECT {
    D3DSWAPEFFECT_DISCARD       = 1,
    D3DSWAPEFFECT_FLIP          = 2,
    D3DSWAPEFFECT_COPY          = 3,
    D3DSWAPEFFECT_FORCE_DWORD   = 0x7fffffff
} D3DSWAPEFFECT;

typedef enum _D3DDEVTYPE {
    D3DDEVTYPE_HAL          = 1,
    D3DDEVTYPE_REF          = 2,
    D3DDEVTYPE_SW           = 3,
    D3DDEVTYPE_FORCE_DWORD  = 0x7fffffff
} D3DDEVTYPE;

typedef enum _D3DRENDERSTATETYPE {
    D3DRS_ZENABLE                   = 7,
    D3DRS_FILLMODE                  = 8,
    D3DRS_SHADEMODE                 = 9,
    D3DRS_LINEPATTERN               = 10,
    D3DRS_ZWRITEENABLE              = 14,
    D3DRS_ALPHATESTENABLE           = 15,
    D3DRS_LASTPIXEL                 = 16,
    D3DRS_SRCBLEND                  = 19,
    D3DRS_DESTBLEND                 = 20,
    D3DRS_CULLMODE                  = 22,
    D3DRS_ZFUNC                     = 23,
    D3DRS_ALPHAREF                  = 24,
    D3DRS_ALPHAFUNC                 = 25,
    D3DRS_DITHERENABLE              = 26,
    D3DRS_ALPHABLENDENABLE          = 27,
    D3DRS_FOGENABLE                 = 28,
    D3DRS_SPECULARENABLE            = 29,
    D3DRS_ZVISIBLE                  = 30,
    D3DRS_FOGCOLOR                  = 34,
    D3DRS_FOGTABLEMODE              = 35,
    D3DRS_FOGSTART                  = 36,
    D3DRS_FOGEND                    = 37,
    D3DRS_FOGDENSITY                = 38,
    D3DRS_EDGEANTIALIAS             = 40,
    D3DRS_ZBIAS                     = 47,
    D3DRS_RANGEFOGENABLE            = 48,
    D3DRS_STENCILENABLE             = 52,
    D3DRS_STENCILFAIL               = 53,
    D3DRS_STENCILZFAIL              = 54,
    D3DRS_STENCILPASS               = 55,
    D3DRS_STENCILFUNC               = 56,
    D3DRS_STENCILREF                = 57,
    D3DRS_STENCILMASK               = 58,
    D3DRS_STENCILWRITEMASK          = 59,
    D3DRS_TEXTUREFACTOR             = 60,
    D3DRS_WRAP0                     = 128,
    D3DRS_WRAP1                     = 129,
    D3DRS_WRAP2                     = 130,
    D3DRS_WRAP3                     = 131,
    D3DRS_WRAP4                     = 132,
    D3DRS_WRAP5                     = 133,
    D3DRS_WRAP6                     = 134,
    D3DRS_WRAP7                     = 135,
    D3DRS_CLIPPING                  = 136,
    D3DRS_LIGHTING                  = 137,
    D3DRS_AMBIENT                   = 139,
    D3DRS_FOGVERTEXMODE             = 140,
    D3DRS_COLORVERTEX               = 141,
    D3DRS_LOCALVIEWER               = 142,
    D3DRS_NORMALIZENORMALS          = 143,
    D3DRS_DIFFUSEMATERIALSOURCE     = 145,
    D3DRS_SPECULARMATERIALSOURCE    = 146,
    D3DRS_AMBIENTMATERIALSOURCE     = 147,
    D3DRS_EMISSIVEMATERIALSOURCE    = 148,
    D3DRS_VERTEXBLEND               = 151,
    D3DRS_CLIPPLANEENABLE           = 152,
    D3DRS_SOFTWAREVERTEXPROCESSING  = 153,
    D3DRS_POINTSIZE                 = 154,
    D3DRS_POINTSIZE_MIN             = 155,
    D3DRS_POINTSPRITEENABLE         = 156,
    D3DRS_POINTSCALEENABLE          = 157,
    D3DRS_POINTSCALE_A              = 158,
    D3DRS_POINTSCALE_B              = 159,
    D3DRS_POINTSCALE_C              = 160,
    D3DRS_MULTISAMPLEANTIALIAS      = 161,
    D3DRS_MULTISAMPLEMASK           = 162,
    D3DRS_PATCHEDGESTYLE            = 163,
    D3DRS_PATCHSEGMENTS             = 164,
    D3DRS_DEBUGMONITORTOKEN         = 165,
    D3DRS_POINTSIZE_MAX             = 166,
    D3DRS_INDEXEDVERTEXBLENDENABLE  = 167,
    D3DRS_COLORWRITEENABLE          = 168,
    D3DRS_TWEENFACTOR               = 170,
    D3DRS_BLENDOP                   = 171,
    D3DRS_POSITIONORDER             = 172,
    D3DRS_NORMALORDER               = 173,
    D3DRS_FORCE_DWORD               = 0x7fffffff
} D3DRENDERSTATETYPE;

typedef enum _D3DTEXTURESTAGESTATETYPE {
    D3DTSS_COLOROP          = 1,
    D3DTSS_COLORARG1        = 2,
    D3DTSS_COLORARG2        = 3,
    D3DTSS_ALPHAOP          = 4,
    D3DTSS_ALPHAARG1        = 5,
    D3DTSS_ALPHAARG2        = 6,
    D3DTSS_BUMPENVMAT00     = 7,
    D3DTSS_BUMPENVMAT01     = 8,
    D3DTSS_BUMPENVMAT10     = 9,
    D3DTSS_BUMPENVMAT11     = 10,
    D3DTSS_TEXCOORDINDEX    = 11,
    D3DTSS_ADDRESSU         = 13,
    D3DTSS_ADDRESSV         = 14,
    D3DTSS_BORDERCOLOR      = 15,
    D3DTSS_MAGFILTER        = 16,
    D3DTSS_MINFILTER        = 17,
    D3DTSS_MIPFILTER        = 18,
    D3DTSS_MIPMAPLODBIAS    = 19,
    D3DTSS_MAXMIPLEVEL      = 20,
    D3DTSS_MAXANISOTROPY    = 21,
    D3DTSS_BUMPENVLSCALE    = 22,
    D3DTSS_BUMPENVLOFFSET   = 23,
    D3DTSS_TEXTURETRANSFORMFLAGS = 24,
    D3DTSS_ADDRESSW         = 25,
    D3DTSS_COLORARG0        = 26,
    D3DTSS_ALPHAARG0        = 27,
    D3DTSS_RESULTARG        = 28,
    D3DTSS_FORCE_DWORD      = 0x7fffffff
} D3DTEXTURESTAGESTATETYPE;

typedef enum _D3DTRANSFORMSTATETYPE {
    D3DTS_VIEW          = 2,
    D3DTS_PROJECTION    = 3,
    D3DTS_TEXTURE0      = 16,
    D3DTS_TEXTURE1      = 17,
    D3DTS_TEXTURE2      = 18,
    D3DTS_TEXTURE3      = 19,
    D3DTS_TEXTURE4      = 20,
    D3DTS_TEXTURE5      = 21,
    D3DTS_TEXTURE6      = 22,
    D3DTS_TEXTURE7      = 23,
    D3DTS_WORLD         = 256,
    D3DTS_FORCE_DWORD   = 0x7fffffff
} D3DTRANSFORMSTATETYPE;

typedef enum _D3DLIGHTTYPE {
    D3DLIGHT_POINT          = 1,
    D3DLIGHT_SPOT           = 2,
    D3DLIGHT_DIRECTIONAL    = 3,
    D3DLIGHT_FORCE_DWORD    = 0x7fffffff
} D3DLIGHTTYPE;

typedef enum _D3DCMPFUNC {
    D3DCMP_NEVER            = 1,
    D3DCMP_LESS             = 2,
    D3DCMP_EQUAL            = 3,
    D3DCMP_LESSEQUAL        = 4,
    D3DCMP_GREATER          = 5,
    D3DCMP_NOTEQUAL         = 6,
    D3DCMP_GREATEREQUAL     = 7,
    D3DCMP_ALWAYS           = 8,
    D3DCMP_FORCE_DWORD      = 0x7fffffff
} D3DCMPFUNC;

typedef enum _D3DBLEND {
    D3DBLEND_ZERO               = 1,
    D3DBLEND_ONE                = 2,
    D3DBLEND_SRCCOLOR           = 3,
    D3DBLEND_INVSRCCOLOR        = 4,
    D3DBLEND_SRCALPHA           = 5,
    D3DBLEND_INVSRCALPHA        = 6,
    D3DBLEND_DESTALPHA          = 7,
    D3DBLEND_INVDESTALPHA       = 8,
    D3DBLEND_DESTCOLOR          = 9,
    D3DBLEND_INVDESTCOLOR       = 10,
    D3DBLEND_SRCALPHASAT        = 11,
    D3DBLEND_BOTHSRCALPHA       = 12,
    D3DBLEND_BOTHINVSRCALPHA    = 13,
    D3DBLEND_FORCE_DWORD        = 0x7fffffff
} D3DBLEND;

typedef enum _D3DBLENDOP {
    D3DBLENDOP_ADD              = 1,
    D3DBLENDOP_SUBTRACT         = 2,
    D3DBLENDOP_REVSUBTRACT      = 3,
    D3DBLENDOP_MIN              = 4,
    D3DBLENDOP_MAX              = 5,
    D3DBLENDOP_FORCE_DWORD      = 0x7fffffff
} D3DBLENDOP;

typedef enum _D3DCULL {
    D3DCULL_NONE        = 1,
    D3DCULL_CW          = 2,
    D3DCULL_CCW         = 3,
    D3DCULL_FORCE_DWORD = 0x7fffffff
} D3DCULL;

typedef enum _D3DFILLMODE {
    D3DFILL_POINT       = 1,
    D3DFILL_WIREFRAME   = 2,
    D3DFILL_SOLID       = 3,
    D3DFILL_FORCE_DWORD = 0x7fffffff
} D3DFILLMODE;

typedef enum _D3DSHADEMODE {
    D3DSHADE_FLAT       = 1,
    D3DSHADE_GOURAUD    = 2,
    D3DSHADE_PHONG      = 3,
    D3DSHADE_FORCE_DWORD = 0x7fffffff
} D3DSHADEMODE;

typedef enum _D3DTEXTUREOP {
    D3DTOP_DISABLE                  = 1,
    D3DTOP_SELECTARG1               = 2,
    D3DTOP_SELECTARG2               = 3,
    D3DTOP_MODULATE                 = 4,
    D3DTOP_MODULATE2X               = 5,
    D3DTOP_MODULATE4X               = 6,
    D3DTOP_ADD                      = 7,
    D3DTOP_ADDSIGNED                = 8,
    D3DTOP_ADDSIGNED2X              = 9,
    D3DTOP_SUBTRACT                 = 10,
    D3DTOP_ADDSMOOTH                = 11,
    D3DTOP_BLENDDIFFUSEALPHA       = 12,
    D3DTOP_BLENDTEXTUREALPHA       = 13,
    D3DTOP_BLENDFACTORALPHA        = 14,
    D3DTOP_BLENDTEXTUREALPHAPM     = 15,
    D3DTOP_BLENDCURRENTALPHA       = 16,
    D3DTOP_PREMODULATE              = 17,
    D3DTOP_MODULATEALPHA_ADDCOLOR  = 18,
    D3DTOP_MODULATECOLOR_ADDALPHA  = 19,
    D3DTOP_MODULATEINVALPHA_ADDCOLOR  = 20,
    D3DTOP_MODULATEINVCOLOR_ADDALPHA  = 21,
    D3DTOP_BUMPENVMAP              = 22,
    D3DTOP_BUMPENVMAPLUMINANCE     = 23,
    D3DTOP_DOTPRODUCT3             = 24,
    D3DTOP_MULTIPLYADD             = 25,
    D3DTOP_LERP                    = 26,
    D3DTOP_FORCE_DWORD             = 0x7fffffff
} D3DTEXTUREOP;

#define D3DTA_SELECTMASK    0x0000000f
#define D3DTA_DIFFUSE       0x00000000
#define D3DTA_CURRENT       0x00000001
#define D3DTA_TEXTURE       0x00000002
#define D3DTA_TFACTOR       0x00000003
#define D3DTA_SPECULAR      0x00000004
#define D3DTA_COMPLEMENT    0x00000010
#define D3DTA_ALPHAREPLICATE 0x00000020

typedef enum _D3DTEXTUREFILTERTYPE {
    D3DTEXF_NONE            = 0,
    D3DTEXF_POINT           = 1,
    D3DTEXF_LINEAR          = 2,
    D3DTEXF_ANISOTROPIC     = 3,
    D3DTEXF_FLATCUBIC       = 4,
    D3DTEXF_GAUSSIANCUBIC   = 5,
    D3DTEXF_FORCE_DWORD     = 0x7fffffff
} D3DTEXTUREFILTERTYPE;

typedef enum _D3DTEXTUREADDRESS {
    D3DTADDRESS_WRAP        = 1,
    D3DTADDRESS_MIRROR      = 2,
    D3DTADDRESS_CLAMP       = 3,
    D3DTADDRESS_BORDER      = 4,
    D3DTADDRESS_MIRRORONCE  = 5,
    D3DTADDRESS_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREADDRESS;

typedef enum _D3DSTENCILOP {
    D3DSTENCILOP_KEEP       = 1,
    D3DSTENCILOP_ZERO       = 2,
    D3DSTENCILOP_REPLACE    = 3,
    D3DSTENCILOP_INCRSAT    = 4,
    D3DSTENCILOP_DECRSAT    = 5,
    D3DSTENCILOP_INVERT     = 6,
    D3DSTENCILOP_INCR       = 7,
    D3DSTENCILOP_DECR       = 8,
    D3DSTENCILOP_FORCE_DWORD = 0x7fffffff
} D3DSTENCILOP;

typedef enum _D3DFOGMODE {
    D3DFOG_NONE         = 0,
    D3DFOG_EXP          = 1,
    D3DFOG_EXP2         = 2,
    D3DFOG_LINEAR       = 3,
    D3DFOG_FORCE_DWORD  = 0x7fffffff
} D3DFOGMODE;

typedef enum _D3DMATERIALCOLORSOURCE {
    D3DMCS_MATERIAL     = 0,
    D3DMCS_COLOR1       = 1,
    D3DMCS_COLOR2       = 2,
    D3DMCS_FORCE_DWORD  = 0x7fffffff
} D3DMATERIALCOLORSOURCE;

typedef enum _D3DZBUFFERTYPE {
    D3DZB_FALSE         = 0,
    D3DZB_TRUE          = 1,
    D3DZB_USEW          = 2,
    D3DZB_FORCE_DWORD   = 0x7fffffff
} D3DZBUFFERTYPE;

typedef enum _D3DVERTEXBLENDFLAGS {
    D3DVBF_DISABLE      = 0,
    D3DVBF_1WEIGHTS     = 1,
    D3DVBF_2WEIGHTS     = 2,
    D3DVBF_3WEIGHTS     = 3,
    D3DVBF_TWEENING     = 255,
    D3DVBF_0WEIGHTS     = 256,
    D3DVBF_FORCE_DWORD  = 0x7fffffff
} D3DVERTEXBLENDFLAGS;

typedef enum _D3DPATCHEDGESTYLE {
    D3DPATCHEDGE_DISCRETE   = 0,
    D3DPATCHEDGE_CONTINUOUS = 1,
    D3DPATCHEDGE_FORCE_DWORD = 0x7fffffff
} D3DPATCHEDGESTYLE;

typedef enum _D3DDEBUGMONITORTOKENS {
    D3DDMT_ENABLE       = 0,
    D3DDMT_DISABLE      = 1,
    D3DDMT_FORCE_DWORD  = 0x7fffffff
} D3DDEBUGMONITORTOKENS;

// ---------------------------------------------------------------------------
// Texture transform flags
// ---------------------------------------------------------------------------

#define D3DTTFF_DISABLE     0
#define D3DTTFF_COUNT1      1
#define D3DTTFF_COUNT2      2
#define D3DTTFF_COUNT3      3
#define D3DTTFF_COUNT4      4
#define D3DTTFF_PROJECTED   256

typedef unsigned long D3DTEXTURETRANSFORMFLAGS;

// ---------------------------------------------------------------------------
// Color write enable flags
// ---------------------------------------------------------------------------

#define D3DCOLORWRITEENABLE_RED     (1L<<0)
#define D3DCOLORWRITEENABLE_GREEN   (1L<<1)
#define D3DCOLORWRITEENABLE_BLUE    (1L<<2)
#define D3DCOLORWRITEENABLE_ALPHA   (1L<<3)

// ---------------------------------------------------------------------------
// Flexible Vertex Format flags
// ---------------------------------------------------------------------------

#define D3DFVF_RESERVED0        0x001
#define D3DFVF_POSITION_MASK    0x00E
#define D3DFVF_XYZ              0x002
#define D3DFVF_XYZRHW           0x004
#define D3DFVF_XYZB1            0x006
#define D3DFVF_XYZB2            0x008
#define D3DFVF_XYZB3            0x00a
#define D3DFVF_XYZB4            0x00c
#define D3DFVF_XYZB5            0x00e
#define D3DFVF_NORMAL           0x010
#define D3DFVF_PSIZE            0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080
#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEX2             0x200
#define D3DFVF_TEX3             0x300
#define D3DFVF_TEX4             0x400
#define D3DFVF_TEX5             0x500
#define D3DFVF_TEX6             0x600
#define D3DFVF_TEX7             0x700
#define D3DFVF_TEX8             0x800
#define D3DFVF_LASTBETA_UBYTE4  0x1000

#define D3DFVF_TEXCOORDSIZE3(CoordIndex)  (1 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(CoordIndex)  (0)
#define D3DFVF_TEXCOORDSIZE4(CoordIndex)  (2 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE1(CoordIndex)  (3 << (CoordIndex*2 + 16))

// ---------------------------------------------------------------------------
// Lock flags
// ---------------------------------------------------------------------------

#define D3DLOCK_READONLY        0x00000010L
#define D3DLOCK_DISCARD         0x00002000L
#define D3DLOCK_NOOVERWRITE     0x00001000L
#define D3DLOCK_NOSYSLOCK       0x00000800L
#define D3DLOCK_NO_DIRTY_UPDATE 0x00008000L

// ---------------------------------------------------------------------------
// Usage flags
// ---------------------------------------------------------------------------

#define D3DUSAGE_RENDERTARGET       0x00000001L
#define D3DUSAGE_DEPTHSTENCIL       0x00000002L
#define D3DUSAGE_WRITEONLY          0x00000008L
#define D3DUSAGE_SOFTWAREPROCESSING 0x00000010L
#define D3DUSAGE_DONOTCLIP          0x00000020L
#define D3DUSAGE_POINTS             0x00000040L
#define D3DUSAGE_RTPATCHES          0x00000080L
#define D3DUSAGE_NPATCHES           0x00000100L
#define D3DUSAGE_DYNAMIC            0x00000200L

// ---------------------------------------------------------------------------
// Vertex processing flags
// ---------------------------------------------------------------------------

#define D3DCREATE_SOFTWARE_VERTEXPROCESSING  0x00000020L
#define D3DCREATE_HARDWARE_VERTEXPROCESSING  0x00000040L
#define D3DCREATE_MIXED_VERTEXPROCESSING     0x00000080L

// ---------------------------------------------------------------------------
// D3D8 Structures
// ---------------------------------------------------------------------------

typedef struct _D3DLIGHT8 {
    D3DLIGHTTYPE    Type;
    D3DCOLORVALUE   Diffuse;
    D3DCOLORVALUE   Specular;
    D3DCOLORVALUE   Ambient;
    D3DVECTOR       Position;
    D3DVECTOR       Direction;
    float           Range;
    float           Falloff;
    float           Attenuation0;
    float           Attenuation1;
    float           Attenuation2;
    float           Theta;
    float           Phi;
} D3DLIGHT8;

typedef struct _D3DMATERIAL8 {
    D3DCOLORVALUE   Diffuse;
    D3DCOLORVALUE   Ambient;
    D3DCOLORVALUE   Specular;
    D3DCOLORVALUE   Emissive;
    float           Power;
} D3DMATERIAL8;

typedef struct _D3DVIEWPORT8 {
    unsigned long X;
    unsigned long Y;
    unsigned long Width;
    unsigned long Height;
    float MinZ;
    float MaxZ;
} D3DVIEWPORT8;

typedef struct _D3DGAMMARAMP {
    unsigned short red[256];
    unsigned short green[256];
    unsigned short blue[256];
} D3DGAMMARAMP;

typedef struct _D3DPRESENT_PARAMETERS_ {
    unsigned int            BackBufferWidth;
    unsigned int            BackBufferHeight;
    D3DFORMAT               BackBufferFormat;
    unsigned int            BackBufferCount;
    D3DMULTISAMPLE_TYPE     MultiSampleType;
    D3DSWAPEFFECT           SwapEffect;
    HWND                    hDeviceWindow;
    int                     Windowed;
    int                     EnableAutoDepthStencil;
    D3DFORMAT               AutoDepthStencilFormat;
    unsigned long           Flags;
    unsigned int            FullScreen_RefreshRateInHz;
    unsigned int            FullScreen_PresentationInterval;
} D3DPRESENT_PARAMETERS;

typedef struct _D3DADAPTER_IDENTIFIER8 {
    char            Driver[512];
    char            Description[512];
    unsigned long   DriverVersionLowPart;
    unsigned long   DriverVersionHighPart;
    unsigned long   VendorId;
    unsigned long   DeviceId;
    unsigned long   SubSysId;
    unsigned long   Revision;
    GUID            DeviceIdentifier;
    unsigned long   WHQLLevel;
} D3DADAPTER_IDENTIFIER8;

typedef struct _D3DCAPS8 {
    D3DDEVTYPE          DeviceType;
    unsigned int        AdapterOrdinal;
    unsigned long       Caps;
    unsigned long       Caps2;
    unsigned long       Caps3;
    unsigned long       PresentationIntervals;
    unsigned long       CursorCaps;
    unsigned long       DevCaps;
    unsigned long       PrimitiveMiscCaps;
    unsigned long       RasterCaps;
    unsigned long       ZCmpCaps;
    unsigned long       SrcBlendCaps;
    unsigned long       DestBlendCaps;
    unsigned long       AlphaCmpCaps;
    unsigned long       ShadeCaps;
    unsigned long       TextureCaps;
    unsigned long       TextureFilterCaps;
    unsigned long       CubeTextureFilterCaps;
    unsigned long       VolumeTextureFilterCaps;
    unsigned long       TextureAddressCaps;
    unsigned long       VolumeTextureAddressCaps;
    unsigned long       LineCaps;
    unsigned long       MaxTextureWidth;
    unsigned long       MaxTextureHeight;
    unsigned long       MaxVolumeExtent;
    unsigned long       MaxTextureRepeat;
    unsigned long       MaxTextureAspectRatio;
    unsigned long       MaxAnisotropy;
    float               MaxVertexW;
    float               GuardBandLeft;
    float               GuardBandTop;
    float               GuardBandRight;
    float               GuardBandBottom;
    float               ExtentsAdjust;
    unsigned long       StencilCaps;
    unsigned long       FVFCaps;
    unsigned long       TextureOpCaps;
    unsigned long       MaxTextureBlendStages;
    unsigned long       MaxSimultaneousTextures;
    unsigned long       VertexProcessingCaps;
    unsigned long       MaxActiveLights;
    unsigned long       MaxUserClipPlanes;
    unsigned long       MaxVertexBlendMatrices;
    unsigned long       MaxVertexBlendMatrixIndex;
    float               MaxPointSize;
    unsigned long       MaxPrimitiveCount;
    unsigned long       MaxVertexIndex;
    unsigned long       MaxStreams;
    unsigned long       MaxStreamStride;
    unsigned long       VertexShaderVersion;
    unsigned long       MaxVertexShaderConst;
    unsigned long       PixelShaderVersion;
    float               MaxPixelShaderValue;
} D3DCAPS8;

typedef struct _D3DSURFACE_DESC {
    D3DFORMAT       Format;
    D3DPOOL         Pool;
    unsigned int    Size;
    D3DMULTISAMPLE_TYPE MultiSampleType;
    unsigned int    Width;
    unsigned int    Height;
} D3DSURFACE_DESC;

typedef struct _D3DLOCKED_RECT {
    int     Pitch;
    void*   pBits;
} D3DLOCKED_RECT;

typedef struct _D3DVOLUME_DESC {
    D3DFORMAT       Format;
    D3DPOOL         Pool;
    unsigned int    Size;
    unsigned int    Width;
    unsigned int    Height;
    unsigned int    Depth;
} D3DVOLUME_DESC;

typedef struct _D3DLOCKED_BOX {
    int     RowPitch;
    int     SlicePitch;
    void*   pBits;
} D3DLOCKED_BOX;

typedef struct _D3DBOX {
    unsigned int Left;
    unsigned int Top;
    unsigned int Right;
    unsigned int Bottom;
    unsigned int Front;
    unsigned int Back;
} D3DBOX;

typedef struct _D3DDISPLAYMODE {
    unsigned int    Width;
    unsigned int    Height;
    unsigned int    RefreshRate;
    D3DFORMAT       Format;
} D3DDISPLAYMODE;

// ---------------------------------------------------------------------------
// D3DCAPS8 flag constants
// ---------------------------------------------------------------------------

#define D3DDEVCAPS_NPATCHES                 0x01000000L
#define D3DDEVCAPS_HWRASTERIZATION          0x00080000L
#define D3DDEVCAPS_HWTRANSFORMANDLIGHT      0x00010000L
#define D3DPTEXTURECAPS_CUBEMAP             0x00000800L
#define D3DPTEXTURECAPS_VOLUMEMAP           0x00002000L
#define D3DPTEXTURECAPS_POW2                0x00000002L
#define D3DPTEXTURECAPS_SQUAREONLY          0x00000020L
#define D3DPRASTERCAPS_FOGRANGE             0x00010000L
#define D3DPRASTERCAPS_WFOG                 0x00100000L
#define D3DPRASTERCAPS_ZBIAS                0x00004000L
#define D3DSTENCILCAPS_KEEP                 0x00000001L
#define D3DSTENCILCAPS_ZERO                 0x00000002L
#define D3DSTENCILCAPS_REPLACE              0x00000004L
#define D3DSTENCILCAPS_INCRSAT              0x00000008L
#define D3DSTENCILCAPS_DECRSAT              0x00000010L
#define D3DSTENCILCAPS_INVERT               0x00000020L
#define D3DSTENCILCAPS_INCR                 0x00000040L
#define D3DSTENCILCAPS_DECR                 0x00000080L
#define D3DPBLENDCAPS_ZERO                  0x00000001L
#define D3DPBLENDCAPS_ONE                   0x00000002L
#define D3DPBLENDCAPS_SRCCOLOR              0x00000004L
#define D3DPBLENDCAPS_INVSRCCOLOR           0x00000008L
#define D3DPBLENDCAPS_SRCALPHA              0x00000010L
#define D3DPBLENDCAPS_INVSRCALPHA           0x00000020L
#define D3DPBLENDCAPS_DESTALPHA             0x00000040L
#define D3DPBLENDCAPS_INVDESTALPHA          0x00000080L
#define D3DPBLENDCAPS_DESTCOLOR             0x00000100L
#define D3DPBLENDCAPS_INVDESTCOLOR          0x00000200L
#define D3DPBLENDCAPS_SRCALPHASAT           0x00000400L
#define D3DPBLENDCAPS_BOTHSRCALPHA          0x00000800L
#define D3DPBLENDCAPS_BOTHINVSRCALPHA       0x00001000L
#define D3DTEXOPCAPS_DISABLE                0x00000001L
#define D3DTEXOPCAPS_SELECTARG1             0x00000002L
#define D3DTEXOPCAPS_SELECTARG2             0x00000004L
#define D3DTEXOPCAPS_MODULATE               0x00000008L
#define D3DTEXOPCAPS_MODULATE2X             0x00000010L
#define D3DTEXOPCAPS_MODULATE4X             0x00000020L
#define D3DTEXOPCAPS_ADD                    0x00000040L
#define D3DTEXOPCAPS_ADDSIGNED              0x00000080L
#define D3DTEXOPCAPS_ADDSIGNED2X            0x00000100L
#define D3DTEXOPCAPS_SUBTRACT               0x00000200L
#define D3DTEXOPCAPS_ADDSMOOTH              0x00000400L
#define D3DTEXOPCAPS_BLENDDIFFUSEALPHA      0x00000800L
#define D3DTEXOPCAPS_BLENDTEXTUREALPHA      0x00001000L
#define D3DTEXOPCAPS_BLENDFACTORALPHA       0x00002000L
#define D3DTEXOPCAPS_BLENDTEXTUREALPHAPM    0x00004000L
#define D3DTEXOPCAPS_BLENDCURRENTALPHA      0x00008000L
#define D3DTEXOPCAPS_PREMODULATE            0x00010000L
#define D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR 0x00020000L
#define D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA 0x00040000L
#define D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR 0x00080000L
#define D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA 0x00100000L
#define D3DTEXOPCAPS_BUMPENVMAP             0x00200000L
#define D3DTEXOPCAPS_BUMPENVMAPLUMINANCE    0x00400000L
#define D3DTEXOPCAPS_DOTPRODUCT3            0x00800000L
#define D3DTEXOPCAPS_MULTIPLYADD            0x01000000L
#define D3DTEXOPCAPS_LERP                   0x02000000L
#define D3DPTFILTERCAPS_MINFPOINT           0x00000100L
#define D3DPTFILTERCAPS_MINFLINEAR          0x00000200L
#define D3DPTFILTERCAPS_MINFANISOTROPIC     0x00000400L
#define D3DPTFILTERCAPS_MIPFPOINT           0x00010000L
#define D3DPTFILTERCAPS_MIPFLINEAR          0x00020000L
#define D3DPTFILTERCAPS_MAGFPOINT           0x01000000L
#define D3DPTFILTERCAPS_MAGFLINEAR          0x02000000L
#define D3DPTFILTERCAPS_MAGFANISOTROPIC     0x04000000L

// ---------------------------------------------------------------------------
// Stub COM Interface declarations
// Minimal classes with AddRef/Release so that inline code in WW3D2 headers
// compiles on non-Windows. The DX8 implementation .cpp files are gated
// behind _WIN32, so these methods are never actually called at runtime.
// ---------------------------------------------------------------------------

struct IUnknown_d3d8_compat {
    virtual unsigned long AddRef() { return 0; }
    virtual unsigned long Release() { return 0; }
    virtual ~IUnknown_d3d8_compat() = default;
};

struct IDirect3DResource8 : public IUnknown_d3d8_compat {};

struct IDirect3DBaseTexture8 : public IDirect3DResource8 {};
struct IDirect3DTexture8 : public IDirect3DBaseTexture8 {
    HRESULT GetSurfaceLevel(unsigned int, struct IDirect3DSurface8**) { return D3DERR_INVALIDCALL; }
    HRESULT GetLevelDesc(unsigned int, D3DSURFACE_DESC*) { return D3DERR_INVALIDCALL; }
    HRESULT LockRect(unsigned int, D3DLOCKED_RECT*, const RECT*, unsigned long) { return D3DERR_INVALIDCALL; }
    HRESULT UnlockRect(unsigned int) { return D3DERR_INVALIDCALL; }
};
struct IDirect3DCubeTexture8 : public IDirect3DBaseTexture8 {
    HRESULT GetLevelDesc(unsigned int, D3DSURFACE_DESC*) { return D3DERR_INVALIDCALL; }
    HRESULT GetCubeMapSurface(unsigned int, unsigned int, struct IDirect3DSurface8**) { return D3DERR_INVALIDCALL; }
    HRESULT LockRect(unsigned int, unsigned int, D3DLOCKED_RECT*, const RECT*, unsigned long) { return D3DERR_INVALIDCALL; }
    HRESULT UnlockRect(unsigned int, unsigned int) { return D3DERR_INVALIDCALL; }
};
struct IDirect3DVolumeTexture8 : public IDirect3DBaseTexture8 {
    HRESULT GetLevelDesc(unsigned int, D3DVOLUME_DESC*) { return D3DERR_INVALIDCALL; }
    HRESULT LockBox(unsigned int, D3DLOCKED_BOX*, const D3DBOX*, unsigned long) { return D3DERR_INVALIDCALL; }
    HRESULT UnlockBox(unsigned int) { return D3DERR_INVALIDCALL; }
};

struct IDirect3DSurface8 : public IUnknown_d3d8_compat {
    HRESULT GetDesc(D3DSURFACE_DESC*) { return D3DERR_INVALIDCALL; }
    HRESULT LockRect(D3DLOCKED_RECT*, const RECT*, unsigned long) { return D3DERR_INVALIDCALL; }
    HRESULT UnlockRect() { return D3DERR_INVALIDCALL; }
};
struct IDirect3DVolume8 : public IUnknown_d3d8_compat {};

struct IDirect3DVertexBuffer8 : public IDirect3DResource8 {
    HRESULT Lock(unsigned int, unsigned int, unsigned char**, unsigned long) { return D3DERR_INVALIDCALL; }
    HRESULT Unlock() { return D3DERR_INVALIDCALL; }
};
struct IDirect3DIndexBuffer8 : public IDirect3DResource8 {
    HRESULT Lock(unsigned int, unsigned int, unsigned char**, unsigned long) { return D3DERR_INVALIDCALL; }
    HRESULT Unlock() { return D3DERR_INVALIDCALL; }
};

struct IDirect3DSwapChain8 : public IUnknown_d3d8_compat {
    HRESULT Present(const RECT*, const RECT*, HWND, void*) { return D3DERR_INVALIDCALL; }
};

struct IDirect3D8 : public IUnknown_d3d8_compat {};
struct IDirect3DDevice8 : public IUnknown_d3d8_compat {};

// ---------------------------------------------------------------------------
// D3DX utility functions (stubs)
// ---------------------------------------------------------------------------

inline HRESULT D3DXCreateTextureFromFile(IDirect3DDevice8*, const char*, IDirect3DTexture8**) { return D3DERR_INVALIDCALL; }
inline HRESULT D3DXLoadSurfaceFromFile(IDirect3DSurface8*, const PALETTEENTRY*, const RECT*, const char*, const RECT*, unsigned long, D3DCOLOR, void*) { return D3DERR_INVALIDCALL; }
inline HRESULT D3DXLoadSurfaceFromSurface(IDirect3DSurface8*, const PALETTEENTRY*, const RECT*, IDirect3DSurface8*, const PALETTEENTRY*, const RECT*, unsigned long, D3DCOLOR) { return D3DERR_INVALIDCALL; }

// Direct3DCreate8 stub
inline IDirect3D8* Direct3DCreate8(unsigned int) { return nullptr; }

#endif // d3d8.h compat
