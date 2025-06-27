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
            m_transform.Rotate(deltaTime * 36.f); // �ڱ� ȸ��
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
        testRender.DrawRectangle(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom, boxColor);

        D2D1_RECT_F dest = D2D1::RectF(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom);

        testRender.DrawBitmap(m_BitmapPtr.Get(), dest);
        testRender.DrawMessage(m_name.c_str(), s_rect.left, s_rect.top, 200, 50, D2D1::ColorF::Black);
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

protected:
    D2DTM::Transform m_transform;

    MAT3X2F m_renderTM; // ������ ��ȯ ���

    D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

    std::wstring m_name = L"";

    bool m_isSelected = false;
    bool m_isLeader = false; // ���� �ڽ� ����

    bool m_isSelfRotation = false; // �ڱ� ȸ�� ����

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;
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

    std::cout << "�¾��� ������ �ؾ� �մϴ�." << std::endl;
    std::cout << "�༺���� ������ �ϸ� ���ÿ� �¿��� ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�."<< std::endl;
    std::cout << "���� ������ �ϸ鼭 ���ÿ� ������ ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�." << std::endl;
    std::cout << "ȸ�� �ӵ��� �����Ӱ� �����ϼ���." << std::endl;

    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Sun.png", *m_SunBitmapPtr.GetAddressOf());
    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Earth.png", *m_EarthBitmapPtr.GetAddressOf());
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

    // �༺ ����
    CreatePlanetObjects(Vec2(0.f, 0.f), m_SunBitmapPtr);
    CreatePlanetObjects(Vec2(200.f, 0.f), m_EarthBitmapPtr, m_PlanetObjects[0]);
    CreatePlanetObjects(Vec2(350.f, 0.f), m_MoonBitmapPtr, m_PlanetObjects[1]);

    // { �༺ ��ġ, ũ�� ����
    // �¾�
    D2DTM::Transform* sunTransform = m_PlanetObjects[0]->GetTransform();
    sunTransform->SetScale(Vec2(1.5f, 1.5f));
    sunTransform->SetPosition(Vec2(-50.f, 50.f));

    // ����
    D2DTM::Transform* earthTransform = m_PlanetObjects[1]->GetTransform();
    earthTransform->SetScale(Vec2(0.5f, 0.5f));

    // ��
    D2DTM::Transform* moonTransform = m_PlanetObjects[2]->GetTransform();
    moonTransform->SetScale(Vec2(0.5f, 0.5f));
            // } �༺ ��ġ, ũ�� ����

    // �༺ ����
    SetPlanetRotation();

    ////////////////////////////////////////////////////////////////////////////////
}

void TransformPracticeScene::Tick(float deltaTime)
{
    // �Է� �̺�Ʈ ó��
    ProcessMouseEvents();
    ProcessKeyboardEvents();

    // ������ �༺ ������Ʈ
    for (auto& planet : m_PlanetObjects)
    {
        planet->Update(deltaTime);
    }

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
    globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::Black);

    for (auto& planet : m_PlanetObjects)
    {
        planet->Draw(globalRenderer, cameraTM);
    }

    globalRenderer.RenderEnd();
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
    SelectPlanetObject(pos);
}

void TransformPracticeScene::CreatePlanetObjects(Vec2 position, ComPtr<ID2D1Bitmap1> planetBitmap)
{
    Planet* pNewPlanet = new Planet(planetBitmap);
    pNewPlanet->SetPosition(position);
    m_PlanetObjects.push_back(pNewPlanet);
}

void TransformPracticeScene::CreatePlanetObjects(Vec2 position, ComPtr<ID2D1Bitmap1> planetBitmap, Planet* parent)
{
    Planet* pNewPlanet = new Planet(planetBitmap);
    pNewPlanet->SetPosition(position);

    pNewPlanet->SetParent(parent);

    m_PlanetObjects.push_back(pNewPlanet);
}

void TransformPracticeScene::SelectPlanetObject(D2D1_POINT_2F point)
{
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    
    // �ڽ� HitTest
    for (auto& box : m_PlanetObjects)
    {
        if (box->IsHitTest(point, cameraTM))
        {
            box->ToggleSelected();
        }
    }
}

void TransformPracticeScene::SetPlanetRotation()
{
    for (auto& planet : m_PlanetObjects)
    {
        if (false == planet->IsSelected())
        {
            planet->ToggleSelfRotation();
        }
    }
}

