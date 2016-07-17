#include <cxxtest/TestSuite.h>

#include "../signals.h"

class TestSignal : public CxxTest::TestSuite {
public:
    void test_insert_index() {
        CyclicBuf buf;
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            buf.insert(static_cast<float>(i));
        }
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            TS_ASSERT_EQUALS(buf[i], static_cast<float>(SIGNAL_LENGTH - 1 - i));
        }
    }

    void test_convolve_point() {
        float ker[4] = {1, 2, 3, 4};
        CyclicBuf buf;
        for (int i = 0; i < SIGNAL_LENGTH; i++) {
            buf.insert(static_cast<float>(i));
        }
        for (int j = 0; j < SIGNAL_LENGTH - 4; j++) {
            int x = SIGNAL_LENGTH - j - 1;
            TS_ASSERT_EQUALS(buf.convolve_point(ker, 4, j), x + 2 * (x - 1) + 3
                    * (x - 2) + 4 * (x - 3));
        }
    }
};
