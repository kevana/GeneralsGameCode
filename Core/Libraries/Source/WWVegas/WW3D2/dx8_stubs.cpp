/*
**	Command & Conquer Generals / Zero Hour
**	Non-Windows stub implementations for DX8 rendering classes.
**	Real implementations are in dx8wrapper.cpp, dx8caps.cpp, etc. (Windows only).
*/

#ifndef _WIN32

#include "dx8wrapper.h"
#include "dx8fvf.h"
#include "dx8vertexbuffer.h"
#include "dx8indexbuffer.h"
#include "dx8renderer.h"
#include "dx8caps.h"
#include "rddesc.h"
#include "surfaceclass.h"
#include "light.h"

// ===== Globals =====
unsigned number_of_DX8_calls = 0;
bool _DX8SingleThreaded = false;
void DX8_Assert() {}
void Log_DX8_ErrorCode(unsigned) {}

// ===== DX8Wrapper statics =====
bool DX8Wrapper::IsInitted = false;
bool DX8Wrapper::IsDeviceLost = false;
void* DX8Wrapper::Hwnd = nullptr;
unsigned DX8Wrapper::_MainThreadID = 0;
bool DX8Wrapper::_EnableTriangleDraw = true;
int DX8Wrapper::CurRenderDevice = -1;
int DX8Wrapper::ResolutionWidth = 800;
int DX8Wrapper::ResolutionHeight = 600;
int DX8Wrapper::BitDepth = 32;
int DX8Wrapper::TextureBitDepth = 32;
bool DX8Wrapper::IsWindowed = false;
D3DFORMAT DX8Wrapper::DisplayFormat = D3DFMT_UNKNOWN;
D3DMATRIX DX8Wrapper::old_world = {};
D3DMATRIX DX8Wrapper::old_view = {};
D3DMATRIX DX8Wrapper::old_prj = {};
DWORD DX8Wrapper::Vertex_Shader = 0;
DWORD DX8Wrapper::Pixel_Shader = 0;
Vector4 DX8Wrapper::Vertex_Shader_Constants[MAX_VERTEX_SHADER_CONSTANTS] = {};
Vector4 DX8Wrapper::Pixel_Shader_Constants[MAX_PIXEL_SHADER_CONSTANTS] = {};
LightEnvironmentClass* DX8Wrapper::Light_Environment = nullptr;
RenderInfoClass* DX8Wrapper::Render_Info = nullptr;
DWORD DX8Wrapper::Vertex_Processing_Behavior = 0;
ZTextureClass* DX8Wrapper::Shadow_Map[MAX_SHADOW_MAPS] = {};
Vector3 DX8Wrapper::Ambient_Color;
bool DX8Wrapper::world_identity = false;
unsigned DX8Wrapper::RenderStates[256] = {};
unsigned DX8Wrapper::TextureStageStates[MAX_TEXTURE_STAGES][32] = {};
IDirect3DBaseTexture8* DX8Wrapper::Textures[MAX_TEXTURE_STAGES] = {};
RenderStateStruct DX8Wrapper::render_state;
unsigned DX8Wrapper::render_state_changed = 0;
bool DX8Wrapper::FogEnable = false;
D3DCOLOR DX8Wrapper::FogColor = 0;
IDirect3D8* DX8Wrapper::D3DInterface = nullptr;
IDirect3DDevice8* DX8Wrapper::D3DDevice = nullptr;
IDirect3DSurface8* DX8Wrapper::CurrentRenderTarget = nullptr;
IDirect3DSurface8* DX8Wrapper::CurrentDepthBuffer = nullptr;
IDirect3DSurface8* DX8Wrapper::DefaultRenderTarget = nullptr;
IDirect3DSurface8* DX8Wrapper::DefaultDepthBuffer = nullptr;
unsigned DX8Wrapper::matrix_changes = 0;
unsigned DX8Wrapper::material_changes = 0;
unsigned DX8Wrapper::vertex_buffer_changes = 0;
unsigned DX8Wrapper::index_buffer_changes = 0;
unsigned DX8Wrapper::light_changes = 0;
unsigned DX8Wrapper::texture_changes = 0;
unsigned DX8Wrapper::render_state_changes = 0;
unsigned DX8Wrapper::texture_stage_state_changes = 0;
unsigned DX8Wrapper::draw_calls = 0;
bool DX8Wrapper::CurrentDX8LightEnables[4] = {};
bool DX8Wrapper::IsRenderToTexture = false;
unsigned DX8Wrapper::DrawPolygonLowBoundLimit = 0;
DX8Caps* DX8Wrapper::CurrentCaps = nullptr;
D3DADAPTER_IDENTIFIER8 DX8Wrapper::CurrentAdapterIdentifier = {};
unsigned long DX8Wrapper::FrameCount = 0;
DX8_CleanupHook* DX8Wrapper::m_pCleanupHook = nullptr;
int DX8Wrapper::ZBias = 0;
float DX8Wrapper::ZNear = 0.0f;
float DX8Wrapper::ZFar = 1.0f;
D3DMATRIX DX8Wrapper::ProjectionMatrix = {};
D3DMATRIX DX8Wrapper::DX8Transforms[D3DTS_WORLD + 1] = {};
#ifdef EXTENDED_STATS
DX8_Stats DX8Wrapper::stats;
#endif

