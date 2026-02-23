# Plan: Porting Generals to macOS

## Progress Tracking

### Phase 1: Build System & CMake Configuration
- [x] **1.1** Add macOS CMake presets (`macos-arm64`, `macos-x86_64`, debug variants)
- [x] **1.2** Add macOS platform detection (`IS_MACOS_BUILD` in compilers.cmake, `cmake/macos.cmake`)
- [x] **1.3** Configure vcpkg for macOS (SDL2, OpenAL Soft as macOS-only deps)
- [x] **1.4** Gate Windows-only subsystems behind `WIN32` (Win32Device, MilesAudio, Bink,
      DirectInput sources; Windows library links; `<windows.h>` PCH; `WIN32` executable flag)
- [x] **1.5** Fix cross-platform compilation issues (`<new.h>` → `<new>`, min/max templates
      for Clang/GCC, `<io.h>` guards in core GameEngine files)
- [x] **Stub targets** for `milesstub`, `binkstub`, `d3d8lib` on non-Windows platforms

### Phase 2: Platform Abstraction Layer
- [x] **2.1** Windowing and event loop abstraction (SDL2)
  - Created `SDLGameEngine.h/cpp` with SDL event pump, window management
  - Created `SDLMain.cpp` portable entry point replacing WinMain
  - Wired into CMakeLists for non-Windows builds
- [x] **2.2** Threading and synchronization abstraction
  - `CriticalSection.h`: `std::recursive_mutex` on non-Windows (Generals + GeneralsMD)
  - `ScopedMutex.h`: `std::mutex` on non-Windows (Generals + GeneralsMD)
  - WWVegas `MutexClass`: `std::recursive_timed_mutex` on Unix
  - WWVegas `CriticalSectionClass`: `std::recursive_mutex` on Unix
  - WWVegas `ThreadClass`: `pthread_create` on Unix, `sched_yield()` for Switch_Thread
  - `WorkerProcess`: Guarded behind `_WIN32` with non-Windows stub
  - `ClientInstance`: POSIX file locking (`flock`) implementation
  - `MiniDumper`: Disabled on macOS via `RTS_CRASHDUMP_ENABLE=OFF`
- [x] **2.3** Registry / configuration abstraction
  - Game engine `registry.cpp`: Return defaults on non-Windows (Generals + GeneralsMD)
  - WWVegas `RegistryClass`: Full stub implementation returning defaults on non-Windows
  - `ReplaySimulation.cpp`: Worker process path guarded behind `_WIN32`
- [x] **2.4** Networking abstraction (Winsock → BSD sockets)
  - Created `socket_compat.h` in Dependencies/Utility with POSIX equivalents:
    `closesocket→close`, `ioctlsocket→fcntl` wrapper, `WSAStartup/Cleanup` as no-ops,
    `WSAGetLastError→errno`, WSA error code mappings, `SOCKET` type, `MAKEWORD/LOBYTE/HIBYTE`
  - Included from `compat.h` (auto-available on non-Windows via `always.h`)
  - Guarded `<winsock.h>` includes in `ftp.h`, GameSpy thread files

### Phase 3: Graphics Rendering
- [x] **3.1** D3D8 type compatibility for non-Windows
  - Created `Dependencies/d3d8-compat/include/d3d8.h` with all D3D8 enums, structs,
    FVF flags, COM interface stubs, and Windows types needed by WW3D2 headers
  - Wired `d3d8lib` CMake INTERFACE target to provide compat header on non-Windows
  - Made `DX8CALL` macros no-ops on non-Windows (both Generals and GeneralsMD)
- [x] **3.2** Stub DX8 implementations for non-Windows compilation
  - Created `Core/Libraries/Source/WWVegas/WW3D2/dx8_stubs.cpp` with no-op stubs for
    all DX8Wrapper, DX8Caps, FVFInfoClass, VertexBuffer, IndexBuffer, and
    DX8MeshRendererClass methods (guarded by `#ifndef _WIN32`)
  - Includes DX8RendererDebugger stubs for GeneralsMD (via `__has_include`)
  - Modified Generals and GeneralsMD WW3D2 CMakeLists: DX8 .cpp files compiled only
    on WIN32, dx8_stubs.cpp used on other platforms
  - Gated `<windows.h>` PCH entry behind WIN32 check
- [x] **3.3** OpenGL 3.3 context creation via SDL2
  - Created `Dependencies/opengl/include/gl_compat.h` header wrapper
    (uses `<OpenGL/gl3.h>` on macOS, `<GL/gl.h>` on Linux)
  - Added `gl_loader` CMake interface target linking OpenGL framework + SDL2
  - SDL window created with GL 3.3 core profile in `SDLGameEngine::init()`
  - `ApplicationHWnd` defined in `SDLMain.cpp` pointing to SDL_Window
  - DX8Wrapper::Init creates GL context, sets default state
  - Basic GL operations: Clear, Set_Viewport, Flip_To_Primary, device setup
- [x] **3.4** Core GL rendering infrastructure
  - Created `gl_render.h` with GLSL shader program management, compilation helpers,
    vertex attribute layout definitions, and D3D→GL matrix conversion
  - Basic vertex shader: MVP transform, pass-through normals/UVs/vertex color
  - Basic fragment shader: texture × vertex color, alpha test, linear fog
  - `GLRenderState` struct tracking transforms, blend, depth, cull, fog state
  - Shader initialization and default uniform setup in DX8Wrapper::Init
  - `Apply_Render_State_Changes` updates transform matrices from D3D state
  - Buffer lock classes allocate memory for vertex/index data upload
- [x] **3.5** Shader translation (fixed-function → GLSL)
  - Mapped ShaderClass bitfield states to GL calls via `GL_ApplyShaderState()`:
    depth compare/mask, color mask, blend src/dst functions, alpha test (shader uniform),
    fog enable/color, cull mode enable/disable
  - Helper functions: `GL_DepthFunc`, `GL_SrcBlend`, `GL_DstBlend`
  - Integrated into `Apply_Render_State_Changes` on `SHADER_CHANGED` flag
- [x] **3.6** Texture format handling for OpenGL
  - `GLTexture` struct wraps GL texture handle inside `IDirect3DTexture8` with ref counting
  - `GL_GetTextureFormat` maps WW3DFormat → GL internal format, format, type:
    A8R8G8B8/X8R8G8B8 → RGBA8+BGRA, R8G8B8 → RGB8+BGR, R5G6B5 → RGB8,
    A1R5G5B5 → RGB5_A1, A4R4G4B4 → RGBA4, A8/L8 → R8, A8L8 → RG8,
    DXT1/3/5 → S3TC compressed formats
  - `_Create_DX8_Texture` creates real GL textures with filtering and mipmaps
  - Texture binding in `Apply_Render_State_Changes` via `dynamic_cast<GLTexture*>`
    with `glActiveTexture` + `glBindTexture` for stages 0-1
  - S3TC extension constants defined with fallback `#ifndef` guard

