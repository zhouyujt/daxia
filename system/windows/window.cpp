#ifdef _WIN32
#include "window.h"

#define WIDTH(rt)(rt.right - rt.left)
#define HEIGHT(rt)(rt.bottom - rt.top)

namespace daxia
{
	namespace system
	{
		namespace windows
		{
	
			Window::Window(HWND handle)
				: handle_(handle)
			{

			}

			Window::~Window()
			{

			}

			HWND Window::GetHandle() const
			{
				return handle_;
			}

			bool Window::IsVisable() const
			{
				return ::IsWindowVisible(handle_);
			}

			void Window::Print(HDC hdcBlt) const
			{
				HWND h = handle_ ? handle_ : ::GetDesktopWindow();
				::PrintWindow(h, hdcBlt, 0);
			}

			void Window::Bitblt(HDC hdcBlt) const
			{
				RECT rt;
				::GetWindowRect(handle_, &rt);
				HDC hdc = ::GetDC(handle_);

				::BitBlt(hdcBlt, rt.left, rt.top, WIDTH(rt), HEIGHT(rt), hdc, 0, 0, SRCCOPY);

				::ReleaseDC(handle_, hdc);
			}
		}
	}
}
#endif // !_WIN32