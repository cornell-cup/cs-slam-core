import numpy as np
import cv2

cap = cv2.VideoCapture(0)

while(1):
  ret,img = cap.read()
  if img == None:
      continue

  # Initiate ORB detector
  # orb = cv2.ORB_create()

  surf = cv2.SURF(400)

  # find the keypoints with ORB
  # kp = orb.detect(img,None)
  kp, des = surf.detectAndCompute(img,None)

  # compute the descriptors with ORB
  # kp, des = orb.compute(img, kp)

  img2 = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

  # draw only keypoints location,not size and orientation
  img2 = cv2.drawKeypoints(img, kp, img2, color=(0,255,0), flags=0)
  cv2.imshow("frame", img2)

  k = cv2.waitKey(1) & 0xff
  if k == 27:
      break

cv2.destroyAllWindows()
cap.release()