// ===== DX8Wrapper methods =====
bool DX8Wrapper::Init(void*, bool) { return false; }
void DX8Wrapper::Shutdown(void) {}
void DX8Wrapper::Do_Onetime_Device_Dependent_Inits(void) {}
void DX8Wrapper::Do_Onetime_Device_Dependent_Shutdowns(void) {}
bool DX8Wrapper::Has_Stencil(void) { return false; }
void DX8Wrapper::Get_Format_Name(unsigned int, StringClass*) {}
void DX8Wrapper::Begin_Scene(void) {}
void DX8Wrapper::End_Scene(bool) {}
void DX8Wrapper::Flip_To_Primary(void) {}
void DX8Wrapper::Clear(bool, bool, const Vector3&, float, float, unsigned int) {}
void DX8Wrapper::Set_Viewport(CONST D3DVIEWPORT8*) {}
void DX8Wrapper::Set_Vertex_Buffer(const VertexBufferClass*) {}
void DX8Wrapper::Set_Vertex_Buffer(const DynamicVBAccessClass&) {}
void DX8Wrapper::Set_Index_Buffer(const IndexBufferClass*, unsigned short) {}
void DX8Wrapper::Set_Index_Buffer(const DynamicIBAccessClass&, unsigned short) {}
void DX8Wrapper::Draw_Sorting_IB_VB(unsigned, unsigned short, unsigned short, unsigned short, unsigned short) {}
void DX8Wrapper::Draw(unsigned, unsigned short, unsigned short, unsigned short, unsigned short) {}
void DX8Wrapper::Draw_Triangles(unsigned, unsigned short, unsigned short, unsigned short, unsigned short) {}
void DX8Wrapper::Draw_Triangles(unsigned short, unsigned short, unsigned short, unsigned short) {}
void DX8Wrapper::Draw_Strip(unsigned short, unsigned short, unsigned short, unsigned short) {}
void DX8Wrapper::Apply_Render_State_Changes() {}
IDirect3DTexture8* DX8Wrapper::_Create_DX8_Texture(unsigned, unsigned, WW3DFormat, MipCountType, D3DPOOL, bool) { return nullptr; }
IDirect3DTexture8* DX8Wrapper::_Create_DX8_Texture(const char*, MipCountType) { return nullptr; }
IDirect3DTexture8* DX8Wrapper::_Create_DX8_Texture(IDirect3DSurface8*, MipCountType) { return nullptr; }
IDirect3DTexture8* DX8Wrapper::_Create_DX8_ZTexture(unsigned, unsigned, WW3DZFormat, MipCountType, D3DPOOL) { return nullptr; }
IDirect3DCubeTexture8* DX8Wrapper::_Create_DX8_Cube_Texture(unsigned, unsigned, WW3DFormat, MipCountType, D3DPOOL, bool) { return nullptr; }
IDirect3DVolumeTexture8* DX8Wrapper::_Create_DX8_Volume_Texture(unsigned, unsigned, unsigned, WW3DFormat, MipCountType, D3DPOOL) { return nullptr; }
IDirect3DSurface8* DX8Wrapper::_Create_DX8_Surface(unsigned, unsigned, WW3DFormat) { return nullptr; }
IDirect3DSurface8* DX8Wrapper::_Create_DX8_Surface(const char*) { return nullptr; }
IDirect3DSurface8* DX8Wrapper::_Get_DX8_Front_Buffer() { return nullptr; }
SurfaceClass* DX8Wrapper::_Get_DX8_Back_Buffer(unsigned) { return nullptr; }
void DX8Wrapper::_Update_Texture(TextureClass*, TextureClass*) {}
void DX8Wrapper::Flush_DX8_Resource_Manager(unsigned) {}
unsigned int DX8Wrapper::Get_Free_Texture_RAM() { return 256u * 1024u * 1024u; }
void DX8Wrapper::Set_Light(unsigned, const D3DLIGHT8*) {}
void DX8Wrapper::Set_Light(unsigned, const LightClass&) {}
void DX8Wrapper::Set_Light_Environment(LightEnvironmentClass* e) { Light_Environment = e; }
IDirect3DSwapChain8* DX8Wrapper::Create_Additional_Swap_Chain(HWND) { return nullptr; }
TextureClass* DX8Wrapper::Create_Render_Target(int, int, WW3DFormat) { return nullptr; }
void DX8Wrapper::Create_Render_Target(int, int, WW3DFormat, WW3DZFormat, TextureClass**, ZTextureClass**) {}
void DX8Wrapper::Set_Render_Target_With_Z(TextureClass*, ZTextureClass*) {}
void DX8Wrapper::Set_Render_Target(IDirect3DSurface8*, bool) {}
void DX8Wrapper::Set_Render_Target(IDirect3DSurface8*, IDirect3DSurface8*) {}
void DX8Wrapper::Set_Render_Target(IDirect3DSwapChain8*) {}
void DX8Wrapper::Set_Gamma(float, float, float, bool, bool) {}
void DX8Wrapper::Set_World_Identity() { render_state_changed |= WORLD_IDENTITY | WORLD_CHANGED; }
void DX8Wrapper::Set_View_Identity() { render_state_changed |= VIEW_IDENTITY | VIEW_CHANGED; }
void DX8Wrapper::Apply_Default_State() {}
bool DX8Wrapper::Validate_Device(void) { return true; }
void DX8Wrapper::Invalidate_Cached_Render_States(void) {}
void DX8Wrapper::Reset_Statistics() {}
void DX8Wrapper::Begin_Statistics() {}
void DX8Wrapper::End_Statistics() {}
unsigned DX8Wrapper::Get_Last_Frame_Matrix_Changes() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_Material_Changes() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_Vertex_Buffer_Changes() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_Index_Buffer_Changes() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_Light_Changes() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_Texture_Changes() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_Render_State_Changes() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_Texture_Stage_State_Changes() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_DX8_Calls() { return 0; }
unsigned DX8Wrapper::Get_Last_Frame_Draw_Calls() { return 0; }
unsigned long DX8Wrapper::Get_FrameCount(void) { return FrameCount; }
WW3DFormat DX8Wrapper::getBackBufferFormat(void) { return WW3D_FORMAT_UNKNOWN; }
bool DX8Wrapper::Reset_Device(bool) { return false; }
void DX8Wrapper::Compute_Caps(WW3DFormat) {}
void DX8Wrapper::Set_Swap_Interval(int) {}
int DX8Wrapper::Get_Swap_Interval(void) { return 0; }
void DX8Wrapper::Set_Polygon_Mode(int) {}
bool DX8Wrapper::Create_Device(void) { return false; }
void DX8Wrapper::Release_Device(void) {}
void DX8Wrapper::Enumerate_Devices() {}
void DX8Wrapper::Set_Default_Global_Render_States(void) {}
bool DX8Wrapper::Set_Any_Render_Device(void) { return false; }
bool DX8Wrapper::Set_Render_Device(const char*, int, int, int, int, bool) { return false; }
bool DX8Wrapper::Set_Render_Device(int, int, int, int, int, bool, bool, bool) { return false; }
bool DX8Wrapper::Set_Next_Render_Device(void) { return false; }
bool DX8Wrapper::Toggle_Windowed(void) { return false; }
int DX8Wrapper::Get_Render_Device_Count(void) { return 0; }
int DX8Wrapper::Get_Render_Device(void) { return CurRenderDevice; }
static RenderDeviceDescClass _dummyDesc;
const RenderDeviceDescClass& DX8Wrapper::Get_Render_Device_Desc(int) { return _dummyDesc; }
const char* DX8Wrapper::Get_Render_Device_Name(int) { return "OpenGL"; }
bool DX8Wrapper::Set_Device_Resolution(int, int, int, int, bool) { return false; }
void DX8Wrapper::Get_Device_Resolution(int& w, int& h, int& b, bool& win) { w = ResolutionWidth; h = ResolutionHeight; b = BitDepth; win = IsWindowed; }
void DX8Wrapper::Get_Render_Target_Resolution(int& w, int& h, int& b, bool& win) { w = ResolutionWidth; h = ResolutionHeight; b = BitDepth; win = IsWindowed; }
void DX8Wrapper::Resize_And_Position_Window() {}
bool DX8Wrapper::Find_Color_And_Z_Mode(int, int, int, D3DFORMAT*, D3DFORMAT*, D3DFORMAT*) { return false; }
bool DX8Wrapper::Find_Color_Mode(D3DFORMAT, int, int, UINT*) { return false; }
bool DX8Wrapper::Find_Z_Mode(D3DFORMAT, D3DFORMAT, D3DFORMAT*) { return false; }
bool DX8Wrapper::Test_Z_Mode(D3DFORMAT, D3DFORMAT, D3DFORMAT) { return false; }
bool DX8Wrapper::Registry_Save_Render_Device(const char*) { return false; }
bool DX8Wrapper::Registry_Save_Render_Device(const char*, int, int, int, int, bool, int) { return false; }
bool DX8Wrapper::Registry_Load_Render_Device(const char*, bool) { return false; }
bool DX8Wrapper::Registry_Load_Render_Device(const char*, char*, int, int&, int&, int&, int&, int&) { return false; }
const char* DX8Wrapper::Get_DX8_Render_State_Name(D3DRENDERSTATETYPE) { return ""; }
const char* DX8Wrapper::Get_DX8_Texture_Stage_State_Name(D3DTEXTURESTAGESTATETYPE) { return ""; }
void DX8Wrapper::Get_DX8_Render_State_Value_Name(StringClass&, D3DRENDERSTATETYPE, unsigned) {}
void DX8Wrapper::Get_DX8_Texture_Stage_State_Value_Name(StringClass&, D3DTEXTURESTAGESTATETYPE, unsigned) {}
const char* DX8Wrapper::Get_DX8_Texture_Op_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Texture_Arg_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Texture_Filter_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Texture_Address_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Texture_Transform_Flag_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_ZBuffer_Type_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Fill_Mode_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Shade_Mode_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Blend_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Cull_Mode_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Cmp_Func_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Fog_Mode_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Stencil_Op_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Material_Source_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Vertex_Blend_Flag_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Patch_Edge_Style_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Debug_Monitor_Token_Name(unsigned) { return ""; }
const char* DX8Wrapper::Get_DX8_Blend_Op_Name(unsigned) { return ""; }

