/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file find_window.h
* \author 漓江里的大虾
* \date 十一月 2021
*
*  窗口遍历查找
*
*/
#ifdef _WIN32
#ifndef __DAXIA_SYSTEM_WINDOWS_FIND_WINDOW_H
#define __DAXIA_SYSTEM_WINDOWS_FIND_WINDOW_H
#include <memory>
#include "window.h"
namespace daxia
{
	namespace system
	{
		namespace windows
		{
			class EnumWindow
			{
			public:
				EnumWindow();
				~EnumWindow();
			public:
				class iterator
				{
					friend EnumWindow;
				public:
					iterator();
					~iterator();
				private:
					iterator(unsigned int cmd, std::shared_ptr<Window> window);
				public:
					iterator& operator++();
					bool operator==(const iterator& iter) const;
					bool operator!=(const iterator& iter) const;
					const std::shared_ptr<Window> operator->() const;
					const std::shared_ptr<Window> operator*() const;
					std::shared_ptr<Window> operator->();
					std::shared_ptr<Window> operator*();
					iterator& operator=(const iterator& iter);
				private:
					unsigned int cmd_;
					std::shared_ptr<Window> window_;
				};

				// STL风格遍历
			public:
				// Z序从顶到底开始遍历，如果指定window则从指定的window的下层窗口开始遍历
				iterator begin(const Window* window = nullptr);
				iterator end();

				// Z序从底到顶开始遍历，如果指定window则从指定的window的上层窗口开始遍历
				iterator rbegin(const Window* window = nullptr);
				iterator rend();
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_FIND_WINDOW_H
#endif // !_WIN32