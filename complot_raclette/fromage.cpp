#include "complot_raclette.h"

using namespace std;

// --- Variables globales ---
int g_width = 600;
int g_height = 400;

HBITMAP hBitmap = NULL;


// --- Fenêtre callback ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        // Charger l'image ici pour être sûr que le chemin est bon
        hBitmap = (HBITMAP)LoadImageW(
            NULL,
            L"repos/complot_raclette/asset/raclette.bmp",
            IMAGE_BITMAP,
            0, 0,
            LR_LOADFROMFILE
        );

        if (!hBitmap)
            MessageBox(hwnd, _T("Impossible de charger raclette.bmp"), _T("Erreur"), MB_OK);

        break; // ← IMPORTANT !!!

    case WM_SIZE:
        g_width = LOWORD(lParam);
        g_height = HIWORD(lParam);

        {
            TCHAR title[100];
            wsprintf(title, _T("raclette - %dx%d"), g_width, g_height);
            SetWindowText(hwnd, title);
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (hBitmap)
        {
            HDC memDC = CreateCompatibleDC(hdc);
            SelectObject(memDC, hBitmap);

            BITMAP bm;
            GetObject(hBitmap, sizeof(bm), &bm);

            BitBlt(
                hdc,
                0, 0, bm.bmWidth, bm.bmHeight,
                memDC,
                0, 0,
                SRCCOPY
            );

            DeleteDC(memDC);
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if (hBitmap)
            DeleteObject(hBitmap);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}


// --- Point d'entrée ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("MaFenetre");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    wc.hbrBackground = CreateSolidBrush(RGB(79, 34, 6)); // fond brun raclette

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        _T("Fenêtre initiale"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        g_width, g_height,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
