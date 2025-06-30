#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "TransformPracticeScene.h"

using TestRenderer = myspace::D2DRenderer;
using Vec2 = MYHelper::Vector2F;

class Planet
{
    Planet() = delete;
    Planet(const Planet&) = delete;
    void operator=(const Planet&) = delete;

public:
    Planet(ComPtr<ID2D1Bitmap1>& bitmap)
    {
        m_BitmapPtr = bitmap;

        m_renderTM = MYTM::MakeRenderMatrix(true);

        D2D1_SIZE_F size = { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top };

        //m_transform.SetPivotPreset(D2DTM::PivotPreset::TopLeft, size);
        //m_transform.SetPivotPreset(D2DTM::PivotPreset::BottomRight, size);
        m_transform.SetPivotPreset(D2DTM::PivotPreset::Center, size);
    }

    ~Planet() = default;

    void Update(float deltaTime)
    {
        if (m_isSelfRotation)
        {
            m_transform.Rotate(deltaTime * rotateSpeed); // �ڱ� ȸ��
        }
    }

    void Draw(TestRenderer& testRender, D2D1::Matrix3x2F viewTM)
    {
        static D2D1_RECT_F s_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();

        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        D2D1::ColorF boxColor = D2D1::ColorF::LightBlue;

        if (m_isLeader) boxColor = D2D1::ColorF::Red;
        else if (m_isSelected) boxColor = D2D1::ColorF::Green;

        testRender.SetTransform(finalTM);
        //testRender.DrawRectangle(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom, boxColor);

        D2D1_RECT_F dest = D2D1::RectF(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom);

        testRender.DrawBitmap(m_BitmapPtr.Get(), dest);
        testRender.DrawMessage(m_name.c_str(), s_rect.left, s_rect.top, 200, 50, D2D1::ColorF::White);
    }

    void SetPosition(const Vec2& pos)
    {
        m_transform.SetPosition(pos);
    }

    void Move(const Vec2& offset)
    {
        m_transform.Translate(offset);
    }

    void Rotate(float angle)
    {
        m_transform.Rotate(angle);
    }

    void ToggleSelected()
    {
        m_isSelected = !m_isSelected;
    }

    bool IsSelected() const
    {
        return m_isSelected;
    }

    void ToggleSelfRotation()
    {
        m_isSelfRotation = !m_isSelfRotation;
    }

    bool IsHitTest(D2D1_POINT_2F worldPoint, D2D1::Matrix3x2F viewTM)
    {
        // �� �������� ��Ʈ �׽�Ʈ
        // 1. ���� ������ �ٿ�� �ڽ� ���ϱ�
        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();
        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        // 2. ���� �簢�� (0,0) - (width, height)�� ��ȯ 
        D2D1_POINT_2F topLeft = finalTM.TransformPoint(D2D1::Point2F(m_rect.left, m_rect.top));
        D2D1_POINT_2F bottomRight = finalTM.TransformPoint(D2D1::Point2F(m_rect.right, m_rect.bottom));

        // 3. �� �������� �簢�� ����
        D2D1_RECT_F viewRect = D2D1::RectF(
            std::min(topLeft.x, bottomRight.x),
            std::min(topLeft.y, bottomRight.y),
            std::max(topLeft.x, bottomRight.x),
            std::max(topLeft.y, bottomRight.y)
        );

        // 4. �� ���������� worldPoint�� ��
        if (MYTM::IsPointInRect(worldPoint, viewRect))
        {
            std::cout << "�� �������� ��Ʈ!" << std::endl;
            return true;
        } 

        return false;
    }

    D2DTM::Transform* GetTransform()
    {
        return &m_transform;
    }

    void SetParent(Planet* parent)
    {
        assert(parent != nullptr);

        if (nullptr != m_transform.GetParent())
        {
            // �̹� �θ� �ִٸ� �θ� ���踦 �����մϴ�.
            m_transform.DetachFromParent();
        }

        m_transform.SetParent(parent->GetTransform());
    }

    void DetachFromParent()
    {
        m_transform.DetachFromParent();
    }

    void SetLeader(bool isLeader)
    {
        m_isLeader = isLeader;
    }

