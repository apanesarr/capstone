#ifndef parameters_h
#define parameters_h

#define GORDO_2

/**
 *  GORDO 1 SETTINGS
 */

#ifdef GORDO_1
#define REC_ID 0

#define RADIUS_SCALE_FACTOR 1.15
#define WHEEL_RADIUS 31 * RADIUS_SCALE_FACTOR
#define TICKS_PER_REV 166.5

/* Power offset for right motor */
#define RIGHT_OFFSET 1

/* Rate in ms for compensating between motors */
#define UPDATE_RATE 50
#define P_GAIN 7
#define D_GAIN 3
#define MAX_OFFSET 39

/**
 *  GORDO 2 SETTINGS
 */

#elif defined GORDO_2
#define REC_ID 1

#define RADIUS_SCALE_FACTOR 1.15
#define WHEEL_RADIUS 31 * RADIUS_SCALE_FACTOR
#define TICKS_PER_REV 166.5

/* Power offset for right motor */
#define RIGHT_OFFSET 0.86

/* Rate in ms for compensating between motors */
#define UPDATE_RATE 50
#define P_GAIN 7
#define D_GAIN 3
#define MAX_OFFSET 39

#endif

#endif
