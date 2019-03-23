#define SET_GYRO_TYPE 'H'
#define MOTOR_TYPE 'M'
#define GET_GYRO_TYPE 'G'
#define STOP_TYPE 'S'
#define INIT 'I'
#define TEMP_TYPE 'T'


struct payloadMsg {
    unsigned msg_id;
    char data [32];
};


struct MotorPayload {
  unsigned int msg_id;
  motorSettings_t setting;
};


struct SensorPayload {
  unsigned int msg_id;
  float temp;
};

struct InitPayload {
  unsigned int msg_id;
};
