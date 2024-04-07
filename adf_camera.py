import rclpy
from rclpy.node import Node

from std_msgs.msg import Bool

import cv2

vid_count = 0
str_location = '~/recordings'
flag = True

class CameraSubscriber(Node):

	def __init__(self):
		super().__init__('minimal_subscriber')
		self.subscription = self.create_subscription(
		Bool,
		'Record',
		self.listener_callback,
		10)
		self.subscription  # prevent unused variable warning

	def listener_callback(self, msg):
		self.get_logger().info('I heard: "%s"' % msg.data)

		if msg.data == True:
			#Initiate a 5min recording
			video = cv2.VideoCapture(0)

			#Setting up videocapture action
			frame_width = int(video.get(3))
			frame_height = int(video.get(4))

			size = (frame_width, frame_height)

			filename = str_location + "cutedoggoeating" + str(vid_count) + ".avi"
			vid_count += 1

			result = cv2.VideoWriter(filename,cv2.VideoWriter_fourcc(*'MJPG'), 10, size)

			flag = True
			while flag  == True:
				retrn, frame = video.read()
				result.write(frame)
				#Continue recording until 5mins are up
				if cv2.waitKey(300000):
					flag = False

def main(args=None):
	rclpy.init(args=args)

	camera_subscriber = CameraSubscriber()

	rclpy.spin(camera_subscriber)

	camera_subscriber.destroy_node()
	rclpy.shutdown()

if __name__ == '__main__':
	main()
