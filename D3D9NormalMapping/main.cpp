//-----------------------------------------------------------------------------
// Copyright (c) 2007-2008 dhpoware. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
//
// This demo implements tangent space normal mapping using a series of Direct3D
// High Level Shading Language (HLSL) shaders. A single D3DX Effect file (.fx)
// is provided with three techniques that implements normal mapping for each
// of the three Direct3D light types: direction, point, and spot lights.
//
// The demo consists of a single cube centered at the world origin. The cube
// consists of only 6 faces with each face composed of 2 triangles.
//
// Tangent space normal mapping involves evaluating the lighting equations per
// pixel in the surface local coordinate space of the normal map texture.
// Rather than using the cube's interpolated vertex normals tangent space
// normal mapping uses the normals from the normal map texture instead. Doing
// this allows additional surface detail to be applied to each face of the cube
// without requiring extra geometry to be added to each face.
//
// The lighting vectors (light, view, and half-angle vectors) are transformed
// into the normal map texture's tangent space using a rotation matrix. This
// rotation matrix must be calculated for each face of the cube. Each face has
// its own rotation matrix because each face is uniquely texture mapped to the
// normal map texture and the rotation matrix must take this into consideration.
// The rotation matrix is constructed using each face's tangent, bitangent, and
// normal vectors.
//
//-----------------------------------------------------------------------------

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(_DEBUG)
#define D3D_DEBUG_INFO
#endif

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <sstream>
#include <stdexcept>
#include <string>

#if defined(_DEBUG)
#include <crtdbg.h>
#endif

//-----------------------------------------------------------------------------
// Macros.
//-----------------------------------------------------------------------------

#define SAFE_RELEASE(x) if ((x) != 0) { (x)->Release(); (x) = 0; }

//-----------------------------------------------------------------------------
// Constants.
//-----------------------------------------------------------------------------

#if !defined(CLEARTYPE_QUALITY)
#define CLEARTYPE_QUALITY 5
#endif

#if !defined(WHEEL_DELTA)
#define WHEEL_DELTA 120
#endif

#if !defined(WM_MOUSEWHEEL)
#define WM_MOUSEWHEEL 0x020A
#endif

#define APP_TITLE "D3D9 Normal Mapping"

const float CAMERA_FOVY = D3DXToRadian(45.0f);
const float CAMERA_ZNEAR = 0.1f;
const float CAMERA_ZFAR = 100.0f;

const float DOLLY_MIN = 0.0f;
const float DOLLY_MAX = 10.0f;

const float MOUSE_ROTATE_SPEED = 0.30f;
const float MOUSE_DOLLY_SPEED = 0.02f;
const float MOUSE_TRACK_SPEED = 0.005f;
const float MOUSE_WHEEL_DOLLY_SPEED = 0.005f;

const float LIGHT_RADIUS = 10.0f;
const float LIGHT_SPOT_INNER_CONE = D3DXToRadian(10.0f);
const float LIGHT_SPOT_OUTER_CONE = D3DXToRadian(30.0f);

//-----------------------------------------------------------------------------
// User Defined Types.
//-----------------------------------------------------------------------------

struct Vertex
{
    float pos[3];
    float texCoord[2];
    float normal[3];
    float tangent[4];
};

struct Light
{
    enum {DIR_LIGHT, POINT_LIGHT, SPOT_LIGHT};

    int type;
    float dir[3];
    float pos[3];
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float spotInnerCone;
    float spotOuterCone;
    float radius;
};

struct Material
{
    float ambient[4];
    float diffuse[4];
    float emissive[4];
    float specular[4];
    float shininess;
};

//-----------------------------------------------------------------------------
// Globals.
//-----------------------------------------------------------------------------

HWND                         g_hWnd;
HINSTANCE                    g_hInstance;
IDirect3D9                  *g_pDirect3D;
IDirect3DDevice9            *g_pDevice;
ID3DXFont                   *g_pFont;
ID3DXEffect                 *g_pEffect;
IDirect3DVertexDeclaration9 *g_pVertexDecl;
IDirect3DVertexBuffer9      *g_pVertexBuffer;
IDirect3DTexture9           *g_pNullTexture;
IDirect3DTexture9           *g_pColorMap;
IDirect3DTexture9           *g_pNormalMap;
DWORD                        g_msaaSamples;
DWORD                        g_maxAnisotrophy;
int                          g_framesPerSecond;
int                          g_windowWidth;
int                          g_windowHeight;
bool                         g_enableVerticalSync;
bool                         g_isFullScreen;
bool                         g_hasFocus;
bool                         g_disableColorMapTexture;
bool                         g_wireframe;
bool                         g_displayHelp;
float                        g_pitch;
float                        g_heading;
float                        g_sceneAmbient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
D3DPRESENT_PARAMETERS        g_params;
D3DXVECTOR3                  g_cameraPos(0.0f, 0.0f, -2.5f);
D3DXVECTOR3                  g_cubePos(0.0f, 0.0f, 2.0f);

Light g_light =
{
    Light::DIR_LIGHT,
    0.0f, 0.0f, 1.0f,                               // dir
    g_cameraPos.x, g_cameraPos.y, g_cameraPos.z,    // pos
    1.0f, 1.0f, 1.0f, 0.0f,                         // ambient
    1.0f, 1.0f, 1.0f, 0.0f,                         // diffuse
    1.0f, 1.0f, 1.0f, 0.0f,                         // specular
    LIGHT_SPOT_INNER_CONE,                          // spotInnerCone
    LIGHT_SPOT_OUTER_CONE,                          // spotOuterCone
    LIGHT_RADIUS                                    // radius
};

Material g_material =
{
    0.2f, 0.2f, 0.2f, 1.0f,                         // ambient
    0.8f, 0.8f, 0.8f, 1.0f,                         // diffuse
    0.0f, 0.0f, 0.0f, 1.0f,                         // emissive
    0.7f, 0.7f, 0.7f, 1.0f,                         // specular
    90.0f                                           // shininess
};

