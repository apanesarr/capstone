#define MOTOR_TYPE 'M'
#define MOTOR_READY 'R'
#define INIT 'I'
#define TEMP_TYPE 'T'
#define FAILURE 'F'

struct payloadMsg {
    unsigned int msg_id;
    char data [32];
};


struct SendMessage{
  unsigned int msgID;
  unsigned int recID;
  char msgType;
  char value [32];
};


struct TempData{
	float Temp;
	float Humidity;
};


struct initData {
	int msgID;
};
