#pragma once
#include "TMHelper.h"

using Vec2F = MYHelper::Vector2F;
using MAT3X2F = D2D1::Matrix3x2F;

// ------------------------------------------------------
// 2D ī�޶� ���� �⺻ Ŭ���� : ȸ���� ����.
// ------------------------------------------------------

class Camera2DBase
{
public:
    Camera2DBase() noexcept
        : m_position({ 0,0 })
        , m_zoom(1.0f)
    {
    }

    void SetPosition(const Vec2F& pos) noexcept
    {
        m_position = pos;
    }

    void SetZoom(float zoom) noexcept
    {
        m_zoom = zoom;
    }

    Vec2F GetPosition() const noexcept
    {
        return m_position;
    }

    float GetZoom() const noexcept
    {
        return m_zoom;
    }

    void Move(const float deltaX, const float deltaY) noexcept
    {
        m_position.x += deltaX;
        m_position.y += deltaY;
    }

protected:
    // �Ļ� ī�޶� ��ü �� ��Ʈ���� ���
    virtual MAT3X2F GetViewMatrix() abstract;

    // ���忡�� ī�޶� ��ġ
    Vec2F m_position;

    // Ȯ��/��� (1.0 = 100%)
    float m_zoom;       
};

// ------------------------------------------------------
// �»�� ������ 2D ī�޶� Ŭ����
// ------------------------------------------------------

class D2DCamera2D : public Camera2DBase
{
public:
    virtual MAT3X2F GetViewMatrix() override
    {
        // 1) ��
        auto S = MAT3X2F::Scale(m_zoom, m_zoom);
        
        // 2) ī�޶� ��ġ��ŭ �ݴ�� ���� �̵�
        auto T = MAT3X2F::Translation(-m_position.x, -m_position.y);

        return S * T;
    }
};

// ------------------------------------------------------
// Unity�� īƮ�þ� ��ǥ�� ī�޶�, 
// 1) ��ũ�� �߽� ����
// 2) y�� ����
// ------------------------------------------------------

class UnityCamera : public Camera2DBase
{
public:
    UnityCamera() = default;

    UnityCamera(float screenWidth,
        float screenHeight) noexcept
        : m_screenWidth(screenWidth)
        , m_screenHeight(screenHeight)
    {
        m_position = { 0, 0 };
        m_zoom = 1.0f;
    }

 
    // World �� CameraPan �� CenterPivot �� Scale+Flip �� BackPivot
    MAT3X2F GetViewMatrixLB()
    {
        // 1) ī�޶� �д�: ���� ��ǥ�� ī�޶� ��ǥ�� �̵�
        auto pan = MAT3X2F::Translation(
            -m_position.x,
            -m_position.y);

        // 2) ȭ�� �߽��� (0,0)���� �ű�� Pivot
        auto toCenter = MAT3X2F::Translation(
            -m_screenWidth * 0.5f,
            -m_screenHeight * 0.5f);

        // 3) ��(scale)�� Y�� ����(flipY)�� �� ����
        auto scaleFlip = MAT3X2F::Scale(
            m_zoom,
            -m_zoom);

        // 4) Pivot ���� (���� ȭ�� ��ġ�� �̵�)
        auto back = MAT3X2F::Translation(
            m_screenWidth * 0.5f,
            m_screenHeight * 0.5f);

        //5) ��� ��ȯ�� ����
        // ����: pan �� toCenter �� scaleFlip
        return pan * toCenter * scaleFlip * back;
    }

    // World �� CameraPan �� Scale+Flip �� CenterPivot
    MAT3X2F GetViewMatrixCenter()
    {
        // 1) ī�޶� �д�: ���� ��ǥ�� ī�޶� ��ǥ�� �̵�
        auto pan = MAT3X2F::Translation(
            -m_position.x,
            -m_position.y);

        // 2) ��(scale) + Y�� ����(flipY)
        auto scaleFlip = MAT3X2F::Scale(m_zoom,
            -m_zoom);

        // 3) ȭ�� ���߾����� �̵�
        auto center = MAT3X2F::Translation(
            m_screenWidth * 0.5f,
            m_screenHeight * 0.5f);

        // ������� ���ϸ�: pan �� scale+flip �� center
        return pan * scaleFlip * center;
    }

    // ī�޶� �� ��Ʈ���� ��ȯ : ���߾� ����
    virtual MAT3X2F GetViewMatrix() override
    {
        //return GetViewMatrixLB(); // ���ϴ� ����
        return GetViewMatrixCenter(); // ���߾� ����
    }

    void SetScreenSize(float width, float height) noexcept
    {
        m_screenWidth = width;
        m_screenHeight = height;
    }

private:
    float m_screenWidth = 0.f;    // ���� Ÿ�� �ʺ�(px)
    float m_screenHeight = 0.f;   // ���� Ÿ�� ����(px)
};