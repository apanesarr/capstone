#define SET_GYRO_TYPE 'H'
#define MOTOR_TYPE 'M'
#define GET_GYRO_TYPE 'G'
#define STOP_TYPE 'S'
#define INIT 'I'
#define TEMP_TYPE 'T'

// typedef enum motorState_e {
//     FORWARD,
//     REVERSE,
//     STEER_LEFT,
//     STEER_RIGHT,
//     STOP
// };

struct payloadMsg {
    unsigned msg_id;
    char data [32];
};