D3DVERTEXELEMENT9 g_vertexElements[] =
{
    {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    {0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
    {0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
    D3DDECL_END()
};

Vertex g_cube[36] =
{
    // -Z face
    {-1.0f,  1.0f, -1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f,  1.0f, -1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f, -1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f, -1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f, -1.0f, -1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f,  1.0f, -1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f, 0.0f, 0.0f },

    // +Z face
    { 1.0f,  1.0f,  1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f,  1.0f,  1.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f, -1.0f,  1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f, -1.0f,  1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f,  1.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f,  1.0f,  1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 0.0f },

    // -Y face
    {-1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f, -1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f,  1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f,  1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f, -1.0f,  1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },

    // +Y face
    {-1.0f,  1.0f,  1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f,  1.0f,  1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f,  1.0f, -1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f,  1.0f, -1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f,  1.0f,  1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },

    // -X face
    {-1.0f,  1.0f,  1.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f,  1.0f, -1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f, -1.0f, -1.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f, -1.0f, -1.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f, -1.0f,  1.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    {-1.0f,  1.0f,  1.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },

    // +X face
    { 1.0f,  1.0f, -1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f,  1.0f,  1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f,  1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f,  1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, -1.0f, -1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f,  1.0f, -1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 0.0f }
};

//-----------------------------------------------------------------------------
// Function Prototypes.
//-----------------------------------------------------------------------------

void    CalcTangentVector(const float pos1[3], const float pos2[3],
                          const float pos3[3], const float texCoord1[2],
                          const float texCoord2[2], const float texCoord3[2],
                          const float normal[3], D3DXVECTOR4 &tangent);
void    ChooseBestMSAAMode(D3DFORMAT backBufferFmt, D3DFORMAT depthStencilFmt,
                           BOOL windowed, D3DMULTISAMPLE_TYPE &type,
                           DWORD &qualityLevels, DWORD &samplesPerPixel);
void    Cleanup();
void    CleanupApp();
HWND    CreateAppWindow(const WNDCLASSEX &wcl, const char *pszTitle);
bool    CreateNullTexture(int width, int height, LPDIRECT3DTEXTURE9 &pTexture);
bool    DeviceIsValid();
float   GetElapsedTimeInSeconds();
LPCSTR  GetLightTypeStr();
bool    Init();
void    InitApp();
void    InitCube();
bool    InitD3D();
bool    InitFont(const char *pszFont, int ptSize, LPD3DXFONT &pFont);
bool    LoadShader(const char *pszFilename, LPD3DXEFFECT &pEffect);
void    Log(const char *pszMessage);
bool    MSAAModeSupported(D3DMULTISAMPLE_TYPE type, D3DFORMAT backBufferFmt,
                          D3DFORMAT depthStencilFmt, BOOL windowed,
                          DWORD &qualityLevels);
void    ProcessMouseInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void    RenderCube();
void    RenderFrame();
void    RenderText();
bool    ResetDevice();
void    SetProcessorAffinity();
void    ToggleFullScreen();
void    UpdateEffect();
void    UpdateFrame(float elapsedTimeSec);
void    UpdateFrameRate(float elapsedTimeSec);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//-----------------------------------------------------------------------------
// Functions.
//-----------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#if defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

    MSG msg = {0};
    WNDCLASSEX wcl = {0};

    wcl.cbSize = sizeof(wcl);
    wcl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcl.lpfnWndProc = WindowProc;
    wcl.cbClsExtra = 0;
    wcl.cbWndExtra = 0;
    wcl.hInstance = g_hInstance = hInstance;
    wcl.hIcon = LoadIcon(0, IDI_APPLICATION);
    wcl.hCursor = LoadCursor(0, IDC_ARROW);
    wcl.hbrBackground = 0;
    wcl.lpszMenuName = 0;
    wcl.lpszClassName = "D3D9WindowClass";
    wcl.hIconSm = 0;

    if (!RegisterClassEx(&wcl))
        return 0;

    g_hWnd = CreateAppWindow(wcl, APP_TITLE);

    if (g_hWnd)
    {
        SetProcessorAffinity();

        if (Init())
        {
            ShowWindow(g_hWnd, nShowCmd);
            UpdateWindow(g_hWnd);

            while (true)
            {
                while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT)
                        break;

                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                if (msg.message == WM_QUIT)
                    break;

                if (g_hasFocus)
                {
                    UpdateFrame(GetElapsedTimeInSeconds());

                    if (DeviceIsValid())
                        RenderFrame();
                }
                else
                {
                    WaitMessage();
                }
            }
        }

        Cleanup();
        UnregisterClass(wcl.lpszClassName, hInstance);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ACTIVATE:
        switch (wParam)
        {
        default:
            break;

        case WA_ACTIVE:
        case WA_CLICKACTIVE:
            g_hasFocus = true;
            break;

        case WA_INACTIVE:
            if (g_isFullScreen)
                ShowWindow(hWnd, SW_MINIMIZE);
            g_hasFocus = false;
            break;
        }
        break;

    case WM_CHAR:
        switch (static_cast<int>(wParam))
        {
        case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        case VK_SPACE:
            if (++g_light.type > Light::SPOT_LIGHT)
                g_light.type = Light::DIR_LIGHT;

            break;

        case VK_BACK:
            if (--g_light.type < Light::DIR_LIGHT)
                g_light.type = Light::SPOT_LIGHT;

            break;

        case 'H':
        case 'h':
            g_displayHelp = !g_displayHelp;
            break;

        case 'T':
        case 't':
            g_disableColorMapTexture = !g_disableColorMapTexture;
            break;

        case 'W':
        case 'w':
            if (g_wireframe = !g_wireframe)
                g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
            else
                g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        g_windowWidth = static_cast<int>(LOWORD(lParam));
        g_windowHeight = static_cast<int>(HIWORD(lParam));
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN)
            ToggleFullScreen();
        break;

    default:
        ProcessMouseInput(hWnd, msg, wParam, lParam);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void CalcTangentVector(const float pos1[3], const float pos2[3],
                       const float pos3[3], const float texCoord1[2],
                       const float texCoord2[2], const float texCoord3[2],
                       const float normal[3], D3DXVECTOR4 &tangent)
{
    // Given the 3 vertices (position and texture coordinates) of a triangle
    // calculate and return the triangle's tangent vector. The handedness of
    // the local coordinate system is stored in tangent.w. The bitangent is
    // then: float3 bitangent = cross(normal, tangent.xyz) * tangent.w.

    // Create 2 vectors in object space.
    //
    // edge1 is the vector from vertex positions pos1 to pos2.
    // edge2 is the vector from vertex positions pos1 to pos3.
    D3DXVECTOR3 edge1(pos2[0] - pos1[0], pos2[1] - pos1[1], pos2[2] - pos1[2]);
    D3DXVECTOR3 edge2(pos3[0] - pos1[0], pos3[1] - pos1[1], pos3[2] - pos1[2]);

    D3DXVec3Normalize(&edge1, &edge1);
    D3DXVec3Normalize(&edge2, &edge2);

    // Create 2 vectors in tangent (texture) space that point in the same
    // direction as edge1 and edge2 (in object space).
    //
    // texEdge1 is the vector from texture coordinates texCoord1 to texCoord2.
    // texEdge2 is the vector from texture coordinates texCoord1 to texCoord3.
    D3DXVECTOR2 texEdge1(texCoord2[0] - texCoord1[0], texCoord2[1] - texCoord1[1]);
    D3DXVECTOR2 texEdge2(texCoord3[0] - texCoord1[0], texCoord3[1] - texCoord1[1]);

    D3DXVec2Normalize(&texEdge1, &texEdge1);
    D3DXVec2Normalize(&texEdge2, &texEdge2);

    // These 2 sets of vectors form the following system of equations:
    //
    //  edge1 = (texEdge1.x * tangent) + (texEdge1.y * bitangent)
    //  edge2 = (texEdge2.x * tangent) + (texEdge2.y * bitangent)
    //
    // Using matrix notation this system looks like:
    //
    //  [ edge1 ]     [ texEdge1.x  texEdge1.y ]  [ tangent   ]
    //  [       ]  =  [                        ]  [           ]
    //  [ edge2 ]     [ texEdge2.x  texEdge2.y ]  [ bitangent ]
    //
    // The solution is:
    //
    //  [ tangent   ]        1     [ texEdge2.y  -texEdge1.y ]  [ edge1 ]
    //  [           ]  =  -------  [                         ]  [       ]
    //  [ bitangent ]      det A   [-texEdge2.x   texEdge1.x ]  [ edge2 ]
    //
    //  where:
    //        [ texEdge1.x  texEdge1.y ]
    //    A = [                        ]
    //        [ texEdge2.x  texEdge2.y ]
    //
    //    det A = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x)
    //
    // From this solution the tangent space basis vectors are:
    //
    //    tangent = (1 / det A) * ( texEdge2.y * edge1 - texEdge1.y * edge2)
    //  bitangent = (1 / det A) * (-texEdge2.x * edge1 + texEdge1.x * edge2)
    //     normal = cross(tangent, bitangent)

    D3DXVECTOR3 bitangent;
    float det = (texEdge1.x * texEdge2.y) - (texEdge1.y * texEdge2.x);

    if (fabsf(det) < 1e-6f)    // almost equal to zero
    {
        tangent.x = 1.0f;
        tangent.y = 0.0f;
        tangent.z = 0.0f;

        bitangent.x = 0.0f;
        bitangent.y = 1.0f;
        bitangent.z = 0.0f;
    }
    else
    {
        det = 1.0f / det;

        tangent.x = (texEdge2.y * edge1.x - texEdge1.y * edge2.x) * det;
        tangent.y = (texEdge2.y * edge1.y - texEdge1.y * edge2.y) * det;
        tangent.z = (texEdge2.y * edge1.z - texEdge1.y * edge2.z) * det;
        tangent.w = 0.0f;
        
        bitangent.x = (-texEdge2.x * edge1.x + texEdge1.x * edge2.x) * det;
        bitangent.y = (-texEdge2.x * edge1.y + texEdge1.x * edge2.y) * det;
        bitangent.z = (-texEdge2.x * edge1.z + texEdge1.x * edge2.z) * det;

        D3DXVec4Normalize(&tangent, &tangent);
        D3DXVec3Normalize(&bitangent, &bitangent);
    }

    // Calculate the handedness of the local tangent space.
    // The bitangent vector is the cross product between the triangle face
    // normal vector and the calculated tangent vector. The resulting bitangent
    // vector should be the same as the bitangent vector calculated from the
    // set of linear equations above. If they point in different directions
    // then we need to invert the cross product calculated bitangent vector. We
    // store this scalar multiplier in the tangent vector's 'w' component so
    // that the correct bitangent vector can be generated in the normal mapping
    // shader's vertex shader.

    D3DXVECTOR3 n(normal[0], normal[1], normal[2]);
    D3DXVECTOR3 t(tangent.x, tangent.y, tangent.z);
    D3DXVECTOR3 b;
        
    D3DXVec3Cross(&b, &n, &t);
    tangent.w = (D3DXVec3Dot(&b, &bitangent) < 0.0f) ? -1.0f : 1.0f;
}

void ChooseBestMSAAMode(D3DFORMAT backBufferFmt, D3DFORMAT depthStencilFmt,
                        BOOL windowed, D3DMULTISAMPLE_TYPE &type,
                        DWORD &qualityLevels, DWORD &samplesPerPixel)
{
    bool supported = false;

    struct MSAAMode
    {
        D3DMULTISAMPLE_TYPE type;
        DWORD samples;
    }
    multsamplingTypes[15] =
    {
        { D3DMULTISAMPLE_16_SAMPLES,  16 },
        { D3DMULTISAMPLE_15_SAMPLES,  15 },
        { D3DMULTISAMPLE_14_SAMPLES,  14 },
        { D3DMULTISAMPLE_13_SAMPLES,  13 },
        { D3DMULTISAMPLE_12_SAMPLES,  12 },
        { D3DMULTISAMPLE_11_SAMPLES,  11 },
        { D3DMULTISAMPLE_10_SAMPLES,  10 },
        { D3DMULTISAMPLE_9_SAMPLES,   9  },
        { D3DMULTISAMPLE_8_SAMPLES,   8  },
        { D3DMULTISAMPLE_7_SAMPLES,   7  },
        { D3DMULTISAMPLE_6_SAMPLES,   6  },
        { D3DMULTISAMPLE_5_SAMPLES,   5  },
        { D3DMULTISAMPLE_4_SAMPLES,   4  },
        { D3DMULTISAMPLE_3_SAMPLES,   3  },
        { D3DMULTISAMPLE_2_SAMPLES,   2  }
    };

    for (int i = 0; i < 15; ++i)
    {
        type = multsamplingTypes[i].type;

        supported = MSAAModeSupported(type, backBufferFmt, depthStencilFmt,
                        windowed, qualityLevels);

        if (supported)
        {
            samplesPerPixel = multsamplingTypes[i].samples;
            return;
        }
    }

    type = D3DMULTISAMPLE_NONE;
    qualityLevels = 0;
    samplesPerPixel = 1;
}

void Cleanup()
{
    CleanupApp();

    SAFE_RELEASE(g_pFont);
    SAFE_RELEASE(g_pDevice);
    SAFE_RELEASE(g_pDirect3D);
}

void CleanupApp()
{
    SAFE_RELEASE(g_pEffect);
    SAFE_RELEASE(g_pNormalMap);
    SAFE_RELEASE(g_pColorMap);
    SAFE_RELEASE(g_pNullTexture);
    SAFE_RELEASE(g_pVertexBuffer);
    SAFE_RELEASE(g_pVertexDecl);
}

HWND CreateAppWindow(const WNDCLASSEX &wcl, const char *pszTitle)
{
    // Create a window that is centered on the desktop. It's exactly 1/4 the
    // size of the desktop. Don't allow it to be resized.

    DWORD wndExStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD wndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                     WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

    HWND hWnd = CreateWindowEx(wndExStyle, wcl.lpszClassName, pszTitle,
                    wndStyle, 0, 0, 0, 0, 0, 0, wcl.hInstance, 0);

    if (hWnd)
    {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int halfScreenWidth = screenWidth / 2;
        int halfScreenHeight = screenHeight / 2;
        int left = (screenWidth - halfScreenWidth) / 2;
        int top = (screenHeight - halfScreenHeight) / 2;
        RECT rc = {0};

        SetRect(&rc, left, top, left + halfScreenWidth, top + halfScreenHeight);
        AdjustWindowRectEx(&rc, wndStyle, FALSE, wndExStyle);
        MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);

        GetClientRect(hWnd, &rc);
        g_windowWidth = rc.right - rc.left;
        g_windowHeight = rc.bottom - rc.top;
    }

    return hWnd;
}

bool CreateNullTexture(int width, int height, LPDIRECT3DTEXTURE9 &pTexture)
{
    // Create an empty white texture. This texture is applied to geometry
    // that doesn't have any texture maps. This trick allows the same shader to
    // be used to draw the geometry with and without textures applied.

    HRESULT hr = D3DXCreateTexture(g_pDevice, width, height, 0, 0,
                    D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &pTexture);

    if (FAILED(hr))
        return false;

    LPDIRECT3DSURFACE9 pSurface = 0;
    
    if (SUCCEEDED(pTexture->GetSurfaceLevel(0, &pSurface)))
    {
        D3DLOCKED_RECT rcLock = {0};

        if (SUCCEEDED(pSurface->LockRect(&rcLock, 0, 0)))
        {
            BYTE *pPixels = static_cast<BYTE*>(rcLock.pBits);
            int widthInBytes = width * 4;

            if (widthInBytes == rcLock.Pitch)
            {
                memset(pPixels, 0xff, widthInBytes * height);
            }
            else
            {
                for (int y = 0; y < height; ++y)
                    memset(&pPixels[y * rcLock.Pitch], 0xff, rcLock.Pitch);
            }

            pSurface->UnlockRect();
            pSurface->Release();
            return true;
        }

        pSurface->Release();
    }

    pTexture->Release();
    return false;
}

bool DeviceIsValid()
{
    HRESULT hr = g_pDevice->TestCooperativeLevel();

    if (FAILED(hr))
    {
        if (hr == D3DERR_DEVICENOTRESET)
            return ResetDevice();
    }

    return true;
}

float GetElapsedTimeInSeconds()
{
    // Returns the elapsed time (in seconds) since the last time this function
    // was called. This elaborate setup is to guard against large spikes in
    // the time returned by QueryPerformanceCounter().

    static const int MAX_SAMPLE_COUNT = 50;

    static float frameTimes[MAX_SAMPLE_COUNT];
    static float timeScale = 0.0f;
    static float actualElapsedTimeSec = 0.0f;
    static INT64 freq = 0;
    static INT64 lastTime = 0;
    static int sampleCount = 0;
    static bool initialized = false;

    INT64 time = 0;
    float elapsedTimeSec = 0.0f;

    if (!initialized)
    {
        initialized = true;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&lastTime));
        timeScale = 1.0f / freq;
    }

    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
    elapsedTimeSec = (time - lastTime) * timeScale;
    lastTime = time;

    if (fabsf(elapsedTimeSec - actualElapsedTimeSec) < 1.0f)
    {
        memmove(&frameTimes[1], frameTimes, sizeof(frameTimes) - sizeof(frameTimes[0]));
        frameTimes[0] = elapsedTimeSec;

        if (sampleCount < MAX_SAMPLE_COUNT)
            ++sampleCount;
    }

    actualElapsedTimeSec = 0.0f;

    for (int i = 0; i < sampleCount; ++i)
        actualElapsedTimeSec += frameTimes[i];

    if (sampleCount > 0)
        actualElapsedTimeSec /= sampleCount;

    return actualElapsedTimeSec;
}

LPCSTR GetLightTypeStr()
{
    switch (g_light.type)
    {
    case Light::DIR_LIGHT:
        return "Directional Lighting";

    case Light::POINT_LIGHT:
        return "Point Lighting";

    case Light::SPOT_LIGHT:
        return "Spot Lighting";

    default:
        return "Unknown Lighting";
    }
}

bool Init()
{
    if (!InitD3D())
    {
        Log("Direct3D initialization failed!");
        return false;
    }

    try
    {
        InitApp();
        return true;
    }
    catch (const std::exception &e)
    {
        std::ostringstream msg;

        msg << "Application initialization failed!" << std::endl << std::endl;
        msg << e.what();

        Log(msg.str().c_str());
        return false;
    }    
}

void InitApp()
{
    if (!InitFont("Arial", 10, g_pFont))
        throw std::runtime_error("Failed to create font.");

    if (!CreateNullTexture(2, 2, g_pNullTexture))
        throw std::runtime_error("Failed to create null texture.");

    if (FAILED(D3DXCreateTextureFromFile(g_pDevice, "Content/Textures/color_map.jpg", &g_pColorMap)))
        throw std::runtime_error("Failed to load texture: color_map.jpg.");

    if (FAILED(D3DXCreateTextureFromFile(g_pDevice, "Content/Textures/normal_map.jpg", &g_pNormalMap)))
        throw std::runtime_error("Failed to load texture: normal_map.jpg.");

    if (!LoadShader("Content/Shaders/normal_mapping.fx", g_pEffect))
        throw std::runtime_error("Failed to load shader: normal_mapping.fx.");

    InitCube();    
}

void InitCube()
{
    // Generate the tangent vectors for the cube.

    Vertex *pVertex1 = 0;
    Vertex *pVertex2 = 0;
    Vertex *pVertex3 = 0;
    D3DXVECTOR4 tangent;

    for (int i = 0; i < 36; i += 3)
    {
        pVertex1 = &g_cube[i];
        pVertex2 = &g_cube[i + 1];
        pVertex3 = &g_cube[i + 2];

        CalcTangentVector(
            pVertex1->pos, pVertex2->pos, pVertex3->pos,
            pVertex1->texCoord, pVertex2->texCoord, pVertex3->texCoord,
            pVertex1->normal, tangent);

        pVertex1->tangent[0] = tangent.x;
        pVertex1->tangent[1] = tangent.y;
        pVertex1->tangent[2] = tangent.z;
        pVertex1->tangent[3] = tangent.w;
        
        pVertex2->tangent[0] = tangent.x;
        pVertex2->tangent[1] = tangent.y;
        pVertex2->tangent[2] = tangent.z;
        pVertex2->tangent[3] = tangent.w;
        
        pVertex3->tangent[0] = tangent.x;
        pVertex3->tangent[1] = tangent.y;
        pVertex3->tangent[2] = tangent.z;
        pVertex3->tangent[3] = tangent.w;
    }

    // Create the vertex declaration for the cube.
    
    if (FAILED(g_pDevice->CreateVertexDeclaration(g_vertexElements, &g_pVertexDecl)))
        throw std::runtime_error("Failed to create vertex declaration.");

    // Create a vertex buffer for the cube.
    
    if (FAILED(g_pDevice->CreateVertexBuffer(sizeof(Vertex) * 36, 0, 0,
            D3DPOOL_MANAGED, &g_pVertexBuffer, 0)))
        throw std::runtime_error("Failed to create vertex buffer.");

    // Fill vertex buffer with the cube's geometry.

    Vertex *pVertices = 0;

    if (FAILED(g_pVertexBuffer->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0)))
        throw std::runtime_error("Failed to lock vertex buffer.");

    memcpy(pVertices, g_cube, sizeof(g_cube));
    g_pVertexBuffer->Unlock();
}