### Phase 4: Audio System
- [x] **4.1** Audio manager stub for non-Windows
  - SDLGameEngine::createAudioManager() returns AudioManagerDummy (no-op)
  - Prevents null pointer crashes; game boots without audio
  - Full OpenAL implementation deferred to future iteration

### Phase 5: Input System
- [x] **5.1** DirectInput compatibility header
  - Created `Dependencies/d3d8-compat/include/dinput.h` with DIK_* scan code
    constants and DIMOFS_* mouse data offsets for non-Windows builds
  - Enables KeyDefs.h to compile without Windows `<dinput.h>`
- [x] **5.2** SDL keyboard implementation
  - `SDLKeyboard` class with `getKey()` → SDL_Scancode to DIK_* translation
  - Complete scan code mapping table (~100 keys including numpad, F-keys,
    arrow keys, modifiers, special keys)
  - Event queue populated from SDLGameEngine event loop
- [x] **5.3** SDL mouse implementation
  - `SDLMouse` class with `getMouseEvent()` → SDL mouse event translation
  - Handles motion, left/right/middle buttons, double-click, wheel scroll
  - Mouse capture via SDL_SetRelativeMouseMode
- [x] **5.4** Input wiring
  - SDLGameEngine routes SDL_KEYDOWN/UP, SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN/UP,
    SDL_MOUSEWHEEL events to SDLKeyboard/SDLMouse
  - W3DGameClient factory methods gated by `_WIN32` (both Generals and GeneralsMD):
    Win32 returns DirectInputKeyboard/W3DMouse, non-Win32 returns SDLKeyboard/SDLMouse
  - BinkVideoPlayer include/factory gated behind `_WIN32`
  - W3DParticleSystemManager now created on SDL builds

### Phase 6: Video Playback
- [x] **6.1** Bink video player gated behind WIN32
  - BinkVideoPlayer.h include and factory method gated in W3DGameClient
  - GeneralsMD: FFmpeg path used when available, nullptr fallback otherwise
  - Generals: nullptr returned on non-Windows (no video playback yet)

### Compilation Fixes
- [x] **PreRTS.h** precompiled headers (Generals + GeneralsMD)
  - Gated all Windows-only system headers (`atlbase.h`, `windows.h`, `mmsystem.h`,
    `shlobj.h`, `vfw.h`, `wininet.h`, etc.) behind `#ifdef _WIN32`
  - Non-Windows path includes `<Utility/compat.h>` for portable type definitions
  - Cross-platform standard headers kept outside guard
  - Unblocks ~1000 source files from compilation on macOS
- [x] **WorldHeightMap.cpp** - Gated `#include "windows.h"` behind `_WIN32`
- [x] **framgrab.h/cpp** - Entire AVI capture wrapped behind `#ifdef _WIN32`
- [x] **WWAudio Threads.h/cpp** - Win32 threading code wrapped behind `#ifdef _WIN32`
- [x] **GameMemory.cpp** - Replaced `GlobalAlloc`/`GlobalFree`/`GlobalSize` with
  `malloc`/`free`/`malloc_size` (macOS) or `malloc_usable_size` (Linux) on non-Windows
- [x] **SystemAllocator.h** - Added `GlobalAlloc`/`GlobalFree` compat macros for non-Windows
- [x] **Debug library** (39 files changed)
  - All Windows-specific cpp files (`debug_debug.cpp`, `debug_cmd.cpp`, `debug_except.cpp`,
    `debug_io_con/flat/net/ods.cpp`, `debug_stack.cpp`) wrapped behind `#ifdef _WIN32`
  - Created `debug_posix_stubs.cpp` with minimal Debug class (stderr-based output,
    `CrashBegin`/`CrashDone` for `DCRASH_RELEASE`, stream operators, static init)
  - Headers (`internal_io.h`, `internal_except.h`, `debug_stack.h`) gated for non-Windows
  - CMake PCH: `<windows.h>` gated behind `$<$<PLATFORM_ID:Windows>:...>`
- [x] **Profile library**
  - `profile.cpp`: Portable `ProfileAllocMemory`/`ProfileReAllocMemory`/`ProfileFreeMemory`
    using `malloc`/`realloc`/`free` on non-Windows; `GetClockCyclesFast` using `std::chrono`
  - `internal.h`/`internal_funclevel.h`: `<windows.h>` gated; `HANDLE testEvent` gated
  - `wsprintf` → `sprintf` in `profile_funclevel.cpp`, `profile_highlevel.cpp`
  - CMake PCH: `<windows.h>` gated behind `$<$<PLATFORM_ID:Windows>:...>`
- [x] **DbgHelpLoader.h/cpp** - Wrapped behind `_WIN32` with non-Windows stub class
- [x] **rcfile.h/cpp** - Wrapped behind `_WIN32` (Windows resource file API)
- [x] **mmsys.h** - Guard `mmsystem.h` include behind `_WIN32`
- [x] **textureloader.cpp** - Guard `mmsystem.h` include behind `_WIN32`
- [x] **Download.cpp** - Guard `mmsystem.h` and `direct.h` behind `_WIN32`
- [x] **9 WWVegas files** - Gated `<windows.h>` in WW3D2 (agg_def.cpp,
  dx8webbrowser.h, texturethumbnail.cpp), WWAudio, WWSaveLoad, WWDownload
- [x] **WebBrowser.h** - Gate `atlbase.h` and `windows.h` behind `_WIN32`
- [x] **Debug.cpp** - Gate `HWND`, `MessageBox`, `SetWindowPos` behind `_WIN32` with stderr fallback
- [x] **wwmemlog.cpp** - Use `thread_local` instead of `__declspec(thread)` on non-MSVC
- [x] **render2dsentence.cpp/h** - Gate GDI font rendering (`Create_GDI_Font`, `Free_GDI_Font`,
  `Store_GDI_Char`) behind `_WIN32` with non-Windows stubs; use `void*` for GDI handles
- [x] **StagingRoomGameInfo.cpp** - Gate SNMP-based local address detection behind `_WIN32`
- [x] **PingThread.cpp** - Gate ICMP ping via `LoadLibrary`/`GetProcAddress` behind `_WIN32`
- [x] **ScriptEngine.cpp** (Generals + GeneralsMD) - Add `HMODULE`/`FARPROC`/`LoadLibrary`/
  `GetProcAddress`/`FreeLibrary` compat stubs for non-Windows (debug/particle editor DLLs)
- [x] **WWCOMUtil.cpp/h** - Wrap entire COM utility implementation behind `_WIN32`
- [x] **dx8webbrowser.h** - Disable `ENABLE_EMBEDDED_BROWSER` on non-Windows
- [x] **FTP.cpp** - Gate `process.h`, `io.h`, `direct.h` behind `_WIN32`; add
  `OutputDebugString` compat macro
