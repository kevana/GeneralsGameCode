/*
**	Command & Conquer Generals / Zero Hour
**	OpenGL 3.3 rendering infrastructure for the non-Windows DX8 replacement.
**	Provides GLSL shader management, VAO/VBO/IBO helpers, and render state tracking.
*/

#ifndef GL_RENDER_H
#define GL_RENDER_H

#ifndef _WIN32

#include <gl_compat.h>

// ============================================================================
// GLSL shader sources for fixed-function emulation
// ============================================================================

// Basic vertex shader: transforms position, passes through normals, UVs, color
static const char* s_basicVertexShader = R"GLSL(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord0;
layout(location = 4) in vec2 aTexCoord1;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uWorld;
uniform vec4 uAmbientColor;
uniform int uHasTexture;

out vec4 vColor;
out vec2 vTexCoord0;
out vec2 vTexCoord1;
out vec3 vNormal;
out vec3 vWorldPos;

void main()
{
    vec4 worldPos = uWorld * vec4(aPosition, 1.0);
    gl_Position = uProjection * uView * worldPos;
    vWorldPos = worldPos.xyz;
    vNormal = mat3(uWorld) * aNormal;
    vColor = aColor;
    vTexCoord0 = aTexCoord0;
    vTexCoord1 = aTexCoord1;
}
)GLSL";

// Basic fragment shader: texture * vertex color with ambient light
static const char* s_basicFragmentShader = R"GLSL(
#version 330 core

in vec4 vColor;
in vec2 vTexCoord0;
in vec2 vTexCoord1;
in vec3 vNormal;
in vec3 vWorldPos;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform int uHasTexture;
uniform int uHasTexture1;
uniform vec4 uAmbientColor;
uniform int uAlphaTestEnable;
uniform float uAlphaRef;
uniform int uFogEnable;
uniform vec4 uFogColor;
uniform float uFogStart;
uniform float uFogEnd;

out vec4 fragColor;

void main()
{
    vec4 color = vColor;

    if (uHasTexture != 0) {
        vec4 texColor = texture(uTexture0, vTexCoord0);
        color *= texColor;
    }

    // Apply ambient lighting
    color.rgb *= uAmbientColor.rgb;

    // Alpha test
    if (uAlphaTestEnable != 0 && color.a < uAlphaRef) {
        discard;
    }

    // Basic fog (linear)
    if (uFogEnable != 0) {
        float dist = length(vWorldPos);
        float fogFactor = clamp((uFogEnd - dist) / (uFogEnd - uFogStart), 0.0, 1.0);
        color.rgb = mix(uFogColor.rgb, color.rgb, fogFactor);
    }

    fragColor = color;
}
)GLSL";

// ============================================================================
// GL shader program management
// ============================================================================

struct GLShaderProgram {
    GLuint program;
    // Uniform locations
    GLint loc_projection;
    GLint loc_view;
    GLint loc_world;
    GLint loc_ambientColor;
    GLint loc_hasTexture;
    GLint loc_hasTexture1;
    GLint loc_texture0;
    GLint loc_texture1;
    GLint loc_alphaTestEnable;
    GLint loc_alphaRef;
    GLint loc_fogEnable;
    GLint loc_fogColor;
    GLint loc_fogStart;
    GLint loc_fogEnd;
};

// Vertex attribute locations (must match shader layout)
enum GLVertexAttrib {
    GL_ATTRIB_POSITION  = 0,
    GL_ATTRIB_NORMAL    = 1,
    GL_ATTRIB_COLOR     = 2,
    GL_ATTRIB_TEXCOORD0 = 3,
    GL_ATTRIB_TEXCOORD1 = 4,
};

// ============================================================================
// GL rendering state
// ============================================================================

struct GLRenderState {
    GLShaderProgram shader;
    GLuint defaultVAO;
    bool initialized;

    // Current transform matrices
    float projectionMatrix[16];
    float viewMatrix[16];
    float worldMatrix[16];

    // Current render state
    bool depthTestEnabled;
    bool depthWriteEnabled;
    bool blendEnabled;
    GLenum srcBlend;
    GLenum dstBlend;
    bool cullEnabled;
    bool alphaTestEnabled;
    float alphaRef;
    bool fogEnabled;
    float fogColor[4];
    float fogStart;
    float fogEnd;
    float ambientColor[4];
};

// ============================================================================
// Inline helpers
// ============================================================================

inline GLuint GL_CompileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        // TODO: proper error logging
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

inline GLuint GL_CreateProgram(const char* vertSrc, const char* fragSrc)
{
    GLuint vs = GL_CompileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint fs = GL_CompileShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

inline void GL_InitShaderProgram(GLShaderProgram& sp, const char* vertSrc, const char* fragSrc)
{
    sp.program = GL_CreateProgram(vertSrc, fragSrc);
    if (!sp.program) return;

    sp.loc_projection     = glGetUniformLocation(sp.program, "uProjection");
    sp.loc_view           = glGetUniformLocation(sp.program, "uView");
    sp.loc_world          = glGetUniformLocation(sp.program, "uWorld");
    sp.loc_ambientColor   = glGetUniformLocation(sp.program, "uAmbientColor");
    sp.loc_hasTexture     = glGetUniformLocation(sp.program, "uHasTexture");
    sp.loc_hasTexture1    = glGetUniformLocation(sp.program, "uHasTexture1");
    sp.loc_texture0       = glGetUniformLocation(sp.program, "uTexture0");
    sp.loc_texture1       = glGetUniformLocation(sp.program, "uTexture1");
    sp.loc_alphaTestEnable = glGetUniformLocation(sp.program, "uAlphaTestEnable");
    sp.loc_alphaRef       = glGetUniformLocation(sp.program, "uAlphaRef");
    sp.loc_fogEnable      = glGetUniformLocation(sp.program, "uFogEnable");
    sp.loc_fogColor       = glGetUniformLocation(sp.program, "uFogColor");
    sp.loc_fogStart       = glGetUniformLocation(sp.program, "uFogStart");
    sp.loc_fogEnd         = glGetUniformLocation(sp.program, "uFogEnd");
}

inline void GL_IdentityMatrix(float m[16])
{
    m[0]=1; m[1]=0; m[2]=0; m[3]=0;
    m[4]=0; m[5]=1; m[6]=0; m[7]=0;
    m[8]=0; m[9]=0; m[10]=1; m[11]=0;
    m[12]=0; m[13]=0; m[14]=0; m[15]=1;
}

// Convert D3DMATRIX (row-major) to OpenGL (column-major)
inline void GL_D3DMatrixToGL(const float* d3d, float* gl)
{
    gl[0]=d3d[0];  gl[1]=d3d[4];  gl[2]=d3d[8];   gl[3]=d3d[12];
    gl[4]=d3d[1];  gl[5]=d3d[5];  gl[6]=d3d[9];   gl[7]=d3d[13];
    gl[8]=d3d[2];  gl[9]=d3d[6];  gl[10]=d3d[10];  gl[11]=d3d[14];
    gl[12]=d3d[3]; gl[13]=d3d[7]; gl[14]=d3d[11];  gl[15]=d3d[15];
}

#endif // !_WIN32
#endif // GL_RENDER_H
