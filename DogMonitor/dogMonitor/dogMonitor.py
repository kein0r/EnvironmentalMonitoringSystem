#!/usr/bin/env python
#
# Based on example found at https://github.com/tensorflow/examples/blob/master/lite/examples/image_classification/raspberry_pi/classify_picamera.py
#
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import argparse
import io
import time
import picamera

from PIL import Image
import tensorflow as tf

import logging

# Because filename and class name are identical
from TelegramBot import TelegramBot

# Import token from config file
import config


def load_labels(path):
    with open(path, 'r') as f:
        return {i: line.strip() for i, line in enumerate(f.readlines())}

def detectObejects(interpreter, image, threshold):
    set_input_tensor(interpreter, image)
    interpreter.invoke()
    # Get all output details
    boxes = get_output_tensor(interpreter, 0)
    classes = get_output_tensor(interpreter, 1)
    scores = get_output_tensor(interpreter, 2)
    count = int(get_output_tensor(interpreter, 3))

    results = []
    for i in range(count):
        if scores[i] >= threshold:
            result = {
                'bounding_box': boxes[i],
                'class_id': classes[i],
                'score': scores[i]
            }
            results.append(result)
    return results

def main():
    logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)
    telegramBot = TelegramBot(config.telegramBotToken)

    # Load models
    labels = load_labels("/home/pi/labels_mobilenet_quant_v1_224.txt")
    interpreter = tf.lite.Interpreter(model_path="/home/pi/mobilenet_v1_1.0_224_quant_edgetpu.tflite")
    interpreter.allocate_tensors()
    _, input_height, input_width, _ = interpreter.get_input_details()[0]['shape']
    with picamera.PiCamera(resolution=(CAMERA_WIDTH, CAMERA_HEIGHT), framerate=30) as camera:
        # No screen connected to pi
        #camera.start_preview()
        try:
            stream = io.BytesIO()
            annotator = Annotator(camera)
            for _ in camera.capture_continuous(stream, format='jpeg', use_video_port=True):
                stream.seek(0)
                image = Image.open(stream).convert('RGB').resize((input_width, input_height), Image.ANTIALIAS)
                start_time = time.monotonic()
                detectedObjects = detectObejects(interpreter, image, 0.3)
                elapsed_ms = (time.monotonic() - start_time) * 1000
                print ('%.1fms' % (elapsed_ms))
                print (detectedObjects)
                stream.seek(0)
                stream.truncate()
        finally:
            print("Done")
            # No screen connected to pi
            # camera.stop_preview()

if __name__ == '__main__':
    print ("Let's go")
    main()
