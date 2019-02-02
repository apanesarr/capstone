#!/usr/bin/env python3
import cv2
import sys

AREA_THRESH = 10000

class BoundaryDetect:

    def __init__(self, frame):
        self.frame, self.contours = self.find_boundaries(frame)

    def find_boundaries(self, frame):
        img = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY_INV)[1]
        mod, contours, hier = cv2.findContours(img,
                                               cv2.RETR_TREE,
                                               cv2.CHAIN_APPROX_SIMPLE)
        contours = self.filter_contours(contours)
        cv2.drawContours(frame, contours, -1, (0,255,0), 3)
        return frame, contours

    def filter_contours(self, contours):
        filtered = [cnt for cnt in contours if cv2.contourArea(cnt) > AREA_THRESH]
        return filtered


def main():

    try:
        frame = cv2.imread(sys.argv[1])
        bounds = BoundaryDetect(frame)
        frame = bounds.frame
        while True:
            cv2.imshow('Video', frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    except IndexError:
        video_capture = cv2.VideoCapture(0)
        while True:
            ret, frame = video_capture.read()
            bounds = BoundaryDetect(frame)
            frame = bounds.frame
            cv2.imshow('Video', frame)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        video_capture.release()

    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()

