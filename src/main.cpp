#include "glad/gl.h"

#include <cstdio>
// must include whole windows head file?
#include <Windows.h>
#include <wingdi.h>

#pragma comment (lib, "opengl32.lib")

GLADapiproc getproc(const char* name) {
    auto p = (GLADapiproc)wglGetProcAddress(name);
    if (p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1)) {
        HMODULE module = LoadLibraryA("opengl32.dll");
        if (module != NULL)
            p = (GLADapiproc)GetProcAddress(module, name);
    }

    return p;
}
bool init_glproc() {
    auto version = gladLoadGL(getproc);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return false;
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    return true;
}
void make_openglrc(HWND hWnd) {
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    HDC ourWindowHandleToDeviceContext = GetDC(hWnd);

    int  letWindowsChooseThisPixelFormat;
    letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);

    auto r = SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);
    HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
    wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
    {
        //MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
        ////wglMakeCurrent(ourWindowHandleToDeviceContext, NULL); Unnecessary; wglDeleteContext will make the context not current
        //wglDeleteContext(ourOpenGLRenderingContext);
    }
    break;
    case WM_CLOSE:
    {
        PostQuitMessage(0);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;

}
HWND create_dummy() {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = L"oglversionchecksample";
    wc.style = CS_OWNDC;
    if (!RegisterClass(&wc))
        return NULL;
    return CreateWindowW(wc.lpszClassName, L"openglversioncheck", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, wc.hInstance, 0);
}
int main() {
    MSG msg = { 0 };

    if (auto p = create_dummy())
        make_openglrc(p);

    if (init_glproc())
        printf("success");

    while (GetMessage(&msg, NULL, 0, 0) > 0)
        DispatchMessage(&msg);

    return 0;
}