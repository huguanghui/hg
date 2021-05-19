#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "base/unitest.h"
#include "base/time.h"
#include "base/def.h"
#include "base/str.h"
#include "base/fs.h"
#include "base/flag.h"
#include "base/log.h"
#include "reg.h"

using namespace std;

namespace test {

typedef struct {
    POINT a;
    POINT b;
    int ret;
}RegTest;

static RegTest tests[] = {
    {
        {10, 90},
        {80, 90},
        0
    },
    {
        {80, 90},
        {100, 90},
        0
    },
    {
        {80, 90},
        {110, 90},
        0
    },
    {
        {100, 90},
        {110, 90},
        0
    },
    {
        {110, 90},
        {9999, 90},
        0
    },
    {
        {110, 90},
        {1000, 90},
        0
    },
    {
        {1000, 90},
        {1010, 90},
        0
    },
    {
        {1001, 90},
        {1010, 90},
        0
    },
};

static RegTest tests01[] = {
    {
        {10, 100},
        {80, 100},
        0
    },
    {
        {80, 100},
        {100, 100},
        1
    },
    {
        {80, 100},
        {110, 100},
        1
    },
    {
        {100, 100},
        {110, 100},
        1
    },
    {
        {110, 100},
        {9999, 100},
        1
    },
    {
        {110, 100},
        {1000, 100},
        1
    },
    {
        {1000, 100},
        {1010, 100},
        1
    },
    {
        {1001, 100},
        {1010, 100},
        0
    },
};

static RegTest tests02[] = {
    {
        {10, 110},
        {80, 110},
        0
    },
    {
        {80, 110},
        {100, 110},
        1
    },
    {
        {80, 110},
        {110, 110},
        1
    },
    {
        {100, 110},
        {110, 110},
        1
    },
    {
        {110, 110},
        {9999, 110},
        1
    },
    {
        {110, 110},
        {1000, 110},
        1
    },
    {
        {1000, 110},
        {1010, 110},
        1
    },
    {
        {1001, 110},
        {1010, 110},
        0
    },
};

static RegTest tests03[] = {
    {
        {10, 999},
        {80, 999},
        0
    },
    {
        {80, 999},
        {100, 999},
        1
    },
    {
        {80, 999},
        {110, 999},
        1
    },
    {
        {100, 999},
        {110, 999},
        1
    },
    {
        {110, 999},
        {9999, 999},
        1
    },
    {
        {110, 999},
        {1000, 999},
        1
    },
    {
        {1000, 999},
        {1010, 999},
        1
    },
    {
        {1001, 999},
        {1010, 999},
        0
    },
};

static RegTest tests04[] = {
    {
        {10, 1000},
        {80, 1000},
        0
    },
    {
        {80, 1000},
        {100, 1000},
        1
    },
    {
        {80, 1000},
        {110, 1000},
        1
    },
    {
        {100, 1000},
        {110, 1000},
        1
    },
    {
        {110, 1000},
        {9999, 1000},
        1
    },
    {
        {110, 1000},
        {1000, 1000},
        1
    },
    {
        {1000, 1000},
        {1010, 1000},
        1
    },
    {
        {1001, 1000},
        {1010, 1000},
        0
    },
};

static RegTest tests05[] = {
    {
        {10, 1010},
        {80, 1010},
        0
    },
    {
        {80, 1010},
        {100, 1010},
        0
    },
    {
        {80, 1010},
        {110, 1010},
        0
    },
    {
        {100, 1010},
        {110, 1010},
        0
    },
    {
        {110, 1010},
        {9999, 1010},
        0
    },
    {
        {110, 1010},
        {1000, 1010},
        0
    },
    {
        {1000, 1010},
        {1010, 1010},
        0
    },
    {
        {1001, 1010},
        {1010, 1010},
        0
    },
};

DEF_test(time) {
    DEF_case(horizontal) {
        RECT rt = {0};
        rt.a.x = 100;
        rt.a.y = 100;
        rt.b.x = 1000;
        rt.b.y = 1000;
        int rc = 0;
        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
        {
            rc = RectOverlapWithLine(rt, tests[i].a, tests[i].b);
            EXPECT_EQ(tests[i].ret, rc);
        }
    }

    DEF_case(h_01) {
        RECT rt = {0};
        rt.a.x = 100;
        rt.a.y = 100;
        rt.b.x = 1000;
        rt.b.y = 1000;
        int rc = 0;
        for (int i = 0; i < sizeof(tests01)/sizeof(tests01[0]); i++)
        {
            rc = RectOverlapWithLine(rt, tests01[i].a, tests01[i].b);
            EXPECT_EQ(tests01[i].ret, rc);
        }
    }

    DEF_case(h_02) {
        RECT rt = {0};
        rt.a.x = 100;
        rt.a.y = 100;
        rt.b.x = 1000;
        rt.b.y = 1000;
        int rc = 0;
        for (int i = 0; i < sizeof(tests02)/sizeof(tests02[0]); i++)
        {
            rc = RectOverlapWithLine(rt, tests02[i].a, tests02[i].b);
            EXPECT_EQ(tests02[i].ret, rc);
        }
    }

    DEF_case(h_03) {
        RECT rt = {0};
        rt.a.x = 100;
        rt.a.y = 100;
        rt.b.x = 1000;
        rt.b.y = 1000;
        int rc = 0;
        for (int i = 0; i < sizeof(tests03)/sizeof(tests03[0]); i++)
        {
            rc = RectOverlapWithLine(rt, tests03[i].a, tests03[i].b);
            EXPECT_EQ(tests03[i].ret, rc);
        }
    }

    DEF_case(h_04) {
        RECT rt = {0};
        rt.a.x = 100;
        rt.a.y = 100;
        rt.b.x = 1000;
        rt.b.y = 1000;
        int rc = 0;
        for (int i = 0; i < sizeof(tests04)/sizeof(tests04[0]); i++)
        {
            rc = RectOverlapWithLine(rt, tests04[i].a, tests04[i].b);
            EXPECT_EQ(tests04[i].ret, rc);
        }
    }

    DEF_case(h_05) {
        RECT rt = {0};
        rt.a.x = 100;
        rt.a.y = 100;
        rt.b.x = 1000;
        rt.b.y = 1000;
        int rc = 0;
        for (int i = 0; i < sizeof(tests05)/sizeof(tests05[0]); i++)
        {
            rc = RectOverlapWithLine(rt, tests05[i].a, tests05[i].b);
            EXPECT_EQ(tests05[i].ret, rc);
        }
    }
}

} // namespace test

int main(int argc, char** argv) {
    unitest::init(argc, argv);
    unitest::run_all_tests();
    return 0;
}