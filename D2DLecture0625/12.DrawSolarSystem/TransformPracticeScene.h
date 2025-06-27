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

    std::vector<Planet*> m_CenterObjects;       // ������ ����
    std::vector<Planet*> m_PlanetObjects;       // �༺
    std::vector<Planet*> m_SatelliteObjects;    // ����

    std::list<Planet*> m_SelectedPlanetObjects; // ���õ� �ڽ� ��ü��

    // { �༺��
    ComPtr<ID2D1Bitmap1> m_CenterBitmapPtr;     // ������ �� �༺ (�����)
    ComPtr<ID2D1Bitmap1> m_SunBitmapPtr;        // �¾�
    ComPtr<ID2D1Bitmap1> m_MercuryBitmapPtr;    // ����
    ComPtr<ID2D1Bitmap1> m_VenusBitmapPtr;      // �ݼ�
    ComPtr<ID2D1Bitmap1> m_EarthBitmapPtr;      // ����
    ComPtr<ID2D1Bitmap1> m_MoonBitmapPtr;       // ��
    ComPtr<ID2D1Bitmap1> m_MarsBitmapPtr;       // ȭ��
    ComPtr<ID2D1Bitmap1> m_JupiterBitmapPtr;    // ��
    ComPtr<ID2D1Bitmap1> m_SaturnBitmapPtr;     // �伺
    ComPtr<ID2D1Bitmap1> m_UranusBitmapPtr;     // õ�ռ�
    ComPtr<ID2D1Bitmap1> m_NeptuneBitmapPtr;    // �ؿռ�
            // } �༺��
};

