#include "CommonPCH.h"
#include "Common/Application/WindowHelper.h"
#include "Common/Application/IApplication.h"
#include "Common/Util/Utf8.h"
#include "Common/Log/Log.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

typedef std::function< IApplication*(const HWND hWnd)> TCreateFunction;

const int WindowHelper(
   const TApplicationFactory& pApplicationFactory,
   const IApplicationParam& applicationParam,
   HINSTANCE hInstance,
   const std::string& applicationName,
   const int nCmdShow
   )
{
   const std::wstring className(Utf8::Utf8ToUtf16( applicationName + std::string("Class")));
   const std::wstring name(Utf8::Utf8ToUtf16( applicationName ));

   // Register class
   {
      WNDCLASSEXW wcex = {};
      wcex.cbSize = sizeof(WNDCLASSEXW);
      wcex.style = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc = WndProc;
      wcex.hInstance = hInstance;
      wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");
      wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
      wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
      wcex.lpszClassName = className.c_str();
      wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
      if (!RegisterClassExW(&wcex))
      {
         return -1;
      }
   }

   // Create window
   {
      RECT rc = { 0, 0, static_cast<LONG>(applicationParam.m_width), static_cast<LONG>(applicationParam.m_height) };

      HWND hwnd = 0;
      TCreateFunction createFunction = [=](const HWND hWndParam)
      {
         return pApplicationFactory(hWndParam, applicationParam);
      };
      DWORD dwStyle = applicationParam.m_bFullScreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
      AdjustWindowRect(&rc, dwStyle, FALSE);
      hwnd = CreateWindowExW(applicationParam.m_bFullScreen ? WS_EX_TOPMOST : 0, className.c_str(), name.c_str(), dwStyle,
         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, 
         nullptr, hInstance, &createFunction);

      if (!hwnd)
      {
         return -1;
      }

      ShowWindow(hwnd, applicationParam.m_bFullScreen ? SW_SHOWMAXIMIZED : nCmdShow);
   }

   return 0;
}

// Windows procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   auto pApplication = reinterpret_cast<IApplication*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
   //LOG_MESSAGE_DEBUG("WndProc message:%d wParam:%p lParam:%p pApplication:%p", message, wParam, lParam, pApplication);

   switch (message)
   {
   case WM_CREATE:
      {
         auto pData = (LPCREATESTRUCTA)(lParam);
         TCreateFunction* pCreateFunction = pData ? (TCreateFunction*)(pData->lpCreateParams) : nullptr;
         if (pCreateFunction)
         {
            auto pApplicationNew = (*pCreateFunction)(hWnd);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pApplicationNew);
         }
      }
      break;
   case WM_PAINT:
      if ((nullptr != pApplication) && (true == pApplication->GetInSizemove()))
      {
         pApplication->Update();
      }
      else
      {
         PAINTSTRUCT ps;
         (void)BeginPaint(hWnd, &ps);
         EndPaint(hWnd, &ps);
      }
      break;

   case WM_MOVE:
      if (nullptr != pApplication)
      {
         pApplication->OnWindowMoved();
      }
      break;

   case WM_SIZE:
      if(nullptr != pApplication)
      {
         if (wParam == SIZE_MINIMIZED)
         {
            if (false == pApplication->GetMinimized())
            {
               pApplication->SetMinimized(true);
               pApplication->OnSuspending();
            }
         }
         else if (true == pApplication->GetMinimized())
         {
            pApplication->SetMinimized(false);
            pApplication->OnResuming();
         }
         else if (false == pApplication->GetInSizemove())
         {
            pApplication->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
         }
      }
      break;
   case WM_ENTERSIZEMOVE:
      if(nullptr != pApplication)
      {
         pApplication->SetInSizemove(true);
      }
      break;
    case WM_EXITSIZEMOVE:
      if(nullptr != pApplication)
      {
         pApplication->SetInSizemove(false);
         RECT rc;
         GetClientRect(hWnd, &rc);
         pApplication->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
      }
      break;
    case WM_GETMINMAXINFO:
        if (lParam)
        {
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    case WM_ACTIVATEAPP:
       if (nullptr != pApplication)
       {
          if (wParam)
          {
             pApplication->OnActivated();
          }
          else
          {
             pApplication->OnDeactivated();
          }
       }
       break;

   case WM_POWERBROADCAST:
      switch (wParam)
      {
      default:
         break;
      case PBT_APMQUERYSUSPEND:
         if (nullptr != pApplication)
         {
            pApplication->OnSuspending();
         }
         return TRUE;
      case PBT_APMRESUMESUSPEND:
         if ((nullptr != pApplication) && (false == pApplication->GetMinimized()))
         {
            pApplication->OnResuming();
         }
         return TRUE;
      }
      break;

   case WM_DESTROY:
      {
         //WM_QUIT is never sent to window, but you can pull it out of the GetMessage/PeekMessage
         //PostQuitMessage(0);
         if (nullptr != pApplication)
         {
             pApplication->Destroy(0);
         }
         SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)0);
      }
      break;
    case WM_SYSKEYDOWN:
      if ((wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) && (nullptr != pApplication))
      {
         // Implements the classic ALT+ENTER fullscreen toggle
         if (pApplication->GetFullScreen())
         {
            pApplication->SetFullScreen(false);
            SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

            const int width = pApplication ? pApplication->GetDefaultWidth() : 800;
            const int height = pApplication ? pApplication->GetDefaultHeight() : 600;

            ShowWindow(hWnd, SW_SHOWNORMAL);
            SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
         }
         else
         {
            pApplication->SetFullScreen(true);
            SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP);
            SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

            SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            ShowWindow(hWnd, SW_SHOWMAXIMIZED);
         }
      }
      break;

    case WM_MENUCHAR:
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