- [x] **GameMemoryInit.cpp** - Gate `GetModuleFileName` behind `_WIN32` with relative path fallback
- [x] **Debug.cpp** - Gate `GetModuleFileName` and `OutputDebugString` behind `_WIN32`
- [x] **ini.cpp** - Gate `OutputDebugString` behind `_WIN32` with `fprintf(stderr)` fallback

### Phase 7–8: Packaging, Testing
- [ ] Not yet started

---

## Phase 9: Implementing Stubbed Functionality

Phases 1–7 focused on getting the codebase to **compile** on macOS/POSIX. Many subsystems
were stubbed with no-ops or safe defaults to unblock compilation. This section catalogs
every stub that needs a real implementation, grouped by subsystem and prioritized by
severity.

### 9.1 GL Rendering — Vertex/Index Buffer Upload & Draw Calls (BLOCKING)

The DX8 stub file (`Core/Libraries/Source/WWVegas/WW3D2/dx8_stubs.cpp`) allocates CPU-side
memory for vertex/index buffer locks but **never uploads data to the GPU or issues draw
calls**. Nothing will appear on screen until these are wired to real GL VBOs/VAOs.

**What's stubbed:**
- [ ] `DX8Wrapper::Set_Vertex_Buffer(const VertexBufferClass*)` — no-op (~line 419)
- [ ] `DX8Wrapper::Set_Vertex_Buffer(const DynamicVBAccessClass&)` — no-op (~line 420)
- [ ] `DX8Wrapper::Set_Index_Buffer(const IndexBufferClass*, unsigned short)` — no-op (~line 421)
- [ ] `DX8Wrapper::Set_Index_Buffer(const DynamicIBAccessClass&, unsigned short)` — no-op (~line 422)
- [ ] `DX8Wrapper::Draw(...)` — no-op (~line 424)
- [ ] `DX8Wrapper::Draw_Triangles(...)` — two overloads, both no-op (~lines 425-426)
- [ ] `DX8Wrapper::Draw_Strip(...)` — no-op (~line 427)
- [ ] `DX8Wrapper::Draw_Sorting_IB_VB(...)` — no-op (~line 423)

**What's partially working:**
- `VertexBufferClass::WriteLockClass` allocates a CPU buffer for vertex data — but the data
  is freed on lock destruction without ever uploading to GL
- `IndexBufferClass::WriteLockClass` same issue for index data
- `DX8VertexBufferClass::Create_Vertex_Buffer()` — empty, never creates a GL VBO
- All `DX8VertexBufferClass::Copy(...)` overloads — empty (~lines 758-763)
- `DynamicVBAccessClass::Allocate_DX8_Dynamic_Buffer()` — empty (~line 777)
- `DynamicVBAccessClass::WriteLockClass` — vertices is nullptr (~line 781)

**Implementation plan:**
1. Create GL VBOs in `DX8VertexBufferClass` constructor / `Create_Vertex_Buffer()`
2. Create GL IBOs in `DX8IndexBufferClass` constructor
3. In `WriteLockClass` destructor, call `glBufferData` / `glBufferSubData` to upload
4. Create a VAO per vertex format in `Set_Vertex_Buffer`, configure `glVertexAttribPointer`
   based on FVF flags
5. Implement `Draw()` / `Draw_Triangles()` / `Draw_Strip()` as `glDrawElements` /
   `glDrawArrays` calls
6. Wire `DynamicVBAccessClass` to use a pooled GL streaming buffer (`GL_STREAM_DRAW`)

### 9.2 GL Rendering — Texture Loading from Files (BLOCKING)

Textures created from raw dimensions work (the `_Create_DX8_Texture(w, h, fmt, ...)` path
creates real GL textures). But textures loaded from **file paths** or **surfaces** return
nullptr.

**What's stubbed:**
- [ ] `DX8Wrapper::_Create_DX8_Texture(const char*, MipCountType)` → `nullptr` (~line 517)
- [ ] `DX8Wrapper::_Create_DX8_Texture(IDirect3DSurface8*, MipCountType)` → `nullptr` (~line 518)
- [ ] `D3DXCreateTextureFromFile(...)` → `D3DERR_INVALIDCALL` (d3d8-compat, ~line 986)
- [ ] `D3DXLoadSurfaceFromFile(...)` → `D3DERR_INVALIDCALL` (d3d8-compat, ~line 987)
- [ ] `D3DXLoadSurfaceFromSurface(...)` → `D3DERR_INVALIDCALL` (d3d8-compat, ~line 988)
- [ ] `DX8Wrapper::_Update_Texture(...)` — no-op (~line 526)

**Implementation plan:**
1. For `_Create_DX8_Texture(const char*)`: Load TGA/DDS using the existing `TargaImage`
   loader or `stb_image`, convert to GL texture
2. For surface-to-texture: Read pixel data from the surface's CPU buffer, upload via
   `glTexImage2D`
3. For `D3DXCreateTextureFromFile`: Route through the same TGA/DDS loading path

### 9.3 GL Rendering — Surfaces, Render Targets, Z-Textures (DEFERRABLE)

These enable render-to-texture effects, screenshots, and special rendering passes. The game
can partially function without them but visual effects will be missing.

**What's stubbed:**
- [ ] `DX8Wrapper::_Create_DX8_ZTexture(...)` → `nullptr` (~line 519)
- [ ] `DX8Wrapper::_Create_DX8_Cube_Texture(...)` → `nullptr` (~line 520)
- [ ] `DX8Wrapper::_Create_DX8_Volume_Texture(...)` → `nullptr` (~line 521)
- [ ] `DX8Wrapper::_Create_DX8_Surface(unsigned, unsigned, WW3DFormat)` → `nullptr` (~line 522)
- [ ] `DX8Wrapper::_Create_DX8_Surface(const char*)` → `nullptr` (~line 523)
- [ ] `DX8Wrapper::_Get_DX8_Front_Buffer()` → `nullptr` (~line 524)
- [ ] `DX8Wrapper::_Get_DX8_Back_Buffer(unsigned)` → `nullptr` (~line 525)
- [ ] `DX8Wrapper::Create_Render_Target(int, int, WW3DFormat)` → `nullptr` (~line 533)
- [ ] `DX8Wrapper::Create_Additional_Swap_Chain(HWND)` → `nullptr` (~line 532)
- [ ] `IDirect3DSurface8::LockRect/UnlockRect/GetDesc` → `D3DERR_INVALIDCALL` (d3d8-compat)
- [ ] `IDirect3DTexture8::LockRect/UnlockRect/GetSurfaceLevel` → `D3DERR_INVALIDCALL`
- [ ] `IDirect3DVertexBuffer8::Lock/Unlock` → `D3DERR_INVALIDCALL`
- [ ] `IDirect3DIndexBuffer8::Lock/Unlock` → `D3DERR_INVALIDCALL`

