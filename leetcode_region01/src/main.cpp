#include <iostream>
#include "reg.h"

using namespace std;

int main(int argc, char** argv)
{
    RECT rt = {0};
    rt.a.x = 100;
    rt.a.y = 100;
    rt.b.x = 1000;
    rt.b.y = 1000;

    POINT a = { 90, 90};
    POINT b = { 45, 45};

    int rc = RectOverlapWithLine(rt, a, b);

    return 0;
}
