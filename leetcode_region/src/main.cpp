#include <iostream>

#include "base/def.h"
#include "base/str.h"
#include "base/fs.h"
#include "base/flag.h"
#include "base/log.h"

using namespace std;

#define COL 160
#define ROW 90

// 704*576

struct Point {
    int x;
    int y;
};

struct PointF {
    float x;
    float y;
};

struct Region {
    struct Point a;
    struct Point b;
    struct Point c;
    struct Point d;
};

struct mf_block {
    char b[ROW][COL];
};

static void block_print(struct mf_block *b)
{
    if (NULL == b) {
       return; 
    }
    int i, j = 0;

    printf("Region:\n");

    for (i = 0; i < ROW; ++i) {
        for (j = 0; j < COL; ++j) {
            printf("%d", b->b[i][j]);            
        }        
        printf("\n");
    }
    return;
}

static bool IsDecimale(float v)
{
    int a = ((int)v)*100;
    int b = (int)(v*100);
    return a == b;
}

static void gen_block(struct Region *reg, struct mf_block *info)
{
    if (NULL == reg || NULL == info) {
       return; 
    }

    struct PointF p[4] = {0};

    // 704*576 - 160*90
    p[0] = {reg->a.x*160.0/704, reg->a.y*90.0/576};
    p[1] = {reg->b.x*160.0/704, reg->b.y*90.0/576};
    p[2] = {reg->c.x*160.0/704, reg->c.y*90.0/576};
    p[3] = {reg->d.x*160.0/704, reg->d.y*90.0/576};

    // 获取矩形X范围
    float x_min, x_max = 0.0;
    float y_min, y_max = 0.0;

    x_min = x_max = p[0].x;
    y_min = y_max = p[0].y;

    for (int i = 1; i < 4; ++i) {
        if (p[i].x < x_min) {
           x_min = p[i].x; 
        }
        if (p[i].x > x_max) {
           x_max = p[i].x; 
        }
        if (p[i].y < y_min) {
           y_min = p[i].y; 
        }
        if (p[i].y > y_max) {
           y_max = p[i].y; 
        }
    }

    COUT << "[" << x_min << ", " << y_min << "]";
    COUT << "[" << x_max << ", " << y_max << "]";

    int left, top, right, bottom = 0;
    
    left = (int)x_min;
    top = (int)y_min;
    if (IsDecimale(x_max)) {
        right = (int)x_max; 
    } else {
        right = ((int)x_max) + 1;
    }
    if (IsDecimale(y_max)) {
        bottom = (int)y_max; 
    } else {
        bottom = ((int)y_max) + 1;
    }

    COUT << "[" << left << ", " << top << "]";
    COUT << "[" << right << ", " << bottom << "]";

    memset(info, 0, sizeof(struct mf_block));
    for (int i = top; i < bottom; ++i) {
       for (int j = left; j < right; ++j) {
          info->b[i][j] = 1; 
       } 
    }
   // float tmp[] = {1,0, 1.1, 1.2, 1.01, 1.001, 2.0, 3.3};
   // bool res = false;

   // for (int i = 0; i < sizeof(tmp)/sizeof(tmp[0]); ++i) {
   //    res = IsDecimale(tmp[i]); 
   //    COUT << tmp[i] << ": " << res;
   // }

    return;
}

int main(int argc, char** argv)
{
    auto v = flag::init(argc, argv);
    log::init();
    
    struct mf_block info;
    struct Region reg;

    memset(&info, 0, sizeof(struct mf_block));
    memset(&reg, 0, sizeof(struct Region));

    reg.a = {45, 60};
    reg.b = {20, 150};
    reg.c = {370, 10};
    reg.d = {690, 500};

    gen_block(&reg, &info);
    
    block_print(&info);

    return 0;
}
