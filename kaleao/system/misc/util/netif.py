import os
import sys
from subprocess import Popen, PIPE
from common import*

def getNPUMngIP():
    try:        
        f_npu_ip_mng = open(f_path_npu_ip_mng, "r")
    except IOError as e:
        print("I/O error({0}): {1}".format(e.errno, e.strerror))
        return None
    
    try:
        npu_ip_mng = f_npu_ip_mng.readline()
    except Exception as ex:
        return None
            
    f_npu_ip_mng.close()
    return npu_ip_mng

def ifaceHasIP(iface):
    (stdout, stderr) = Popen(["ifconfig", iface], stdout=PIPE).communicate()
    if (str(stdout).find("inet addr:")==-1):
        return False
    return True            

def ifaceIsUp(iface):
    (stdout, stderr) = Popen(["ifconfig"], stdout=PIPE).communicate()
    if (str(stdout).find(iface)==-1):            
        return False
    return True  

def checkIface(iface):
    (stdout, stderr) = Popen(["ifconfig", "-a"], stdout=PIPE).communicate()
    if (str(stdout).find(iface)==-1):            
        return False
    return True