#include <tchar.h>
#include <windows.h>
#include <cstdint>
#include <vector>
#include <thread>
#include <iostream>
#include <fstream>

#include "Grayscale.h"

using namespace std;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("WindowsApp");

HWND textField1, textBox1, textField2, textBox2, textField3, textBox3, button1, button2, OKButton, textField4;

GrayscaleParams params[64] = {};
vector<thread> threads;

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
        0,                   /* Extended possibilites for variation */
        szClassName,         /* Classname */
        _T("Konwerter obrazu kolorowego na monochromatyczny"),       /* Title Text */
        WS_MINIMIZEBOX | WS_SYSMENU, /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        500,                 /* The programs width */
        300,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0))
    {
        //TODO liczenie czasu tu
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
        textField1 = CreateWindow("STATIC", "Podaj nazwê pliku", WS_VISIBLE | WS_CHILD,20, 20, 150, 20,hwnd,NULL,NULL,NULL);
        textBox1 = CreateWindow("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE,180, 20, 290, 20,hwnd,NULL,NULL,NULL);
        textField2 = CreateWindow("STATIC", "Podaj liczbê w¹tków", WS_VISIBLE | WS_CHILD,20, 50, 150, 20,hwnd,NULL,NULL,NULL);
        textBox2 = CreateWindow("EDIT", "",WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER, 180, 50, 30, 20,hwnd,NULL,NULL,NULL);
        textField3 = CreateWindow("STATIC", "Czas [s]", WS_BORDER | WS_CHILD | WS_VISIBLE,20, 170, 100, 20,hwnd, NULL, NULL,  NULL );
        textBox3 = CreateWindow("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_READONLY, 140, 170, 100, 20, hwnd, NULL, NULL, NULL);
        button1 = CreateWindow("BUTTON", "u¿yj DLL z ASM",WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON,20, 80, 150, 20,hwnd, (HMENU)1, NULL,NULL);
        button2 = CreateWindow("BUTTON","u¿yj DLL z C++",WS_VISIBLE | WS_CHILD | BS_RADIOBUTTON, 20, 110, 150, 20,hwnd,(HMENU)2,NULL,NULL);
        textField4 = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE, 20, 140, 400, 20, hwnd, NULL, NULL, NULL);
        OKButton = CreateWindow("BUTTON", "OK", WS_VISIBLE | WS_CHILD, 20, 220, 70, 20,hwnd,(HMENU)3,NULL,NULL);                  
        SendMessage(textBox1, EM_LIMITTEXT, 255, 0);
        SendMessage(textBox2, EM_LIMITTEXT, 2, 0);
        SendDlgItemMessage(hwnd, 1, BM_SETCHECK, 1, 0);
        break;
    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case 1:
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
                if(SendDlgItemMessage(hwnd, 1, BM_GETCHECK, 0, 0) == 0)
                    {
                    SendDlgItemMessage(hwnd, 1, BM_SETCHECK, 1, 0);
                    SendDlgItemMessage(hwnd, 2, BM_SETCHECK, 0, 0);
                    }
            }
            break;

        case 2:
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
                if (SendDlgItemMessage(hwnd, 2, BM_GETCHECK, 0, 0) == 0)
                {
                    SendDlgItemMessage(hwnd, 2, BM_SETCHECK, 1, 0);
                    SendDlgItemMessage(hwnd, 1, BM_SETCHECK, 0, 0);
                }
            }
            break;
        case 3:

            int gwtstat1 = 0, gwtstat2 = 0, numberThreads = 0;
            char fileName[255];
            gwtstat1 = GetWindowText(textBox1, fileName, 255);

            char threadsNumber[3];
            gwtstat2 = GetWindowText(textBox2, threadsNumber, 3);

            char time[16];        
            numberThreads = atoi(threadsNumber);

            if (numberThreads < 1)
                numberThreads = 1;
            if (numberThreads > 64)
                numberThreads = 64;


            if (SendDlgItemMessage(hwnd, 1, BM_GETCHECK, 0, 0) == 0)
            {
                runGrayscale(params, fileName, numberThreads, hwnd, false, threads, time, textField4);
                SetWindowText(textBox3, time);
            }
            else
            { 
                runGrayscale(params, fileName, numberThreads, hwnd, true, threads, time, textField4);
                SetWindowText(textBox3, time);
            }
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
