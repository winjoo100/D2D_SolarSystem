#pragma once
#include "OnlyForTestScene.h"
#include "Camera2D.h"
#include <list>
#include <vector>
#include <wrl/client.h>
#include <d2d1_1.h>
using Vec2 = MYHelper::Vector2F;

class Planet;
class TransformPracticeScene : public OnlyForTestScene
{
public:
    TransformPracticeScene() = default;
    virtual ~TransformPracticeScene();
    
    void SetUp(HWND hWnd) override;

    void Tick(float deltaTime) override;

    void OnResize(int width, int height) override;

private:

    void ProcessKeyboardEvents();

    void OnMouseRButtonDown(D2D1_POINT_2F pos) override;
    void OnMouseWheel(int delta, D2D1_POINT_2F pos) override;

    void CreatePlanetObjects(Vec2 position, ComPtr<ID2D1Bitmap1> planetBitmap);
    void CreateSatelliteObjects(Vec2 position, ComPtr<ID2D1Bitmap1> planetBitmap, Planet* parent);

    void SelectPlanetObject(D2D1_POINT_2F point);

    void SetPlanetRotation();
   
    void UpdatePlanetObjects(float deltaTime);

    UnityCamera m_UnityCamera;

    std::vector<Planet*> m_CenterObjects;       // 공전용 더미
    std::vector<Planet*> m_PlanetObjects;       // 행성
    std::vector<Planet*> m_SatelliteObjects;    // 위성

    std::list<Planet*> m_SelectedPlanetObjects; // 선택된 박스 객체들

    // { 행성들
    ComPtr<ID2D1Bitmap1> m_CenterBitmapPtr;     // 원점이 될 행성 (고양이)
    ComPtr<ID2D1Bitmap1> m_SunBitmapPtr;        // 태양
    ComPtr<ID2D1Bitmap1> m_MercuryBitmapPtr;    // 수성
    ComPtr<ID2D1Bitmap1> m_VenusBitmapPtr;      // 금성
    ComPtr<ID2D1Bitmap1> m_EarthBitmapPtr;      // 지구
    ComPtr<ID2D1Bitmap1> m_MoonBitmapPtr;       // 달
    ComPtr<ID2D1Bitmap1> m_MarsBitmapPtr;       // 화성
    ComPtr<ID2D1Bitmap1> m_JupiterBitmapPtr;    // 목성
    ComPtr<ID2D1Bitmap1> m_SaturnBitmapPtr;     // 토성
    ComPtr<ID2D1Bitmap1> m_UranusBitmapPtr;     // 천왕성
    ComPtr<ID2D1Bitmap1> m_NeptuneBitmapPtr;    // 해왕성
            // } 행성들
};