// ===== DX8Caps =====
DX8Caps::DX8Caps(IDirect3D8*, IDirect3DDevice8*, WW3DFormat, const D3DADAPTER_IDENTIFIER8&)
	: MaxDisplayWidth(4096), MaxDisplayHeight(4096), hwVPCaps{}, swVPCaps{},
	  SupportTnL(false), SupportDXTC(true), supportGamma(false), SupportNPatches(false),
	  SupportBumpEnvmap(false), SupportBumpEnvmapLuminance(false),
	  SupportTextureFormat{}, SupportRenderToTextureFormat{}, SupportDepthStencilFormat{},
	  SupportDot3(false), SupportPointSprites(false),
	  MaxTexturesPerPass(4), VertexShaderVersion(0), PixelShaderVersion(0),
	  MaxSimultaneousTextures(4) {}
void DX8Caps::Shutdown(void) {}
void DX8Caps::Init_Caps(IDirect3DDevice8*) {}
void DX8Caps::Compute_Caps(WW3DFormat, const D3DADAPTER_IDENTIFIER8&) {}
void DX8Caps::Check_Bumpmap_Support(const D3DCAPS8&) {}
void DX8Caps::Check_Texture_Compression_Support(const D3DCAPS8&) {}
void DX8Caps::Check_Texture_Format_Support(WW3DFormat, const D3DCAPS8&) {}
void DX8Caps::Check_Render_To_Texture_Support(WW3DFormat, const D3DCAPS8&) {}
void DX8Caps::Check_Depth_Stencil_Support(WW3DFormat, const D3DCAPS8&) {}
void DX8Caps::Check_Maximum_Texture_Support(const D3DCAPS8&) {}
void DX8Caps::Check_Shader_Support(const D3DCAPS8&) {}
void DX8Caps::Vendor_Specific_Hacks(const D3DADAPTER_IDENTIFIER8&) {}
DX8Caps::VendorIdType DX8Caps::Define_Vendor(unsigned) { return VENDOR_UNKNOWN; }
DX8Caps::DeviceTypeATI DX8Caps::Get_ATI_Device(unsigned) { return DEVICE_ATI_UNKNOWN; }
DX8Caps::DeviceType3DLabs DX8Caps::Get_3DLabs_Device(unsigned) { return DEVICE_3DLABS_UNKNOWN; }
DX8Caps::DeviceTypeNVidia DX8Caps::Get_NVidia_Device(unsigned) { return DEVICE_NVIDIA_UNKNOWN; }
DX8Caps::DeviceType3Dfx DX8Caps::Get_3Dfx_Device(unsigned) { return DEVICE_3DFX_UNKNOWN; }
DX8Caps::DeviceTypeMatrox DX8Caps::Get_Matrox_Device(unsigned) { return DEVICE_MATROX_UNKNOWN; }
DX8Caps::DeviceTypePowerVR DX8Caps::Get_PowerVR_Device(unsigned) { return DEVICE_POWERVR_UNKNOWN; }
DX8Caps::DeviceTypeS3 DX8Caps::Get_S3_Device(unsigned) { return DEVICE_S3_UNKNOWN; }
DX8Caps::DeviceTypeIntel DX8Caps::Get_Intel_Device(unsigned) { return DEVICE_INTEL_UNKNOWN; }

