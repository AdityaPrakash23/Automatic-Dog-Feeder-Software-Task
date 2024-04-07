import rclpy
from rclpy.node import Node

import serial

from std_msgs.msg import Bool

ard_sensor_actuator = '/dev/ttyUSB0'

class MainPublisher(Node):

	def __init__(self):
		#Creating publisher that intiates
		super().__init__('main_publisher')
		self.publisher_ = self.create_publisher(Bool, 'Record', 10)
		timer_period = 1
		#Creating timer to initiate publisher every 5 secs
		self.timer = self.create_timer(timer_period, self.timer_callback)

	def timer_callback(self):
		msg = Bool()
		#Read serial message from Sensor Arduino
		arduino = serial.Serial(ard_sensor_actuator,9600,timeout = 0,parity = serial.PARITY_EVEN, rtscts=1)
		arduino.open()
		m = arduino.readline()
		#Parse message about dog's presence and sending enable or disable to feeders
		if  m[0] == '1':
			msg.data = True
			#Trigger recording function
			self.publisher_.publish(msg)
			self.get_logger().info('Publishing: "%s"' % msg.data)
			if m[1] == '0':
				#Bowls are not yet full
				arduino.write("1".encode())
			elif m[1] == '1':
				#Bowls are full
				arduino.write("0".encode())
		else:
			msg.data = False
			#Trigger recording function
			self.publisher_.publish(msg)
			self.get_logger().info('Publishing: "%s"' % msg.data)
			arduino.write("0",encode())

def main(args=None):
	rclpy.init(args=args)

	main_publisher = MainPublisher()
	rclpy.spin(main_publisher)

	rclpy.shutdown()


if __name__ == '__main__':
	main()
