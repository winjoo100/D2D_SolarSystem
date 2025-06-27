#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// Windows
#include <windows.h>
#include <objbase.h> // CoInitializeEx, CoUninitialize

#include <stdio.h>
#include <iostream>

#include <cassert>
#include <exception>
#include <crtdbg.h>

#include <array>
#include <list>
#include <vector>
#include <string>


// COM �� DirectX �������̽�
#include <wrl/client.h>              // ComPtr
#include <d3d11.h>                   // Direct3D 11
#include <dxgi1_6.h>                 // DXGI 1.6 (Windows 10 �̻� �ֽ� ����ü��)
#include <d2d1_3.h>                  // Direct2D 1.3 (ID2D1Factory4)
#include <d2d1_3helper.h>            // D2D1::Helper Ŭ������
#include <dwrite_3.h>                // DirectWrite (�ֽ� �ؽ�Ʈ ����)
#include <wincodec.h>                // WIC (�̹��� �ε�)


#define _USE_MATH_DEFINES
#include <cmath>

#include <stdexcept>

//https://github.com/Microsoft/DirectXTK/wiki/throwIfFailed
namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) : result(hr) {}

        const char* what() const noexcept override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X",
                static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
}

#define _CRTDBG_MAP_ALLOC
