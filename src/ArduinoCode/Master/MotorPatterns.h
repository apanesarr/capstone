#ifndef MotorPatterns_h
#define MotorPatterns_h

                      /* STATE    DEG  MS */
#define TEST_PATTERN_1 { FORWARD, 0,   1000 }, \
                       { LEFT,    90,  0 },    \
                       { FORWARD, 0,   1000 }, \
                       { RIGHT,   90,  0 },    \
                       { FORWARD, 0,   1000 }, \
                       { STOP,    0,   1000,}, \
                       { REVERSE, 0,   1000 }, \
                       { LEFT,    90,  0 },    \
                       { REVERSE, 0,   1000 }, \
                       { RIGHT,   90,  0 },    \
                       { REVERSE, 0,   1000 }, \
                       { STOP,    0,   1000}
#define LEN_PATTERN_1 12

#define TEST_PATTERN_2  { LEFT,    90,  0 },    \
                        { STOP,    0,   1000,}, \
                        { RIGHT,   90,  0 },    \
                        { STOP,    0,   1000,}
#define LEN_PATTERN_2 4

#endif
