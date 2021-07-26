#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include "process.h"
#include "path.h"

namespace daxia
{
	namespace win32
	{
		Process::Process()
		{
			handle_ = ::GetCurrentProcess();
		}

		Process::Process(unsigned long id)
		{
			handle_ = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);

			// 尝试开启权限
			if (handle_ == nullptr)
			{
				Process process;
				auto token = process.GetAccessToken();
				token->EnablePrivilege(SE_DEBUG_NAME, true);
				handle_ = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
			}
		}

		Process::~Process()
		{
			if (handle_ != nullptr)
			{
				::CloseHandle(handle_);
				handle_ = nullptr;
			}
		}

		bool Process::Kill()
		{
			if (::TerminateProcess(handle_, 0))
			{
				::CloseHandle(handle_);
				handle_ = nullptr;

				return true;
			}

			return false;
		}

		unsigned long Process::GetId() const
		{
			return ::GetProcessId(handle_);
		}

		void* Process::GetHandle() const
		{
			return handle_;
		}

		daxia::tstring Process::GetName() const
		{
			daxia::tstring name;

			HANDLE  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetId());
			if (hSnapshot == INVALID_HANDLE_VALUE) return name;

			MODULEENTRY32 me;
			me.dwSize = sizeof(me);
			if (::Module32FirstW(hSnapshot, &me))
			{
				name = me.szModule;
			}

			::CloseHandle(hSnapshot);