**Implementation plan:**
1. Use GL Framebuffer Objects (FBOs) for render targets
2. Implement `LockRect`/`UnlockRect` via `glReadPixels` (read) / PBO (write)
3. Z-textures → GL depth textures attached to FBOs
4. Front/back buffer access → `glReadPixels` from default framebuffer

### 9.4 GL Rendering — Lighting (DEFERRABLE)

Lighting is set up through the DX8 light API but the stubs discard all light data.

**What's stubbed:**
- [ ] `DX8Wrapper::Set_Light(unsigned, const D3DLIGHT8*)` — no-op (~line 529)
- [ ] `DX8Wrapper::Set_Light(unsigned, const LightClass&)` — no-op (~line 530)

**Implementation plan:**
1. Pass light parameters (position, direction, color, attenuation) as shader uniforms
2. Extend the GLSL fragment shader with directional + point light calculations
3. The game primarily uses a small number of lights (ambient + 1–2 directional)

### 9.5 GL Rendering — Device Mode Selection (DEFERRABLE)

These functions manage display mode enumeration, resolution switching, and device selection.
They return `false` which means fullscreen mode switching won't work, but windowed mode is
fine.

**What's stubbed:**
- [ ] `DX8Wrapper::Set_Next_Render_Device()` → `false` (~line 605)
- [ ] `DX8Wrapper::Toggle_Windowed()` → `false` (~line 606)
- [ ] `DX8Wrapper::Find_Color_And_Z_Mode()` → `false` (~line 623)
- [ ] `DX8Wrapper::Find_Color_Mode()` → `false` (~line 624)
- [ ] `DX8Wrapper::Find_Z_Mode()` → `false` (~line 625)
- [ ] `DX8Wrapper::Registry_Save_Render_Device(...)` → `false` (~lines 627-630)
- [ ] `DX8Wrapper::Registry_Load_Render_Device(...)` → `false` (~lines 629-630)

**Implementation plan:**
1. Use `SDL_GetDisplayMode` to enumerate available resolutions
2. `Toggle_Windowed` → `SDL_SetWindowFullscreen`
3. Registry save/load → use the config file backend once registry is implemented

### 9.6 GL Rendering — DX8Caps (DEFERRABLE)

GPU capability detection returns safe defaults. This means the engine won't auto-detect
optimal quality settings.

**What's stubbed:**
- [ ] `DX8Caps::Init_Caps()` — empty (~line 663)
- [ ] `DX8Caps::Compute_Caps()` — empty (~line 664)
- [ ] All `DX8Caps::Check_*_Support()` — empty (~lines 665-682)
- [ ] Vendor detection returns `VENDOR_UNKNOWN`

**Implementation plan:**
1. Query GL capabilities via `glGetIntegerv` (max texture size, extensions, etc.)
2. Set DX8Caps fields accordingly (e.g., `MaxTextureWidth`, `MaxSimultaneousTextures`)
3. Check for S3TC extension support for compressed textures

### 9.7 GL Rendering — Mesh Renderer (BLOCKING for 3D models)

The mesh renderer pipeline is entirely stubbed. Without it, no 3D models (units,
buildings, terrain meshes) will render.

**What's stubbed:**
- [ ] `DX8MeshRendererClass::Init()` — empty (~line 841)
- [ ] `DX8MeshRendererClass::Shutdown()` — empty (~line 842)
- [ ] `DX8MeshRendererClass::Flush()` — empty (~line 843)
- [ ] `DX8MeshRendererClass::Set_Current_Mesh(...)` — empty (~line 844)
- [ ] `DX8MeshRendererClass::Set_Polygon_Count(...)` — empty (~line 845)
- [ ] All `FVFCategoryContainer` methods — empty (~lines 854-869)

**Implementation plan:**
1. Depends on VBO/VAO infrastructure from 9.1
2. Implement `Set_Current_Mesh` to bind the mesh's vertex/index buffers
3. `Flush` should issue the accumulated draw calls for the current batch

### 9.8 Font Rendering — Replace GDI with Cross-Platform Backend (BLOCKING)

All in-game text (menus, HUD, chat, tooltips) relies on GDI font rasterization. The
non-Windows stub creates zero-width empty character entries, so all text will be invisible.

**Files:** `Core/Libraries/Source/WWVegas/WW3D2/render2dsentence.cpp/h`

**What's stubbed:**
- [ ] `FontCharsClass::Create_GDI_Font(const char*)` — estimates metrics but creates no
  bitmap or font handle (~line 1626 in .cpp)
- [ ] `FontCharsClass::Store_GDI_Char(WCHAR)` — returns zero-width character data with
  no pixel data (~line 1464 in .cpp)
- [ ] `FontCharsClass::Free_GDI_Font()` — just nulls out pointers (~line 1687 in .cpp)
- [ ] GDI handle members in `FontCharsClass` are `void*` placeholders on non-Windows
  (~lines 131-136 in .h)

**Implementation plan (recommended: FreeType + texture atlas):**
1. Add FreeType2 as a vcpkg dependency
2. In `Create_GDI_Font`, open a `.ttf` file via `FT_New_Face`, set pixel size
3. In `Store_GDI_Char`, call `FT_Load_Char` + `FT_Render_Glyph` to get a bitmap, then
   copy it into the existing `GDIBitmapBits` buffer (the rest of the pipeline that
   uploads to a GL texture should work as-is)
4. Map Windows font names ("Arial", "Courier New") to macOS system font paths or bundled
   TTF files

### 9.9 Audio System — Replace Miles with OpenAL Soft (BLOCKING)

The game currently uses `AudioManagerDummy` (no-op) on macOS. There is no sound at all.

**File:** `Core/GameEngineDevice/Source/SDLDevice/Common/SDLGameEngine.cpp` (~line 193)

**What's stubbed:**
- [ ] `SDLGameEngine::createAudioManager()` returns `AudioManagerDummy` — all audio calls
  are silently ignored

**Implementation plan:**
1. Create `OpenALAudioManager` class extending `AudioManager` (the abstract base already
   exists at `Core/GameEngine/Include/Common/GameAudio.h`)
2. Key methods to implement:
   - `openDevice()` → `alcOpenDevice` / `alcCreateContext`
   - `playAudioEvent()` → `alGenSources` / `alSourcePlay`
   - `set3DSoundPosition()` → `alSource3f(AL_POSITION, ...)`
   - `setListenerPosition()` → `alListener3f(AL_POSITION, ...)`
   - `streaming playback` → buffer queue with `alSourceQueueBuffers`
3. Audio format decoding: WAV (trivial PCM), MP3 (use minimp3 header-only library)
4. Wire `SDLGameEngine::createAudioManager()` to return the OpenAL implementation

### 9.10 Video Playback — Wire FFmpeg for macOS (MODERATE)

Bink video is Windows/licensed only. An `FFmpegVideoPlayer` already exists in the codebase
but calls `initializeBinkWithMiles()` which won't work on macOS.