    void SetRotateSpeed(float value)
    {
        rotateSpeed *= value;
    }

    float GetRotateSpeed() const {
        return rotateSpeed;
    }

protected:
    D2DTM::Transform m_transform;

    MAT3X2F m_renderTM; // ������ ��ȯ ���

    D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

    std::wstring m_name = L"";

    bool m_isSelected = false;
    bool m_isLeader = false; // ���� �ڽ� ����

    bool m_isSelfRotation = false; // �ڱ� ȸ�� ����

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;

    float rotateSpeed = 60.f;  // �⺻ ���� �ӵ�
};

TransformPracticeScene::~TransformPracticeScene()  
{  
    for (auto& box : m_PlanetObjects)
    {
        delete box;
    }
}

void TransformPracticeScene::SetUp(HWND hWnd)
{
    m_hWnd = hWnd;

    SetWindowText(m_hWnd, 
    L"������ �¾�踦 ����� �ּ���. ���� ��Ģ�� ���� �մϴ�. ^^;;");

    std::cout << "�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << std::endl;
    std::cout << "�ѤѰ��̵���ΤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << std::endl;
    std::cout << "�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << std::endl;
    std::cout << "�¾��� ������ �ؾ� �մϴ�." << std::endl;
    std::cout << "�༺���� ������ �ϸ� ���ÿ� �¿��� ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�."<< std::endl;
    std::cout << "���� ������ �ϸ鼭 ���ÿ� ������ ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�." << std::endl;
    std::cout << "ȸ�� �ӵ��� �����Ӱ� �����ϼ���." << std::endl;
    std::cout << "\n" << std::endl;
    std::cout << "�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << std::endl;
    std::cout << "�Ѥ�����Ű�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << std::endl;
    std::cout << "�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << std::endl;
    std::cout << "ī�޶� �̵� = ȭ��ǥ ����Ű" << std::endl;
    std::cout << "���콺 �� = ī�޶� ���� / �ܾƿ�" << std::endl;
    std::cout << "\n" << std::endl;


    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Center.png", *m_CenterBitmapPtr.GetAddressOf());

    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Sun.png", *m_SunBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Mercury.png", *m_MercuryBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Venus.png", *m_VenusBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Earth.png", *m_EarthBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Mars.png", *m_MarsBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Jupiter.png", *m_JupiterBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Saturn.png", *m_SaturnBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Uranus.png", *m_UranusBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Neptune.png", *m_NeptuneBitmapPtr.GetAddressOf());

    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Moon.png", *m_MoonBitmapPtr.GetAddressOf());

    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        float w = static_cast<float>(rc.right - rc.left);
        float h = static_cast<float>(rc.bottom - rc.top);

        m_UnityCamera.SetScreenSize(w, h);
    }

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    // �༺ ���� (�¾�_00, ����_01, �ݼ�_02, ����_03, ��_04, ȭ��_05, ��_06, �伺_07, õ�ռ�_08, �ؿռ�_09)
    CreatePlanetObjects(Vec2(0.f, 0.f), m_SunBitmapPtr);
    CreatePlanetObjects(Vec2(200.f, 0.f), m_MercuryBitmapPtr);
    CreatePlanetObjects(Vec2(400.f, 0.f), m_VenusBitmapPtr);
    CreatePlanetObjects(Vec2(600.f, 0.f), m_EarthBitmapPtr);
    CreatePlanetObjects(Vec2(800.f, 0.f), m_MarsBitmapPtr);
    CreatePlanetObjects(Vec2(1000.f, 0.f), m_JupiterBitmapPtr);
    CreatePlanetObjects(Vec2(1200.f, 0.f), m_SaturnBitmapPtr);
    CreatePlanetObjects(Vec2(1400.f, 0.f), m_UranusBitmapPtr);
    CreatePlanetObjects(Vec2(1600.f, 0.f), m_NeptuneBitmapPtr);

    // { �༺ ��ġ, ȸ��, ũ�� ����
    D2DTM::Transform* centerTransform;  // ������ ���� Ʈ������
    D2DTM::Transform* planetTransform;  // �༺ ��ü Ʈ������
    for (int i = 0; i < m_PlanetObjects.size(); i++)
    {
        planetTransform = m_PlanetObjects[i]->GetTransform();
        centerTransform = m_CenterObjects[i]->GetTransform();

        switch (i)
        {
        case 0: // �¾�
            planetTransform->SetScale(Vec2(1.5f, 1.5f));
            planetTransform->SetPosition(Vec2(0.f, 0.f));

            m_CenterObjects[i]->SetRotateSpeed(0);          // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(0.20f);      // ���� �ӵ� ����
            break;
        case 1: // ����
            planetTransform->SetScale(Vec2(0.5f, 0.5f));

            m_CenterObjects[i]->SetRotateSpeed(1.00f);      // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(0.20f);      // ���� �ӵ� ����
            break;
        case 2: // �ݼ�
            planetTransform->SetScale(Vec2(0.64f, 0.64f));
            
            m_CenterObjects[i]->SetRotateSpeed(0.73f);      // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(0.10f);      // ���� �ӵ� ����
            break;
        case 3: // ����
            planetTransform->SetScale(Vec2(0.64f, 0.64f));
            
            m_CenterObjects[i]->SetRotateSpeed(0.62f);      // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(0.50f);      // ���� �ӵ� ����
            break;
        case 4: // ȭ��
            planetTransform->SetScale(Vec2(0.57f, 0.57f));
            
            m_CenterObjects[i]->SetRotateSpeed(0.50f);      // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(0.48f);      // ���� �ӵ� ����
            break;
        case 5: // ��
            planetTransform->SetScale(Vec2(0.9f, 0.9f));

            m_CenterObjects[i]->SetRotateSpeed(0.27f);      // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(1.00f);      // ���� �ӵ� ����
            break;
        case 6: // �伺
            planetTransform->SetScale(Vec2(0.85f, 0.85f));

            m_CenterObjects[i]->SetRotateSpeed(0.20f);      // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(0.93f);      // ���� �ӵ� ����
            break;
        case 7: // õ�ռ�
            planetTransform->SetScale(Vec2(0.71f, 0.71f));

            m_CenterObjects[i]->SetRotateSpeed(0.14f);      // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(0.68f);      // ���� �ӵ� ����
            break;
        case 8: // �ؿռ�
            planetTransform->SetScale(Vec2(0.78f, 0.78f));

            m_CenterObjects[i]->SetRotateSpeed(0.11f);      // ���� �ӵ� ����
            m_PlanetObjects[i]->SetRotateSpeed(0.72f);      // ���� �ӵ� ����
            break;
        default:
            break;
        }
    }       // } �༺ ��ġ, ȸ��, ũ�� ����

    // ���� ����
    CreateSatelliteObjects(Vec2(500.f, 0.f), m_MoonBitmapPtr, m_PlanetObjects[3]);

    // �༺ ����
    SetPlanetRotation();

    ////////////////////////////////////////////////////////////////////////////////

    // �ʱ� ī�޶� �� ����
    m_UnityCamera.SetZoom(0.3f);
}

