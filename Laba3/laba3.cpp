#include <windows.h>
#include "resource.h"
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")


HINSTANCE ghInstance;   // ���������� ��� �������� ������� �������� (�� ���������)  

//���������������� ���������� ����������
HWND hList1,                //��������� �� ������
     hSlider,               //��������� �� �������
     hScroll;               //��������� �� ������� ���������
INT  ListIndex,             //������� ������ ������ � ������
     InstanceIndex;         //��������������� ������
char szListText[256];       //����� ������

//���������������� ���������� �������
HBITMAP bit = NULL;         //��������� �� ������� ����� (��������)
HDC dc = NULL;              //��������� �� ������� ����������
BYTE* buffer = NULL;        //������ ��� �������� ����� ���� ������� �����
DWORD bsize = 0U;           //������ �������
PAINTSTRUCT ps;             //��������� ��� ���������

//���������� ���������� �������� ������ ��������� � ��������
INT  SliderParam = 100, //��������� �������� ��������
     nTekV = 0 ,        //��������� �������� ������������ �������
     nTekG = 0;         //��������� �������� �������������� �������

BOOL CALLBACK   PviewDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

// ������� ���������� ���������
int WINAPI    WinMain(HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow)
{
    HWND hWndDialog;
    MSG     msg;

    ghInstance = hInstance;
    // ��������  ����������� ����
    hWndDialog = CreateDialogParam(hInstance,
        MAKEINTRESOURCE(LABA3_DLG),
        NULL,
        (DLGPROC)PviewDlgProc,
        (LONG)0);
    // ����������� ���� ��������� ��������� ����������
    while (GetMessage(&msg, NULL, 0, 0))
        if (!IsDialogMessage(hWndDialog, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    DestroyWindow(hWndDialog);
    return 0;
}

//��������� ��������� ��������� ����������� ����
BOOL CALLBACK   PviewDlgProc(HWND    hWnd,
    UINT    wMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
    switch (wMsg)
    {
    //������� ��������� ��� ����������� ����
    case WM_DESTROY:
        {
            if (buffer)             //��������� ������ �� ��� �����
                delete[] buffer;    //���� ���, �� ������� ���
            buffer = NULL;          //������ ��������� �� ������
            DeleteObject(bit);      //������� ������� �����
            DeleteDC(dc);           //�������� ��������� �� ������� ����������
            PostQuitMessage(0);     //������ ��������� � �������� ���������
        }
    break;

    //��������� � ������������� ����������� ����
    case WM_INITDIALOG:
        {
            InitCommonControls(); //������� ������������� ��������� ���������� �� �����

            //������������� ������  
            hList1 = GetDlgItem(hWnd, IDC_LIST1);   //�������� ���������� ���� ������ 
            //��������� ������ ����� ��������
            for (InstanceIndex = 0; InstanceIndex < 2; InstanceIndex++)
            {
                sprintf_s(szListText, "�������� - %d", InstanceIndex + 1);
                ListIndex = SendMessage(hList1, LB_ADDSTRING, 0, (long)szListText);
            }
            SendMessage(hList1, WM_SETREDRAW, TRUE, 0);

            // ������������� �������� 
            hSlider = GetDlgItem(hWnd, IDC_SLIDER2);                    //�������� ��������� �� �������
            SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100)); //���������� �������� ������ �������� �� 0 �� 100
            SendMessage(hSlider, TBM_SETPOS, TRUE, 100);                //���������� ������ � ��������� ������� 100

            //������������� ����� ���������
            hScroll = GetDlgItem(hWnd, IDC_SCROLLBAR1);     //�������� ������ ���� ������������ ������� ���������
            SetScrollRange(hScroll, SB_CTL, 0, 100, TRUE);  // ���������� �������� ������ �� 0 �� 100
            SetScrollPos(hScroll, SB_CTL, 0, TRUE); // ���������� ��������� ��������� ������

            // ��������� �� �� �������� ��� �������������� �������
            hScroll = GetDlgItem(hWnd, IDC_SCROLLBAR2);
            SetScrollRange(hScroll, SB_CTL, 0, 100, TRUE);
            SetScrollPos(hScroll, SB_CTL, 0, TRUE);
        }
    break;

    //��������� �� ��������� ��������� �������������� �������
    case WM_HSCROLL:
        {
            if (lParam == (LPARAM)hSlider) //���� ������ ��������� �� ��������
                switch (LOWORD(wParam))
                {
                    //�������������� �� �������� TB - TrackBar
                case TB_PAGEUP: SliderParam = max(0, SliderParam - 10); break;    //����� �� ��������� ������� ����� (10)
                case TB_LINEUP: SliderParam = max(0, SliderParam - 1); break;     //����� �� ���� ����� �����
                case TB_PAGEDOWN: SliderParam = min(100, SliderParam + 10); break;  //����� �� ��������� ������� ������ (10)
                case TB_LINEDOWN: SliderParam = min(100, SliderParam + 1); break;   //����� �� ���� ����� ������
                case TB_THUMBPOSITION:
                case TB_THUMBTRACK: SliderParam = HIWORD(wParam); break;            //������ � �������������� ��������
                }
            SendMessage(hSlider, TBM_SETPOS, TRUE, SliderParam);                    //��������� �������� � ��������� ���������

            if (lParam == (LPARAM)hScroll) //���� ������ ��������� �� ������� ���������
                switch (LOWORD(wParam))
                {
                    //�������������� �� ������� SB - ScrollBar
                case SB_PAGEUP: nTekG = max(0, nTekG - 10); break;      //����� �� ��������� ������� ����� (10)
                case SB_LINEUP: nTekG = max(0, nTekG - 1); break;       //����� �� ���� ����� �����
                case SB_PAGEDOWN: nTekG = min(100, nTekG + 10); break;  //����� �� ��������� ������� ������ (10)
                case SB_LINEDOWN: nTekG = min(100, nTekG + 1); break;   //����� �� ���� ����� ������
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK: nTekG = HIWORD(wParam); break;      //������ � �������������� ������� ���������
                }
            SetScrollPos((HWND)lParam, SB_CTL, nTekG, TRUE);            //��������� ������� � ��������� ���������
            InvalidateRect(hWnd, 0, 0);                                 //������ �� ����������� ����                         
        }
    break;

    //��������� �� ��������� ��������� ������������� ������
    case WM_VSCROLL:
        {
            switch (LOWORD(wParam))
            {
                //�������������� �� ������� SB - ScrollBar
            case SB_PAGEUP: nTekV = max(0, nTekV - 10); break;     //����� �� ��������� ������� ����� (10)
            case SB_LINEUP: nTekV = max(0, nTekV - 1); break;      //����� �� ���� ����� �����
            case SB_PAGEDOWN: nTekV = min(100, nTekV + 10); break; //����� �� ��������� ������� ������ (10)
            case SB_LINEDOWN: nTekV = min(100, nTekV + 1); break;  //����� �� ���� ����� ������
            case SB_THUMBPOSITION:
            case SB_THUMBTRACK: nTekV = HIWORD(wParam); break;     //������ � �������������� ������� ���������
            }
            SetScrollPos((HWND)lParam, SB_CTL, nTekV, TRUE);       //��������� ������� � ��������� ���������
            InvalidateRect(hWnd, 0, 0);                            //������ �� ����������� ���� 
        }
    break;

    //��������� �� ����������� �������� ����
    case WM_PAINT:
        {
        BeginPaint(hWnd, &ps); //�������������� ���������
        if (bit) {
                BITMAP b;                                       //������ ��������� ������� �����, ��� ������ � ��� � ���� ����� ����
                ZeroMemory(&b, sizeof(BITMAP));                 //������� ��������� b �� ��������� ���������� ��������
                GetObject(bit, sizeof(BITMAP), (LPVOID)&b);     //����������� ��������� ������� ����� � �������
                SelectObject(dc, bit);                          //����������� ������� ����� � ��������� ����������
                StretchBlt(
                    ps.hdc,  //������ �� ���������� ������
                    50,      //�� ����������� - ����� ������� ��� - ������ ��������� �� ���� 
                    20,      //�� ��������� - ����� ������� ��� - ������ ��������� �� ����
                    300,     //������ ������������� ���� �� �����������
                    300,     //������ ������������� ���� �� ���������
                    dc,      //������ ������
                    b.bmWidth * nTekG / 100,                         //�� ����������� - ����� ������� ��� - ������������ ������
                    b.bmHeight * nTekV / 100,                        //�� ��������� - ������ �������  ��� - ������������� ������
                    b.bmWidth * SliderParam/ 100,                    //������ ���� ������������� ������
                    b.bmHeight * SliderParam / 100,                  //������ ���� ������������� ������
                    SRCCOPY);
            }
            EndPaint(hWnd, &ps);    //��������� ��������� ���������
        }
    break;

    //��������� � �������� ����������� ����
    case WM_CLOSE:
        {
            PostQuitMessage(0); //������ ��������� � �������� ���������
        }
    break;

    //��������� �� ��������� ����������� ����
    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            //������ �� �������
            case IDC_LIST1:
                //��������� ����� ������������� �� �������� ����� wParam. ��� ��������� �� ������ ���������� � LBN_
                if (HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) == LBN_SELCHANGE)
                {
                    ListIndex = SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0); //�������� ������ ���������� ������ � ������
                    //��������� switch �� ������ �������� � ������
                    switch (ListIndex) {
                    case 0:
                        //� ���������� bit ��������� ����������� � ���� ������� �����
                        //��������� �������������� ������� IDB_BITMAP1, ������� ������� ���� ��������� � ������
                        bit = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
                        break;
                    case 1:
                        //� ���������� bit ��������� ����������� � ���� ������� �����
                        //��������� �������������� ������� IDB_BITMAP2, ������� ������� ���� ��������� � ������
                        bit = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
                        break;
                    }
                    BITMAP b;                                       //������ ��������� ������� �����, ��� ������ � ��� � ���� ����� ����
                    ZeroMemory(&b, sizeof(BITMAP));                 //������� ��������� b �� ��������� ���������� ��������
                    GetObject(bit, sizeof(BITMAP), (LPVOID)&b);     //����������� ��������� ������� ����� � �������
                    bsize = b.bmWidth * b.bmHeight * b.bmBitsPixel; //�������� ������(��� � ������) ������� �����
                    buffer = new  BYTE[bsize];                      //������� ������ ��� �������� ��� 
                    dc = ::CreateCompatibleDC(GetDC(hWnd));         //������ �����

                    //���� ����� ������� ����� ��������� ������ ��, ������ ��������� �� ������
                    if (!bit)
                        MessageBox(hWnd, "������ ��� �������� ����� !!!", "BITMAP ERROR", MB_OK | MB_ICONERROR);
                    InvalidateRect(hWnd, 0, 0); //����������� ����������� ����
                }
                break;

            default:
                return FALSE; //���������� FALSE, ���� �� �� �� ���������� �� ���� �� �������
            }
            break;

        default:
            return FALSE; //���������� FALSE, ���� �� �� �� ���������� �� ���� �� �������
        }
    }

    return TRUE;
}
