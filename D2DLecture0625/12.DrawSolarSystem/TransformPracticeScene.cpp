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
            m_transform.Rotate(deltaTime * rotateSpeed); // 자기 회전
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
        // 뷰 공간에서 히트 테스트
        // 1. 월드 공간의 바운딩 박스 구하기
        D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();
        D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

        // 2. 로컬 사각형 (0,0) - (width, height)를 변환 
        D2D1_POINT_2F topLeft = finalTM.TransformPoint(D2D1::Point2F(m_rect.left, m_rect.top));
        D2D1_POINT_2F bottomRight = finalTM.TransformPoint(D2D1::Point2F(m_rect.right, m_rect.bottom));

        // 3. 뷰 공간에서 사각형 정의
        D2D1_RECT_F viewRect = D2D1::RectF(
            std::min(topLeft.x, bottomRight.x),
            std::min(topLeft.y, bottomRight.y),
            std::max(topLeft.x, bottomRight.x),
            std::max(topLeft.y, bottomRight.y)
        );

        // 4. 뷰 공간에서의 worldPoint와 비교
        if (MYTM::IsPointInRect(worldPoint, viewRect))
        {
            std::cout << "뷰 공간에서 히트!" << std::endl;
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
            // 이미 부모가 있다면 부모 관계를 해제합니다.
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

    MAT3X2F m_renderTM; // 렌더링 변환 행렬

    D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);

    std::wstring m_name = L"";

    bool m_isSelected = false;
    bool m_isLeader = false; // 리더 박스 여부

    bool m_isSelfRotation = false; // 자기 회전 여부

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;

    float rotateSpeed = 60.f;  // 기본 자전 속도
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
    L"가상의 태양계를 만들어 주세요. 물리 법칙은 무시 합니다. ^^;;");

    std::cout << "ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << std::endl;
    std::cout << "ㅡㅡ가이드라인ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << std::endl;
    std::cout << "ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << std::endl;
    std::cout << "태양은 자전을 해야 합니다." << std::endl;
    std::cout << "행성들은 자전을 하며 동시에 태영의 자전에 영향을 받아 공전하는 것처럼 보입니다."<< std::endl;
    std::cout << "달은 자전을 하면서 동시에 지구의 자전에 영향을 받아 공전하는 것처럼 보입니다." << std::endl;
    std::cout << "회전 속도는 자유롭게 설정하세요." << std::endl;
    std::cout << "\n" << std::endl;
    std::cout << "ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << std::endl;
    std::cout << "ㅡㅡ조작키ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << std::endl;
    std::cout << "ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << std::endl;
    std::cout << "카메라 이동 = 화살표 방향키" << std::endl;
    std::cout << "마우스 휠 = 카메라 줌인 / 줌아웃" << std::endl;
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

    // 행성 생성 (태양_00, 수성_01, 금성_02, 지구_03, 달_04, 화성_05, 목성_06, 토성_07, 천왕성_08, 해왕성_09)
    CreatePlanetObjects(Vec2(0.f, 0.f), m_SunBitmapPtr);
    CreatePlanetObjects(Vec2(200.f, 0.f), m_MercuryBitmapPtr);
    CreatePlanetObjects(Vec2(400.f, 0.f), m_VenusBitmapPtr);
    CreatePlanetObjects(Vec2(600.f, 0.f), m_EarthBitmapPtr);
    CreatePlanetObjects(Vec2(800.f, 0.f), m_MarsBitmapPtr);
    CreatePlanetObjects(Vec2(1000.f, 0.f), m_JupiterBitmapPtr);
    CreatePlanetObjects(Vec2(1200.f, 0.f), m_SaturnBitmapPtr);
    CreatePlanetObjects(Vec2(1400.f, 0.f), m_UranusBitmapPtr);
    CreatePlanetObjects(Vec2(1600.f, 0.f), m_NeptuneBitmapPtr);

    // { 행성 위치, 회전, 크기 조절
    D2DTM::Transform* centerTransform;  // 공전용 더미 트랜스폼
    D2DTM::Transform* planetTransform;  // 행성 자체 트랜스폼
    for (int i = 0; i < m_PlanetObjects.size(); i++)
    {
        planetTransform = m_PlanetObjects[i]->GetTransform();
        centerTransform = m_CenterObjects[i]->GetTransform();

        switch (i)
        {
        case 0: // 태양
            planetTransform->SetScale(Vec2(1.5f, 1.5f));
            planetTransform->SetPosition(Vec2(0.f, 0.f));

            m_CenterObjects[i]->SetRotateSpeed(0);          // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(0.20f);      // 자전 속도 배율
            break;
        case 1: // 수성
            planetTransform->SetScale(Vec2(0.5f, 0.5f));

            m_CenterObjects[i]->SetRotateSpeed(1.00f);      // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(0.20f);      // 자전 속도 배율
            break;
        case 2: // 금성
            planetTransform->SetScale(Vec2(0.64f, 0.64f));
            
            m_CenterObjects[i]->SetRotateSpeed(0.73f);      // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(0.10f);      // 자전 속도 배율
            break;
        case 3: // 지구
            planetTransform->SetScale(Vec2(0.64f, 0.64f));
            
            m_CenterObjects[i]->SetRotateSpeed(0.62f);      // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(0.50f);      // 자전 속도 배율
            break;
        case 4: // 화성
            planetTransform->SetScale(Vec2(0.57f, 0.57f));
            
            m_CenterObjects[i]->SetRotateSpeed(0.50f);      // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(0.48f);      // 자전 속도 배율
            break;
        case 5: // 목성
            planetTransform->SetScale(Vec2(0.9f, 0.9f));

            m_CenterObjects[i]->SetRotateSpeed(0.27f);      // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(1.00f);      // 자전 속도 배율
            break;
        case 6: // 토성
            planetTransform->SetScale(Vec2(0.85f, 0.85f));

            m_CenterObjects[i]->SetRotateSpeed(0.20f);      // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(0.93f);      // 자전 속도 배율
            break;
        case 7: // 천왕성
            planetTransform->SetScale(Vec2(0.71f, 0.71f));

            m_CenterObjects[i]->SetRotateSpeed(0.14f);      // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(0.68f);      // 자전 속도 배율
            break;
        case 8: // 해왕성
            planetTransform->SetScale(Vec2(0.78f, 0.78f));

            m_CenterObjects[i]->SetRotateSpeed(0.11f);      // 공전 속도 배율
            m_PlanetObjects[i]->SetRotateSpeed(0.72f);      // 자전 속도 배율
            break;
        default:
            break;
        }
    }       // } 행성 위치, 회전, 크기 조절

    // 위성 생성
    CreateSatelliteObjects(Vec2(500.f, 0.f), m_MoonBitmapPtr, m_PlanetObjects[3]);

    // 행성 자전
    SetPlanetRotation();

    ////////////////////////////////////////////////////////////////////////////////

    // 초기 카메라 줌 셋팅
    m_UnityCamera.SetZoom(0.3f);
}

