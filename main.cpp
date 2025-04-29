#define UNICODE
#define _UNICODE

#include <windows.h>
#include <dwmapi.h>
#include <iostream>
#include "transcricao.h"
#include "resource.h"

#pragma comment(lib, "dwmapi.lib")

const wchar_t windowClassName[] = L" ";

#define ID_LISTBOX       101
#define ID_EDITBOX       103
#define ID_BUTTON        104
#define ID_BUTTON_CLEAR  105

HWND hCopyCheckBox, hListBox, hEditBox, hConvertButton, hClearButton, hParenthesesCheckBox;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_CREATE:
        hEditBox = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                 20, 60, 200, 25, hwnd, (HMENU)ID_EDITBOX, NULL, NULL);

        hConvertButton = CreateWindowW(L"BUTTON", L"Convert", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                       230, 60, 100, 25, hwnd, (HMENU)ID_BUTTON, NULL, NULL);

        hClearButton = CreateWindowW(L"BUTTON", L"Clean", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                     340, 60, 80, 25, hwnd, (HMENU)ID_BUTTON_CLEAR, NULL, NULL);

        hListBox = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | LBS_STANDARD | WS_VSCROLL,
                                 20, 100, 400, 200, hwnd, (HMENU)ID_LISTBOX, NULL, NULL);

        hCopyCheckBox = CreateWindowW(L"BUTTON", L"Copy to Clipboard",
                                      WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                                      20, 30, 250, 20, hwnd, NULL, NULL, NULL);
        hParenthesesCheckBox = CreateWindowW(L"BUTTON", L"Add parentheses",
                                             WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                                             20, 10, 250, 20, hwnd, NULL, NULL, NULL);

        SendMessageW(hParenthesesCheckBox, BM_SETCHECK, BST_CHECKED, 0);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON && HIWORD(wParam) == BN_CLICKED)
        {
            wchar_t input[64];
            GetWindowTextW(hEditBox, input, sizeof(input) / sizeof(wchar_t));

            wchar_t output[512];

            BOOL isCopyChecked = SendMessage(hCopyCheckBox, BM_GETCHECK, 0, 0);
            BOOL isParenthesesChecked = SendMessage(hParenthesesCheckBox, BM_GETCHECK, 0, 0);

            transcribe_value(input, output, isCopyChecked == BST_CHECKED, isParenthesesChecked == BST_CHECKED);

            SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)output);
            SetWindowTextW(hEditBox, L"");
        }
        else if (LOWORD(wParam) == ID_BUTTON_CLEAR && HIWORD(wParam) == BN_CLICKED)
        {
            SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
        }
        else if (LOWORD(wParam) == ID_LISTBOX && HIWORD(wParam) == LBN_DBLCLK)
        {
            int selectedIndex = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
            if (selectedIndex != LB_ERR)
            {
                int textLength = SendMessage(hListBox, LB_GETTEXTLEN, selectedIndex, 0);
                wchar_t* buffer = new wchar_t[textLength + 1];
                SendMessage(hListBox, LB_GETTEXT, selectedIndex, (LPARAM)buffer);
                buffer[textLength] = L'\0';

                if (OpenClipboard(hwnd))
                {
                    EmptyClipboard();
                    HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (textLength + 1) * sizeof(wchar_t));
                    if (hClipboardData)
                    {
                        wchar_t* clipboardBuffer = (wchar_t*)GlobalLock(hClipboardData);
                        wcscpy(clipboardBuffer, buffer);
                        GlobalUnlock(hClipboardData);
                        SetClipboardData(CF_UNICODETEXT, hClipboardData);
                    }
                    CloseClipboard();
                }

                delete[] buffer;
            }
        }
        break;

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkColor(hdc, RGB(30, 30, 30));
        static HBRUSH hBrush = NULL;
        if (!hBrush)
            hBrush = CreateSolidBrush(RGB(30, 30, 30));
        return (INT_PTR)hBrush;
    }

    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;
        minMaxInfo->ptMinTrackSize.x = 295;
        minMaxInfo->ptMinTrackSize.y = 210;
        return 0;
    }

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        MoveWindow(hListBox, 20, 100, width - 40, height - 110, TRUE);
        MoveWindow(hEditBox, 20, 60, width - 230, 25, TRUE);
        MoveWindow(hConvertButton, width - 205, 60, 100, 25, TRUE);
        MoveWindow(hClearButton, width - 100, 60, 80, 25, TRUE);
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = windowClassName;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Error registering window class.", L"Erro", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(0, windowClassName, L"Transcribe", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 460, 320,
                          NULL, NULL, hInstance, NULL);

    if (!hwnd)
    {
        MessageBox(NULL, L"Error creating window.", L"Erro", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    BOOL enableDarkMode = TRUE;
    DwmSetWindowAttribute(hwnd, 20, &enableDarkMode, sizeof(enableDarkMode));

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
