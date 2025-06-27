#include "pch.h"
#include "TMHelper.h"
#include "InputManager.h"
#include "OnlyForTestScene.h"

using namespace D2D1;

inline D2D1_POINT_2F ToFloatPoint(const MouseState& ms)
{
    return { static_cast<float>(ms.pos.x),
             static_cast<float>(ms.pos.y) };
}

void OnlyForTestScene::ProcessMouseEvents()
{
    // ���� ������ ���� (�Լ�-���� static)
    static MouseState prevMouseStatus = {};

    // ���� ������ ����
    const MouseState curMouseStatus = InputManager::Instance().GetMouseState();

    // ��ư�� ���� ������ ����
    auto detect = [&](bool wasPressed, bool isPressed,
        auto&& onDown, auto&& onUp)
        {
            if (!wasPressed && isPressed) onDown(curMouseStatus);
            else if (wasPressed && !isPressed) onUp(curMouseStatus);
        };

    // ���� ��ư �̺�Ʈ
    detect(
        prevMouseStatus.leftPressed, curMouseStatus.leftPressed,
        [&](const MouseState&) { OnMouseLButtonDown(ToFloatPoint(curMouseStatus)); },
        [&](const MouseState&) { OnMouseLButtonUp(ToFloatPoint(curMouseStatus)); }
    );

    // ������ ��ư �̺�Ʈ
    detect(
        prevMouseStatus.rightPressed, curMouseStatus.rightPressed,
        [&](const MouseState&) { OnMouseRButtonDown(ToFloatPoint(curMouseStatus)); },
        [&](const MouseState&) { OnMouseRButtonUp(ToFloatPoint(curMouseStatus)); }
    );

    if (IsMouseMove(prevMouseStatus, curMouseStatus))
    {
        OnMouseMove(ToFloatPoint(prevMouseStatus), ToFloatPoint(curMouseStatus));
    }

    prevMouseStatus = curMouseStatus;
}

void OnlyForTestScene::OnMouseLButtonDown(D2D1_POINT_2F pos)
{
    //std::cout << "Left button down at: " << pos.x << ", " << pos.y << std::endl;
}

void OnlyForTestScene::OnMouseLButtonUp(D2D1_POINT_2F pos)
{
    //std::cout << "Left button up at: " << pos.x << ", " << pos.y << std::endl;
}

void OnlyForTestScene::OnMouseRButtonDown(D2D1_POINT_2F pos)
{
    //std::cout << "Right button down at: " << pos.x << ", " << pos.y << std::endl;
}

void OnlyForTestScene::OnMouseRButtonUp(D2D1_POINT_2F pos)
{
   // std::cout << "Right button up at: " << pos.x << ", " << pos.y << std::endl;
}

void OnlyForTestScene::OnMouseMove(D2D1_POINT_2F prev, D2D1_POINT_2F cur)
{
   /* std::cout << "Mouse moved from (" << prev.x << ", " << prev.y << ") to ("
        << cur.x << ", " << cur.y << ")" << std::endl;*/
}


