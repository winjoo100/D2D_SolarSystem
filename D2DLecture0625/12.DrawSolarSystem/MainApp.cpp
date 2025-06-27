#include "pch.h"
#include "MainApp.h"
#include "InputManager.h"
#include "GameTimer.h"
#include "SolarSystemRenderer.h"
#include "DefaultScene.h"
#include "HierarchicalTransformTestScene.h"
#include "TransformPracticeScene.h"
//
// class MainApp
// 
bool MainApp::Initialize()
{
    const wchar_t* className = L"D2DLesson";
    const wchar_t* windowName = L"Solar System";

    if (false == __super::Create(className, windowName, 800, 800))
    {
        return false;
    }

    if (false == InputManager::Instance().Initialize(m_hWnd))
    {
        return false;
    }
    
    SolarSystemRenderer::Instance().Initialize(m_hWnd);

    m_TimerPtr = std::make_unique<GameTimer>();

    m_TimerPtr->Start();


    // [�׽�Ʈ �¾�]�� ���⿡�� �ؿ�.
    // �׽�Ʈ�� ���� �ϳ��� ����� �Ӵϴ�.

    //m_TestScenePtr = std::make_shared<DefaultTestScene>();

    m_TestScenePtr = std::make_shared<TransformPracticeScene>();

    //m_TestScenePtr = std::make_shared<HierarchicalTransformTestScene>();
    
    
    m_TestScenePtr->SetUp(m_hWnd);

    return true;
}

void MainApp::Run()
{
    MSG msg = { 0 };
    
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (false == InputManager::Instance().OnHandleMessage(msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            UpdateTime();
     
            // �̰� ���� ������ �ƴ϶� ��� �� �׽�Ʈ ������ �ñ�ϴ�. 
            if (m_TestScenePtr != nullptr)
            {
                m_TestScenePtr->Tick(m_TimerPtr->DeltaTime());
            }
        }
    }
}
void MainApp::Finalize()
{
    __super::Destroy();

}

void MainApp::UpdateTime()
{
    assert(m_TimerPtr != nullptr);

    m_TimerPtr->Tick();
}

void MainApp::OnResize(int width, int height)
{
    __super::OnResize(width, height);

    SolarSystemRenderer::Instance().Resize(width, height);

    if (nullptr != m_TestScenePtr)m_TestScenePtr->OnResize(width, height);
}

void MainApp::OnClose()
{
    std::cout << "OnClose !!!" << std::endl;

    SolarSystemRenderer::Instance().Uninitialize();

}
