#!/usr/bin/env python
#
# Based on example found at https://github.com/tensorflow/examples/blob/master/lite/examples/image_classification/raspberry_pi/classify_picamera.py
#
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os

import argparse
import io
import time
import numpy as np
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


def set_input_tensor(interpreter, image):
    tensor_index = interpreter.get_input_details()[0]['index']
    input_tensor = interpreter.tensor(tensor_index)()[0]
    input_tensor[:, :] = image

def get_output_tensor(interpreter, index):
    output_details = interpreter.get_output_details()[index]
    tensor = np.squeeze(interpreter.get_tensor(output_details['index']))
    return tensor

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


def printResults(detectedObjects, labels):
    os.system('clear')
    for obj in detectedObjects:
        print('%s: %.2f' % (labels[obj['class_id']], obj['score']))

def main():
    logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)
    telegramBot = TelegramBot(config.telegramBotToken)

    # Load models
#    labels = load_labels("/home/pi/labels_mobilenet_quant_v1_224.txt")
#    labels = load_labels("/home/pi/coco_labels.txt")
    labels = load_labels("/home/pi/coco_ssd_mobilenet_v1_1.0_quant_2018_06_29_label.txt")
#    interpreter = tf.lite.Interpreter(model_path="/home/pi/mobilenet_v1_1.0_224_quant.tflite")
#    interpreter = tf.lite.Interpreter(model_path="/home/pi/mobilenet_ssd_v2_coco_quant_postprocess_edgetpu.tflite")
#    interpreter = tf.lite.Interpreter(model_path="/home/pi/mobilenet_v1_1.0_224_quant_edgetpu.tflite")
    interpreter = tf.lite.Interpreter(model_path="/home/pi/coco_ssd_mobilenet_v1_1.0_quant_2018_06_29.tflite")
    interpreter.allocate_tensors()
    _, input_height, input_width, _ = interpreter.get_input_details()[0]['shape']
    with picamera.PiCamera(resolution=(config.CAMERA_WIDTH, config.CAMERA_HEIGHT), framerate=30) as camera:
        # No screen connected to pi
        #camera.start_preview()
        try:
            stream = io.BytesIO()
            for _ in camera.capture_continuous(stream, format='jpeg', use_video_port=True):
                stream.seek(0)
                image = Image.open(stream).convert('RGB').resize((input_width, input_height), Image.ANTIALIAS)
                start_time = time.monotonic()
                detectedObjects = detectObejects(interpreter, image, 0.4)
                printResults(detectedObjects, labels)
                elapsed_ms = (time.monotonic() - start_time) * 1000
                print ('%.1fms' % (elapsed_ms))
                stream.seek(0)
                stream.truncate()
        finally:
            print("Done")
            # No screen connected to pi
            # camera.stop_preview()

if __name__ == '__main__':
    print ("Let's go")
    main()
