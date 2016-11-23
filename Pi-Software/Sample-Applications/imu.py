from math import atan2
from random import seed, gauss
import numpy as np

class imu():
	def __init__(self, ts=0.1):
		""" An imu object that defines the true movement and simulated sensor data for a 9dof imu """
		# Real positions
		self.theta_x = 0.0
		self.theta_y = 0.0
		self.theta_z = 0.0
		
		# True sensor bias
		self.bias_x = 0.1
		self.bias_y = 0.2
		self.bias_z = 0.3

		# Process Noise
		self.processNoise = 1

		# Sensor Properties
		self.gyroStdDev_x = 0.4
		self.gyroStdDev_y = 0.4
		self.gyroStdDev_z = 0.4
		self.gyroVar_x = pow(self.gyroStdDev_x, 2)
		self.gyroVar_y = pow(self.gyroStdDev_y, 2)
		self.gyroVar_z = pow(self.gyroStdDev_z, 2)

		self.accStdDev_x = 0.4
		self.accStdDev_y = 0.4
		self.accStdDev_z = 0.4
		self.accVar_x = pow(self.accStdDev_x, 2)
		self.accVar_y = pow(self.accStdDev_y, 2)
		self.accVar_z = pow(self.accStdDev_z, 2)
		self.magStdDev_x = 0.4
		self.magStdDev_y = 0.4
		self.magStdDev_z = 0.4
		self.magVar_x = pow(self.magStdDev_x, 2)
		self.magVar_y = pow(self.magStdDev_y, 2)
		self.magVar_z = pow(self.magStdDev_z, 2)
		# time
		self.time = 0
		self.ts = ts
		
		# random seed
		seed()

		# Matrices
		self.states = np.zeros((6,1))
		self.P = np.array([[self.gyroVar_x, 0, 0, 0, 0, 0],
						   [0, self.gyroVar_x, 0, 0, 0, 0],
						   [0, 0, self.gyroVar_y, 0, 0, 0],
   						   [0, 0, 0, self.gyroVar_y, 0, 0],
						   [0, 0, 0, 0, self.gyroVar_z, 0],
   						   [0, 0, 0, 0, 0, self.gyroVar_z]
						])

		self.Q = np.array([
			[0.1, 0, 0],
			[0, 0.1, 0],
			[0, 0, 0.1]
		])

		self.R = np.array([
			[self.accStdDev_x, 0, 0, 0, 0, 0],
			[0, 			   1, 0, 0, 0, 0],
			[0, 0, self.accStdDev_y, 0, 0, 0],
			[0, 0, 0, 				 1, 0, 0],
			[0, 0, 0, 0, self.accStdDev_z, 0],
			[0, 0, 0, 0, 0, 			   1]
		])

		# Initial conditions
		self.position = np.zeros((6, 1))
		
		# Sensor values
		self.gyro_x = 0.0
		self.gyro_y = 0.0
		self.gyro_z = 0.0

		self.acc_x = 0.0
		self.acc_y = 0.0
		self.acc_z = 0.0

		self.mag_x = 0.0
		self.mag_y = 0.0
		self.mag_z = 0.0

	def update_actual_position(self):
		self.theta_x = self.theta_x + self.wx*self.ts
		self.theta_y = self.theta_y + self.wy*self.ts
		self.theta_z = self.theta_z + self.wz*self.ts
		self.time = self.time + self.ts
		self.actual_pos = np.array([self.theta_x, self.theta_y, self.theta_z])

	def move_body(self, ax, ay, az):
		self.ax = ax + gauss(0, self.processNoise)
		self.ay = ay + gauss(0, self.processNoise)
		self.az = az + gauss(0, self.processNoise)
		self.a = np.array([self.ax, self.ay, self.az])
		self.wx = self.ax*self.ts
		self.wy = self.ay*self.ts
		self.wz = self.az*self.ts
		self.w = np.array([self.wx, self.wy, self.wz])
		self.update_actual_position()

	def get_predict_sensor(self):
		# Get Gyroscope readings
		self.gyro_x = self.wx + gauss(0, self.gyroStdDev_x) - self.bias_x
		self.gyro_y = self.wy + gauss(0, self.gyroStdDev_y) - self.bias_y
		self.gyro_z = self.wz + gauss(0, self.gyroStdDev_z) - self.bias_z
		self.gyro = np.array([self.gyro_x, self.gyro_y, self.gyro_z])
	
	def get_update_sensor(self, acc_mag='acc'):
		if acc_mag == 'acc':
			self.acc_x = self.ax + gauss(0, self.accStdDev_x)
			self.acc_y = self.ay + gauss(0, self.accStdDev_y)
			self.acc_z = self.az + gauss(0, self.accStdDev_z)
			self.acc = np.array([self.acc_x, self.acc_y, self.acc_z])
		elif acc_mag == 'mag':
			self.mag_x = self.ax + gauss(0, self.magStdDev_x)
			self.mag_y = self.ay + gauss(0, self.magStdDev_y)
			self.mag_z = self.az + gauss(0, self.magStdDev_z)
			self.mag = np.array([self.mag_x, self.mag_y, self.mag_z])

		else:
			pass

	def update_prediction_matrices(self):
		self.A = np.array([
			[1.0, -self.ts, 0, 0, 0, 0],
			[0, 1.0, 		0, 0, 0, 0],
			[0, 0, 1.0, -self.ts, 0, 0],
			[0, 0, 0, 		 1.0, 0, 0],
			[0, 0, 0, 0, 1.0, -self.ts],
			[0, 0, 0, 0, 0, 	   1.0]
		])

		self.B = np.array([
			[self.ts, 0, 0, 0, 0, 0],
			[0, 0, 		 0, 0, 0, 0],
			[0, 0, self.ts, 0, 0, 0],
			[0, 0, 		 0, 0, 0, 0],
			[0, 0, 0, 0, self.ts, 0],
			[0, 0, 		 0, 0, 0, 0]
		])
		self.inputs = np.array([
			[self.gyro_x],
			[0],
			[self.gyro_y],
			[0],
			[self.gyro_z],
			[0]
		])

		self.process_model = np.dot(self.A, self.states) + np.dot(self.B, self.inputs)
		self.states = self.process_model

		self.G = np.array([
			[self.ts, 0, 0],
			[0, 0, 		 0],
			[0, self.ts, 0],
			[0, 0, 		 0],
			[0, 0, self.ts],
			[0, 0, 		 0]
		])

	def update_correction_matrices(self):
		self.Z = np.array([
			[atan2(self.acc_y,self.acc_z)],
			[0],
			[atan2(self.acc_x,self.acc_z)],
			[0],
			[atan2(self.acc_y,self.acc_x)],
			[0]
		])

		self.H = np.array([
			[1, 0, 0, 0, 0, 0],
			[0, 0, 0, 0, 0, 0],
			[0, 0, 1, 0, 0, 0],
			[0, 0, 0, 0, 0, 0],
			[0, 0, 0, 0, 1, 0],
			[0, 0, 0, 0, 0, 0]
		])

		self.V = np.array([
			[1, 0, 0],
			[0, 0, 0],
			[0, 1, 0],
			[0, 0, 0],
			[0, 0, 1],
			[0, 0, 0]
		])