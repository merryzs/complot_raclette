#include "framework.h"
#include "complot_raclette.h"
#include <windows.h>

#include <windows.h>
#include <tchar.h>
#include <vector>

using namespace std;

// --- Variables globales ---
int g_width = 600;
int g_height = 400;
int g_colorIndex = 0;
HBRUSH g_hBrush = NULL;

// Pour le déplacement de la fenêtre
static BOOL g_bDragging = FALSE;
static POINT g_ptOffset;

// Pour le dessin avec la souris
vector<POINT> g_points;
BOOL g_bDrawing = FALSE;

// Handles pour les contrôles
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
        g_hBrush = CreateSolidBrush(RGB(0, 0, 255));
        SetWindowText(hwnd, _T("Fenêtre initiale"));



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


        break;

    case WM_SIZE:
        g_width = LOWORD(lParam);
        g_height = HIWORD(lParam);

        {
            TCHAR title[100];
            wsprintf(title, _T("Fenêtre - %dx%d"), g_width, g_height);
            SetWindowText(hwnd, title);
        }
        break;

    case WM_SETCURSOR:
    {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hwnd, &pt);
        if (pt.x < g_width / 2)
            SetCursor(LoadCursor(NULL, IDC_HAND));
        else
            SetCursor(LoadCursor(NULL, IDC_CROSS));
        return TRUE;
    }
    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        // Déplacement de la fenêtre
        g_bDragging = TRUE;
        g_ptOffset = { x, y };
        SetCapture(hwnd);

        // Dessin à main levée
        g_bDrawing = TRUE;
        g_points.push_back({ x, y });

        // Changement de couleur dynamique
        g_colorIndex = (g_colorIndex + 1) % 3;
        COLORREF c = (g_colorIndex == 0) ? RGB(255, 0, 0) :
            (g_colorIndex == 1) ? RGB(0, 255, 0) : RGB(0, 0, 255);
        if (g_hBrush)
            DeleteObject(g_hBrush);
        g_hBrush = CreateSolidBrush(c);

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }




    case WM_MOUSEMOVE:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        if (g_bDragging)
        {
            POINT ptScreen = { x, y };
            ClientToScreen(hwnd, &ptScreen);
            SetWindowPos(hwnd, NULL, ptScreen.x - g_ptOffset.x, ptScreen.y - g_ptOffset.y,
                0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }

        if (g_bDrawing)
        {
            g_points.push_back({ x, y });
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }
    break;

    case WM_LBUTTONUP:
        g_bDragging = FALSE;
        g_bDrawing = FALSE;
        ReleaseCapture();
        break;

    case WM_KEYDOWN:
    {
        TCHAR keyName[32] = _T("");
        LONG lParamCopy = (lParam & 0x01FF0000) | ((lParam & 0x0000FFFF) << 16);
        GetKeyNameText(lParamCopy, keyName, 32);
        SetWindowText(hwnd, keyName);

        if (wParam == VK_ESCAPE)
            PostQuitMessage(0);
    }
    break;

    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, g_hBrush);
        return 1; // effacement géré
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1) // Créer
        {
            TCHAR buffer[256];
            GetWindowText(hEdit, buffer, 256);
            MessageBox(hwnd, buffer, _T("Texte saisi"), MB_OK);
        }
        else if (LOWORD(wParam) == 2) // Quitter
        {
            PostQuitMessage(0);
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Rectangle rouge centré
        int rectWidth = 200;
        int rectHeight = 150;
        RECT rect = {
            g_width / 2 - rectWidth / 2,
            g_height / 2 - rectHeight / 2,
            g_width / 2 + rectWidth / 2,
            g_height / 2 + rectHeight / 2
        };

        HBRUSH hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(hdc, &rect, hBrushRed);
        DeleteObject(hBrushRed);

        // Dessin à main levée
        if (g_points.size() > 1)
        {
            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            HGDIOBJ oldPen = SelectObject(hdc, hPen);
            MoveToEx(hdc, g_points[0].x, g_points[0].y, NULL);
            for (size_t i = 1; i < g_points.size(); i++)
            {
                LineTo(hdc, g_points[i].x, g_points[i].y);
            }
            SelectObject(hdc, oldPen);
            DeleteObject(hPen);
        }

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        if (g_hBrush)
            DeleteObject(g_hBrush);
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
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

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
