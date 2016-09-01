from picamera import PiCamera
from time import sleep
from PIL import Image

camera = PiCamera()
camera.start_preview()
camera.annotate_text = 'Hello World!'
sleep(20)
camera.stop_preview()
