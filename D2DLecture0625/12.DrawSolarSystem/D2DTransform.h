#pragma once
#include "TMHelper.h"
#include <vector>
#include <algorithm>


namespace D2DTM
{
    enum class PivotPreset
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Center
    };

    class Transform
    {
    public:
        using Vec2 = MYHelper::Vector2F;
        using Mat3x2 = D2D1::Matrix3x2F;

        Transform()
            : m_position{ 0, 0 }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
            m_dirty(false), m_parent(nullptr)
        {
            m_matrixLocal = D2D1::Matrix3x2F::Identity();
            m_matrixWorld = D2D1::Matrix3x2F::Identity();
        }

        ~Transform()
        {
            for (auto* child : m_children)
                child->m_parent = nullptr;
        }

        // ** �θ�-�ڽ� ���� ���� **

        Transform* GetParent() const { return m_parent; }

        void SetParent(Transform* newParent)
        {
            assert(newParent != this); // �ڱ� �ڽ��� �θ�� ������ �� ����
            assert(m_parent == nullptr); // DetachFromParent �� ���� ȣ���ϰ� �ٽ� SetParent�� ȣ���ؾ� ��

            m_parent = newParent;
            m_parent->AddChild(this);

            SetDirty();
        }

        void DetachFromParent()
        {
            if (m_parent == nullptr) return;
        
            m_parent->RemoveChild(this);

            m_parent = nullptr;

            SetDirty();
        }

        void AddChild(Transform* child)
        {
            // �ڽ��� ���� ��ǥ�� �θ� ��ǥ��� ��ȯ
            // �ڽ��� ���� Ʈ������ * �θ��� ���� Ʈ�������� ������� ���ϰ� ���� ����
            D2D1::Matrix3x2F chiledLocalTM = child->GetLocalMatrix();
            chiledLocalTM = chiledLocalTM * GetInverseWorldMatrix();
            
            auto M_noPivot = MYTM::RemovePivot(chiledLocalTM, child->GetPivotPoint());
            MYTM::DecomposeMatrix3x2(M_noPivot, child->m_position, child->m_rotation, child->m_scale);

            m_children.push_back(child);
        }

        void RemoveChild(Transform* child)
        {
            // ����� ������.
            D2D1::Matrix3x2F chiledLocalTM = child->GetLocalMatrix();
            chiledLocalTM = chiledLocalTM * GetWorldMatrix();

            auto M_noPivot = MYTM::RemovePivot(chiledLocalTM, child->GetPivotPoint());
            MYTM::DecomposeMatrix3x2(M_noPivot, child->m_position, child->m_rotation, child->m_scale);

            m_children.erase(
                std::remove(m_children.begin(), m_children.end(), child),
                m_children.end()
            );
        }

        // ** Ʈ������ ���� **
        void SetPosition(const Vec2& pos) { m_position = pos; SetDirty(); }
        void SetRotation(float degree) { m_rotation = degree; SetDirty(); }
        void SetScale(const Vec2& scale) { m_scale = scale; SetDirty(); }

        const Vec2& GetPosition() const { return m_position; }
        float GetRotation() const { return m_rotation; }
        const Vec2& GetScale() const { return m_scale; }

        void Translate(const Vec2& delta)
        {
            m_position.x += delta.x;
            m_position.y += delta.y;
            SetDirty();
        }

        void Translate(const float x, const float y)
        {
			m_position.x += x;
			m_position.y += y;
			SetDirty();
		}

        void Rotate(float degree)
        {
			m_rotation += degree;
			SetDirty();
		}

        // ** ���� ���� **
        Vec2 GetForward() const
        {
            float radian = MYHelper::DegreeToRadian(m_rotation);
            return { std::cosf(radian), std::sinf(radian) };
        }

        // ** ��ȯ ��� **
        const Mat3x2& GetLocalMatrix()
        {
            if (m_dirty) UpdateMatrices();

            return m_matrixLocal;
        }

        const Mat3x2& GetWorldMatrix()
        {
            if (m_dirty) UpdateMatrices();
          
            return m_matrixWorld;
        }

        Mat3x2 GetInverseWorldMatrix()
        {
            Mat3x2 inv = GetWorldMatrix();
            inv.Invert();
            return inv;
        }

        // ** ȸ���� �������� ���� �Ǻ� ���� **
        void SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size);

        D2D1_POINT_2F GetPivotPoint() const
        {
            return m_pivot;
        }

    private:
        void SetDirty()
        {
            m_dirty = true;
            for (auto* child : m_children)
            {
                child->SetDirty();
            }
        }

        void UpdateMatrices();

    private:

        bool m_dirty;

        Vec2     m_position = { 0.f, 0.f }; // translation position
        float    m_rotation = 0.f;          // in degrees
        Vec2     m_scale = { 1.f, 1.f };

        
        Transform* m_parent;
        std::vector<Transform*> m_children;

        Mat3x2 m_matrixLocal;
        Mat3x2 m_matrixWorld;

        D2D1_POINT_2F m_pivot{ 0.0f, 0.0f };
    };
}

