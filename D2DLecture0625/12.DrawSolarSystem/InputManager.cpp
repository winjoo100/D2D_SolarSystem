#include "pch.h"
#include "InputManager.h"


inline void GetVkName(const RAWKEYBOARD& kb, WCHAR* out, int cch)
{
    if (cch <= 0 || !out) return;

    // ��ĵ�ڵ�+Ȯ�� �÷��׸� lParam �������� ��ȯ
    UINT scan = kb.MakeCode;
    if (kb.Flags & RI_KEY_E0)
        scan |= 0xE000;            // Ȯ�� Ű(��: ������ Ctrl, ȭ��ǥ ��)

    LONG lParam = static_cast<LONG>(scan << 16);
    if (kb.Flags & RI_KEY_E0)
        lParam |= (1 << 24);

    if (!GetKeyNameTextW(lParam, out, cch))
        swprintf_s(out, cch, L"VK 0x%02X", kb.VKey); // ���� �� VK �ڵ�� ��ü
}


bool InputManager::Initialize(HWND hwnd)
{
#if _USE_RAWINPUTDEVICE
    RAWINPUTDEVICE rid[2] = {};

    // Ű����
    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x06; // Keyboard
    rid[0].dwFlags = 0;
    rid[0].hwndTarget = hwnd;

    // ���콺
    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x02; // Mouse
    rid[1].dwFlags = 0;
    rid[1].hwndTarget = hwnd;

    if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE))) return false;
#endif// _USE_RAWINPUTDEVICE
    return true;
}

bool InputManager::GetKeyPressed(UINT vk)
{
    bool p = m_keyEdge[vk].pressed;
    m_keyEdge[vk].pressed = false;   
    
    return p;
}

bool InputManager::GetKeyDown(UINT vk) const
{
    return m_keyDown[vk];
}


bool InputManager::OnHandleMessage(const MSG& msg)
{
    switch (msg.message)
    {
    case WM_INPUT:
    {
        HandleRawInput(msg.lParam);
    }
    break;

    case WM_KEYDOWN:
    {
        HandleMsgKeyDown(msg.wParam, msg.lParam);
    }
    break;

    case WM_KEYUP:
    {
        HandleMsgKeyUp(msg.wParam, msg.lParam);
    }
    break;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:  
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    {
        HandleMsgMouse(msg); 
    }
    break;

    default:
        return false; // Unhandled message
    }

    return true; 
}

void InputManager::HandleMsgKeyDown(WPARAM wParam, LPARAM lParam)
{
    assert(wParam < 256);

    bool wasDown = (lParam & (1 << 30)) != 0;
    m_keyDown[wParam] = true;

    if (!wasDown)
    {
        m_keyEdge[wParam].pressed = true;   // ó�� ������ ���� true
    }

    m_keyEdge[wParam].released = false; // ���� �����̹Ƿ� released = false

#ifdef _VERBOSE_KEYBOARD
    char buf[128];
    std::snprintf(buf, sizeof(buf),
        "KEY DOWN  VK=0x%02X  (wasDown=%d)\n",
        static_cast<unsigned int>(wParam), wasDown);  

    std::cout << buf;
#endif
}

void InputManager::HandleMsgKeyUp(WPARAM wParam, LPARAM lParam)
{
    assert(wParam < 256);

    m_keyDown[wParam] = false; // ���� Ű�� ������ ������ ���
    m_keyEdge[wParam].released = true; // Ű�� ���������� ���

#ifdef _VERBOSE_KEYBOARD
    char buf[64];
    std::snprintf(buf, sizeof(buf),
        "KEY UP    VK=0x%02X\n",
        static_cast<unsigned int>(wParam));

    std::cout << buf;
#endif
}

void InputManager::HandleMsgMouse(const MSG& msg)
{
    int x = GetXFromLParam(msg.lParam);
    int y = GetYFromLParam(msg.lParam); // [���� ����]

    m_CurMouse.pos = { x, y };

    if (msg.message == WM_LBUTTONDOWN)
    {
        m_CurMouse.leftPressed = true;
        SetCapture(msg.hwnd);
    }
    else if (msg.message == WM_RBUTTONDOWN)
    {
        m_CurMouse.rightPressed = true;
        SetCapture(msg.hwnd);
    }
    else if (msg.message == WM_LBUTTONUP)
    {
        m_CurMouse.leftPressed = false;
        ReleaseCapture();
    }
    else if (msg.message == WM_RBUTTONUP)
    {
        m_CurMouse.rightPressed = false;
        ReleaseCapture();
    }
   
#ifdef _VERBOSE_MOUSE
    char buf[128];
    std::snprintf(buf, sizeof(buf),
        "MOUSE (%d,%d)  left=%d right=%d\n",
        x, y, m_CurMouse.leftPressed, m_CurMouse.rightPressed);

    std::cout << buf;
#endif
}

void InputManager::HandleRawInput(LPARAM lParam)
{
    UINT dwSize = 0;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

    if (dwSize == 0)
    {
        // ���� ó��: dwSize�� 0�� ���� �߸��� �Է��̰ų� �������� �ʴ� �Է��� �� ����
        if (GetLastError() != ERROR_SUCCESS)
        {
            std::cerr << "GetRawInputData failed with error: " << GetLastError() << std::endl;
        }
        return;
    }
        
    std::unique_ptr<BYTE[]> lpb(new BYTE[dwSize]);
    
    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.get(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
    {
        std::cerr << "GetRawInputData failed to read data." << std::endl;
        return;
    }

    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.get());

    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        HandleKeyboardInput(*raw);
    }
    else if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        HandleMouseInput(*raw);
    }
}

void InputManager::HandleKeyboardInput(RAWINPUT& raw)
{
    RAWKEYBOARD& kb = raw.data.keyboard;
    bool breakCode = (kb.Flags & RI_KEY_BREAK) != 0;   // 1 = KeyUp
    UINT vk = kb.VKey;

    if (vk >= 0xFF)  return;      // 0xFF = �߸��� Ű ��ȣ
       

    KeyEdge& edge = m_keyEdge[vk];
    bool& down = m_keyDown[vk];

    if (breakCode) // KeyUp
    {
        if (true == down) edge.released = true;
        down = false;

        std::cout << "Key Up: VK " << vk << "time"  << std::endl;
    
    }
    else // KeyDown
    {
        if (false == down) edge.pressed = true;
        down = true;

        std::cout << "Key Down: VK " << vk << std::endl;
    }
}

void InputManager::HandleMouseInput(RAWINPUT& raw)
{
    // ���� ����!
}
