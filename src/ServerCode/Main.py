import threading
import time
import queue

from imutils.video import VideoStream
import numpy as np
import cv2
import imutils

from Communication import runCommunication
import Parameters
import Location

def handleEventQueue(queue):
    handleComEvent(queue)

def handleComEvent(queue):
    if queue.empty():
        return

    item = queue.get()

    if not (item.recipient == 'MAIN'):
        queue.put(item)
        return

    elif (item.command == 'RECORD_MEASUREMENT'):
        # Record a measurement

    elif (item.command == 'GET_NEXT_LOCATION'):
        newCommand = {
            'recipient': 'COM',
            'command': 'SEND_NEW_LOCATION',
            'location': nextLocation()
        }

def createRegions():
    regions = np.full([NUM_REGIONS_X, NUM_REGIONS_Y], {})

    for i in range(0, self.regions.shape[0]):
    for j in range(0, self.regions.shape[1]):
        region = {'visited': False }

        x = (i / NUM_REGIONS_X) * (1.5) * (regionSizeX)
        y = (j / NUM_REGIONS_Y) * (1/5) * (regionSizeY)

        region['center'] = (x, y)

        self.regions[i][j] = region

    return regions

if __name__ == '__main__':
    queue_communication = queue.Queue()
    event_communication = threading.Event()
    event_communication.queue = queue_communication

    thread_communication = threading.Thread(
        name='communication',
        target=runCommunication,
        args=(event_communication,)
    )

    thread_communication.start()

    regions = createRegions()
    rawLocation = (-1, -1)

    vs = VideoStream(src=0).start()

    # Let the webcam warm up
    time.sleep(2.0)

    pts = deque(maxlen=args["buffer"])

    while True:
        handleEventQueue(queue)

        frame = vs.read()

        if frame is None:
            break

        frame = imutils.resize(
            frame,
            width=CAMERA_RES_W,
            height=CAMERA_RES_H
        )

        blurred = cv2.GaussianBlur(frame, (11, 11), 0)
        hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

        mask = cv2.inRange(hsv, COLOR_LOWER, COLOR_UPPER)
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
            # find the largest contour in the mask, then use
            # it to compute the minimum enclosing circle and
            # centroid
            c = max(cnts, key=cv2.contourArea)
            ((x, y), radius) = cv2.minEnclosingCircle(c)
            M = cv2.moments(c)
            center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

            # only proceed if the radius meets a minimum size
            if radius > MIN_RADIUS and radius < MAX_RADIUS:
                # draw the circle and centroid on the frame,
                # then update the list of tracked points
                cv2.circle(frame, (int(x), int(y)), int(radius),
                            (0, 255, 255), 2)
                cv2.circle(frame, center, 5, (0, 0, 255), -1)

                rawLocation = center

        # update the points queue
        pts.appendleft(center)

        # loop over the set of tracked points
        for i in range(1, 1):  # for i in range(1, len(pts)):
            # if either of the tracked points are None, ignore
            # them
            if pts[i - 1] is None or pts[i] is None:
                continue

            # otherwise, compute the thickness of the line and
            # draw the connecting lines
            thickness = int(np.sqrt(args["buffer"] / float(i + 1)) * 2.5)
            cv2.line(frame, pts[i - 1], pts[i], (0, 0, 255), thickness)

        # show the frame to our screen
        cv2.imshow("Frame", frame)
        key = cv2.waitKey(1) & 0xFF

        # if the 'q' key is pressed, stop the loop
        if key == ord("q"):
            break

        time.sleep(0.5)

    vs.stop()

    cv2.destroyAllWindows()