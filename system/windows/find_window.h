/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file find_window.h
* \author �콭��Ĵ�Ϻ
* \date ʮһ�� 2021
*
*  ���ڱ�������
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

				// STL������
			public:
				// Z��Ӷ����׿�ʼ���������ָ��window���ָ����window���²㴰�ڿ�ʼ����
				iterator begin(const Window* window = nullptr);
				iterator end();

				// Z��ӵ׵�����ʼ���������ָ��window���ָ����window���ϲ㴰�ڿ�ʼ����
				iterator rbegin(const Window* window = nullptr);
				iterator rend();
			};
		}
	}
}
#endif // !__DAXIA_SYSTEM_WINDOWS_FIND_WINDOW_H
#endif // !_WIN32