**Files:**
- `Core/GameEngineDevice/Source/VideoDevice/FFmpeg/FFmpegVideoPlayer.cpp`
- `Core/GameEngineDevice/Source/VideoDevice/Bink/BinkVideoPlayer.cpp` (gated behind WIN32)

**What's stubbed:**
- [ ] Non-Windows video creation returns `nullptr` in W3DGameClient — no video playback
- [ ] `FFmpegVideoPlayer` exists but has Miles audio dependency in initialization

**Implementation plan:**
1. Remove Miles dependency from `FFmpegVideoPlayer::initialize()` — use FFmpeg's own audio
   decoding + feed PCM to OpenAL
2. Ensure FFmpeg decodes video frames and uploads to GL texture for display
3. Enable `RTS_BUILD_OPTION_FFMPEG=ON` in macOS CMake presets
4. Wire W3DGameClient to return `FFmpegVideoPlayer` on non-Windows

### 9.11 Registry / Settings Persistence (DEFERRABLE)

All game settings (resolution, volume, keybindings, online login) are lost on restart
because registry operations are no-ops.

**Files:**
- `GeneralsMD/Code/GameEngine/Source/Common/System/registry.cpp` (~lines 169-189)
- `Generals/Code/GameEngine/Source/Common/System/registry.cpp` (same pattern)
- `Core/Libraries/Source/WWVegas/WWLib/registry.cpp` (~lines 631-740)

**What's stubbed:**
- [ ] All `Get*FromGeneralsRegistry()` functions return the default value parameter
- [ ] All `Set*InGeneralsRegistry()` functions are no-ops (data not persisted)
- [ ] All `RegistryClass::Get_*()` return defaults, `Set_*()` are no-ops
- [ ] `RegistryClass::Load_Registry()` / `Save_Registry()` — no-ops

**Implementation plan:**
1. Create a simple INI or JSON config file backend at
   `~/Library/Application Support/Generals/settings.ini` (macOS) or
   `~/.config/generals/settings.ini` (Linux)
2. Implement `Get_*` → read from parsed config, `Set_*` → update in-memory + flush to disk
3. Game engine registry functions route through the same backend

### 9.12 Debug / Diagnostics Infrastructure (DEFERRABLE)

The POSIX debug stubs provide basic stderr output but lack stack traces, crash dump
collection, and the interactive debug command system.

**File:** `Core/Libraries/Source/debug/debug_posix_stubs.cpp`

**What's stubbed:**
- [ ] `DebugStackwalk` — `GetFrameCount()` returns 0, no actual stack walking (~line 384)
- [ ] `AddFrameEntry()` → `nullptr` — no per-frame debug tracking (~line 352)
- [ ] `ExecCommand()` — empty, no debug console commands (~line 365)
- [ ] `AddLogGroup()` → `nullptr` — no log group management (~line 374)

**Implementation plan:**
1. Stack walking: use `backtrace()` / `backtrace_symbols()` from `<execinfo.h>` (macOS)
2. Crash dumps: register `SIGSEGV`/`SIGABRT` handler to write a crash log
3. Debug commands and log groups: low priority, mainly useful for development

### 9.13 DX8 Rendering Statistics (DEFERRABLE)

All frame statistics counters return 0. This only affects debug/profiling overlays.

**What's stubbed (~lines 560-569 in dx8_stubs.cpp):**
- [ ] `Get_Last_Frame_Matrix_Changes()` → 0
- [ ] `Get_Last_Frame_Material_Changes()` → 0
- [ ] `Get_Last_Frame_Vertex_Buffer_Changes()` → 0
- [ ] `Get_Last_Frame_Index_Buffer_Changes()` → 0
- [ ] `Get_Last_Frame_Light_Changes()` → 0
- [ ] `Get_Last_Frame_Texture_Changes()` → 0
- [ ] `Get_Last_Frame_Render_State_Changes()` → 0
- [ ] `Get_Last_Frame_Texture_Stage_State_Changes()` → 0
- [ ] `Get_Last_Frame_DX8_Calls()` → 0
- [ ] `Get_Last_Frame_Draw_Calls()` → 0

**Implementation plan:** Increment atomic counters in the corresponding GL operations.

---

### Priority Order for Phase 9

| Priority | Section | Subsystem | Why |
|----------|---------|-----------|-----|
| **P0** | 9.1 | VBO/VAO/Draw calls | Nothing renders without this |
| **P0** | 9.2 | Texture loading from files | No textures = white screen |
| **P0** | 9.7 | Mesh renderer | No 3D models without this |
| **P0** | 9.8 | Font rendering (FreeType) | No readable text in menus/HUD |
| **P1** | 9.9 | Audio (OpenAL) | Game is playable but silent |
| **P1** | 9.10 | Video playback (FFmpeg) | Cutscenes won't play |
| **P1** | 9.4 | Lighting | Scene will be flat-lit but visible |
| **P2** | 9.3 | Surfaces / render targets | Advanced effects (water, shadows) |
| **P2** | 9.11 | Registry / settings | Settings lost on restart |
| **P2** | 9.5 | Display mode selection | Fullscreen switching |
| **P3** | 9.6 | DX8Caps | Auto-quality detection |
| **P3** | 9.12 | Debug infrastructure | Developer diagnostics only |
| **P3** | 9.13 | Render statistics | Profiling overlay only |

This document outlines a plan for porting Command & Conquer: Generals (and Zero Hour) to
run on macOS in addition to Windows. The codebase is a C++ game engine with deep Windows
dependencies across graphics (DirectX 8), audio (Miles Sound System), input (DirectInput),
windowing (Win32), and system services (Registry, Winsock, threading). However, significant
cross-platform groundwork already exists -- C++17 `StdDevice` file I/O, POSIX compatibility
headers, and endian support for macOS are already in place. The plan is organized into
phases ordered by dependency: build system first, then platform abstraction, then the major
subsystem ports (graphics, audio, input/windowing), and finally packaging and testing.

---

## Current State Assessment

### What already works cross-platform

- **File I/O**: `StdDevice` (StdLocalFile, StdLocalFileSystem, StdBIGFile, StdBIGFileSystem)
  uses C++17 `<filesystem>` with Unix case-insensitive path matching already implemented.
- **Endianness**: `Dependencies/Utility/Utility/endian_compat.h` already has `__APPLE__`
  support using `<libkern/OSByteOrder.h>`.
- **Threading**: `thread_compat.h` provides pthread-based `Sleep()`, `GetCurrentThreadId()`.
- **Time**: `time_compat.h` maps `timeGetTime()` and `GetTickCount()` to POSIX
  `clock_gettime()`.
- **Strings**: `string_compat.h` maps `_stricmp`/`_strnicmp` to `strcasecmp`/`strncasecmp`.
- **Intrinsics**: `intrin_compat.h` has GCC/Clang builtins for `__debugbreak`, `_rdtsc`,
  `cpuid` with non-x86 fallbacks.
