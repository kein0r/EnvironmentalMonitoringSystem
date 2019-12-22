#!/usr/bin/env python

import logging

# Because filename and class name are identical
from TelegramBot import TelegramBot


logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)
telegramBot = TelegramBot("")
