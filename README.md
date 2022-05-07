# Face Recognition and Internet of Things (PWM LED Control)
We have used a Raspberry Pi 4,  Pi Camera, a Lidar/IR sensor and an LED in this project. When the Lidar/IR sensor detects presence, we activate the Pi camera to take an image and save it. We process this image to detect faces and recognise if any. 

Once the recognition is complete we run the LED PWM controls to show that access was granted. 

## Sensors 

The Lidar/IR sensors are connected via I2C to the Raspberry Pi. We have used the Pi Camera and attached it to the CSI port on the Raspberry Pi using the ribbon cable.

## Face Recognition using OpenCV

For the facial recognition, we are using OpenCV. The OpenCV Cascade Classifier is used to detect the number of faces. If more than one person is detecting it will not move on to the recognition. This is a project for authentication, so we need to ensure that only a recognised (or authenticated) person can enter. If one face is detected, we try to recognise the person and print out the results.

## Controlling the LED 

For the LED, we are demonstrating the PWM control. 
