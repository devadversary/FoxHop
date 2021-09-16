#pragma once

#include <windows.h>

#include <dcommon.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include "component_motion.hpp"

/**
    @brief ��ġ���� ����ü
*/
typedef struct _POSITION
{
    float x;
    float y;
    union {
        struct {
            float width;
            float height;
        };
        struct {
            float x2;
            float y2;
        };
    };
}POSITION;

/**
    @brief ���ø����̼ǿ��� ���� ������Ʈ Ŭ����
*/
class Object {
#if 0 /*2020.02.10 Devadversary : ������ ������ Ȯ���� ���� �߻�ż��� ����*/
public:
    virtual void update(unsigned long ElapsedTime) = 0;
    virtual void render() = 0;
#endif
};