// ===== FVFInfoClass =====
static unsigned Compute_FVF_Size(unsigned fvf) {
	unsigned sz = 0;
	if (fvf & D3DFVF_XYZ) sz += 12;
	if (fvf & D3DFVF_XYZRHW) sz += 16;
	if (fvf & D3DFVF_NORMAL) sz += 12;
	if (fvf & D3DFVF_DIFFUSE) sz += 4;
	if (fvf & D3DFVF_SPECULAR) sz += 4;
	unsigned tc = (fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
	sz += tc * 8;
	return sz;
}

FVFInfoClass::FVFInfoClass(unsigned fvf)
	: FVF(fvf), fvf_size(Compute_FVF_Size(fvf)),
	  location_offset(0), normal_offset(0), tex_offset{},
	  diffuse_offset(0), specular_offset(0) {}
void FVFInfoClass::Get_FVF_Name(StringClass& name) const { name = "FVF_STUB"; }

// ===== VertexBufferClass =====
VertexBufferClass::VertexBufferClass(unsigned t, unsigned fvf, unsigned short vc)
	: type(t), VertexCount(vc), engine_refs(0) { fvf_info = new FVFInfoClass(fvf); }
VertexBufferClass::~VertexBufferClass() { delete fvf_info; }
unsigned VertexBufferClass::Get_Total_Buffer_Count() { return 0; }
unsigned VertexBufferClass::Get_Total_Allocated_Vertices() { return 0; }
unsigned VertexBufferClass::Get_Total_Allocated_Memory() { return 0; }
void VertexBufferClass::Add_Engine_Ref() const { const_cast<VertexBufferClass*>(this)->engine_refs++; }
void VertexBufferClass::Release_Engine_Ref() const { const_cast<VertexBufferClass*>(this)->engine_refs--; }
VertexBufferClass::WriteLockClass::WriteLockClass(VertexBufferClass*, int) : Vertices(nullptr) {}
VertexBufferClass::WriteLockClass::~WriteLockClass() {}
VertexBufferClass::AppendLockClass::AppendLockClass(VertexBufferClass*, unsigned, unsigned) : Vertices(nullptr) {}
VertexBufferClass::AppendLockClass::~AppendLockClass() {}

// DX8VertexBufferClass
DX8VertexBufferClass::DX8VertexBufferClass(unsigned fvf, unsigned short vc, UsageType)
	: VertexBufferClass(BUFFER_TYPE_DX8, fvf, vc), VertexBuffer(nullptr) {}
DX8VertexBufferClass::DX8VertexBufferClass(const Vector3*, const Vector3*, const Vector2*, unsigned short vc, UsageType)
	: VertexBufferClass(BUFFER_TYPE_DX8, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1, vc), VertexBuffer(nullptr) {}
DX8VertexBufferClass::DX8VertexBufferClass(const Vector3*, const Vector3*, const Vector4*, const Vector2*, unsigned short vc, UsageType)
	: VertexBufferClass(BUFFER_TYPE_DX8, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1, vc), VertexBuffer(nullptr) {}
DX8VertexBufferClass::DX8VertexBufferClass(const Vector3*, const Vector2*, unsigned short vc, UsageType)
	: VertexBufferClass(BUFFER_TYPE_DX8, D3DFVF_XYZ|D3DFVF_TEX1, vc), VertexBuffer(nullptr) {}
DX8VertexBufferClass::DX8VertexBufferClass(const Vector3*, const Vector3*, unsigned short vc, UsageType)
	: VertexBufferClass(BUFFER_TYPE_DX8, D3DFVF_XYZ|D3DFVF_NORMAL, vc), VertexBuffer(nullptr) {}
DX8VertexBufferClass::~DX8VertexBufferClass() {}
void DX8VertexBufferClass::Create_Vertex_Buffer(UsageType) {}
void DX8VertexBufferClass::Copy(const Vector3*, const Vector3*, const Vector2*, unsigned, unsigned) {}
void DX8VertexBufferClass::Copy(const Vector3*, unsigned, unsigned) {}
void DX8VertexBufferClass::Copy(const Vector3*, const Vector2*, unsigned, unsigned) {}
void DX8VertexBufferClass::Copy(const Vector3*, const Vector3*, unsigned, unsigned) {}
void DX8VertexBufferClass::Copy(const Vector3*, const Vector3*, const Vector2*, const Vector4*, unsigned, unsigned) {}
void DX8VertexBufferClass::Copy(const Vector3*, const Vector2*, const Vector4*, unsigned, unsigned) {}

// SortingVertexBufferClass
SortingVertexBufferClass::SortingVertexBufferClass(unsigned short vc)
	: VertexBufferClass(BUFFER_TYPE_SORTING, D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2|D3DFVF_DIFFUSE, vc),
	  VertexBuffer(nullptr) {}
SortingVertexBufferClass::~SortingVertexBufferClass() { delete[] VertexBuffer; }

// DynamicVBAccessClass - has const FVFInfoClass& reference member
static FVFInfoClass _stubDynamicFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2|D3DFVF_DIFFUSE);
DynamicVBAccessClass::DynamicVBAccessClass(unsigned t, unsigned, unsigned short vc)
	: FVFInfo(_stubDynamicFVF), Type(t), VertexCount(vc), VertexBufferOffset(0), VertexBuffer(nullptr) {}
