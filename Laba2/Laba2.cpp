// Laba2.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Laba2.h"
#include <commdlg.h>
#include <richedit.h>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

OPENFILENAME ofn; //имя файла в файловой системе
HDC hdc;          //указатель на устройство
HDC hmemdc;       //указатель на память устройство вывода
HBITMAP hbmp;     //битовая карта для указателя для устройства вывода
RECT rect;        //прямоуголник рабочей зоны
HBRUSH hb1;       //кисть для закрашивания
static char szFileName[_MAX_PATH]; //буфер хранения имени файла
//структура для сообщения о выделении строки в текстовом поле
static CHARRANGE szSel;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LABA2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABA2));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABA2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LABA2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
        {
            hdc = GetDC(hWnd);  //Получить контекст окна на экране
            hmemdc = CreateCompatibleDC(hdc);   //Создать контекст памяти совместимый с контекстом экрана 
            GetClientRect(hWnd, &rect); //Получить контекст клиентской области окна
            hbmp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top); //Создать БитМап (холст) для вывода графики в память машины
            SelectObject(hmemdc, hbmp); //Присоединить холст к контексту памяти
            hb1 = CreateSolidBrush(RGB(255, 255, 255)); //Создать белую кисть
            FillRect(hmemdc, &rect, hb1);   //Залить холст белым цветом
            ReleaseDC(hWnd, hdc);   //Убрать из памяти ненужные объекты: контекст экрана и белую кисть
            DeleteObject(hb1);
            //Инициализация структуры окна выбора файлов:
            ofn.lStructSize = sizeof(OPENFILENAME);	//размер структуры
            ofn.hwndOwner = hWnd;	//дискриптор окна владельца
            ofn.hInstance = NULL;	//хендел потока родителя
            ofn.lpstrFilter = L"Text Files (*.TXT)\0*.txt\0";	//строка допустимых фильтров выбора файлов
            ofn.lpstrCustomFilter = NULL;	//строка с выбранным фильтром
            ofn.nMaxCustFilter = 0;	//длина строки предыдущего поля
            ofn.nFilterIndex = 0;	//индекс выбранной строки фильтра
            ofn.lpstrFile = NULL;	//строка с выбранным файлом и путем к нему
            ofn.nMaxFile = _MAX_PATH;	//максимально допустимая длина предыдущей строки
            ofn.lpstrFileTitle = NULL;	//строка с именем выбранного файла
            ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;  //макс.допустимая  длина пред. строки
            ofn.lpstrInitialDir = NULL;	//имя каталога
            ofn.lpstrTitle = NULL;	//строка с заголовком диалогового окна
            ofn.Flags = 0;	//Флаги диалогового окна
            ofn.nFileOffset = 0;	//Смещение имени файла в строке
            ofn.nFileExtension = 0;	//Смещение расширения файла в строке
            ofn.lpstrDefExt = L"txt";	//расширение выбираемых файлов по-умолчанию
            ofn.lCustData = 0L;	//данные для обработки
            ofn.lpfnHook = NULL;	//указатель для функции обработки (ФОВ)
            ofn.lpTemplateName = NULL;	//имя шаблона диалогового окна

        //инициализация структуры окна редактирования строки для выделения в нем текста
            szSel.cpMin = 0;	//позиция первого выделяемого символа в строке
            szSel.cpMax = -1;	//выделить все символы
        }
        break;
        //Нажатие на правую кнопку мыши
    case WM_RBUTTONDOWN:
        {
            //указываем положение буферов для получения результатов
            ofn.lpstrFile = (LPWSTR)szFileName;
            //отображать все файлы
            ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
            //открыть диалог по выбору файлов			
            GetOpenFileName(&ofn);
            //отобразить имя файла на холсте
            TextOut(hmemdc, rect.right/2, rect.bottom/2, (LPCWSTR)szFileName, 100);
            //запрос на перересовку
            InvalidateRect(hWnd, 0, 0);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            //Копирование изображения из памяти на экран
            BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hmemdc, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
