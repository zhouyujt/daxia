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

			// ���Կ���Ȩ��
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

			// �����ڴ�
			char* address = (char*)::VirtualAllocEx(handle_, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (address == nullptr) return nullptr;
			memset(address, 0, size);

			// ���ڴ�DLL���ݰ�ӳ������С��SectionAlignment��ӳ�䵽�ո�������ڴ���
			mapImage(data, len, address, size);
			
			// �޸�PE�ļ����ض�λ����Ϣ
			adjustRelocation(address);

			// ��дPE�ļ��ĵ������Ϣ
			adjustImport(address);

			// �޸�PE�ļ��ļ��ػ�ַIMAGE_NT_HEADERS.OptionalHeader.ImageBase
			setImageBase(address);

			// ����DLL����ں���DllMain,������ַ��ΪPE�ļ�����ڵ�AddressOfEntryPoint
			callDllMain(address,DLL_PROCESS_ATTACH);
		
			return address;
		}

		FARPROC Process::GetMemProcAddress(void* address, const char* name) const
		{
			// ��ȡDosͷ
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)address;
			// ��ȡNTͷ
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
			// ��ȡDosͷ
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageData;
			if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			{
				// ����PE�ļ�
				return 0;
			}

			// ��ȡNTͷ
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);
			if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
			{
				// ����PE�ļ�
				return 0;
			}

			// ��ȡ�ļ�ӳ���С
			return ntHeader->OptionalHeader.SizeOfImage;
		}

		void Process::mapImage(const char* imageData, size_t len, char* address, size_t size) const
		{
			// ��ȡDosͷ
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageData;
			// ��ȡNTͷ
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);
			// ��ȡ����ͷ��+���α�Ĵ�С
			DWORD headerSize = ntHeader->OptionalHeader.SizeOfHeaders;
			// ��ȡ��������
			WORD sectionsNum = ntHeader->FileHeader.NumberOfSections;

			// ��ȡ���α��������Ԫ��
			PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeader);

			// ��ͷ�����������α��������ڴ�
			memcpy_s(address, size, imageData, headerSize);

			// ������������
			for (WORD i = 0; i < sectionsNum; i++)
			{
				//���˵���Ч����
				if (0 == section->VirtualAddress || 0 == section->SizeOfRawData)
				{
					section++;
					continue;
				}

				// ��ȡ�������ļ��е�λ��
				const char* src = imageData + section->PointerToRawData;
				// ��ȡ����ӳ�䵽�ڴ��е�λ��
				char* dest = address + section->VirtualAddress;
				// ��ȡ�������ļ��еĴ�С
				DWORD rawDataSize = section->SizeOfRawData;
				// ���������ݿ������ڴ���
				memcpy_s(dest, size - section->VirtualAddress, src, rawDataSize);

				// ��ȡ��һ������ͷ�����ԣ�
				section++;
			}
		}

		void Process::adjustRelocation(char* address) const
		{
			// ��ȡDosͷ
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)address;
			// ��ȡNTͷ
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);
			// ��ȡ�ض�λ��ĵ�ַ
			PIMAGE_BASE_RELOCATION reloc = (PIMAGE_BASE_RELOCATION)((const char*)dosHeader +
				ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

			// �ж��Ƿ����ض�λ��
			if ((const char*)reloc == (const char*)dosHeader)
			{
				// û���ض�λ��
				return;
			}

			// ��ʼ�޸��ض�λ
			while (reloc->VirtualAddress != 0 && reloc->SizeOfBlock != 0)
			{
				// ���㱾����ÿһ��������4KB��С��������ض�λ��Ϣ����Ҫ�������ض�λ�������
				int relocNum = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

				for (int i = 0; i < relocNum; i++)
				{
					// ��ȡIMAGE_BASE_RELOCATION�ṹ��������ݵĵ�ַ
					WORD* relocData = (WORD*)((const char*)reloc + sizeof(IMAGE_BASE_RELOCATION));

					// ÿ��WORD����������ɣ���4λָ�����ض�λ�����ͣ�WINNT.H�е�һϵ��IMAGE_REL_BASED_xxx�������ض�λ���͵�ȡֵ��
					// �󲿷��ض�λ����ֵ����0x3
					// ��12λ�������IMAGE_BASE_RELOCATION�е�һ��Ԫ��VirtualAddress����λ�õ�ƫ��
					// �ҳ���Ҫ�����ĵ�ַ
					WORD type = relocData[i] >> 12;
					if (IMAGE_REL_BASED_DIR64 == type || IMAGE_REL_BASED_HIGHLOW == type)
					{
						// ��ȡ��Ҫ�������ݵĵ�ַ,    ��λ��������ȼ��ȼӼ��˳���
						char** relocAddress = (char**)((const char*)dosHeader + reloc->VirtualAddress + (relocData[i] & 0x0FFF));

						// �����޸�

						*relocAddress += (unsigned long long)((const char*)dosHeader - ntHeader->OptionalHeader.ImageBase);;
					}
				}

				// ��һ���ض�λ��
				reloc = (PIMAGE_BASE_RELOCATION)((const char*)reloc + reloc->SizeOfBlock);
			}
		}

		void Process::adjustImport(char* address) const
		{
			// ��ȡDosͷ
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)address;
			// ��ȡNTͷ
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);
			// ��ȡ������ַ
			PIMAGE_IMPORT_DESCRIPTOR import = (PIMAGE_IMPORT_DESCRIPTOR)((const char*)dosHeader +
				ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

			// ѭ�����������
			while (import->Name)
			{
				// ��ȡ������е�Dll����
				const char* name = (const char*)dosHeader + import->Name;
				// ����Dllģ���ȡģ����
				HMODULE module = ::GetModuleHandleA(name);
				// ��ȡʧ��
				if (NULL == module)
				{
					//����Dllģ���ȡģ����
					module = ::LoadLibraryA(name);
					//����ʧ��
					if (NULL == module)
					{
						import++;
						continue;
					}
				}

				// ��ȡIAT
				PIMAGE_THUNK_DATA iat = (PIMAGE_THUNK_DATA)((const char*)dosHeader + import->FirstThunk);

				FARPROC func = NULL;
				// ����IAT�к���
				while (iat->u1.Ordinal)
				{
					// �жϵ���ĺ��������Ƶ��뻹����ŵ���
					// �ж����λ�Ƿ�Ϊ1�������1��ô����ŵ���
					if (iat->u1.Ordinal >> (sizeof(iat->u1.Ordinal) * 8 - 1))
					{
						// ��ȡ������ַ
						auto temp = iat->u1.Ordinal << 1;
						temp = temp >> 1;
						func = GetProcAddress(module, (LPCSTR)(temp));
					}
					// ���Ƶ���
					else
					{
						// ��ȡIMAGE_IMPORT_BY_NAME�ṹ
						PIMAGE_IMPORT_BY_NAME ImportName = (PIMAGE_IMPORT_BY_NAME)((const char*)dosHeader + iat->u1.AddressOfData);
						// ��ȡ������ַ
						func = GetProcAddress(module, (LPCSTR)ImportName->Name);
					}
					// ��������ַ���뵽IAT��
					iat->u1.Function = (decltype(iat->u1.Function))func;
					iat++;
				}
				import++;
			}
		}

		void Process::setImageBase(char* address) const
		{
			// ��ȡDosͷ
			PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)address;
			// ��ȡNTͷ
			PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((const char*)pDosHeader + pDosHeader->e_lfanew);
			// �޸�Ĭ�ϼ��ػ�ַ
			pNtHeader->OptionalHeader.ImageBase = (decltype(pNtHeader->OptionalHeader.ImageBase))address;
		}

		void Process::callDllMain(char* address, int reason) const
		{
			typedef BOOL(APIENTRY *DllMain)(HMODULE, DWORD, LPVOID);

			//��ȡDosͷ
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)address;
			//��ȡNTͷ
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((const char*)dosHeader + dosHeader->e_lfanew);


			DllMain main = (DllMain)((const char*)dosHeader + ntHeader->OptionalHeader.AddressOfEntryPoint);

			main((HINSTANCE)address, reason, NULL);
		}

	}
}