void TransformPracticeScene::Tick(float deltaTime)
{
    // 입력 이벤트 처리
    ProcessMouseEvents();
    ProcessKeyboardEvents();

    // 생성된 행성 업데이트
    UpdatePlanetObjects(deltaTime);

    // 카메라 업데이트
    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    MAT3X2F renderTM = MYTM::MakeRenderMatrix(true);
    MAT3X2F finalTM = renderTM * cameraTM;

    // 렌더링
    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();

    wchar_t buffer[128] = L"";
    MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    // 카메라 위치 표시
    globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
    globalRenderer.DrawCircle(0.f, 0.f, 5.f, D2D1::ColorF::Red);
    //globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::White);

    // 더미는 안그려도 됨.
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

    // 휠 입력 초기화
    InputManager::Instance().SetMouseWheelDelta(0);
}

void TransformPracticeScene::OnResize(int width, int height)
{ 
    // 윈도우 크기 변경 시 카메라의 화면 크기를 업데이트
    m_UnityCamera.SetScreenSize(width, height);
}

void TransformPracticeScene::ProcessKeyboardEvents()
{
    // 행성 회전
    if (InputManager::Instance().GetKeyPressed(VK_F1))
    {
        SetPlanetRotation();
    }

    // 카메라 이동 처리, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17부터는 structured binding을 사용하여 더 간결하게 표현할 수 있습니다.
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
    // 우클릭 금지
    // SelectPlanetObject(pos);
}

void TransformPracticeScene::OnMouseWheel(int delta, D2D1_POINT_2F pos)
{
    float zoom = m_UnityCamera.GetZoom();

    constexpr float zoomSensitivity = 0.001f;
    zoom += delta * zoomSensitivity;

    zoom = std::clamp(zoom, 0.1f, 20.f); // 너무 작거나 크게 안 가도록 제한
    m_UnityCamera.SetZoom(zoom);
}

void TransformPracticeScene::CreatePlanetObjects(Vec2 position, ComPtr<ID2D1Bitmap1> planetBitmap)
{
    // 공전을 위한 더미(냥이) 생성
    Planet* pCenterPlanet = new Planet(m_CenterBitmapPtr);
    pCenterPlanet->SetPosition(Vec2(0.f, 0.f));
    pCenterPlanet->GetTransform()->SetScale(Vec2(1.f, 1.f));
    pCenterPlanet->GetTransform()->SetPosition(Vec2(-50.f, 50.f));
    m_CenterObjects.push_back(pCenterPlanet);

    // 행성 생성
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
    // 선택 금지.
    //MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();
    //
    //// 박스 HitTest
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

