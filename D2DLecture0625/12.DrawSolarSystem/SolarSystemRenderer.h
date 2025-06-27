/**
	@file      SolarSystemRenderer.h
	@brief     Transform �� Sprite �н��� ���� ����
	@author    skyfish (���������)
	@date      JUNE.2025 
   
**/

#pragma once

#include "SingletonBase.h"
#include "D2DRender.h"

class SolarSystemRenderer : public SingletonBase<myspace::D2DRenderer>
{
	// private/protected �����ڸ� ���
	SolarSystemRenderer() { /* �ʱ�ȭ */ }
	~SolarSystemRenderer() { /* ���� */ }

	// SingletonBase<MyManager> �� ģ���̹Ƿ� ������ ȣ�� ����
	friend class SingletonBase<myspace::D2DRenderer>;

public:

};

