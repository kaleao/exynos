import os
import sys
import threading
from subprocess import Popen, PIPE

from common import*

#PCI Speeds
PCIE_GEN1 = 1
PCIE_GEN2 = 2
PCIE_GEN3 = 3


def getPCISpeed(device):
    (stdout, stderr) = Popen(["lspci -s " + device + " -vv 2> /dev/null | grep 'LnkSta:'"], stdout=PIPE, shell=True).communicate()

    line = str(stdout)
    
    tmp = line.find("Speed ")
    tmp2 = line.find("GT/s")

    if(tmp==-1 or tmp2==-1):
        return None

    speedstr = line[tmp + len("Speed ") : tmp2]

    res = "gen1"

    if(speedstr=="8"):
        res = PCIE_GEN3
    elif(speedstr=="5"):
        res = PCIE_GEN2
    elif(speedstr=="2.5"):
        res = PCIE_GEN1
    else:
        res = None

    return res


def checkNPU_PCIe_Endpoint():
    (stdout, stderr) = Popen(["lspci", "-s 0001:01:00.0", "-v"], stdout=PIPE).communicate()
    out = str(stdout)   
    if (out.find("PLDA Device 1111")==-1):
        return False
    
    return True

def checkSPU_PCIe_Endpoint():
    (stdout, stderr) = Popen(["lspci", "-s 0000:01:00.0", "-v"], stdout=PIPE).communicate()
    out = str(stdout)   
    if (out.find("PLDA Device 1111")==-1):
        return False
    
    return True

def checkNPU_PCIe_Root():
    (stdout, stderr) = Popen(["lspci", "-s 0001:00:00.0", "-v"], stdout=PIPE).communicate()
    out = str(stdout)   
    if (out.find("Samsung Electronics Co")==-1):
        return False
    
    return True

def checkSPU_PCIe_Root():
    (stdout, stderr) = Popen(["lspci", "-s 0000:00:00.0", "-v"], stdout=PIPE).communicate()
    out = str(stdout)   
    if (out.find("Samsung Electronics Co")==-1):
        return False
    
    return True
