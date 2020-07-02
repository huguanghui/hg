#ifndef __REG_H__
#define __REG_H__ 

typedef struct {
    double x;
    double y;
}POINT;

typedef struct {
    POINT a;
    POINT b;
}RECT;

int RectOverlapWithLine(RECT rt, POINT a, POINT b);

#endif /* ifndef __REG_H__ */