DynamicVBAccessClass::~DynamicVBAccessClass() {}
void DynamicVBAccessClass::_Deinit() {}
void DynamicVBAccessClass::Allocate_DX8_Dynamic_Buffer() {}
void DynamicVBAccessClass::Allocate_Sorting_Dynamic_Buffer() {}
void DynamicVBAccessClass::_Reset(bool) {}
unsigned short DynamicVBAccessClass::Get_Default_Vertex_Count(void) { return 5000; }
DynamicVBAccessClass::WriteLockClass::WriteLockClass(DynamicVBAccessClass*) : DynamicVBAccess(nullptr), Vertices(nullptr) {}
DynamicVBAccessClass::WriteLockClass::~WriteLockClass() {}

// ===== IndexBufferClass =====
IndexBufferClass::IndexBufferClass(unsigned t, unsigned short ic) : type(t), index_count(ic), engine_refs(0) {}
IndexBufferClass::~IndexBufferClass() {}
unsigned IndexBufferClass::Get_Total_Buffer_Count() { return 0; }
unsigned IndexBufferClass::Get_Total_Allocated_Indices() { return 0; }
unsigned IndexBufferClass::Get_Total_Allocated_Memory() { return 0; }
void IndexBufferClass::Add_Engine_Ref() const { const_cast<IndexBufferClass*>(this)->engine_refs++; }
void IndexBufferClass::Release_Engine_Ref() const { const_cast<IndexBufferClass*>(this)->engine_refs--; }
void IndexBufferClass::Copy(unsigned int*, unsigned, unsigned) {}
void IndexBufferClass::Copy(unsigned short*, unsigned, unsigned) {}
IndexBufferClass::WriteLockClass::WriteLockClass(IndexBufferClass*, int) : Indices(nullptr) {}
IndexBufferClass::WriteLockClass::~WriteLockClass() {}
IndexBufferClass::AppendLockClass::AppendLockClass(IndexBufferClass*, unsigned, unsigned) : Indices(nullptr) {}
IndexBufferClass::AppendLockClass::~AppendLockClass() {}

