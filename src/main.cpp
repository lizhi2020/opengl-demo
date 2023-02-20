#include "glad/gl.h"

#include <cstdio>
#include <chrono>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wingdi.h>

#pragma comment (lib, "opengl32.lib")

/// <summary>
/// init opengl api within glad. require a valid gl context been current
/// </summary>
/// <returns></returns>
bool init_glproc() {
    auto f = [](const char* name) {
        auto p = (GLADapiproc)wglGetProcAddress(name);
        if (p == 0 ||
            (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
            (p == (void*)-1)) {
            HMODULE module = LoadLibraryA("opengl32.dll");
            if (module != NULL)
                p = (GLADapiproc)GetProcAddress(module, name);
        }

        return p;
    };
    auto version = gladLoadGL((GLADloadfunc)f);
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return false;
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    return true;
}
HDC make_openglrc(HWND hWnd) {
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
    return ourWindowHandleToDeviceContext;
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
extern void drawcall();
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

    HDC hdc;
    if (auto p = create_dummy())
        hdc = make_openglrc(p);
    else {
        return 1;
    }

    if (init_glproc())
        printf("success");

    auto last = std::chrono::system_clock::now();

    // why when i resize the window, draw will be stopped?
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            // do i need dispatch here?
            DispatchMessage(&msg);
        };
        // detect frame time
        // always greater than 20ms?
        //auto cur = std::chrono::system_clock::now();
        //auto dur = std::chrono::duration_cast<std::chrono::microseconds>(cur - last);
        //last = cur;
        //printf("%lld\n", dur.count());
        drawcall();
        SwapBuffers(hdc);
    }
    return 0;
}