- **Build system**: CMake 3.25+ with vcpkg; MinGW cross-compilation already established.

### What needs porting (major areas)

| Subsystem | Current Tech | Files Affected | Difficulty |
|-----------|-------------|----------------|------------|
| Graphics/Rendering | DirectX 8 (WW3D engine) | 236+ files | Very High |
| Audio | Miles Sound System | ~10 core files | High |
| Input | DirectInput 8 | ~8 files | Medium |
| Windowing/Event Loop | Win32 (WinMain, WndProc) | ~5 files | Medium |
| Networking | Winsock2 | ~30 files | Low-Medium |
| Registry/Config | Windows Registry | ~15 files | Low |
| Threading/Sync | Win32 (CriticalSection, Events) | ~88 files | Medium |
| Video Playback | Bink SDK | ~5 files | Low (FFmpeg exists) |
| COM/OLE | Win32 COM | ~26 files | Low (mostly tools) |
| Resource Files (.rc) | Win32 Resources | 16 files | Low (game only needs 1) |

---

## Phase 1: Build System & CMake Configuration

**Goal**: Get the project configuring and partially compiling on macOS with stubs.

### 1.1 Add macOS CMake presets

Add to `CMakePresets.json`:
- `macos-arm64` preset (Apple Silicon, primary target)
- `macos-x86_64` preset (Intel Macs, secondary)
- `macos-universal` preset (fat binary, optional/future)

Create toolchain files:
- `cmake/toolchains/macos-arm64.cmake`
- `cmake/toolchains/macos-x86_64.cmake`

### 1.2 Add platform detection to compilers.cmake

Extend `cmake/compilers.cmake` to detect `APPLE` and set `IS_MACOS_BUILD`. Add
Clang-specific warning flags and define macOS-appropriate compiler options (e.g.,
`-mmacosx-version-min=13.0` for macOS Ventura as the minimum target, since it is the
oldest version that supports the latest Metal features needed).

### 1.3 Configure vcpkg for macOS

Verify vcpkg triplets work for macOS (`arm64-osx`, `x64-osx`). The current dependencies
(zlib, ffmpeg) already have macOS vcpkg ports. Add any new dependencies required by later
phases (e.g., SDL2/SDL3).

### 1.4 Stub out Windows-only subsystems

Create CMake options to conditionally compile platform-specific device code:
- `RTS_BUILD_PLATFORM_WIN32` (existing behavior)
- `RTS_BUILD_PLATFORM_MACOS` (new)

