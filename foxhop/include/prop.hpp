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
    @n     {x, y, width, height}
    @n     {x, y, x2, y2}
    @n     {x, y, r, (dummy)}
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
        struct {
            float r;     /**< ������*/
            float dummy; /**< ������������ ���� �� ���� ���õ�*/
        };
    };
}POSITION;

/**
    @brief ���ø����̼ǿ��� ���� ������Ʈ Ŭ����
*/
class PropBase {
public:
    PropBase() {
    }
    ~PropBase() {}
};

/**
    @biref ��ġ ����ü �ʱ�ȭ
    @param x                ����x��ǥ
    @param y                ����y��ǥ
    @param width_or_x2_or_r �ʺ� Ȥ�� ��x ��ǥ �Ǵ� ������r��
    @param height_or_y2     ���� Ȥ�� ��y ��ǥ (������ �� ���� �� ���� ���õ�)

*/
inline POSITION InitPosition(float x, float y, float width_or_x2_or_r, float height_or_y2)
{
    return {x, y, width_or_x2_or_r, height_or_y2};
}