bool InitD3D()
{
    HRESULT hr = 0;
    D3DDISPLAYMODE desktop = {0};

    g_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!g_pDirect3D)
        return false;

    // Just use the current desktop display mode.
    hr = g_pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &desktop);

    if (FAILED(hr))
    {
        g_pDirect3D->Release();
        g_pDirect3D = 0;
        return false;
    }

    // Setup Direct3D for windowed rendering.
    g_params.BackBufferWidth = 0;
    g_params.BackBufferHeight = 0;
    g_params.BackBufferFormat = desktop.Format;
    g_params.BackBufferCount = 1;
    g_params.hDeviceWindow = g_hWnd;
    g_params.Windowed = TRUE;
    g_params.EnableAutoDepthStencil = TRUE;
    g_params.AutoDepthStencilFormat = D3DFMT_D24S8;
    g_params.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    g_params.FullScreen_RefreshRateInHz = 0;

    if (g_enableVerticalSync)
        g_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    else
        g_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Swap effect must be D3DSWAPEFFECT_DISCARD for multi-sampling support.
    g_params.SwapEffect = D3DSWAPEFFECT_DISCARD;

    // Select the highest quality multi-sample anti-aliasing (MSAA) mode.
    ChooseBestMSAAMode(g_params.BackBufferFormat, g_params.AutoDepthStencilFormat,
        g_params.Windowed, g_params.MultiSampleType, g_params.MultiSampleQuality,
        g_msaaSamples);

    // Most modern video cards should have no problems creating pure devices.
    // Note that by creating a pure device we lose the ability to debug vertex
    // and pixel shaders.
    hr = g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
        &g_params, &g_pDevice);

    if (FAILED(hr))
    {
        // Fall back to software vertex processing for less capable hardware.
        // Note that in order to debug vertex shaders we must use a software
        // vertex processing device.
        hr = g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
            g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_params, &g_pDevice);
    }

    if (FAILED(hr))
    {
        g_pDirect3D->Release();
        g_pDirect3D = 0;
        return false;
    }

    D3DCAPS9 caps;

    // Prefer anisotropic texture filtering if it's supported.
    if (SUCCEEDED(g_pDevice->GetDeviceCaps(&caps)))
    {
        if (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY)
            g_maxAnisotrophy = caps.MaxAnisotropy;
        else
            g_maxAnisotrophy = 1;
    }

    return true;
}

