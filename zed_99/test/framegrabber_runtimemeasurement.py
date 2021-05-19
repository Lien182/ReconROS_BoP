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
        
        self.tstart = 0
        self.tend = 0       
        self.subscription = self.create_subscription(Image,'/image_raw',self.listener_callback,  10)
        self.subscription  # prevent unused variable warning
        timer_period = 0.5  # seconds
        self.cnt = 0
        self.bridge = CvBridge()


    def listener_callback(self, msg):
        cv_image = self.bridge.imgmsg_to_cv2(msg, "bgra8")
        cv2.imshow("Image window", cv_image)
        cv2.waitKey(3)
        self.tend = time.time()
        print('%f' % (self.tend - self.tstart))
        self.cnt += 1
        if self.cnt == 1000:
            self.timer.cancel()
            self.destroy_node()
            exit()
        
            
def main(args=None):
    rclpy.init(args=args)

    pubsub = ImagePubSub()
    rclpy.spin(pubsub)




if __name__ == '__main__':
    main()