DX8IndexBufferClass::DX8IndexBufferClass(unsigned short ic, UsageType)
	: IndexBufferClass(BUFFER_TYPE_DX8, ic), index_buffer(nullptr) {}
DX8IndexBufferClass::~DX8IndexBufferClass() {}
SortingIndexBufferClass::SortingIndexBufferClass(unsigned short ic)
	: IndexBufferClass(BUFFER_TYPE_SORTING, ic), index_buffer(nullptr) {}
SortingIndexBufferClass::~SortingIndexBufferClass() { delete[] index_buffer; }

DynamicIBAccessClass::DynamicIBAccessClass(unsigned short t, unsigned short ic)
	: Type(t), IndexCount(ic), IndexBufferOffset(0), IndexBuffer(nullptr) {}
DynamicIBAccessClass::~DynamicIBAccessClass() {}
void DynamicIBAccessClass::_Deinit() {}
void DynamicIBAccessClass::Allocate_DX8_Dynamic_Buffer() {}
void DynamicIBAccessClass::Allocate_Sorting_Dynamic_Buffer() {}
void DynamicIBAccessClass::_Reset(bool) {}
unsigned short DynamicIBAccessClass::Get_Default_Index_Count(void) { return 10000; }
DynamicIBAccessClass::WriteLockClass::WriteLockClass(DynamicIBAccessClass*) : DynamicIBAccess(nullptr), Indices(nullptr) {}
DynamicIBAccessClass::WriteLockClass::~WriteLockClass() {}

