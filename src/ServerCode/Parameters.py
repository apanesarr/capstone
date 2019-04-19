SURVEY_TIME = 10 * 60 # Max time, in seconds, for the surveying process

# Parameters - Object Tracking #

MIN_RADIUS  = 10
MAX_RADIUS  = 50
COLOR_LOWER = (29, 86, 6)
COLOR_UPPER = (64, 255, 255)

# Parameters - Geometry #

NUM_REGIONS_X = 10
NUM_REGIONS_Y = 10

REGION_SIZE_X = 10
REGION_SIZE_Y = 10

# Parameters - Serial I/O #

SERIAL_PORT_IN  = '/dev/cu.usbmodem14101'
SERIAL_PORT_OUT = '/dev/cu.usbmodem14101'
SERIAL_BODE     = 115200
SERIAL_TIMEOUT  = 10000

PAIR_TIME       = 10 # Amount of time to wait for pairing
RESEND_DELAY    = 1  # Amount of time in seconds to wait before resending a message
DEAD_DELAY      = 10 # Amount of inactivity in seconds before an insect is considered dead

# Parameters - Command Codes #

MSG_TYPE_REQUEST_MEASUREMENT    : "RequestMeasurement"
MSG_TYPE_SET_STATE              : "SetState"
MSG_TYPE_GET_LOCATION           : "GetLocation" 

# Parameters - Response Codes #

RES_OK              = "R"
RES_TEMP            = "T"

# Parameters - Insects #

NUM_INSECTS         = 1

# Parameters - WebSockets #

SOCKET_HOST = '192.168.43.91'
SOCKET_PORT = 8095

RANDOM_RANGE = 5