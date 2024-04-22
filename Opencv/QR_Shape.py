import cv2
import numpy as np
import math
from pyzbar.pyzbar import decode
from datetime import datetime 
import time

class QRcode:
    def qr_decode(self,frame):

        dark_frame = cv2.convertScaleAbs(frame, alpha=0.1, beta=0)

        QR = {}

        # Decode QR codes in the frame
        decoded_objects = decode(dark_frame)

        for obj in decoded_objects:

            #if obj is not None:
            # QR 코드 데이터 출력
            QR = {"QR" : obj.data.decode('utf-8')}
            #else:
                #QR["QR"] = " "
            # Optionally, draw a rectangle around the QR code in the frame
            cv2.rectangle(frame, (obj.rect.left, obj.rect.top), 
                          (obj.rect.width + obj.rect.left, obj.rect.height + obj.rect.top), 
                          (255, 0, 0), 2)

            return QR

class Shapes:
    def shapes_decode(self,img):


        rec_detected = "" 

        # 이미지를 HSV로 변환
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

        # 빨간색, 노란색, 초록색, 파란색의 색상 범위 설정
        lower_red = np.array([0, 100, 100])
        upper_red = np.array([180, 255, 255])

        lower_yellow = np.array([15, 50, 50])
        upper_yellow = np.array([30, 255, 255])

        lower_green = np.array([40, 40, 40])
        upper_green = np.array([80, 255, 255])

        lower_blue = np.array([90, 100, 100])
        upper_blue = np.array([130, 255, 255])

        lower_orange = np.array([0,50,50])
        upper_orange = np.array([30,255,255])

        # 각 색상에 대한 마스크 생성
        mask_red = cv2.inRange(hsv, lower_red, upper_red)
        mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
        mask_green = cv2.inRange(hsv, lower_green, upper_green)
        mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)
        mask_orange = cv2.inRange(hsv, lower_orange, upper_orange)

        # 각 마스크를 합치기
        mask =  mask_green | mask_red | mask_yellow | mask_blue | mask_orange

        # 마스크를 이미지에 적용하여 색상을 검출
        masked_image = cv2.bitwise_and(img, img, mask=mask)

        dark_frame = cv2.convertScaleAbs(masked_image, alpha=0.8, beta=2)
        edge_img = cv2.Canny(dark_frame, 100, 200)

        contours, _ = cv2.findContours(edge_img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
       
        
        for contour in contours:
            approx = cv2.approxPolyDP(contour, cv2.arcLength(contour, True) * 0.02, True)

            if len(approx) == 4 and 23500 < abs(cv2.contourArea(approx)) > 23800: #and cv2.isContourConvex(approx):

                for i in range(4):
                    cv2.circle(img, tuple(approx[i][0]), 3, (255, 0, 0), 3)

                for i in range(4):
                    cv2.line(img, tuple(approx[i][0]), tuple(approx[(i + 1) % 4][0]), (0, 0, 255), 2)                

                #cv2.imwrite('/home/nsf/사진/rectangle.jpg', img)
                #cnt += 1
                rec_detected = "rectangle"
                break

            

        circles = cv2.HoughCircles(
            edge_img, cv2.HOUGH_GRADIENT, dp=1, minDist=100, param1=200, param2=25, minRadius=60, maxRadius=90)    

        if circles is not None and len(circles) > 0:
            circles = np.round(circles[0, :]).astype("int")

            for (x, y, r) in circles:
                cv2.circle(img, (x, y), r, (0, 255, 0), 3)

            for circle in circles:
                area = np.pi * circle[2] * circle[2]
                #print(area)
                if 13100 < area < 14800:
                    rec_detected = "circle"
                    break

        #print(rec_detected)
        shape_result = {}
        if rec_detected == "rectangle":
            shape_result["Shape"] = "Success"
        elif rec_detected == "circle":
            shape_result["Shape"] = "Error"
        else:
            shape_result["Shape"] = "Failed"

        return shape_result



if __name__ == "__main__":

    qr = QRcode()
    shape = Shapes()
       
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1024)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 768)
    cap.set(cv2.CAP_PROP_FPS, 30)

    QR = None
    shape_result = None

    count = 0
    for count in range(10):
        ret, frame = cap.read()
        if not ret:
            print("캡쳐 오류")
            continue

        QR = qr.qr_decode(frame)

        #print(f"count: {count}")
        shape_result = shape.shapes_decode(frame)
        if shape_result["Shape"] != "Failed":
            break
        time.sleep(0.2)

        #time.sleep(3)
        #if QR and shape_result and QR != {"QR" :" "} and shape_result != {"shape" : " "}:
        #print(shape_result)
    if QR is not None and shape_result is not None:
        combined_result = {**QR, **shape_result}
        #print(combined_result)
    elif shape_result["Shape"] == "Error":
        QR = {"QR": ""}
        combined_result = {**QR, **shape_result}
        #print(combined_result)
    else:
        QR = {"QR": ""}
        shape_result["Shape"] = "Failed"
        combined_result = {**QR, **shape_result}
        #print(combined_result)


    print(combined_result)


        #ret, frame = cap.read()
        #cv2.imshow('cam', frame)
        #cv2.imwrite(f'/home/nsf/사진/img.jpg', frame)
        #cv2.imshow('cam', frame)