// ===== DX8MeshRendererClass =====
DX8MeshRendererClass TheDX8MeshRenderer;
bool DX8TextureCategoryClass::m_gForceMultiply = false;

DX8MeshRendererClass::DX8MeshRendererClass() {}
DX8MeshRendererClass::~DX8MeshRendererClass() {}
void DX8MeshRendererClass::Init(void) {}
void DX8MeshRendererClass::Shutdown(void) {}
void DX8MeshRendererClass::Clear_Pending_Delete_Lists() {}
void DX8MeshRendererClass::Unregister_Mesh_Type(MeshModelClass*) {}
void DX8MeshRendererClass::Register_Mesh_Type(MeshModelClass*) {}
void DX8MeshRendererClass::Request_Log_Statistics() {}
void DX8MeshRendererClass::Flush(void) {}
void DX8MeshRendererClass::Add_To_Render_List(DecalMeshClass*) {}
void DX8MeshRendererClass::Render_Decal_Meshes(void) {}
void DX8MeshRendererClass::Log_Statistics_String(bool) {}
void DX8MeshRendererClass::Invalidate(bool) {}

// DX8FVFCategoryContainer subclasses
DX8RigidFVFCategoryContainer::DX8RigidFVFCategoryContainer(unsigned, bool) {}
DX8RigidFVFCategoryContainer::~DX8RigidFVFCategoryContainer() {}
void DX8RigidFVFCategoryContainer::Log(bool) {}
void DX8RigidFVFCategoryContainer::Render(void) {}
bool DX8RigidFVFCategoryContainer::Check_If_Mesh_Fits(MeshModelClass*) { return false; }
void DX8RigidFVFCategoryContainer::Add_Mesh(MeshModelClass*) {}