bool InitFont(const char *pszFont, int ptSize, LPD3DXFONT &pFont)
{
    static DWORD dwQuality = 0;

    // Prefer ClearType font quality if available.

    if (!dwQuality)
    {
        DWORD dwVersion = GetVersion();
        DWORD dwMajorVersion = static_cast<DWORD>((LOBYTE(LOWORD(dwVersion))));
        DWORD dwMinorVersion = static_cast<DWORD>((HIBYTE(LOWORD(dwVersion))));

        // Windows XP and higher will support ClearType quality fonts.
        if (dwMajorVersion >= 6 || (dwMajorVersion == 5 && dwMinorVersion == 1))
            dwQuality = CLEARTYPE_QUALITY;
        else
            dwQuality = ANTIALIASED_QUALITY;
    }

    int logPixelsY = 0;

    // Convert from font point size to pixel size.

    if (HDC hDC = GetDC((0)))
    {
        logPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
        ReleaseDC(0, hDC);
    }

    int fontCharHeight = -ptSize * logPixelsY / 72;

    // Now create the font. Prefer anti-aliased text.

    HRESULT hr = D3DXCreateFont(
        g_pDevice,
        fontCharHeight,                 // height
        0,                              // width
        FW_BOLD,                        // weight
        1,                              // mipmap levels
        FALSE,                          // italic
        DEFAULT_CHARSET,                // char set
        OUT_DEFAULT_PRECIS,             // output precision
        dwQuality,                      // quality
        DEFAULT_PITCH | FF_DONTCARE,    // pitch and family
        pszFont,                        // font name
        &pFont);

    return SUCCEEDED(hr) ? true : false;
}

