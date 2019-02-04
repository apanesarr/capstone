import serial
import threading
from time import sleep

class ReadSerial(threading.Thread):
    def __init__(self, port, bps):
        self.port = port
        self.bps = bps
        super().__init__()

    def run(self):
        with serial.Serial(self.port, self.bps) as s_read:
            while True:
                if (s_read.inWaiting() > 0):
                    print("INCOMING:", s_read.readline())
                sleep(0.01)

class WriteSerial(threading.Thread):
    def __init__(self, port, bps):
        self.port = port
        self.bps = bps
        super().__init__()

    def run(self):
        sleep(5.0)
        with serial.Serial(self.port, self.bps) as s_write:
            while True:
                s_write.write(b"Hello World\r\n")
                print("OUTGOING: MSG")
                sleep(2.0)


read_thread = ReadSerial("/dev/cu.usbserial-A500YYFI", 115200)
write_thread = WriteSerial("/dev/cu.usbmodem14201", 115200)

read_thread.start()
write_thread.start()