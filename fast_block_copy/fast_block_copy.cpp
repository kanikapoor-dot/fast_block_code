// fast_block_copy.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

static void PrintLastErrorW(const wchar_t* message) {
	std::wcerr << message << L" Error: " << GetLastError() << L"\n";
}

bool BlockCopy(const std::wstring& srcPath, const std::wstring& dstPath)
{
	//Source Volume or snapshot
	HANDLE hSrc = CreateFileW(
		srcPath.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		nullptr
	);

	if (hSrc == INVALID_HANDLE_VALUE)
	{
		PrintLastErrorW(L"Failed to open source path device.");
		return false;
	}

	// Source Destination

	HANDLE hDst = CreateFileW(
		dstPath.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		nullptr
	);

	if (hDst == INVALID_HANDLE_VALUE)
	{
		PrintLastErrorW(L"Failed to open Destination path");
		CloseHandle(hSrc);
		return false;
	}

	//using 4MB buffer
	const DWORD BUFFER_SIZE = 4 * 1024 * 1024;
	std::vector<char> buffer(BUFFER_SIZE);

	while (true)
	{
		DWORD bytesRead = 0;
		if (!ReadFile(hSrc, buffer.data(), BUFFER_SIZE, &bytesRead, nullptr))
		{
			DWORD error = GetLastError();
			if(error == ERROR_HANDLE_EOF || bytesRead == 0)
			{
				break;
			}
			PrintLastErrorW(L"ReadFile failed.");
			CloseHandle(hSrc);
			CloseHandle(hDst);
			return false;
		}

		if(bytesRead == 0)
		{
			break;
		}

		DWORD bytesWritten = 0;
		if (!WriteFile(hDst, buffer.data(), bytesRead, &bytesWritten, nullptr))
		{
			PrintLastErrorW(L"WriteFile failed");
			CloseHandle(hSrc);
			CloseHandle(hDst);
			return false;
		}

		if (bytesWritten != bytesRead)
		{
			std::wcerr << L"WriteFile wrote fewer bytes than read.\n";
			CloseHandle(hSrc);
			CloseHandle(hDst);
			return false;
		}
	}

	if (FlushFileBuffers(hDst) == FALSE)
	{
		PrintLastErrorW(L"FlushFileBuffers failed.");
		CloseHandle(hSrc);
		CloseHandle(hDst);
		return false;
	}

	CloseHandle(hSrc);
	CloseHandle(hDst);
	return true;
}

int wmain(int argc, wchar_t* argv[])
{
	if (argc < 5)
	{
		std::wcout <<
			L"Usaga:\n"
			L"fast_block_copy.exe -svp <source> -tpp <destination>\n\n";

		return 1;
	}

	std::wstring srcPath, dstPath;

	for (int i = 1; i + 1 < argc; ++i)
	{
		if (_wcsicmp(argv[i], L"-svp") == 0)
		{
			srcPath = argv[++i];
		}
		else if (_wcsicmp(argv[i], L"-tpp") == 0)
		{
			dstPath = argv[++i];
		}
	}

	if (srcPath.empty() || dstPath.empty())
	{
		std::cerr << "Invalid Arguments.\n";
		return 1;
	}

	bool ok = BlockCopy(srcPath, dstPath);
	std::wcout << (ok ? L"Copy Completed" : L"Copy Failed");
	return ok ? 0 : 1;
}