bool LoadShader(const char *pszFilename, LPD3DXEFFECT &pEffect)
{
    ID3DXBuffer *pCompilationErrors = 0;
    DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER;

    // Both vertex and pixel shaders can be debugged. To enable shader
    // debugging add the following flag to the dwShaderFlags variable:
    //      dwShaderFlags |= D3DXSHADER_DEBUG;
    //
    // Vertex shaders can be debugged with either the REF device or a device
    // created for software vertex processing (i.e., the IDirect3DDevice9
    // object must be created with the D3DCREATE_SOFTWARE_VERTEXPROCESSING
    // behavior). Pixel shaders can be debugged only using the REF device.
    //
    // To enable vertex shader debugging add the following flag to the
    // dwShaderFlags variable:
    //     dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    //
    // To enable pixel shader debugging add the following flag to the
    // dwShaderFlags variable:
    //     dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;

    HRESULT hr = D3DXCreateEffectFromFile(g_pDevice, pszFilename, 0, 0,
                    dwShaderFlags, 0, &pEffect, &pCompilationErrors);

    if (FAILED(hr))
    {
        if (pCompilationErrors)
        {
            std::string compilationErrors(static_cast<const char *>(
                pCompilationErrors->GetBufferPointer()));

            pCompilationErrors->Release();
            throw std::runtime_error(compilationErrors);
        }
    }

    if (pCompilationErrors)
        pCompilationErrors->Release();

    return pEffect != 0;
}

