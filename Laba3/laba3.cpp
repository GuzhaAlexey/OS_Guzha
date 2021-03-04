#include <windows.h>
#include "resource.h"
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")


HINSTANCE ghInstance;   // Переменная для хранения хендела процесса (по умолчанию)  

//Пользовательскте глобальные переменные
HWND hList1,                //указатель на список
     hSlider,               //указатель на слайдер
     hScroll;               //указатель на линейку прокрутки
INT  ListIndex,             //текущий индекс строки в списке
     InstanceIndex;         //вспомогательный индекс
char szListText[256];       //текст списка

//Пользовательские глобальные объекты
HBITMAP bit = NULL;         //указатель на битовую карту (картинку)
HDC dc = NULL;              //указатель на текущее устройство
BYTE* buffer = NULL;        //буффер для хранения битов всей битовой карты
DWORD bsize = 0U;           //размер буффера
PAINTSTRUCT ps;             //структура для рисования

//Переменные начального значения линеек прокрутки и слайдера
INT  SliderParam = 100, //начальный параметр слайдера
     nTekV = 0 ,        //начальный параметр вертикальной линейки
     nTekG = 0;         //начальный параметр горизонтальной линейки

BOOL CALLBACK   PviewDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

// Главное приложение программы
int WINAPI    WinMain(HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    int         nCmdShow)
{
    HWND hWndDialog;
    MSG     msg;

    ghInstance = hInstance;
    // Создание  диалогового окна
    hWndDialog = CreateDialogParam(hInstance,
        MAKEINTRESOURCE(LABA3_DLG),
        NULL,
        (DLGPROC)PviewDlgProc,
        (LONG)0);
    // Стандартный цикл обработки сообщений приложения
    while (GetMessage(&msg, NULL, 0, 0))
        if (!IsDialogMessage(hWndDialog, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    DestroyWindow(hWndDialog);
    return 0;
}

//Процедура обработки сообщений диалогового окна
BOOL CALLBACK   PviewDlgProc(HWND    hWnd,
    UINT    wMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
    switch (wMsg)
    {
    //Событие возникает при уничтожение окна
    case WM_DESTROY:
        {
            if (buffer)             //проверяем пустой ли наш буфер
                delete[] buffer;    //если так, то очищаем его
            buffer = NULL;          //удалем указатель на буффер
            DeleteObject(bit);      //удаляем битовую карту
            DeleteDC(dc);           //удалеяем указатель на текущее устройство
            PostQuitMessage(0);     //кидаем сообщение о закрытии программы
        }
    break;

    //Сообщение о инициализации диалогового окна
    case WM_INITDIALOG:
        {
            InitCommonControls(); //функция инициализации элементов управления на форме

            //Инициализация списка  
            hList1 = GetDlgItem(hWnd, IDC_LIST1);   //получить дескриптор окна списка 
            //заполнить список двумя строками
            for (InstanceIndex = 0; InstanceIndex < 2; InstanceIndex++)
            {
                sprintf_s(szListText, "Картинка - %d", InstanceIndex + 1);
                ListIndex = SendMessage(hList1, LB_ADDSTRING, 0, (long)szListText);
            }
            SendMessage(hList1, WM_SETREDRAW, TRUE, 0);

            // Инициализация слайдера 
            hSlider = GetDlgItem(hWnd, IDC_SLIDER2);                    //получить указатель на слайдер
            SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100)); //установить диапазон работы слайдера от 0 до 100
            SendMessage(hSlider, TBM_SETPOS, TRUE, 100);                //установить движок в начальную позицию 100

            //Инициализация полос прокрутки
            hScroll = GetDlgItem(hWnd, IDC_SCROLLBAR1);     //получить хендел окна вертикальной линейки прокрутки
            SetScrollRange(hScroll, SB_CTL, 0, 100, TRUE);  // установить диапазон работы от 0 до 100
            SetScrollPos(hScroll, SB_CTL, 0, TRUE); // установить начальное положение движка

            // выполнить те же действия для горизонтальной линейки
            hScroll = GetDlgItem(hWnd, IDC_SCROLLBAR2);
            SetScrollRange(hScroll, SB_CTL, 0, 100, TRUE);
            SetScrollPos(hScroll, SB_CTL, 0, TRUE);
        }
    break;

    //Сообщение об изменении положения горизонтальный движков
    case WM_HSCROLL:
        {
            if (lParam == (LPARAM)hSlider) //если придет сообщение от слайдера
                switch (LOWORD(wParam))
                {
                    //Индетификаторы от слайдера TB - TrackBar
                case TB_PAGEUP: SliderParam = max(0, SliderParam - 10); break;    //сдвиг на несколько пунктов влево (10)
                case TB_LINEUP: SliderParam = max(0, SliderParam - 1); break;     //сдвиг на один пункт влево
                case TB_PAGEDOWN: SliderParam = min(100, SliderParam + 10); break;  //сдвиг на несколько пунктов вправо (10)
                case TB_LINEDOWN: SliderParam = min(100, SliderParam + 1); break;   //сдвиг на один пункт вправо
                case TB_THUMBPOSITION:
                case TB_THUMBTRACK: SliderParam = HIWORD(wParam); break;            //запрос о местоположении слайдера
                }
            SendMessage(hSlider, TBM_SETPOS, TRUE, SliderParam);                    //установка слайдера в указанное положение

            if (lParam == (LPARAM)hScroll) //если придет сообщение от линейки прокрутки
                switch (LOWORD(wParam))
                {
                    //Индетификаторы от линейки SB - ScrollBar
                case SB_PAGEUP: nTekG = max(0, nTekG - 10); break;      //сдвиг на несколько пунктов влево (10)
                case SB_LINEUP: nTekG = max(0, nTekG - 1); break;       //сдвиг на один пункт влево
                case SB_PAGEDOWN: nTekG = min(100, nTekG + 10); break;  //сдвиг на несколько пунктов вправо (10)
                case SB_LINEDOWN: nTekG = min(100, nTekG + 1); break;   //сдвиг на один пункт вправо
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK: nTekG = HIWORD(wParam); break;      //запрос о местоположении линейки прокрутки
                }
            SetScrollPos((HWND)lParam, SB_CTL, nTekG, TRUE);            //установка линейки в указанное положение
            InvalidateRect(hWnd, 0, 0);                                 //запрос на перерисовку окна                         
        }
    break;

    //Сообщение об изменении положения вертикального движка
    case WM_VSCROLL:
        {
            switch (LOWORD(wParam))
            {
                //Индетификаторы от линейка SB - ScrollBar
            case SB_PAGEUP: nTekV = max(0, nTekV - 10); break;     //сдвиг на несколько пунктов влево (10)
            case SB_LINEUP: nTekV = max(0, nTekV - 1); break;      //сдвиг на один пункт влево
            case SB_PAGEDOWN: nTekV = min(100, nTekV + 10); break; //сдвиг на несколько пунктов вправо (10)
            case SB_LINEDOWN: nTekV = min(100, nTekV + 1); break;  //сдвиг на один пункт вправо
            case SB_THUMBPOSITION:
            case SB_THUMBTRACK: nTekV = HIWORD(wParam); break;     //запрос о местоположении линейки прокрутки
            }
            SetScrollPos((HWND)lParam, SB_CTL, nTekV, TRUE);       //установка линейки в указанное положение
            InvalidateRect(hWnd, 0, 0);                            //запрос на перерисовку окна 
        }
    break;

    //Сообщение на перерисовку рабочего окна
    case WM_PAINT:
        {
        BeginPaint(hWnd, &ps); //инициализируем рисование
        if (bit) {
                BITMAP b;                                       //задаем локальную битовую карту, для работы с ней в этой части кода
                ZeroMemory(&b, sizeof(BITMAP));                 //очищаем переменую b от возможных предыдущих значений
                GetObject(bit, sizeof(BITMAP), (LPVOID)&b);     //привязываем временную битовую карту к текущей
                SelectObject(dc, bit);                          //привязываем битовую карту к контексту устройства
                StretchBlt(
                    ps.hdc,  //хендел на устройство вывода
                    50,      //по горизонтали - левый верхний угл - начало отрисовки на окне 
                    20,      //по вертикали - левый верхний угл - начало отрисовки на окне
                    300,     //ширина отображаемого поля по горизонтали
                    300,     //ширина отображаемого поля по вертикали
                    dc,      //хендел холста
                    b.bmWidth * nTekG / 100,                         //по горизонтали - левый верхний угл - изображемого холста
                    b.bmHeight * nTekV / 100,                        //по вертикали - правый верхний  угл - изображаемого холста
                    b.bmWidth * SliderParam/ 100,                    //ширина поля изображаемого холста
                    b.bmHeight * SliderParam / 100,                  //высота поля изображаемого холста
                    SRCCOPY);
            }
            EndPaint(hWnd, &ps);    //завершаем процедуру рисования
        }
    break;

    //Сообщение о закрытии диалогового окна
    case WM_CLOSE:
        {
            PostQuitMessage(0); //кидаем сообщение о закрытии программы
        }
    break;

    //Сообщение от элементов диалогового окна
    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            //Работа со списком
            case IDC_LIST1:
                //извещения можно анализировать по старшему слову wParam. Все извещения от списка начинаются с LBN_
                if (HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) == LBN_SELCHANGE)
                {
                    ListIndex = SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0); //получить индекс выделенной строки в списке
                    //запускаем switch по номеру элемента в списке
                    switch (ListIndex) {
                    case 0:
                        //в переменную bit загружаем изображение в виде битовой карты
                        //загружаем соответсвующие ресурсы IDB_BITMAP1, которые заранее были добавлены в проект
                        bit = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
                        break;
                    case 1:
                        //в переменную bit загружаем изображение в виде битовой карты
                        //загружаем соответсвующие ресурсы IDB_BITMAP2, которые заранее были добавлены в проект
                        bit = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
                        break;
                    }
                    BITMAP b;                                       //задаем локальную битовую карту, для работы с ней в этой части кода
                    ZeroMemory(&b, sizeof(BITMAP));                 //очищаем переменую b от возможных предыдущих значений
                    GetObject(bit, sizeof(BITMAP), (LPVOID)&b);     //привязываем временную битовую карту к текущей
                    bsize = b.bmWidth * b.bmHeight * b.bmBitsPixel; //выясняем размер(вес в байтах) битовой карты
                    buffer = new  BYTE[bsize];                      //создаем буффер для хранения там 
                    dc = ::CreateCompatibleDC(GetDC(hWnd));         //задаем холст

                    //если вдруг битовая карта оказалась пустой то, выдаем сообщение об ошибке
                    if (!bit)
                        MessageBox(hWnd, "Ошибка при загрузки файла !!!", "BITMAP ERROR", MB_OK | MB_ICONERROR);
                    InvalidateRect(hWnd, 0, 0); //запрашиваем перерисовку окна
                }
                break;

            default:
                return FALSE; //возвращаем FALSE, если ни мы не обработали ни одно из событий
            }
            break;

        default:
            return FALSE; //возвращаем FALSE, если ни мы не обработали ни одно из событий
        }
    }

    return TRUE;
}
