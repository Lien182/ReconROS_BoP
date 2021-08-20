import os
import time
import sys
import re
import paramiko
import threading
from stat import *

import threading, paramiko

strdata=''
fulldata=''

sem = threading.Semaphore()


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    WHITE = '\033[97m'



class ReconROSClient:
    _id = 0
    _name = ""

    def __init__(self, fname, id):
        self.id = id
        self.name=fname
        self.ExitRequest = False
        print("New ReconROS client: "+ fname )

    def build_sw(self):
        os.system("cd "+self.name+" && rm build.sw -r -f && rdk export_sw && rdk build_sw")



    def get_ip(self):
        return "192.168.2." + self.id


subfolders = [ f.path for f in os.scandir('.') if f.is_dir() ]
clients = []

for f in subfolders:
    if re.search('(?<=./zed)_[0-9]', f):        
        id =re.search('(?<=./zed_)[0-9][0-9]', f)[0]
        clients.append(ReconROSClient(f,id))

threads = []

for c in clients:
    print(c.name)
    c.build_sw()