void Log(const char *pszMessage)
{
    MessageBox(0, pszMessage, "Error", MB_ICONSTOP);
}

bool MSAAModeSupported(D3DMULTISAMPLE_TYPE type, D3DFORMAT backBufferFmt,
                       D3DFORMAT depthStencilFmt, BOOL windowed,
                       DWORD &qualityLevels)
{
    DWORD backBufferQualityLevels = 0;
    DWORD depthStencilQualityLevels = 0;

    HRESULT hr = g_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
                    D3DDEVTYPE_HAL, backBufferFmt, windowed, type,
                    &backBufferQualityLevels);

    if (SUCCEEDED(hr))
    {
        hr = g_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
                D3DDEVTYPE_HAL, depthStencilFmt, windowed, type,
                &depthStencilQualityLevels);

        if (SUCCEEDED(hr))
        {
            if (backBufferQualityLevels == depthStencilQualityLevels)
            {
                // The valid range is between zero and one less than the level
                // returned by IDirect3D9::CheckDeviceMultiSampleType().

                if (backBufferQualityLevels > 0)
                    qualityLevels = backBufferQualityLevels - 1;
                else
                    qualityLevels = backBufferQualityLevels;

                return true;
            }
        }
    }

    return false;
}

void ProcessMouseInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    enum MouseMode {MOUSE_NONE, MOUSE_TRACK, MOUSE_DOLLY, MOUSE_ROTATE};

    static MouseMode mode = MOUSE_NONE;
    static POINT ptMousePrev = {0};
    static POINT ptMouseCurrent = {0};
    static int mouseButtonsDown = 0;
    static float dx = 0.0f;
    static float dy = 0.0f;
    static float wheelDelta = 0.0f;

    switch (msg)
    {
    case WM_LBUTTONDOWN:
        mode = MOUSE_TRACK;
        ++mouseButtonsDown;
        SetCapture(hWnd);
        ptMousePrev.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
        ptMousePrev.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
        ClientToScreen(hWnd, &ptMousePrev);
        break;

    case WM_RBUTTONDOWN:
        mode = MOUSE_ROTATE;
        ++mouseButtonsDown;
        SetCapture(hWnd);
        ptMousePrev.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
        ptMousePrev.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
        ClientToScreen(hWnd, &ptMousePrev);
        break;

    case WM_MBUTTONDOWN:
        mode = MOUSE_DOLLY;
        ++mouseButtonsDown;
        SetCapture(hWnd);
        ptMousePrev.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
        ptMousePrev.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
        ClientToScreen(hWnd, &ptMousePrev);
        break;

    case WM_MOUSEMOVE:
        ptMouseCurrent.x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
        ptMouseCurrent.y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
        ClientToScreen(hWnd, &ptMouseCurrent);

        switch (mode)
        {
        case MOUSE_TRACK:
            dx = static_cast<float>(ptMouseCurrent.x - ptMousePrev.x);
            dx *= MOUSE_TRACK_SPEED;

            dy = static_cast<float>(ptMousePrev.y - ptMouseCurrent.y);
            dy *= MOUSE_TRACK_SPEED;

            g_cubePos.x += dx;
            g_cubePos.y += dy;

            break;

        case MOUSE_DOLLY:
            dy = static_cast<float>(ptMousePrev.y - ptMouseCurrent.y);
            dy *= MOUSE_DOLLY_SPEED;

            g_cubePos.z += dy;

            if (g_cubePos.z > DOLLY_MAX)
                g_cubePos.z = DOLLY_MAX;

            if (g_cubePos.z < DOLLY_MIN)
                g_cubePos.z = DOLLY_MIN;

            break;

        case MOUSE_ROTATE:
            dx = static_cast<float>(ptMousePrev.x - ptMouseCurrent.x);
            dx *= MOUSE_ROTATE_SPEED;

            dy = static_cast<float>(ptMousePrev.y - ptMouseCurrent.y);
            dy *= MOUSE_ROTATE_SPEED;

            g_heading += dx;
            g_pitch += dy;

            if (g_pitch > 90.0f)
                g_pitch = 90.0f;

            if (g_pitch < -90.0f)
                g_pitch = -90.0f;

            break;
        }

        ptMousePrev.x = ptMouseCurrent.x;
        ptMousePrev.y = ptMouseCurrent.y;
        break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        if (--mouseButtonsDown <= 0)
        {
            mouseButtonsDown = 0;
            mode = MOUSE_NONE;
            ReleaseCapture();
        }
        else
        {
            if (wParam & MK_LBUTTON)
                mode = MOUSE_TRACK;
            else if (wParam & MK_RBUTTON)
                mode = MOUSE_ROTATE;
            else if (wParam & MK_MBUTTON)
                mode = MOUSE_DOLLY;
        }
        break;

    case WM_MOUSEWHEEL:
        wheelDelta = static_cast<float>(static_cast<int>(wParam) >> 16);
        g_cubePos.z += wheelDelta * MOUSE_WHEEL_DOLLY_SPEED;

        if (g_cubePos.z > DOLLY_MAX)
            g_cubePos.z = DOLLY_MAX;

        if (g_cubePos.z < DOLLY_MIN)
            g_cubePos.z = DOLLY_MIN;

        break;

    default:
        break;
    }
}

