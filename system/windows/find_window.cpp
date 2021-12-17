#ifdef _WIN32
#include <windows.h>
#include "find_window.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{

			EnumWindow::EnumWindow()
			{

			}

			EnumWindow::~EnumWindow()
			{

			}

			EnumWindow::iterator::iterator()
			{

			}

			EnumWindow::iterator::~iterator()
			{

			}

			EnumWindow::iterator::iterator(unsigned int cmd, std::shared_ptr<Window> window)
				: cmd_(cmd)
				, window_(window)
			{

			}

			EnumWindow::iterator& EnumWindow::iterator::operator++()
			{
				if (!window_) return *this;

				HWND h = NULL;
				switch (cmd_)
				{
				case GW_HWNDNEXT:
				case GW_HWNDPREV:
					h = ::GetWindow(window_->GetHandle(), cmd_);
					if (h)
					{
						window_ = std::shared_ptr<Window>(new Window(h));
					}
					else
					{
						window_.reset();
					}
					break;
				default:
					window_.reset();
					break;
				}

				return *this;
			}

			const std::shared_ptr<daxia::system::windows::Window> EnumWindow::iterator::operator->() const
			{
				return window_;
			}

			std::shared_ptr<daxia::system::windows::Window> EnumWindow::iterator::operator->()
			{
				return window_;
			}

			const std::shared_ptr<daxia::system::windows::Window> EnumWindow::iterator::operator*() const
			{
				return window_;
			}

			std::shared_ptr<daxia::system::windows::Window> EnumWindow::iterator::operator*()
			{
				return window_;
			}

			EnumWindow::iterator& EnumWindow::iterator::operator=(const iterator& iter)
			{
				window_ = iter.window_;

				return *this;
			}

			bool EnumWindow::iterator::operator==(const iterator& iter) const
			{
				if ((!window_ && !iter.window_)
					|| window_ && iter.window_ && window_->GetHandle() == iter.window_->GetHandle() )
				{
					return true;
				}

				return false;
			}

			bool EnumWindow::iterator::operator!=(const iterator& iter) const
			{
				return !(*this == iter);
			}

			EnumWindow::iterator EnumWindow::begin(const Window* window /* = nullptr */)
			{
				HWND h = NULL;
				if (window)
				{
					h = GetWindow(window->GetHandle(), GW_HWNDNEXT);
				}
				else
				{
					h = GetTopWindow(NULL);
				}

				return h ? EnumWindow::iterator(GW_HWNDNEXT, std::shared_ptr<Window>(new Window(h))) : end();
			}

			EnumWindow::iterator EnumWindow::end()
			{
				return EnumWindow::iterator();
			}

			EnumWindow::iterator EnumWindow::rbegin(const Window* window /*= nullptr*/)
			{
				HWND h = NULL;
				if (window)
				{
					h = GetWindow(window->GetHandle(), GW_HWNDPREV);
				}
				else
				{
					h = GetTopWindow(NULL);
					h = GetWindow(h, GW_HWNDLAST);
				}

				return h ? EnumWindow::iterator(GW_HWNDPREV, std::shared_ptr<Window>(new Window(h))) : end();
			}

			EnumWindow::iterator EnumWindow::rend()
			{
				return EnumWindow::iterator();
			}

		}
	}
}
#endif // !_WIN32