/**
	@file      SigletonBase.h
	@brief     Meyers�� Singleton ����
	@author    SKYFISH
	@date      JUNE.2023
    @reference https://bitboom.github.io/2019-06-14/meyer-singleton
**/

#pragma once
#include <mutex>

// SingletonBase: ���ø��� ��ӹ޴� T�� ���� �ν��Ͻ��� ����.
template<typename T>
class SingletonBase
{
public:
    
    inline static T& Instance()
    {
        static T instance;
        return instance;
    }

protected:
    SingletonBase() = default;
    virtual ~SingletonBase() = default;

private:
    // ����/�̵� ����
    SingletonBase(const SingletonBase&) = delete;
    SingletonBase& operator=(const SingletonBase&) = delete;
    SingletonBase(SingletonBase&&) = delete;
    SingletonBase& operator=(SingletonBase&&) = delete;

    // ���� T���� �������Ҹ��� ������ �� �ֵ��� friend ����
    friend T;
};