void RenderCube()
{
    static UINT totalPasses;
    static D3DXHANDLE hTechnique;

    // Select the shader to use based on current lighting type.
    switch (g_light.type)
    {
    case Light::DIR_LIGHT:
        hTechnique = g_pEffect->GetTechniqueByName("NormalMappingDirectionalLighting");
        break;

    case Light::POINT_LIGHT:
        hTechnique = g_pEffect->GetTechniqueByName("NormalMappingPointLighting");
        break;

    case Light::SPOT_LIGHT:
        hTechnique = g_pEffect->GetTechniqueByName("NormalMappingSpotLighting");
        break;

    default:
        hTechnique = 0;
        break;
    }

    // Bind vertex buffer.
    g_pDevice->SetVertexDeclaration(g_pVertexDecl);
    g_pDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex));

    // Render the cube.
    if (SUCCEEDED(g_pEffect->SetTechnique(hTechnique)))
    {
        if (SUCCEEDED(g_pEffect->Begin(&totalPasses, 0)))
        {
            for (UINT pass = 0; pass < totalPasses; ++pass)
            {
                if (SUCCEEDED(g_pEffect->BeginPass(pass)))
                {
                    g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
                    g_pEffect->EndPass();
                }
            }

            g_pEffect->End();
        }
    }
}

void RenderFrame()
{
    g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_XRGB(100, 149, 237),   // CornflowerBlue
        1.0f, 0);

    if (FAILED(g_pDevice->BeginScene()))
        return;    

    RenderCube();
    RenderText();

    g_pDevice->EndScene();
    g_pDevice->Present(0, 0, 0, 0);
}

void RenderText()
{
    static RECT rcClient;

    std::ostringstream output;

    if (g_displayHelp)
    {
        output
            << "Left click and drag with the mouse to translate the cube" << std::endl
            << "Middle click and drag with the mouse to zoom in and out of the cube" << std::endl
            << "Right click and drag with the mouse to rotate the cube" << std::endl
            << "Mouse wheel to zoom in and out of the cube" << std::endl
            << std::endl
            << "Press T to toggle the color map texture on and off" << std::endl
            << "Press W to toggle wire frame mode on and off" << std::endl
            << "Press SPACE to cycle forwards through the light types" << std::endl
            << "Press BACKSPACE to cycle backwards through the light types" << std::endl
            << "Press ALT + ENTER to toggle between full screen and windowed modes" << std::endl
            << "Press ESC to exit the application" << std::endl           
            << std::endl
            << "Press H to hide help";
    }
    else
    {
        output
            << "FPS: " << g_framesPerSecond << std::endl
            << "Multisample anti-aliasing: " << g_msaaSamples << "x" << std::endl
            << "Anisotropic filtering: " << g_maxAnisotrophy << "x" << std::endl
            << GetLightTypeStr() << std::endl
            << std::endl
            << "Press H to display help";
    }

    GetClientRect(g_hWnd, &rcClient);
    rcClient.left += 4;
    rcClient.top += 2;

    g_pFont->DrawText(0, output.str().c_str(), -1, &rcClient,
        DT_EXPANDTABS | DT_LEFT, D3DCOLOR_XRGB(255, 255, 0));
}

bool ResetDevice()
{
    if (FAILED(g_pEffect->OnLostDevice()))
        return false;

    if (FAILED(g_pFont->OnLostDevice()))
        return false;

    if (FAILED(g_pDevice->Reset(&g_params)))
        return false;

    if (FAILED(g_pFont->OnResetDevice()))
        return false;

    if (FAILED(g_pEffect->OnResetDevice()))
        return false;

    return true;
}

void SetProcessorAffinity()
{
    // Assign the current thread to one processor. This ensures that timing
    // code runs on only one processor, and will not suffer any ill effects
    // from power management.
    //
    // Based on the DXUTSetProcessorAffinity() function in the DXUT framework.

    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;
    HANDLE hCurrentProcess = GetCurrentProcess();

    if (!GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask))
        return;

    if (dwProcessAffinityMask)
    {
        // Find the lowest processor that our process is allowed to run against.

        DWORD_PTR dwAffinityMask = (dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1));

        // Set this as the processor that our thread must always run against.
        // This must be a subset of the process affinity mask.

        HANDLE hCurrentThread = GetCurrentThread();

        if (hCurrentThread != INVALID_HANDLE_VALUE)
        {
            SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
            CloseHandle(hCurrentThread);
        }
    }

    CloseHandle(hCurrentProcess);
}

