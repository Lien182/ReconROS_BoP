import rclpy
from rclpy.node import Node

import time
import struct
import random

import os

import numpy as np

from cv_bridge import CvBridge
from sensor_msgs.msg import Image
import cv2
from cv_bridge import CvBridge


class ImagePubSub(Node):

    def __init__(self):
        super().__init__('Image')
        
        self.publisher_ = self.create_publisher(Image, '/image_filtered', 10)
        self.cv_image = cv2.imread('image.jpg') ### an RGB image 
        self.cv_image = cv2.cvtColor(self.cv_image, cv2.COLOR_BGR2BGRA)
        self.bridge = CvBridge()
        self.tstart = 0
        self.tend = 0       
        timer_period = 0.1  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.msg = self.bridge.cv2_to_imgmsg(np.array(self.cv_image), "bgra8")
        self.cnt = 0


    def timer_callback(self):
        self.publisher_.publish(self.msg)
        print("Data published")
#        self.tstart = time.time()
            
def main(args=None):
    rclpy.init(args=args)

    pubsub = ImagePubSub()
    rclpy.spin(pubsub)




if __name__ == '__main__':
    main()