			return name;
		}

		daxia::tstring Process::GetPath() const
		{
			daxia::tstring path;

			HANDLE  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetId());
			if (hSnapshot == INVALID_HANDLE_VALUE) return path;

			MODULEENTRY32 me;
			me.dwSize = sizeof(me);
			if (::Module32FirstW(hSnapshot, &me))
			{
				path = me.szExePath;
			}

			::CloseHandle(hSnapshot);

			return path;
		}

		daxia::tstring Process::GetDirectory() const
		{
			daxia::tstring dir;
			::GetCurrentDirectoryW(MAX_PATH, dir.GetBuffer(MAX_PATH));
			dir.ReleaseBuffer();

			return dir;
		}

		std::shared_ptr<AccessToken> Process::GetAccessToken()
		{
			if (!token_)
			{
				token_ = std::shared_ptr<AccessToken>(new AccessToken(handle_));
				if (!*token_)
				{
					token_.reset();
				}
			}

			return token_;
		}

		void* Process::LoadMemLibrary(const char* data, size_t len) const
		{
			unsigned long size = getImageSize(data);

			// 分配内存
			char* address = (char*)::VirtualAllocEx(handle_, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (address == nullptr) return nullptr;
			memset(address, 0, size);

			// 将内存DLL数据按映像对齐大小（SectionAlignment）映射到刚刚申请的内存中
			mapImage(data, len, address, size);
			
			// 修改PE文件的重定位表信息
			adjustRelocation(address);

			// 填写PE文件的导入表信息
			adjustImport(address);

			// 修改PE文件的加载基址IMAGE_NT_HEADERS.OptionalHeader.ImageBase
			setImageBase(address);

			// 调用DLL的入口函数DllMain,函数地址即为PE文件的入口点AddressOfEntryPoint
			callDllMain(address,DLL_PROCESS_ATTACH);
		
			return address;
		}

		FARPROC Process::GetMemProcAddress(void* address, const char* name) const
		{
			// 获取Dos头
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)address;
			// 获取NT头
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);

			if (ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
				ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
				return NULL;

			DWORD offsetStart = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
			DWORD size = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

			PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((const char*)address + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

			int base = exportDirectory->Base;
			int numberOfFunctions = exportDirectory->NumberOfFunctions;
			int numberOfNames = exportDirectory->NumberOfNames; //<= iNumberOfFunctions

			LPDWORD addressOfFunctions = (LPDWORD)((const char*)address + exportDirectory->AddressOfFunctions);
			LPWORD addressofOrdinals = (LPWORD)((const char*)address + exportDirectory->AddressOfNameOrdinals);
			LPDWORD addressofNames = (LPDWORD)((const char*)address + exportDirectory->AddressOfNames);

			int ordinal = -1;
			if (((size_t)name & 0xFFFF0000) == 0) //IT IS A ORDINAL!
			{
				ordinal = (size_t)name & 0x0000FFFF - base;
			}
			else //use name
			{
				int found = -1;
				for (int i = 0; i < numberOfNames; i++)
				{
					const char* pName = (const char*)address + addressofNames[i];
					if (strcmp(pName, name) == 0)
					{
						found = i; break;
					}
				}

				if (found >= 0)
				{
					ordinal = (int)(addressofOrdinals[found]);
				}
			}

			if (ordinal < 0 || ordinal >= numberOfFunctions) return NULL;
			else
			{
				DWORD functionOffset = addressOfFunctions[ordinal];
				if (functionOffset > offsetStart && functionOffset < (offsetStart + size))//maybe Export Forwarding
					return NULL;
				else return (FARPROC)((const char*)address + functionOffset);

			}

		}

		FARPROC Process::GetMemProcAddress(void* address, int order) const
		{
			return GetMemProcAddress(address, (const char*)MAKELONG(order, 0));
		}

		void Process::FreeMemLibrary(void* address) const
		{
			callDllMain((char*)address, DLL_PROCESS_DETACH);
			::VirtualFreeEx(handle_, address, 0, MEM_RELEASE);
		}

		bool Process::RevertToSelf()
		{
			return ::RevertToSelf() != FALSE;
		}

		unsigned long Process::getImageSize(const char* imageData) const
		{
			// 获取Dos头
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageData;
			if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			{
				// 不是PE文件
				return 0;
			}

			// 获取NT头
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);
			if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
			{
				// 不是PE文件
				return 0;
			}

			// 获取文件映像大小
			return ntHeader->OptionalHeader.SizeOfImage;
		}

		void Process::mapImage(const char* imageData, size_t len, char* address, size_t size) const
		{
			// 获取Dos头
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageData;
			// 获取NT头
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);
			// 获取所有头部+区段表的大小
			DWORD headerSize = ntHeader->OptionalHeader.SizeOfHeaders;
			// 获取区段数量
			WORD sectionsNum = ntHeader->FileHeader.NumberOfSections;

			// 获取区段表数组的首元素
			PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeader);

			// 将头部（包括区段表）拷贝到内存
			memcpy_s(address, size, imageData, headerSize);

			// 加载所有区段
			for (WORD i = 0; i < sectionsNum; i++)
			{
				//过滤掉无效区段
				if (0 == section->VirtualAddress || 0 == section->SizeOfRawData)
				{
					section++;
					continue;
				}

				// 获取区段在文件中的位置
				const char* src = imageData + section->PointerToRawData;
				// 获取区段映射到内存中的位置
				char* dest = address + section->VirtualAddress;
				// 获取区段在文件中的大小
				DWORD rawDataSize = section->SizeOfRawData;
				// 将区段数据拷贝到内存中
				memcpy_s(dest, size - section->VirtualAddress, src, rawDataSize);

				// 获取下一个区段头（属性）
				section++;
			}
		}

		void Process::adjustRelocation(char* address) const
		{
			// 获取Dos头
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)address;
			// 获取NT头
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);
			// 获取重定位表的地址
			PIMAGE_BASE_RELOCATION reloc = (PIMAGE_BASE_RELOCATION)((const char*)dosHeader +
				ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

			// 判断是否有重定位表
			if ((const char*)reloc == (const char*)dosHeader)
			{
				// 没有重定位表
				return;
			}

			// 开始修复重定位
			while (reloc->VirtualAddress != 0 && reloc->SizeOfBlock != 0)
			{
				// 计算本区域（每一个描述了4KB大小的区域的重定位信息）需要修正的重定位项的数量
				int relocNum = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

				for (int i = 0; i < relocNum; i++)
				{
					// 获取IMAGE_BASE_RELOCATION结构后面的数据的地址
					WORD* relocData = (WORD*)((const char*)reloc + sizeof(IMAGE_BASE_RELOCATION));

					// 每个WORD由两部分组成，高4位指出了重定位的类型，WINNT.H中的一系列IMAGE_REL_BASED_xxx定义了重定位类型的取值。
					// 大部分重定位属性值都是0x3
					// 低12位是相对于IMAGE_BASE_RELOCATION中第一个元素VirtualAddress描述位置的偏移
					// 找出需要修正的地址
					WORD type = relocData[i] >> 12;
					if (IMAGE_REL_BASED_DIR64 == type || IMAGE_REL_BASED_HIGHLOW == type)
					{
						// 获取需要修正数据的地址,    按位与计算优先级比加减乘除低
						char** relocAddress = (char**)((const char*)dosHeader + reloc->VirtualAddress + (relocData[i] & 0x0FFF));

						// 进行修改

						*relocAddress += (unsigned long long)((const char*)dosHeader - ntHeader->OptionalHeader.ImageBase);;
					}
				}

				// 下一个重定位块
				reloc = (PIMAGE_BASE_RELOCATION)((const char*)reloc + reloc->SizeOfBlock);
			}
		}

		void Process::adjustImport(char* address) const
		{
			// 获取Dos头
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)address;
			// 获取NT头
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);
			// 获取导入表地址
			PIMAGE_IMPORT_DESCRIPTOR import = (PIMAGE_IMPORT_DESCRIPTOR)((const char*)dosHeader +
				ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

			// 循环遍历导入表
			while (import->Name)
			{
				// 获取导入表中的Dll名称
				const char* name = (const char*)dosHeader + import->Name;
				// 检索Dll模块获取模块句柄
				HMODULE module = ::GetModuleHandleA(name);
				// 获取失败
				if (NULL == module)
				{
					//加载Dll模块获取模块句柄
					module = ::LoadLibraryA(name);
					//加载失败
					if (NULL == module)
					{
						import++;
						continue;
					}
				}

				// 获取IAT
				PIMAGE_THUNK_DATA iat = (PIMAGE_THUNK_DATA)((const char*)dosHeader + import->FirstThunk);

				FARPROC func = NULL;
				// 遍历IAT中函数
				while (iat->u1.Ordinal)
				{
					// 判断导入的函数是名称导入还是序号导入
					// 判断最高位是否为1，如果是1那么是序号导入
					if (iat->u1.Ordinal >> (sizeof(iat->u1.Ordinal) * 8 - 1))
					{
						// 获取函数地址
						auto temp = iat->u1.Ordinal << 1;
						temp = temp >> 1;
						func = GetProcAddress(module, (LPCSTR)(temp));
					}
					// 名称导入
					else
					{
						// 获取IMAGE_IMPORT_BY_NAME结构
						PIMAGE_IMPORT_BY_NAME ImportName = (PIMAGE_IMPORT_BY_NAME)((const char*)dosHeader + iat->u1.AddressOfData);
						// 获取函数地址
						func = GetProcAddress(module, (LPCSTR)ImportName->Name);
					}
					// 将函数地址填入到IAT中
					iat->u1.Function = (decltype(iat->u1.Function))func;
					iat++;
				}
				import++;
			}
		}

		void Process::setImageBase(char* address) const
		{
			// 获取Dos头
			PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)address;
			// 获取NT头
			PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((const char*)pDosHeader + pDosHeader->e_lfanew);
			// 修改默认加载基址
			pNtHeader->OptionalHeader.ImageBase = (decltype(pNtHeader->OptionalHeader.ImageBase))address;
		}

		void Process::callDllMain(char* address, int reason) const
		{
			typedef BOOL(APIENTRY *DllMain)(HMODULE, DWORD, LPVOID);

			//获取Dos头
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)address;
			//获取NT头
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);


			DllMain main = (DllMain)((const char*)dosHeader + ntHeader->OptionalHeader.AddressOfEntryPoint);

			main((HINSTANCE)address, reason, NULL);
		}

	}
}