Gate `Win32Device/`, `MilesAudioDevice/`, DirectInput code, and DX8 SDK behind
`RTS_BUILD_PLATFORM_WIN32`. Create stub/placeholder device implementations for macOS so
the project compiles (even if it doesn't link or run yet).

### 1.5 Fix compilation issues

Address all compiler errors from Clang on macOS:
- MSVC-specific pragmas guarded by `#ifdef _MSC_VER`
- Windows header includes guarded by `#ifdef _WIN32`
- SEH (`__try/__except`) replaced with C++ exceptions or removed on macOS
- Fix any C++20 conformance issues Clang is stricter about than MSVC

---

## Phase 2: Platform Abstraction Layer

**Goal**: Create clean interfaces that decouple game logic from platform specifics.

### 2.1 Windowing and event loop abstraction

**Recommended approach**: Use **SDL2** (or SDL3) as the cross-platform windowing layer.

Create a new device directory structure:
```
Core/GameEngineDevice/Include/SDLDevice/
Core/GameEngineDevice/Source/SDLDevice/
```

Implement:
- `SDLGameEngine.h/cpp` -- replacement for `Win32GameEngine`, handling SDL window creation,
  event pump, and application lifecycle
- Main entry point: Replace `WinMain()` with a portable `main()` that initializes SDL

SDL provides window management, event handling, and OpenGL/Metal context creation, which
covers windowing, input, and graphics context needs in one library.

**Why SDL over a custom Cocoa/AppKit layer**: SDL is battle-tested for game ports, handles
macOS quirks (Retina scaling, fullscreen spaces, app activation), and avoids writing
Objective-C++ directly. It also makes a future Linux port trivial.

### 2.2 Threading and synchronization abstraction

Replace Win32 threading primitives with `std::thread`, `std::mutex`, `std::condition_variable`
(C++17/20):

| Win32 API | Replacement |
|-----------|-------------|
| `CRITICAL_SECTION` | `std::mutex` or `std::recursive_mutex` |
| `CreateThread` | `std::thread` |
| `CreateEvent` / `SetEvent` / `WaitForSingleObject` | `std::condition_variable` + `std::mutex` |
| `WaitForMultipleObjects` | Custom wrapper or per-use refactor |
| `InterlockedIncrement/Decrement` | `std::atomic<int>` |
| `Sleep()` | `std::this_thread::sleep_for()` |

The existing `CriticalSection.h` wrapper is a good place to start -- replace its internals
with `std::recursive_mutex` while keeping the same interface.

### 2.3 Registry / configuration abstraction

Create a platform-agnostic configuration store interface:
- Windows: Continue using Windows Registry
- macOS: Use a JSON or INI config file in `~/Library/Application Support/Generals/`

The existing `Registry.h` interface already provides a clean abstraction. Implement a new
backend (`FileRegistry` or `ConfigStore`) that reads/writes a config file.

### 2.4 Networking abstraction

Winsock and BSD sockets are very similar. The main changes needed:
- Replace `WSAStartup()`/`WSACleanup()` with no-ops on macOS (not needed on POSIX)
- Replace `closesocket()` with `close()`
- Replace `SOCKET` type with `int`
- Replace `WSAEWOULDBLOCK` with `EWOULDBLOCK`
- Replace `ioctlsocket()` with `fcntl()`

These are small, well-understood changes. The GameSpy SDK may already have some POSIX
support since it was originally multi-platform.

---

## Phase 3: Graphics Rendering (DirectX 8 to OpenGL/Metal)

**Goal**: Get the WW3D engine rendering on macOS. This is the largest single workstream.

### Approach evaluation

| Option | Pros | Cons |
|--------|------|------|
| **A. SDL2 + OpenGL 3.3** | Mature, well-understood, SDL handles context | OpenGL deprecated on macOS (still works, Apple ships 4.1) |
| **B. SDL2 + Metal via SDL_Renderer** | Native perf, future-proof | SDL_Renderer too high-level for WW3D's needs |
| **C. Direct Metal port** | Best performance | Requires Objective-C++, macOS-only |
| **D. SDL2 + MoltenVK + DXVK/D3D8-to-Vulkan** | Minimal WW3D changes | Complex dependency chain, translation overhead |
| **E. SDL2 + OpenGL 3.3 (now) + Metal (later)** | Pragmatic, iterative | Two ports eventually |

**Recommended: Option A (SDL2 + OpenGL 3.3 core profile)** as the initial target, with a
path to Metal later if performance requires it. Rationale:
- macOS still ships OpenGL 4.1 and shows no signs of removing it soon
- DX8-era rendering maps well to OpenGL 3.3 (fixed-function features can be replicated
  with simple shaders)
- Gets something running fastest
- The rendering abstraction created for OpenGL can later be retargeted to Metal

### 3.1 Create a rendering abstraction interface

The current code calls D3D8 APIs directly through `dx8wrapper`. Create an abstract
rendering interface:

```cpp
class RenderDevice {
public:
    virtual ~RenderDevice() = default;
    virtual bool Init(void* windowHandle, int width, int height, bool fullscreen) = 0;
    virtual void BeginScene() = 0;
    virtual void EndScene() = 0;
    virtual void Present() = 0;
    virtual void Clear(unsigned color, float depth) = 0;
    virtual TextureHandle CreateTexture(...) = 0;
    virtual VertexBufferHandle CreateVertexBuffer(...) = 0;
    virtual IndexBufferHandle CreateIndexBuffer(...) = 0;
    virtual void SetRenderState(...) = 0;
    virtual void SetTextureStageState(...) = 0;
    virtual void DrawPrimitive(...) = 0;
    virtual void DrawIndexedPrimitive(...) = 0;
    // ... etc
};
```

### 3.2 Implement DX8 backend (preserve existing behavior)

Refactor `dx8wrapper` to implement the `RenderDevice` interface. This preserves the
existing Windows rendering path and ensures no regressions.

### 3.3 Implement OpenGL 3.3 backend

Create `GLRenderDevice` implementing the same interface:

Key mapping from D3D8 to OpenGL:
| D3D8 Concept | OpenGL Equivalent |
|-------------|-------------------|
| `IDirect3DDevice8` | OpenGL context (via SDL) |
| Vertex Buffers | `glGenBuffers` / `GL_ARRAY_BUFFER` |
| Index Buffers | `glGenBuffers` / `GL_ELEMENT_ARRAY_BUFFER` |
| Textures | `glGenTextures` / `glTexImage2D` |
| Render States | `glEnable`/`glDisable` + state calls |
| Texture Stage States | GLSL fragment shader uniforms |
| Fixed-function pipeline | Simple vertex/fragment shaders |
| `D3DXCreateTextureFromFile` | stb_image + `glTexImage2D` |
| Vertex declarations (FVF) | Vertex Array Objects (VAOs) |
| `DrawPrimitive` | `glDrawArrays` |
| `DrawIndexedPrimitive` | `glDrawElements` |

### 3.4 Shader translation

D3D8 uses a fixed-function pipeline with limited programmable shaders. For OpenGL 3.3 core
profile (which has no fixed-function pipeline), create a small set of GLSL shaders that
replicate the fixed-function behavior:

- Basic vertex transform + lighting shader
- Multi-texture blending shader (for terrain)
- Alpha-test / alpha-blend shader
- Fog shader
- Shadow/decal projection shader

The `W3DShaderManager` already centralizes shader selection, making it a natural place to
add the OpenGL shader path.

### 3.5 Texture format handling

D3D8 uses D3DFMT_* texture formats. Create a mapping layer:
- `D3DFMT_A8R8G8B8` -> `GL_RGBA8`
- `D3DFMT_X8R8G8B8` -> `GL_RGBA8` (ignore alpha)
- `D3DFMT_DXT1/3/5` -> `GL_COMPRESSED_RGBA_S3TC_DXT1/3/5_EXT` (widely supported)
- `D3DFMT_A4R4G4B4` -> `GL_RGBA4`

Note: BGRA vs RGBA byte order will need swizzling or use of `GL_BGRA` extension.

### 3.6 Port WW3D2 library internals

Files in `Core/Libraries/Source/WWVegas/WW3D2/` that need updates:
- `dx8wrapper.h/cpp` -- Core D3D8 interface (route through RenderDevice)
- `dx8caps.h/cpp` -- GPU capability detection (create GL equivalent)
- `dx8vertexbuffer.h/cpp` -- Vertex buffer management
- `dx8indexbuffer.h/cpp` -- Index buffer management
- `dx8fvf.h/cpp` -- Flexible Vertex Format (map to VAO attributes)
- `texture.cpp` -- Texture loading and management
- `ww3d.cpp` -- Engine initialization
- `sortingrenderer.cpp` -- Sorted/transparent rendering
- `meshmdl.cpp`, `mesh.cpp` -- Mesh rendering

---

## Phase 4: Audio System

**Goal**: Replace Miles Sound System with a cross-platform audio solution.

### Recommended approach: OpenAL Soft + libsndfile (or SDL_mixer)

**Option A: OpenAL Soft** -- Provides 3D positional audio similar to Miles. Open source,
works on macOS natively. Pair with a decoder library for WAV/MP3.

**Option B: SDL_mixer** -- Simpler API, integrates with SDL2. Less control over 3D audio.

**Recommended: OpenAL Soft** because the game uses 3D positional audio extensively
(H3DSAMPLE in Miles), and OpenAL's API maps well to Miles' concepts.

### 4.1 Create audio device abstraction

The game already has `AudioManager` as a base class with `MilesAudioManager` as the
implementation. Create:

```
Core/GameEngineDevice/Include/OpenALDevice/OpenALAudioManager.h
Core/GameEngineDevice/Source/OpenALDevice/OpenALAudioManager.cpp
```

### 4.2 Implement OpenAL backend

Map Miles concepts to OpenAL:
| Miles Concept | OpenAL Equivalent |
|--------------|-------------------|
| `HSAMPLE` | `ALuint` source |
| `H3DSAMPLE` | `ALuint` source with 3D position |
| `HSTREAM` | Streaming source with buffer queue |
| `AIL_open_stream` | Buffer queue with decoder thread |
| `AIL_set_3D_position` | `alSource3f(AL_POSITION, ...)` |
| `AIL_set_3D_velocity` | `alSource3f(AL_VELOCITY, ...)` |
| `AIL_set_listener_3D_position` | `alListener3f(AL_POSITION, ...)` |

### 4.3 Audio format decoding

Miles handles format decoding internally. For OpenAL, add decoders for:
- WAV (trivial, direct PCM loading)
- MP3 (use minimp3 or dr_mp3 -- single-header, no dependencies)

---

## Phase 5: Input System

**Goal**: Replace DirectInput with SDL input handling.

### 5.1 SDL Keyboard implementation

Create `SDLKeyboard.h/cpp` extending the existing `Keyboard` base class.
- Map SDL scancodes to the game's key enum
- Handle key repeat, modifier keys
- The existing `Win32DIKeyboard` has a comprehensive DirectInput-to-game-key mapping table
  that needs to be replicated for SDL scancodes

### 5.2 SDL Mouse implementation

Create `SDLMouse.h/cpp` extending the existing `Mouse` base class.
- Button state tracking from SDL events
- Relative/absolute mouse modes (SDL handles pointer grab)
- Cursor visibility and custom cursor support
- Mouse wheel support

### 5.3 Gamepad support (optional, future)

SDL provides gamepad support out of the box. The original game didn't support gamepads, but
this could be added later.

---

## Phase 6: Video Playback

**Goal**: Get video cutscenes playing on macOS.

The codebase already has an FFmpeg-based video player (`FFmpegVideoPlayer.h/cpp`) behind
the `RTS_BUILD_OPTION_FFMPEG` flag. FFmpeg works on macOS via vcpkg or Homebrew.

### 6.1 Enable FFmpeg path on macOS

- Set `RTS_BUILD_OPTION_FFMPEG=ON` for macOS builds
- Ensure `FFmpegVideoPlayer` renders to the new OpenGL backend instead of D3D8 surfaces
- Verify video texture upload path works with OpenGL

### 6.2 Disable Bink path on macOS

Gate the Bink video player behind `RTS_BUILD_PLATFORM_WIN32` since Bink SDK is
Windows-only.

---

## Phase 7: macOS Application Packaging

**Goal**: Create a proper macOS application bundle.

### 7.1 App bundle structure

Create CMake rules to produce a `.app` bundle:
```
Generals.app/
  Contents/
    Info.plist
    MacOS/
      generals          (executable)
    Resources/
      AppIcon.icns      (app icon)
    Frameworks/         (bundled dylibs if needed)
```

### 7.2 Info.plist

Define app metadata: bundle identifier, version, minimum macOS version, display name,
required capabilities.

### 7.3 Code signing

Add CMake post-build step for `codesign` (required for macOS Ventura+). For development,
ad-hoc signing (`codesign -s -`) is sufficient.

### 7.4 Asset path resolution

macOS apps typically store assets inside the bundle or in `~/Library/Application Support/`.
Add platform-specific logic to resolve game data paths:
- Check bundle Resources directory
- Check working directory (for development)
- Check a user-configurable path

---

## Phase 8: Testing and Validation

### 8.1 Unit/integration test infrastructure

- Verify existing tests compile and pass on macOS
- Add platform-specific test cases for new abstractions (file I/O, threading, networking)

### 8.2 Rendering validation

- Compare screenshots between Windows D3D8 and macOS OpenGL renders
- Verify terrain rendering, unit models, particle effects, water, shadows, UI
- Test all resolutions and fullscreen/windowed modes

### 8.3 Audio validation

- Verify 3D positional audio behaves correctly
- Test music streaming, sound effect mixing, volume controls

### 8.4 Gameplay testing

- Full campaign playthrough on macOS
- Multiplayer between Windows and macOS clients
- Performance profiling (frame rate, memory, CPU usage)

---

## Dependency Summary

New dependencies required for macOS:

| Library | Purpose | License | Availability |
|---------|---------|---------|-------------|
| SDL2 (or SDL3) | Windowing, input, GL context | zlib | vcpkg, Homebrew |
| OpenAL Soft | 3D audio | LGPL-2.1 | vcpkg, Homebrew |
| minimp3 or dr_mp3 | MP3 decoding | MIT / Public domain | Header-only |
| FFmpeg | Video playback | LGPL/GPL | vcpkg (already a dep) |

All are well-established, actively maintained, and available through vcpkg.

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| OpenGL deprecation on macOS | Low (short-term) | High | Apple still ships GL 4.1; Metal backend can be added later |
| D3D8 fixed-function pipeline hard to replicate | Medium | High | D3D8 era is simple; a handful of GLSL shaders suffice |
| Miles audio feature parity | Medium | Medium | OpenAL Soft is mature; 3D audio maps well |
| GameSpy SDK macOS compilation | Medium | Medium | SDK has some POSIX code; may need patches |
| Endianness issues on ARM (Apple Silicon) | Low | Medium | Endian compat header already exists; test thoroughly |
| Performance regression vs D3D8 | Low | Low | D3D8-era complexity is trivial for modern GPUs |
| Game logic with Windows assumptions | Medium | Medium | Path separators, case sensitivity already handled in StdDevice |

---

## Phase Order and Dependencies

```
Phase 1: Build System
    |
    v
Phase 2: Platform Abstraction (threading, config, networking)
    |
    +---> Phase 3: Graphics (largest effort, can start subsystem by subsystem)
    |
    +---> Phase 4: Audio (independent of graphics)
    |
    +---> Phase 5: Input (depends on SDL from Phase 2.1)
    |
    +---> Phase 6: Video (depends on graphics context from Phase 3)
    |
    v
Phase 7: Packaging
    |
    v
Phase 8: Testing
```

Phases 3, 4, and 5 can be worked on in parallel once Phase 2 establishes the SDL
foundation. Phase 6 depends on Phase 3 for rendering video frames. Phases 7 and 8 come
last.

---

## Files Most Affected (Top 20)

1. `CMakePresets.json` -- Add macOS presets
2. `cmake/compilers.cmake` -- macOS compiler detection
3. `GeneralsMD/Code/Main/WinMain.cpp` -- Replace with portable entry point
4. `Core/Libraries/Source/WWVegas/WW3D2/dx8wrapper.h/cpp` -- Rendering abstraction
5. `Core/Libraries/Source/WWVegas/WW3D2/dx8caps.h/cpp` -- GPU caps abstraction
6. `Core/Libraries/Source/WWVegas/WW3D2/dx8vertexbuffer.h/cpp` -- VB abstraction
7. `Core/Libraries/Source/WWVegas/WW3D2/dx8indexbuffer.h/cpp` -- IB abstraction
8. `Core/GameEngineDevice/Source/Win32Device/GameClient/Win32DIKeyboard.cpp` -- Input
9. `Core/GameEngineDevice/Source/Win32Device/GameClient/Win32DIMouse.cpp` -- Input
10. `Core/GameEngineDevice/Source/MilesAudioDevice/MilesAudioManager.cpp` -- Audio
11. `Core/GameEngineDevice/Source/Win32Device/Common/Win32GameEngine.cpp` -- Engine init
12. `Core/GameEngine/Source/Common/System/registry.cpp` -- Config storage
13. `Core/GameEngine/Include/Common/CriticalSection.h` -- Threading
14. `Core/GameEngineDevice/CMakeLists.txt` -- Device selection
15. `Core/Libraries/Source/WWVegas/WW3D2/texture.cpp` -- Texture loading
16. `Core/Libraries/Source/WWVegas/WW3D2/ww3d.cpp` -- Engine init
17. `Core/GameEngine/Source/GameNetwork/Transport.cpp` -- Networking
18. `Core/GameEngine/Source/GameNetwork/LANAPI.cpp` -- LAN networking
19. `Core/GameEngineDevice/Source/VideoDevice/Bink/BinkVideoPlayer.cpp` -- Video
20. `vcpkg.json` -- New macOS dependencies
