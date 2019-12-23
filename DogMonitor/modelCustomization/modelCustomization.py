#
# Example taken from https://github.com/tensorflow/examples/blob/master/tensorflow_examples/lite/model_customization/demo/image_classification.ipynb
#
from __future__ import absolute_import, division, print_function, unicode_literals

import numpy as np

import tensorflow as tf
assert tf.__version__.startswith('2')

from tensorflow_examples.lite.model_customization.core.data_util.image_dataloader import ImageClassifierDataLoader
from tensorflow_examples.lite.model_customization.core.task import image_classifier
from tensorflow_examples.lite.model_customization.core.task.model_spec import efficientnet_b0_spec
from tensorflow_examples.lite.model_customization.core.task.model_spec import ImageModelSpec

import matplotlib.pyplot as plt

#image_path = tf.keras.utils.get_file('dogs', 'images/dogs.tgz', untar=True)
data = ImageClassifierDataLoader.from_folder("images")
model = image_classifier.create(data)
loss, accuracy = model.evaluate()
model.export('dogs_classifier.tflite', 'dog_labels.txt')
