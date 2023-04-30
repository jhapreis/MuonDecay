import logging
from typing import Union


class LogManager:
    
    def __init__(self, name: str, level: str, filename: Union[str,None]=None) -> None:
        
        self.__name     = name
        self.__level    = self.__convert_log_level(level=level)
        self.__filename = filename
        
        self.log_manager = logging.getLogger(self.__name)
        if not self.log_manager.hasHandlers():
            self.log_manager.setLevel(self.__level)
            self.__set_handler()
        else:
            self.log_manager.setLevel(self.__level)


    def __set_handler(self):
        
        formatter = CustomFormatter()
        
        file_handler = logging.FileHandler(filename=self.__filename)
        file_handler.setLevel(self.__level)
        file_handler.setFormatter(formatter)
        
        console_handler = logging.StreamHandler()
        console_handler.setLevel(self.__level)
        console_handler.setFormatter(formatter)
        
        self.log_manager.addHandler(file_handler)
        self.log_manager.addHandler(console_handler)
        
        return self
        
        
    def __convert_log_level(self, level: str) -> int:

        name_to_level = {
            'CRITICAL': logging.CRITICAL,
            'FATAL'   : logging.FATAL,
            'ERROR'   : logging.ERROR,
            'WARN'    : logging.WARNING,
            'WARNING' : logging.WARNING,
            'INFO'    : logging.INFO,
            'DEBUG'   : logging.DEBUG,
            'NOTSET'  : logging.NOTSET,
        }
        
        log_level = name_to_level.get(level.upper(), logging.WARNING)
        
        return log_level

        
    def debug(self, msg: str):
        self.log_manager.debug(msg=msg)

    def info(self, msg: str):
        self.log_manager.info(msg=msg)

    def warning(self, msg: str):
        self.log_manager.warning(msg=msg)
        
    def error(self, msg: str):
        self.log_manager.error(msg=msg)
        
    def critical(self, msg: str):
        self.log_manager.critical(msg=msg)
        
    def exception(self, msg: str):
        self.log_manager.exception(msg=msg)



class CustomFormatter(logging.Formatter):

    grey = "\x1b[38;20m"
    green = "\x1b[32;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"
    reset = "\x1b[0m"
    format = "[%(asctime)s] %(levelname)8s @%(name)12s --- %(message)s"

    FORMATS = {
        logging.DEBUG   : grey + format + reset,
        logging.INFO    : green + format + reset,
        logging.WARNING : yellow + format + reset,
        logging.ERROR   : red + format + reset,
        logging.CRITICAL: bold_red + format + reset
    }

    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt, "%Y-%m-%d %H:%M:%S")
        return formatter.format(record)
