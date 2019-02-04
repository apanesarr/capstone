from collections import deque
import threading
import time
import queue

from imutils.video import VideoStream
import numpy as np
import cv2
import imutils
import argparse

import Communication
import Parameters
import Location

import Reader
import Writer

def handleComEvent(queue):
    if queue.empty():
        return

    item = queue.get()

    if not (item["recipient"] == "MAIN"):
        queue.put(item)
        return

    elif (item["command"] == "RECORD_MEASUREMENT"):
        measurement = item["measurement"]

        measurements.append({
            "location": rawLocation,
            "measurement": measurement
        })

        location.hasTarget = False

if __name__ == "__main__":
    # Parse command line args
    ap = argparse.ArgumentParser()
    
    ap.add_argument(
        "-v", "--video",
        help="path to the (optional) video file"
    )

    ap.add_argument(
        "-b", "--buffer",
        type=int,
        default=64,
        help="max buffer size"
    )

    args = vars(ap.parse_args())

    # If a video path was not supplied, grab the reference
    # to the webcam
    if not args.get("video", False):
        vs = VideoStream(src=0).start()

    # Otherwise, grab a reference to the video file
    else:
        vs = cv2.VideoCapture(args["video"])

    # Set up Communication thread

    queue_writer = queue.Queue()
    event_writer = threading.Event()
    event_writer.queue = queue_writer

    queue_reader = queue.Queue()
    event_reader = threading.Event()
    event_reader.queue = queue_reader 

    measurements = []
    recieved = []

    reader = Reader.Reader(Parameters.SERIAL_PORT_IN, Parameters.SERIAL_BODE, queue_reader, recieved)
    writer = Writer.Writer(Parameters.SERIAL_PORT_OUT, Parameters.SERIAL_BODE, queue_writer, recieved)

    reader.start()
    writer.start()

    rawLocation = (-1, -1)

    nextLocation = (-1, -1)

    # Let the webcam warm up
    time.sleep(2.0)

    location = Location.Location()

    # Main loop
    while True:
        handleComEvent(queue_reader)

        frame = vs.read()

        if frame is None:
            break

        frame = imutils.resize(
            frame,
            width=Parameters.CAMERA_RES_W,
            height=Parameters.CAMERA_RES_H
        )

        blurred = cv2.GaussianBlur(frame, (11, 11), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

        # Apply color filtering
        mask = cv2.inRange(hsv, Parameters.COLOR_LOWER, Parameters.COLOR_UPPER)
        mask = cv2.erode(mask, None, iterations=2)
        mask = cv2.dilate(mask, None, iterations=2)

        cnts = cv2.findContours(
            mask.copy(),
            cv2.RETR_EXTERNAL,
            cv2.CHAIN_APPROX_SIMPLE
        )

        cnts = imutils.grab_contours(cnts)
        center = None

        if len(cnts) > 0:
            c = max(cnts, key=cv2.contourArea)

            ((x, y), radius) = cv2.minEnclosingCircle(c)

            M = cv2.moments(c)

            center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

            if radius > Parameters.MIN_RADIUS and radius < Parameters.MAX_RADIUS:
                cv2.circle(
                    frame,
                    (int(x), int(y)),
                    int(radius),
                    (0, 255, 255),
                    2
                )

                cv2.circle(
                    frame,
                    center,
                    5,
                    (0, 0, 255),
                    -1
                )

                location.updateLocation(center)
        
        if location.reachedTarget():
            queue_writer.put({
                "recipient": "COM",
                "command": "GET_TEMP"
            })
            location.hasTarget = True

        if not location.hasTarget:
            loc = location.nextLocation()

            queue_writer.put({
                "recipient": "COM",
                "command": "SEND_NEW_LOCATION",
                "angle": location.angle(rawLocation, loc)
            })

            location.target = loc
            location.hasTarget = True

        # show the frame to our screen
        cv2.imshow("Frame", frame)
        key = cv2.waitKey(1) & 0xFF

        # if the "q" key is pressed, stop the loop
        if key == ord("q"):
            break

        time.sleep(0.1)

    vs.stop()

    cv2.destroyAllWindows()