void TransformPracticeScene::Tick(float deltaTime)
{
    // �Է� �̺�Ʈ ó��
    ProcessMouseEvents();
    ProcessKeyboardEvents();

    // ������ �༺ ������Ʈ
    UpdatePlanetObjects(deltaTime);

    // ī�޶� ������Ʈ
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    MAT3X2F renderTM = MYTM::MakeRenderMatrix(true);
    MAT3X2F finalTM = renderTM * cameraTM;

    // ������
    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();

    wchar_t buffer[128] = L"";
    MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    // ī�޶� ��ġ ǥ��
    globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
    globalRenderer.DrawCircle(0.f, 0.f, 5.f, D2D1::ColorF::Red);
    //globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::White);

    // ���̴� �ȱ׷��� ��.
    /*for (auto& center : m_CenterObjects)
    {
        center->Draw(globalRenderer, cameraTM);
    }*/
    for (auto& planet : m_PlanetObjects)
    {
        planet->Draw(globalRenderer, cameraTM);
    }
    for (auto& satellite : m_SatelliteObjects)
    {
        satellite->Draw(globalRenderer, cameraTM);
    }

    globalRenderer.RenderEnd();

    // �� �Է� �ʱ�ȭ
    InputManager::Instance().SetMouseWheelDelta(0);
}

