#include "complot_raclette.h"


using namespace std;

// --- Variables globales ---
int g_width = 600;
int g_height = 400;

HBITMAP hBitmap = NULL;
HWND hEdit;

// Menus
HMENU Mebar;
HMENU MeHelp;

// Chemins des images
const wchar_t* imgRaclette = L"C:/Users/cdidier/source/repos/complot_raclette/complot_raclette/asset/raclette.bmp";
const wchar_t* imgPizza = L"C:/Users/cdidier/source/repos/complot_raclette/complot_raclette/asset/imaget.bmp";
const wchar_t* imgBurger = L"C:/Users/cdidier/source/repos/complot_raclette/complot_raclette/asset/burger.bmp";
wchar_t currentImage[512] = L""; // image courante
wchar_t hiddenImage[512] = L"C:/Users/cdidier/source/repos/complot_raclette/complot_raclette/asset/imaget.bmp";

// --- Stéganographie LSB ---
vector<bool> textToBits(const string& text) {
    vector<bool> bits;
    for (char c : text) {
        for (int i = 7; i >= 0; --i)
            bits.push_back((c >> i) & 1);
    }
    bits.push_back(0); // marqueur fin
    return bits;
}

bool hideTextInBMP(const wchar_t* bmpPath, const string& message, const wchar_t* outputPath) {
    ifstream file(bmpPath, ios::binary);
    if (!file) return false;

    vector<char> header(54);
    file.read(header.data(), 54);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int padding = (4 - (width * 3) % 4) % 4;
    int pixelDataSize = (width * 3 + padding) * height;

    vector<unsigned char> pixels(pixelDataSize);
    file.read((char*)pixels.data(), pixelDataSize);
    file.close();

    vector<bool> bits = textToBits(message + '\0');

    int bitIndex = 0;
    for (int i = 0; i < pixels.size() && bitIndex < bits.size(); ++i) {
        pixels[i] &= 0xFE;
        pixels[i] |= bits[bitIndex++];
    }

    ofstream out(outputPath, ios::binary);
    if (!out) return false;
    out.write(header.data(), 54);
    out.write((char*)pixels.data(), pixelDataSize);
    out.close();
    return true;
}

string revealTextFromBMP(const wchar_t* bmpPath) {
    ifstream file(bmpPath, ios::binary);
    if (!file) return "";

    vector<char> header(54);
    file.read(header.data(), 54);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int padding = (4 - (width * 3) % 4) % 4;
    int pixelDataSize = (width * 3 + padding) * height;

    vector<unsigned char> pixels(pixelDataSize);
    file.read((char*)pixels.data(), pixelDataSize);
    file.close();

    string result;
    unsigned char c = 0;
    int count = 0;

    for (int i = 0; i < pixels.size(); ++i) {
        c = (c << 1) | (pixels[i] & 1);
        count++;
        if (count == 8) {
            if (c == 0) break;
            result += c;
            c = 0;
            count = 0;
        }
    }
    return result;
}

// --- Charger une image dans hBitmap ---
bool loadImage(const wchar_t* path) {
    HBITMAP hNew = (HBITMAP)LoadImageW(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hNew) return false;
    if (hBitmap) DeleteObject(hBitmap);
    hBitmap = hNew;
    wcscpy_s(currentImage, 512, path);
    return true;
}

// --- Fenetre callback ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // Charger image initiale
        if (!loadImage(imgRaclette))
            MessageBox(hwnd, _T("Impossible de charger raclette.bmp"), _T("Erreur"), MB_OK);

        // Champ texte
        hEdit = CreateWindowEx(0, _T("EDIT"), _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            20, 20, 250, 30, hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        // Boutons
        CreateWindowEx(0, _T("BUTTON"), _T("Creer"),
            WS_CHILD | WS_VISIBLE,
            20, 70, 100, 30, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindowEx(0, _T("BUTTON"), _T("Extraire"),
            WS_CHILD | WS_VISIBLE,
            140, 70, 100, 30, hwnd, (HMENU)4, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindowEx(0, _T("BUTTON"), _T("Quitter"),
            WS_CHILD | WS_VISIBLE,
            260, 70, 100, 30, hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        // Menu images
        Mebar = CreateMenu();
        MeHelp = CreateMenu();
        AppendMenu(MeHelp, MF_STRING, 101, _T("Raclette"));
        AppendMenu(MeHelp, MF_STRING, 102, _T("Pizza"));
        AppendMenu(MeHelp, MF_STRING, 103, _T("Burger"));
        AppendMenu(Mebar, MF_POPUP, (UINT_PTR)MeHelp, _T("Images"));
        SetMenu(hwnd, Mebar);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1: // Bouton Creer
        {
            TCHAR buffer[256];
            GetWindowText(hEdit, buffer, 256);

            char message[256];
            WideCharToMultiByte(CP_ACP, 0, buffer, -1, message, 256, NULL, NULL);

            if (hideTextInBMP(currentImage, message, hiddenImage)) {
                loadImage(hiddenImage);
                InvalidateRect(hwnd, NULL, TRUE);
                MessageBox(hwnd, _T("Texte cache dans l'image !"), _T("Succes"), MB_OK);
            }
            else {
                MessageBox(hwnd, _T("Erreur lors du masquage du texte"), _T("Erreur"), MB_OK);
            }
            break;
        }

        case 2: // Bouton Quitter
            PostQuitMessage(0);
            break;

        case 4: // Bouton Extraire
        {
            string message = revealTextFromBMP(hiddenImage);
            wchar_t wmsg[256];
            MultiByteToWideChar(CP_ACP, 0, message.c_str(), -1, wmsg, 256);
            MessageBox(hwnd, wmsg, _T("Message extrait"), MB_OK);
            break;
        }

        case 101: // Menu Raclette
            loadImage(imgRaclette);
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case 102: // Menu Pizza
            loadImage(imgPizza);
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case 103: // Menu Burger
            loadImage(imgBurger);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (hBitmap) {
            HDC memDC = CreateCompatibleDC(hdc);
            SelectObject(memDC, hBitmap);
            BITMAP bm;
            GetObject(hBitmap, sizeof(bm), &bm);
            BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY);
            DeleteDC(memDC);
        }
        EndPaint(hwnd, &ps);
        return 0;
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

// --- Point d'entree ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("MaFenetre");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(79, 34, 6));

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, _T("Complot Raclette"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        g_width, g_height, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
