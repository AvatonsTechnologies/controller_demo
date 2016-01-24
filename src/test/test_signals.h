#include <cxxtest/TestSuite.h>

#include "../signals.h"

class TestSignal : public CxxTest::TestSuite {
public:
    void test_insert_index() {
        cyclic_buf_t buf = EMPTY_BUF;
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            cyclic_insert(&buf, (float)i);
        }
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            TS_ASSERT_EQUALS(cyclic_index(&buf, i), SIGNAL_LENGTH - 1 - i);
        }
    }

    void test_insert_index2() {
        cyclic_buf2_t buf = EMPTY_BUF2;
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            float arr[2] = {(float)i, (float)i + 1};
            cyclic_insert2(&buf, arr);
        }
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            TS_ASSERT_EQUALS(cyclic_index2(&buf, i)[0], SIGNAL_LENGTH - 1 - i);
            TS_ASSERT_EQUALS(cyclic_index2(&buf, i)[1], SIGNAL_LENGTH - 1 - i + 1);
        }
    }

    void test_convolve_with() {
        float ker[4] = {1, 2, 3, 4};
        cyclic_buf_t buf = EMPTY_BUF;
        cyclic_buf_t out = EMPTY_BUF;
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            cyclic_insert(&buf, (float)i);
        }
        convolve_with(&buf, 0, SIGNAL_LENGTH - 4, ker, 4, &out);
        for (int i = 0; i < SIGNAL_LENGTH - 4; i++) {
            int j = SIGNAL_LENGTH - 1 - i;
            TS_ASSERT_EQUALS(cyclic_index(&out, i), j + 2 * (j - 1) + 3 * (j - 2) + 4 * (j - 3));
        }
    }

    void test_convolve_point() {
        float ker[4] = {1, 2, 3, 4};
        cyclic_buf_t buf = EMPTY_BUF;
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            cyclic_insert(&buf, (float)i);
        }
        int j = SIGNAL_LENGTH - 1;
        TS_ASSERT_EQUALS(convolve_point(&buf, 0, ker, 4), j + 2 * (j - 1) + 3 * (j - 2) + 4 * (j - 3));
    }
};