void TransformPracticeScene::OnResize(int width, int height)
{ 
    // ������ ũ�� ���� �� ī�޶��� ȭ�� ũ�⸦ ������Ʈ
    m_UnityCamera.SetScreenSize(width, height);
}

void TransformPracticeScene::ProcessKeyboardEvents()
{
    // �༺ ȸ��
    if (InputManager::Instance().GetKeyPressed(VK_F1))
    {
        SetPlanetRotation();
    }

    // ī�޶� �̵� ó��, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17���ʹ� structured binding�� ����Ͽ� �� �����ϰ� ǥ���� �� �ֽ��ϴ�.
    for (auto& [vk, dir] : kCameraMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
            m_UnityCamera.Move(dir.x, dir.y);
        }
    }
}

void TransformPracticeScene::OnMouseRButtonDown(D2D1_POINT_2F pos)
{
    // ��Ŭ�� ����
    // SelectPlanetObject(pos);
}

void TransformPracticeScene::OnMouseWheel(int delta, D2D1_POINT_2F pos)
{
    float zoom = m_UnityCamera.GetZoom();

    constexpr float zoomSensitivity = 0.001f;
    zoom += delta * zoomSensitivity;

    zoom = std::clamp(zoom, 0.1f, 20.f); // �ʹ� �۰ų� ũ�� �� ������ ����
    m_UnityCamera.SetZoom(zoom);
}

void TransformPracticeScene::CreatePlanetObjects(Vec2 position, ComPtr<ID2D1Bitmap1> planetBitmap)
{
    // ������ ���� ����(����) ����
    Planet* pCenterPlanet = new Planet(m_CenterBitmapPtr);
    pCenterPlanet->SetPosition(Vec2(0.f, 0.f));
    pCenterPlanet->GetTransform()->SetScale(Vec2(1.f, 1.f));
    pCenterPlanet->GetTransform()->SetPosition(Vec2(-50.f, 50.f));
    m_CenterObjects.push_back(pCenterPlanet);

    // �༺ ����
    Planet* pNewPlanet = new Planet(planetBitmap);
    pNewPlanet->SetPosition(position);
    pNewPlanet->SetParent(pCenterPlanet);
    m_PlanetObjects.push_back(pNewPlanet);
}

void TransformPracticeScene::CreateSatelliteObjects(Vec2 position, ComPtr<ID2D1Bitmap1> planetBitmap, Planet* parent)
{
    Planet* pNewSatellite = new Planet(planetBitmap);
    pNewSatellite->SetPosition(position);
    pNewSatellite->GetTransform()->SetScale(Vec2(0.3f, 0.3f));
    pNewSatellite->SetParent(parent);
    m_SatelliteObjects.push_back(pNewSatellite);
}

void TransformPracticeScene::SelectPlanetObject(D2D1_POINT_2F point)
{
    // ���� ����.
    //MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    //
    //// �ڽ� HitTest
    //for (auto& box : m_PlanetObjects)
    //{
    //    if (box->IsHitTest(point, cameraTM))
    //    {
    //        box->ToggleSelected();
    //    }
    //}
}

void TransformPracticeScene::SetPlanetRotation()
{
    for (auto& center : m_CenterObjects)
    {
        if (false == center->IsSelected())
        {
            center->ToggleSelfRotation();
        }
    }

    for (auto& planet : m_PlanetObjects)
    {
        if (false == planet->IsSelected())
        {
            planet->ToggleSelfRotation();
        }
    }

    for (auto& satellite : m_SatelliteObjects)
    {
        if (false == satellite->IsSelected())
        {
            satellite->ToggleSelfRotation();
        }
    }
}

void TransformPracticeScene::UpdatePlanetObjects(float deltaTime)
{
    for (auto& center : m_CenterObjects)
    {
        center->Update(deltaTime);
    }

    for (auto& planet : m_PlanetObjects)
    {
        planet->Update(deltaTime);
    }

    for (auto& satellite : m_SatelliteObjects)
    {
        satellite->Update(deltaTime);
    }
}

