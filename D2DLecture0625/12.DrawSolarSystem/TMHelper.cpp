#include "pch.h"
#include <sstream>
#include <iomanip>
#include "TMHelper.h"

using namespace MYHelper;

namespace MYTM
{
	D2D1_MATRIX_3X2_F MakeTranslationMatrix(D2D1_SIZE_F size)
	{
		D2D1_MATRIX_3X2_F _translation;

		_translation._11 = 1.0f; _translation._12 = 0.0f;
		_translation._21 = 0.0f; _translation._22 = 1.0f;
		_translation._31 = size.width; _translation._32 = size.height;

		return _translation;
	}

	D2D1_MATRIX_3X2_F MakeRotationMatrix_Origin(FLOAT angle)
	{
		float _rad = DegreeToRadian(angle);

		D2D1_MATRIX_3X2_F _rotation;

		_rotation._11 = cos(_rad); _rotation._21 = -sin(_rad); _rotation._31 = 0;
		_rotation._12 = sin(_rad); _rotation._22 = cos(_rad); _rotation._32 = 0;

		return _rotation;
	}

	D2D1_MATRIX_3X2_F MakeScaleMatrix_Origin(D2D1_SIZE_F size)
	{
		D2D1_MATRIX_3X2_F _scale;

		_scale._11 = size.width * 1.0f; _scale._12 = 0.0f;
		_scale._21 = 0.0f; _scale._22 = size.height * 1.0f;
		_scale._31 = 0.0f; _scale._32 = 0.0f;

		return _scale;
	}

	D2D1_MATRIX_3X2_F MakeRotationMatrix(FLOAT angle, D2D1_POINT_2F center /*= D2D1::Point2F()*/)
	{
		D2D1_MATRIX_3X2_F _translateToOrigin = MakeTranslationMatrix(D2D1::Size(-center.x, -center.y));
		D2D1_MATRIX_3X2_F _rotateOnOrigin = MakeRotationMatrix_Origin(angle);
		D2D1_MATRIX_3X2_F _translateToCenter = MakeTranslationMatrix(D2D1::Size(center.x, center.y));

		D2D1_MATRIX_3X2_F _resultTM = _translateToOrigin * _rotateOnOrigin * _translateToCenter;

		return _resultTM;
	}

	D2D1_MATRIX_3X2_F MakeScaleMatrix(D2D1_SIZE_F size, D2D1_POINT_2F center /*= D2D1::Point2F()*/)
	{
		D2D1_MATRIX_3X2_F _translateToOrigin = MakeTranslationMatrix(D2D1::Size(-center.x, -center.y));
		D2D1_MATRIX_3X2_F _scaleOnOrigin = MakeScaleMatrix_Origin(size);
		D2D1_MATRIX_3X2_F _translateToCenter = MakeTranslationMatrix(D2D1::Size(center.x, center.y));

		D2D1_MATRIX_3X2_F _resultTM = _translateToOrigin * _scaleOnOrigin * _translateToCenter;

		return _resultTM;
	}

	D2D1::Matrix3x2F MakeRenderMatrix(bool bUnityCoords, bool bMirror, float offsetX, float offsetY)
	{
		float scaleY = bUnityCoords ? -1.0f : 1.0f;     // ����Ƽ ��ǥ��� y�� ���� ����
		float scaleX = bMirror ? -1.0f : 1.0f;          // �̹��� �¿���� �̸� x�� ���� 
		
		offsetX = bMirror ? offsetX : -offsetX;         // �¿�����϶� �̹��� �̵� �� ó��
		offsetY = bUnityCoords ? offsetY : -offsetY;    // ����Ƽ ��ǥ���϶� �̹��� �̵� �� ó��

		return D2D1::Matrix3x2F::Scale(scaleX, scaleY) * D2D1::Matrix3x2F::Translation(offsetX, offsetY);
	}

	void DecomposeMatrix3x2
	(const D2D1::Matrix3x2F& M, MYHelper::Vector2F& outTranslation, float& outRotation, MYHelper::Vector2F& outScale)
	{
		// 1) Translation
		outTranslation.x = M._31;
		outTranslation.y = M._32;

		// 2) Scale: �� �� ������ ����

		outScale.x = sqrtf(M._11 * M._11 + M._12 * M._12);
		outScale.y = sqrtf(M._21 * M._21 + M._22 * M._22);

		// 3) Rotation (rad �� deg):
		// M = [ m11  m12  ]
		//     [ m21  m22  ]
		// rotation = atan2(m12/sx, m11/sx) == atan2(m12, m11)
		// ��_rad = atan2(m12, m11)
		// ��_deg = ��_rad * (180/��)
		const float radians = atan2(M._12, M._11);

		outRotation = MYHelper::RadianToDegree(radians);
	}
	
	D2D1::Matrix3x2F RemovePivot(const D2D1::Matrix3x2F& M_local, const D2D1_POINT_2F& pivot)
	{
		// 1) pivot ���� ��(��������) : T(-pivot)
		auto P0 = D2D1::Matrix3x2F::Translation(-pivot.x, -pivot.y);
		// 2) pivot ���� ��         : T(+pivot)
		auto P1 = D2D1::Matrix3x2F::Translation(pivot.x, pivot.y);
		// 3) ���ʿ� ���ؼ� �ǹ��� ����
		//    P1 * (P0 * M_local * P1) * P0 == M_local ������
		//    pivot ������ ������: M_noPivot = P1 * M_local * P0
		return P1 * M_local * P0;
	}

	
	bool IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) noexcept
	{
		// ��ǥ ����ȭ: left <= right, top <= bottom
		float left = std::min(rect.left, rect.right);
		float right = std::max(rect.left, rect.right);
		float top = std::min(rect.top, rect.bottom);
		float bottom = std::max(rect.top, rect.bottom);

		return (point.x >= left && point.x <= right) && (point.y >= top && point.y <= bottom);
	}

}//namespace