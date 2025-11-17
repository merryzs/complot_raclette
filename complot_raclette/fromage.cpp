#include "complot_raclette.h"

using namespace std;

// --- Variables globales ---
int g_width = 600;
int g_height = 400;

HBITMAP hBitmap = NULL;

HWND hEdit;




HMENU Mebar;
HMENU MeFile;
HMENU MeHelp;



// --- Fenêtre callback ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        // Charger l'image ici pour être sûr que le chemin est bon
        hBitmap = (HBITMAP)LoadImageW(
            NULL,
            L"C:/Users/cdidier/source/repos/complot_raclette/complot_raclette/asset/raclette.bmp",
            IMAGE_BITMAP,
            0, 0,
            LR_LOADFROMFILE
        );

        if (!hBitmap)
            MessageBox(hwnd, _T("Impossible de charger raclette.bmp"), _T("img de mrd"), MB_OK);


        // Créer les contrôles (Niveau 4)
        hEdit = CreateWindowEx(0, _T("EDIT"), _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            20, 20, 200, 25, hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindowEx(0, _T("BUTTON"), _T("Créer"),
            WS_CHILD | WS_VISIBLE,
            20, 60, 80, 25, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindowEx(0, _T("BUTTON"), _T("Quitter"),
            WS_CHILD | WS_VISIBLE,
            120, 60, 80, 25, hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindowEx(0, _T("BUTTON"), _T("Quitter"),
            WS_CHILD | WS_VISIBLE,
            120, 60, 80, 25, hwnd, (HMENU)3, ((LPCREATESTRUCT)lParam)->hInstance, NULL);


        Mebar = CreateMenu();
        MeFile = CreateMenu();
        MeHelp = CreateMenu();

        AppendMenu(MeFile, MF_STRING, 2, _T("tu leave pas"));
        AppendMenu(MeHelp, MF_STRING, 1, _T("A propos (non, t'aura rien)"));
        AppendMenu(MeHelp, MF_STRING, 3, _T("fond"));

        AppendMenu(Mebar, MF_POPUP, (UINT_PTR)MeFile, _T("un truc jaune"));
        AppendMenu(Mebar, MF_POPUP, (UINT_PTR)MeHelp, _T("Aide??? y'a pas"));

        SetMenu(hwnd, Mebar);



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