#include <stdio.h>
#include <stdlib.h>
#include "reg.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// 检测水平坐标区间[x1, x2] [x3, x4]是否有相交
static int intervalOverlap(double x1, double x2, double x3, double x4)
{
    int rc = 0;

    if ((x3 > x2) || (x1 > x4)) {
        rc = 0; 
    } else {
        rc = 1;
    }

    return rc;
}

int RectOverlapWithLine(RECT rt, POINT a, POINT b)
{
    int ret = 0;
    printf("[HGH-TEST][%s %d] rect [%lf %lf] [%lf %lf]\n", 
            __FUNCTION__, __LINE__, rt.a.x, rt.a.y, rt.b.x, rt.b.y);
    printf("[HGH-TEST][%s %d] a[%lf %lf]\n", __FUNCTION__, __LINE__, a.x, a.y);
    printf("[HGH-TEST][%s %d] b[%lf %lf]\n", __FUNCTION__, __LINE__, b.x, b.y);

    int rc = 0;
    rc = intervalOverlap(20, 40, 10, 21);
    printf("[HGH-TEST][%s %d] rc[%d]\n", __FUNCTION__, __LINE__, rc);

    double rect_y_min = rt.a.y;
    double rect_y_max = rt.a.y;
    if (rt.a.y < rt.b.y) {
        rect_y_max = rt.b.y;
    } else {
        rect_y_min = rt.b.y;
    }

    double rect_x_min = rt.a.x;
    double rect_x_max = rt.a.x;
    if (rt.a.x < rt.b.x) {
        rect_x_max = rt.b.x;
    } else {
        rect_x_min = rt.b.x;
    }

    // 线与x轴平行
    if (a.y == b.y) {
        if ((a.y >= rect_y_min) && (a.y <= rect_y_max)) {
            int line_x_min = a.x;
            int line_x_max = b.x;
            if (a.x < b.x) {
               line_x_max = b.x; 
            } else {
               line_x_min = b.x; 
            }
            return intervalOverlap(line_x_min, line_x_max, rect_x_min, rect_x_max);
        } else {
            return 0;  
        }     
    }

    POINT a0 = a;
    POINT a1 = b;
    
    // 保证a1点的坐标在a0下方
    if (a.y > b.y) {
       a0 = b;
       a1 = a;
    }

    POINT C = a0;
    POINT D = a1;
    double k = (a1.x - a0.x)/(a1.y - a0.y);

    if (a0.y < rect_y_min) {
        C.y = rect_y_min;
        C.x = a0.x + k * (C.y - a0.y);
    } else {
        C = a0; 
    }

    if (a1.y > rect_y_max) {
        D.y = rect_y_max;
        D.x = a0.x + k(D.y - a0.y);
    } else {
        D = a1; 
    }


    if (D.y >= C.y) {
        return intervalOverlap(MIN(C.x, D.x), MAX(C.x, D.x), rect_x_min, rect_x_max);
    }

    return 0;
}
