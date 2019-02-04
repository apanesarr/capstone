# Parameters - Object Tracking #

MIN_RADIUS = 10
MAX_RADIUS = 50
COLOR_LOWER = (29, 86, 6)
COLOR_UPPER = (64, 255, 255)

# Parameters - Geometry #

CAMERA_RES_W = 500
CAMERA_RES_H = 500

COVERAGE_AREA_X_MIN = 50
COVERAGE_AREA_Y_MIN = 50
COVERAGE_AREA_X_MAX = 450
COVERAGE_AREA_Y_MAX = 450

NUM_REGIONS_X = 10
NUM_REGIONS_Y = 10

# Parameters - Serial I/O #

SERIAL_PORT_IN  = '/dev/cu.usbmodem14101'
SERIAL_PORT_OUT = '/dev/cu.usbmodem14101'
SERIAL_BODE     = 115200
SERIAL_TIMEOUT  = 10000

# Parameters - Command Codes #

CMD_SET_GYRO        = "H"
CMD_START_MOTOR     = "M"
CMD_GET_GYRO        = "G"
CMD_STOP_MOTOR      = "S"
CMD_INIT            = "I"
CMD_GET_TEMP        = "T"

# Parameters - Response Codes #

RES_OK              = "R"
RES_TEMP            = "T"