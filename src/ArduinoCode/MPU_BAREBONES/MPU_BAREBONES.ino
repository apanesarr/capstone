


// I2C Libraries 

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif


MPU6050 mpu;


#define INTERRUPT_PIN 2 
bool calibrate1 = false;
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container
float euler[3];         // [psi, theta, phi]    Euler angle container


float current = 0;
float previous = 0;
bool cal = false;

int count = 0;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void initer(){
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
  #endif
  Serial.begin(115200);
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);
  if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
    Serial.println("Calibrate");
    //calibrate();
}

void calibrate(){
  int count = 0;
  float current = 0;
  float prev = 100;
  while ( count !=5000){

  prev = current;
  
  Serial.print("\t Current : ");
  current = getOri();
  delay(3000);
  Serial.print(current);
  Serial.print("\t Prev: ");
  Serial.print(prev);
  Serial.print("\t Current - Prev = ");
  Serial.println(abs(current)-abs(prev));
  count++;
  }

}


float getOri(){
  if (!dmpReady) return;
    // reset interrupt flag and get INT_STATUS byte
   while (!mpuInterrupt && fifoCount < packetSize) {
        if (mpuInterrupt && fifoCount < packetSize) {
          // try to get out of the infinite loop 
          fifoCount = mpu.getFIFOCount();
        } 
    }
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    // get current FIFO count
   
    fifoCount = mpu.getFIFOCount();
  if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
      while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetEuler(euler, &q);
        current = euler[0] * 180/M_PI;
        Serial.print("Current: \t");
        Serial.println(current);
        if(!calibrate1){
            current = previous;
            delay(500);
            if(current == previous){
              Serial.println("Calibrated");
            }
               
        }

        
        //Serial.print("euler\t");
       // Serial.print(euler[0] * 180/M_PI);
        return (euler[0] * 180/M_PI);
       // Serial.print("\t");
       // Serial.print(euler[1] * 180/M_PI);
       // Serial.print("\t");
       // Serial.println(euler[2] * 180/M_PI); 
  }
}


void setup() {
  // put your setup code here, to run once:
  initer();

}

void loop() {
  getOri();
  //Serial.print("Euler: \t");
  //Serial.println(getOri());

}
