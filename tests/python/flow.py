'''
Lucas-Kanade homography tracker
===============================
Lucas-Kanade sparse optical flow demo. Uses goodFeaturesToTrack
for track initialization and back-tracking for match verification
between frames. Finds homography between reference and current views.
Usage
-----
lk_homography.py [<video_source>]
Keys
----
ESC   - exit
SPACE - start tracking
r     - toggle RANSAC
'''

import numpy as np
import cv2

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
cap.set(cv2.CAP_PROP_FPS, 30)

lk_params = dict( winSize  = (19, 19),
                  maxLevel = 2,
                  criteria = (cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03))

feature_params = dict( maxCorners = 1000,
                       qualityLevel = 0.01,
                       minDistance = 8,
                       blockSize = 19 )

def checkedTrace(img0, img1, points_prev, back_threshold = 1.0):
    # calculate optical flow from prev frame to cur frame
    points_cur, st, err = cv2.calcOpticalFlowPyrLK(img0, img1, points_prev, None, **lk_params)
    # calculate optical flow backwards
    points_prev_recalc, st, err = cv2.calcOpticalFlowPyrLK(img1, img0, points_cur, None, **lk_params)
    # calculate discripency between prev and recalculated prev points
    d = abs(points_prev-points_prev_recalc).reshape(-1, 2).max(-1)
    # set a status vector where 1 means the point is good and 0 means the point is bad
    status = d < back_threshold
    # return the new points and the status vector
    return points_cur, status

green = (0, 255, 0)
red = (0, 0, 255)

# initial feature vector
points_t0 = None
# current feature vector, with entries corresponding to initial feature vector 
points_cur = None

# set to true to find and remove outliers
use_ransac = True

# current ticks
ticks = 0
# the number of ticks between feature updates
ticks_to_update = 10

while True:
    # get the current frame
    ret, frame = cap.read()
    # convert the frame to grayscale
    frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # vis is the base layer for the visualization
    vis = frame.copy()
    if points_t0 is not None:
        # calculate the new points from optical flow and the current validity of each point
        p2, trace_status = checkedTrace(prev_frame_gray, frame_gray, points_cur)

        # update the points
        points_cur = p2[trace_status].copy()
        points_t0 = points_t0[trace_status].copy()

        # update the prev gray frame
        prev_frame_gray = frame_gray

        # if we don't have enough points from the first tick, just continue
        if len(points_t0) < 4:
            points_t0 = None
            continue

        # calculate the 2D planar homography matrix and the status of each of the points
        H, status = cv2.findHomography(points_t0, points_cur, (0, cv2.RANSAC)[use_ransac], 10.0)

        # print H

        # caluclate height and width
        height, width = frame.shape[:2]

        # the overlay image warped to new perspective, should match up
        overlay = cv2.warpPerspective(frame0, H, (width, height))

        # add the overlay to the current frame at 50% opacity
        vis = cv2.addWeighted(vis, 0.5, overlay, 0.5, 0.0)

        # plot the points and lines
        for (x0, y0), (x1, y1), good in zip(points_t0[:,0], points_cur[:,0], status[:,0]):
            if good:
                cv2.line(vis, (x0, y0), (x1, y1), (0, 128, 0))
            cv2.circle(vis, (x1, y1), 2, (red, green)[good], -1)

    else:
        # if not currently tracking features, just find good features
        p = cv2.goodFeaturesToTrack(frame_gray, **feature_params)
        if p is not None:
            for x, y in p[:,0]:
                cv2.circle(vis, (x, y), 2, green, -1)

    cv2.imshow('lk_homography', vis)

    if ticks < ticks_to_update:
        ticks+=1
    else:
        ticks = 0
        # save the original frame, for cool overlay effect
        frame0 = frame.copy()

        # set the points to be tracked for the next ticks_to_update ticks
        points_t0 = cv2.goodFeaturesToTrack(frame_gray, **feature_params)
        if points_t0 is not None:
            points_cur = points_t0
            prev_frame_gray = frame_gray

    ch = cv2.waitKey(1)
    if ch == 27:
        break
    if ch == ord('r'):
        use_ransac = not use_ransac

cv2.destroyAllWindows()