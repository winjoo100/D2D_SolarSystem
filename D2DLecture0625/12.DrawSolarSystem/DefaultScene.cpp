#include "pch.h"
#include "SolarSystemRenderer.h"
#include "InputManager.h"
#include "DefaultScene.h"

std::wstring& DefaultTestScene::GetTitleString() const
{
    static const std::wstring unity = L"����Ƽ ī�޶� ���";
    static const std::wstring d2d = L"D2D ī�޶� ���";

    static std::wstring ret;
    ret.clear();
    ret = (m_IsUnityCamera ? unity : d2d);
    ret += L" - ";
    ret += (m_IsRenerTMOn ? L"������ TM ����" : L"������ TM ������");

    return ret;
}

void DefaultTestScene::SetUp(HWND hWnd)
{
    m_hWnd = hWnd;

    SetWindowText(m_hWnd, L"�̵� Ű�� ī�޶� �̵�, F1 ��ǥ�� ��� ��ȯ, F2 ���� ��� ��ȯ");

    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        float w = static_cast<float>(rc.right - rc.left);
        float h = static_cast<float>(rc.bottom - rc.top);

        m_unityCamera.SetScreenSize(w, h);
    }

}

void DefaultTestScene::Tick(float deltaTime)
{
    // Ű��Ʈ �̺�Ʈ ó��
    
    ProcessKeyboardEvents();

    // ī�޶� ������Ʈ

    MAT3X2F cameraTM = GetCameraTransform();

    MAT3X2F renderTM = MAT3X2F::Identity();
        
    if (m_IsRenerTMOn) renderTM = MYTM::MakeRenderMatrix(m_IsUnityCamera);

    MAT3X2F finalTM =  renderTM * cameraTM;

    // ī�޶� ��ġ ���
    wchar_t buffer[128] = L"";
    MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    // ������

    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();;

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
    globalRenderer.DrawCircle(0.f, 0.f, 5.f, D2D1::ColorF::Red);
    globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::Black);

    globalRenderer.RenderEnd();
}

void DefaultTestScene::OnResize(int width, int height)
{
    // ������ ũ�� ���� �� ī�޶��� ȭ�� ũ�⸦ ������Ʈ
    m_unityCamera.SetScreenSize(width, height);
}

void DefaultTestScene::ProcessKeyboardEvents()
{
    // F1 Ű�� ������ �� ī�޶� ��� ��ȯ
    if (InputManager::Instance().GetKeyPressed(VK_F1))
    {
        FlipCameraMode();
    }

    // F2 Ű�� ������ �� ������ ��� ��ȯ
    if (InputManager::Instance().GetKeyPressed(VK_F2))
    {
        FlipRenderMode();
    }

    // ī�޶� �̵� ó��
    if (InputManager::Instance().GetKeyDown(VK_RIGHT))
    {
        m_d2dCamera.Move(1.f, 0.f);

        m_unityCamera.Move(1.f, 0.f);
    }

    if (InputManager::Instance().GetKeyDown(VK_LEFT))
    {
        m_d2dCamera.Move(-1.f, 0.f);

        m_unityCamera.Move(-1.f, 0.f);
    }

    if (InputManager::Instance().GetKeyDown(VK_UP))
    {
        m_d2dCamera.Move(0.f, 1.f);

        m_unityCamera.Move(0.f, 1.f);
    }

    if (InputManager::Instance().GetKeyDown(VK_DOWN))
    {
        m_d2dCamera.Move(0.f, -1.f);

        m_unityCamera.Move(0.f, -1.f);
    }
}

void DefaultTestScene::FlipCameraMode()
{
    m_IsUnityCamera = !m_IsUnityCamera;

    SetWindowText(m_hWnd, GetTitleString().c_str());
}

void DefaultTestScene::FlipRenderMode()
{
    m_IsRenerTMOn = !m_IsRenerTMOn;

    SetWindowText(m_hWnd, GetTitleString().c_str());
}

MAT3X2F DefaultTestScene::GetCameraTransform()
{
    if (m_IsUnityCamera)
    {
        return m_unityCamera.GetViewMatrix();
    }
 
    return m_d2dCamera.GetViewMatrix();
}
