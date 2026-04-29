#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>

#ifndef CTL_CODE
#define CTL_CODE(DeviceType, Function, Method, Access) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
#endif

#define Base_code    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2A2, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CR3_code     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2A3, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define Read_code    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2A4, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define Write_code   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2A5, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define Pattern_code CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2A6, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define Mouse_code   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3A1, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _read_mem {
    INT32 ProcessId;
    ULONGLONG Address;
    ULONGLONG Buffer;
    ULONGLONG Size;
} read_mem, * pread;

typedef struct _base_new {
    INT32 process_id;
    ULONGLONG BaseAddress;
} base_new, * pbase_new;

typedef struct _movemouse {
    long x;
    long y;
    unsigned short button_flags;
} movemouse, * pmovemouse;

class DRV {
public:
    static inline HANDLE driver_handle = INVALID_HANDLE_VALUE;
    uint32_t process_id = 0;
    uintptr_t process_base = 0;

    static bool IsValidUserAddress(uintptr_t address, size_t size = 1) {
        constexpr uintptr_t min_user_address = 0x10000;
        constexpr uintptr_t max_user_address = 0x00007FFFFFFFFFFF;

        if (address < min_user_address || address > max_user_address)
            return false;

        if (size == 0)
            return false;

        const uintptr_t end = address + size - 1;
        if (end < address || end > max_user_address)
            return false;

        return true;
    }

    static bool Init() {
        driver_handle = CreateFileW(L"\\\\.\\WinDrvMgr", GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (driver_handle == INVALID_HANDLE_VALUE) {
            std::cout << "[!] Khong the ket noi toi driver (Error: " << GetLastError() << ")" << std::endl;
            return false;
        }
        std::cout << "[+] Da ket noi toi driver thành công!" << std::endl;
        return true;
    }

    bool Attach(const char* process_name) {
        process_id = GetProcessIdByName(process_name);
        if (process_id == 0) return false;

        base_new args = { 0 };
        args.process_id = process_id;
        
        DWORD bytes_returned;
        if (DeviceIoControl(driver_handle, Base_code, &args, sizeof(args), &args, sizeof(args), &bytes_returned, NULL)) {
            process_base = args.BaseAddress;
            return process_base != 0;
        }
        return false;
    }

    uintptr_t GetModuleBase() {
        return process_base;
    }

    template<typename T>
    T rpm(uintptr_t address) {
        T buffer = { 0 };
        ReadRaw(address, &buffer, sizeof(T));
        return buffer;
    }

    bool ReadRaw(uintptr_t address, void* buffer, size_t size) {
        if (driver_handle == INVALID_HANDLE_VALUE || buffer == nullptr)
            return false;

        if (!IsValidUserAddress(address, size))
            return false;

        read_mem args;
        args.ProcessId = process_id;
        args.Address = address;
        args.Buffer = (ULONGLONG)buffer;
        args.Size = size;

        DWORD bytes_returned = 0;
        return DeviceIoControl(driver_handle, Read_code, &args, sizeof(args), &args, sizeof(args), &bytes_returned, NULL) != FALSE;
    }

    bool WriteRaw(uintptr_t address, void* buffer, size_t size) {
        if (driver_handle == INVALID_HANDLE_VALUE || buffer == nullptr)
            return false;

        if (!IsValidUserAddress(address, size))
            return false;

        read_mem args;
        args.ProcessId = process_id;
        args.Address = address;
        args.Buffer = (ULONGLONG)buffer;
        args.Size = size;

        DWORD bytes_returned = 0;
        return DeviceIoControl(driver_handle, Write_code, &args, sizeof(args), &args, sizeof(args), &bytes_returned, NULL) != FALSE;
    }

    bool MouseMove(long x, long y) {
        if (driver_handle == INVALID_HANDLE_VALUE) return false;

        movemouse args;
        args.x = x;
        args.y = y;
        args.button_flags = 0;

        return DeviceIoControl(driver_handle, Mouse_code, &args, sizeof(args), nullptr, 0, NULL, NULL);
    }

private:
    uint32_t GetProcessIdByName(const char* process_name) {
        uint32_t pid = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(entry);
            if (Process32First(snapshot, &entry)) {
                do {
                    if (_stricmp(entry.szExeFile, process_name) == 0) {
                        pid = entry.th32ProcessID;
                        break;
                    }
                } while (Process32Next(snapshot, &entry));
            }
            CloseHandle(snapshot);
        }
        return pid;
    }
};
