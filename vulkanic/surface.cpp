
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "vulkanic/surface.h"
#include <Windows.h>
#include <algorithm>

namespace vulkaninc
{
#ifdef _WIN32

    struct Surface::NativeWindow
    {
      public:
        NativeWindow(const char * i_title)
        {
            const char wnd_class_name[] = "vulkanic_wnd";

            auto const app_inst = GetModuleHandleA(NULL);

            WNDCLASSEXA wnd_class;
            ZeroMemory(&wnd_class, sizeof(wnd_class));
            wnd_class.cbSize        = sizeof(wnd_class);
            wnd_class.style         = 0;
            wnd_class.lpfnWndProc   = &procedure;
            wnd_class.hInstance     = app_inst;
            wnd_class.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
            wnd_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
            wnd_class.lpszClassName = wnd_class_name;
            RegisterClassExA(&wnd_class);

            auto const returned_handle = CreateWindowExA(
              WS_EX_APPWINDOW,
              wnd_class_name,
              i_title,
              WS_CAPTION | WS_VISIBLE,
              CW_USEDEFAULT,
              CW_USEDEFAULT,
              CW_USEDEFAULT,
              CW_USEDEFAULT,
              NULL,
              NULL,
              app_inst,
              this);

            check(
              IsWindow(returned_handle) && returned_handle == m_handle,
              "CreateWindowExA has failed");
        }

        ~NativeWindow()
        {
            if (m_handle != NULL)
                ::DestroyWindow(m_handle);
        }

        NativeWindow(const NativeWindow &) = delete;

        NativeWindow & operator=(const NativeWindow &) = delete;

        void * handle() const noexcept { return m_handle; }

        vk::Extent2D internal_extent() const noexcept
        {
            RECT rect{};
            if (GetClientRect(m_handle, &rect))
            {
                auto const width  = (std::max)(rect.right - rect.left, LONG(0));
                auto const height = (std::max)(rect.bottom - rect.top, LONG(0));
                return vk::Extent2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
            }
            return vk::Extent2D();
        }

      private:
        static LRESULT CALLBACK procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            switch (uMsg)
            {
            case WM_CREATE:
            {
                auto const create_struct = reinterpret_cast<CREATESTRUCT *>(lParam);
                auto const wnd = reinterpret_cast<NativeWindow *>(create_struct->lpCreateParams);
                wnd->m_handle  = hwnd;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)wnd);
                return 0;
            }

            default:
                return DefWindowProcA(hwnd, uMsg, wParam, lParam);
            }
        }

      private:
        HWND m_handle;
    };

#endif

    Surface::Surface(vk::Instance i_instance, const char * i_title)
        : m_native_window(new NativeWindow(i_title))
    {
#ifdef _WIN32
        vk::Win32SurfaceCreateInfoKHR surface_info;
        surface_info.hwnd      = (HWND)m_native_window->handle();
        surface_info.hinstance = GetModuleHandle(NULL);
        m_surface              = SurfaceHandle(
          i_instance.createWin32SurfaceKHR(surface_info),
          handle_deleters::SurfaceDeleter{i_instance});
#endif
    }

    Surface::~Surface() = default;

    vk::Extent2D Surface::internal_extent() const noexcept
    {
        return m_native_window->internal_extent();
    }


} // namespace vulkaninc
