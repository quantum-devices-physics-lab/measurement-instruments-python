from Instruments import *
import time

def do_something_and_wait(a):
    a = a+1
    time.sleep(5)
    return a
