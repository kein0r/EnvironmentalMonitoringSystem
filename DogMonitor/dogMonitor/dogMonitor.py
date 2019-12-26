#!/usr/bin/env python

import logging
#import PiCamera

# Because filename and class name are identical
from TelegramBot import TelegramBot

# Import token from config file
import config


def main():
    logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)
    telegramBot = TelegramBot(telegramBotToken)


if __name__ == '__main__':
  main()
