#pragma once

#include <ntddk.h>

#define DRIVER_NAME     L"\\\\.\\HyperEye"
#define DEVICE_NAME     L"\\Device\\HyperEye"
#define DOS_DEVICE_NAME L"\\DosDevices\\HyperEye"

#define HEYE_PING  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)
#define HEYE_ENTER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_NEITHER, FILE_ANY_ACCESS)
#define HEYE_LEAVE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_NEITHER, FILE_ANY_ACCESS)
