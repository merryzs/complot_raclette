#include "complot_raclette.h"

using namespace std;


int g_width = 600;
int g_height = 400;
HBITMAP hBitmap = NULL;
BYTE* pBits = nullptr; // pointeur vers les pixels
BITMAPINFO bih = {};
HWND hTextBox = NULL;
HWND hButton = NULL;

HMENU Mebar;
HMENU MeFile;
HMENU MeHelp;
HBITMAP hbit;


// Fonction pour charger un BMP en mémoire avec CreateDIBSection
bool LoadBitmapDIBSection(const wchar_t* filename, HDC hdc)
{
    HANDLE hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    BITMAPFILEHEADER bfh;
    DWORD bytesRead;
    ReadFile(hFile, &bfh, sizeof(bfh), &bytesRead, NULL);

    BITMAPINFOHEADER bmih;
    ReadFile(hFile, &bmih, sizeof(bmih), &bytesRead, NULL);

    bih.bmiHeader = bmih;

   

    // Lire les pixels
    SetFilePointer(hFile, bfh.bfOffBits, NULL, FILE_BEGIN);
    ReadFile(hFile, pBits, bmih.biSizeImage, &bytesRead, NULL);

    CloseHandle(hFile);
    return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        HDC hdc = GetDC(hwnd);
        hbit = (HBITMAP)LoadImageW(
            NULL,

            L"complot_raclette/asset/raclette.bmp",

            IMAGE_BITMAP,
            0, 0,
            LR_LOADFROMFILE
        );



        if (!hBitmap)
            MessageBox(hwnd, _T("Impossible de charger raclette.bmp"), _T("img de mrd"), MB_OK);



    case WM_SIZE:
        g_width = LOWORD(lParam);
        g_height = HIWORD(lParam);
        InvalidateRect(hwnd, NULL, TRUE);
        break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        switch (wmId)
        {
        case 102: // Bouton "Créer"
        {
            wchar_t buffer[256];
            GetWindowTextW(hTextBox, buffer, 256);
            MessageBoxW(hwnd, buffer, L"Contenu de la text box", MB_OK);
            break;
        }
        case 103: // Bouton "Quitter"
        {
            DestroyWindow(hwnd); // ferme la fenêtre
            break;
        }

        // Menu (exemple)
        case 201:
            MessageBoxW(hwnd, L"Option 1 sélectionnée", L"Menu", MB_OK);
            break;
        case 202:
            MessageBoxW(hwnd, L"A propos...", L"Aide", MB_OK);
            break;
        case 203:
            MessageBoxW(hwnd, L"Fond sélectionné", L"Aide", MB_OK);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

        BITMAP bmp;
        GetObject(hBitmap, sizeof(BITMAP), &bmp);

        BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
    }

    case WM_DESTROY:
        if (hBitmap) DeleteObject(hBitmap);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MaFenetre";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc))
    {
        MessageBoxW(NULL, L"Erreur lors de l'enregistrement de la classe", L"Erreur", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"Fenêtre Raclette",
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