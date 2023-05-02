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
    @brief 위치정보 구조체
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
            float r;     /**< 반지름*/
            float dummy; /**< 반지름값으로 사용시 이 값은 무시됨*/
        };
    };
}POSITION;

/**
    @brief 어플리케이션에서 사용될 오브젝트 클래스
*/
class PropBase {
public:
    PropBase() {
    }
    ~PropBase() {}
};

/**
    @biref 위치 구조체 초기화
    @param x                시작x좌표
    @param y                시작y좌표
    @param width_or_x2_or_r 너비 혹은 끝x 좌표 또는 반지름r값
    @param height_or_y2     높이 혹은 끝y 좌표 (반지름 값 사용시 이 값은 무시됨)

*/
inline POSITION InitPosition(float x, float y, float width_or_x2_or_r, float height_or_y2)
{
    return {x, y, width_or_x2_or_r, height_or_y2};
}