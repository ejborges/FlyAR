cmake_minimum_required(VERSION 3.3)
project(MPU6050DMP6_HMC5883L)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    helper_3dmath.h
    HMC5883L.cpp
    HMC5883L.h
    I2Cdev.cpp
    I2Cdev.h
    MPU6050.cpp
    MPU6050.h
    MPU6050_6Axis_MotionApps20.h
    MPU6050DMP6_HMC5883L.ino)

add_executable(MPU6050DMP6_HMC5883L ${SOURCE_FILES})