void ToggleFullScreen()
{
    static DWORD savedExStyle;
    static DWORD savedStyle;
    static RECT rcSaved;

    g_isFullScreen = !g_isFullScreen;

    if (g_isFullScreen)
    {
        // Moving to full screen mode.

        savedExStyle = GetWindowLong(g_hWnd, GWL_EXSTYLE);
        savedStyle = GetWindowLong(g_hWnd, GWL_STYLE);
        GetWindowRect(g_hWnd, &rcSaved);

        SetWindowLong(g_hWnd, GWL_EXSTYLE, 0);
        SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        g_windowWidth = GetSystemMetrics(SM_CXSCREEN);
        g_windowHeight = GetSystemMetrics(SM_CYSCREEN);

        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0,
            g_windowWidth, g_windowHeight, SWP_SHOWWINDOW);

        // Update presentation parameters.

        g_params.Windowed = FALSE;
        g_params.BackBufferWidth = g_windowWidth;
        g_params.BackBufferHeight = g_windowHeight;

        if (g_enableVerticalSync)
        {
            g_params.FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT;
            g_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
        }
        else
        {
            g_params.FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_IMMEDIATE;
            g_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        }
    }
    else
    {
        // Moving back to windowed mode.

        SetWindowLong(g_hWnd, GWL_EXSTYLE, savedExStyle);
        SetWindowLong(g_hWnd, GWL_STYLE, savedStyle);
        SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        g_windowWidth = rcSaved.right - rcSaved.left;
        g_windowHeight = rcSaved.bottom - rcSaved.top;

        SetWindowPos(g_hWnd, HWND_NOTOPMOST, rcSaved.left, rcSaved.top,
            g_windowWidth, g_windowHeight, SWP_SHOWWINDOW);

        // Update presentation parameters.

        g_params.Windowed = TRUE;
        g_params.BackBufferWidth = g_windowWidth;
        g_params.BackBufferHeight = g_windowHeight;
        g_params.FullScreen_RefreshRateInHz = 0;

        if (g_enableVerticalSync)
            g_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
        else
            g_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    ResetDevice();
}

void UpdateEffect()
{
    static D3DXMATRIX world, view, proj;
    static D3DXMATRIX xRot, yRot, translation;
    static D3DXMATRIX worldViewProjectionMatrix;
    static D3DXMATRIX worldInverseTransposeMatrix;

    // Calculate the perspective projection matrix.
    D3DXMatrixPerspectiveFovLH(&proj, CAMERA_FOVY,
        static_cast<float>(g_windowWidth) / static_cast<float>(g_windowHeight),
        CAMERA_ZNEAR, CAMERA_ZFAR);

    // Calculate the view matrix.
    D3DXMatrixLookAtLH(&view, &g_cameraPos, &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
        &D3DXVECTOR3(0.0f, 1.0f, 0.0f));  

    // Calculate world matrix to transform the cube.
    D3DXMatrixRotationX(&xRot, D3DXToRadian(g_pitch));
    D3DXMatrixRotationY(&yRot, D3DXToRadian(g_heading));
    D3DXMatrixMultiply(&world, &yRot, &xRot);
    D3DXMatrixTranslation(&translation, g_cubePos.x, g_cubePos.y, g_cubePos.z);
    D3DXMatrixMultiply(&world, &world, &translation);

    // Calculate combined world-view-projection matrix.
    worldViewProjectionMatrix = world * view * proj;

    // Calculate the transpose of the inverse of the world matrix.
    D3DXMatrixInverse(&worldInverseTransposeMatrix, 0, &world);
    D3DXMatrixTranspose(&worldInverseTransposeMatrix, &worldInverseTransposeMatrix);

    // Set the matrices for the shader.
    g_pEffect->SetMatrix("worldMatrix", &world);
    g_pEffect->SetMatrix("worldInverseTransposeMatrix", &worldInverseTransposeMatrix);
    g_pEffect->SetMatrix("worldViewProjectionMatrix", &worldViewProjectionMatrix);

    // Set the camera position.
    g_pEffect->SetValue("cameraPos", &g_cameraPos, sizeof(g_cameraPos));

    // Set the scene global ambient term.
    g_pEffect->SetValue("globalAmbient", &g_sceneAmbient, sizeof(g_sceneAmbient));

    // Set the lighting parameters for the shader.
    g_pEffect->SetValue("light.dir", g_light.dir, sizeof(g_light.dir));
    g_pEffect->SetValue("light.pos", g_light.pos, sizeof(g_light.pos));
    g_pEffect->SetValue("light.ambient", g_light.ambient, sizeof(g_light.ambient));
    g_pEffect->SetValue("light.diffuse", g_light.diffuse, sizeof(g_light.diffuse));
    g_pEffect->SetValue("light.specular", g_light.specular, sizeof(g_light.specular));
    g_pEffect->SetFloat("light.spotInnerCone", g_light.spotInnerCone);
    g_pEffect->SetFloat("light.spotOuterCone", g_light.spotOuterCone);
    g_pEffect->SetFloat("light.radius", g_light.radius);

    // Set the material parameters for the shader.
    g_pEffect->SetValue("material.ambient", g_material.ambient, sizeof(g_material.ambient));
    g_pEffect->SetValue("material.diffuse", g_material.diffuse, sizeof(g_material.diffuse));
    g_pEffect->SetValue("material.emissive", g_material.emissive, sizeof(g_material.emissive));
    g_pEffect->SetValue("material.specular", g_material.specular, sizeof(g_material.specular));
    g_pEffect->SetFloat("material.shininess", g_material.shininess);

    // Bind the textures to the shader.
    g_pEffect->SetTexture("colorMapTexture", g_disableColorMapTexture ? g_pNullTexture : g_pColorMap);
    g_pEffect->SetTexture("normalMapTexture", g_pNormalMap);
}

void UpdateFrame(float elapsedTimeSec)
{
    UpdateFrameRate(elapsedTimeSec);
    UpdateEffect();
}

void UpdateFrameRate(float elapsedTimeSec)
{
    static float accumTimeSec = 0.0f;
    static int frames = 0;

    accumTimeSec += elapsedTimeSec;

    if (accumTimeSec > 1.0f)
    {
        g_framesPerSecond = frames;

        frames = 0;
        accumTimeSec = 0.0f;
    }
    else
    {
        ++frames;
    }
}