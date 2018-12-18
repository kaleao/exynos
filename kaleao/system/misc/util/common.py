import os
import sys
import threading
import time
import datetime
from subprocess import Popen, PIPE


# Shared Memory proc fs paths
f_path_command ="/proc/kgex/mng/command"
f_path_status ="/proc/kgex/mng/status"
f_path_info = "/proc/kgex/mng/info"

# Repository configuration files
f_path_recovery_repo = "/etc/opkg/myrepo.conf"
f_path_ubuntu_repo = "/etc/apt/sources.list"

f_ubuntu = "/root/ubuntu.tar.gz"

# MAC and IP addresses
f_path_update_server = "/proc/kgex/conf/update_srv_ip"
f_path_mac_a = "/proc/kgex/conf/mac_a_addr"
f_path_mac_b = "/proc/kgex/conf/mac_b_addr"
f_path_mac_mng = "/proc/kgex/conf/mac_mng_addr"
f_path_mac_stg = "/proc/kgex/conf/mac_stg_addr"

# Log file paths
f_path_powermng_log = "/root/util/powermng.log"
f_path_netmng_log = "/root/util/netmng.log"

RED   = "\033[1;31m"  
BLUE  = "\033[1;34m"
CYAN  = "\033[1;36m"
GREEN = "\033[0;32m"
RESET = "\033[0;0m"
BOLD    = "\033[;1m"
REVERSE = "\033[;7m"

MODE_RECOVERY = 0
MODE_PRODUCTION = 1
MODE_UNKNOWN = 2

def log(logFile, message):
    os.system('echo "' + message +'" >> ' + logFile)
    os.system("sync")

def pr(srv, message, logFile):
    print(srv + ": " + message)
    if ((logFile!=None) and (logFile!="")):
        ts = time.time()
        st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M.%S')
        log(logFile, st + " " + message)

def prerr(srv, message, logFile):
    sys.stdout.write(RED)
    pr(srv, "Error: " + message, logFile)
    sys.stdout.write(RESET)
    
def prwarn(srv, message, logFile):
    pr(srv, "Warning: " + message, logFile)
    
def prierr(srv, message, logFile):
    sys.stdout.write(RED)
    pr(srv, "Internal Error: " + message, logFile)
    sys.stdout.write(RESET)
    

class MngTimer():
    def __init__(self,t,hFunction):
        self.t=t
        self.hFunction = hFunction
        self.thread = threading.Timer(self.t,self.handle_function)
        self.sched = True

    def handle_function(self):
        self.hFunction()
        self.thread = threading.Timer(self.t,self.handle_function)
        if (self.sched==True):
            self.thread.start()

    def stop(self):
        self.sched = False

    def start(self):
        self.thread.start()

    def cancel(self):
        self.sched = False
        self.thread.cancel()
    

def getMyID():
    try:
        f_mng_mac = open(f_path_mac_mng,"r")
        line = f_mng_mac.read()
        f_mng_mac.close()
        return line.strip()[-1]       
    except Exception:
        print("Cannot read from '" + f_path_mac_mng + "'")
        return ""
    
    
def getRunningMode():
    status = os.path.isfile("/etc/init.d/netexy")
    if (status==True):        
        return MODE_PRODUCTION 
    else:
        status = os.path.isfile("/etc/init.d/S41kmaxnet")  
        if (status==True):
            return MODE_RECOVERY
        else:
            return MODE_UNKNOWN


        