DX8SkinFVFCategoryContainer::DX8SkinFVFCategoryContainer(bool) {}
DX8SkinFVFCategoryContainer::~DX8SkinFVFCategoryContainer() {}
void DX8SkinFVFCategoryContainer::Log(bool) {}
void DX8SkinFVFCategoryContainer::Render(void) {}
bool DX8SkinFVFCategoryContainer::Check_If_Mesh_Fits(MeshModelClass*) { return false; }
void DX8SkinFVFCategoryContainer::clearVisibleSkinList() {}
void DX8SkinFVFCategoryContainer::Add_Visible_Skin(MeshClass*) {}
void DX8SkinFVFCategoryContainer::Reset() {}
void DX8SkinFVFCategoryContainer::Add_Mesh(MeshModelClass*) {}

// ===== DX8RendererDebugger (GeneralsMD only) =====
#if __has_include("dx8rendererdebugger.h")
#include "dx8rendererdebugger.h"
bool DX8RendererDebugger::Enabled = false;
void DX8RendererDebugger::Enable(bool enable) { Enabled = enable; }
void DX8RendererDebugger::Get_String(StringClass&) {}
void DX8RendererDebugger::Update() {}
void DX8RendererDebugger::Disable_Mesh(unsigned) {}
void DX8RendererDebugger::Enable_Mesh(unsigned) {}
void DX8RendererDebugger::Disable_All() {}
void DX8RendererDebugger::Enable_All() {}
#endif

#endif // !_WIN32
