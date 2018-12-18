#!/usr/bin/python

import os
import sys
import threading
from subprocess import Popen, PIPE
import time
sys.path.insert(0, "/root/util/iperf3.py")
import iperf3
import subprocess

from common import *
from pcie import *
from netif import *
from repo import *

srv = "powermng"

str_id = "0"
ip_addr_npu = "10.0.0.2"
ip_addr_npu_test = "10.0.0.2"
ip_addr_ex1_nic_b = "192.168.4.101"

CMD_IDLE = "0"
CMD_POWER_OFF = "1"
CMD_PING_NPU  = "11"
CMD_PING_EX1_NIC_B = "12"
CMD_IPERF_MNG = "10"
CMD_UBUNTU_FULL_UPDATE = "13"
CMD_RECOVERY_UPDATE = "14"
CMD_IPERF_SRV_NODE2 = "15"
CMD_IPERF_SRV_NODE3 = "16"
CMD_IPERF_SRV_NODE4 = "17"
CMD_IPERF_CLIENT_TO_N2_A = "18"
CMD_IPERF_CLIENT_TO_N2_B = "19"
CMD_IPERF_CLIENT_TO_N4_A = "20"
CMD_SET_IP	       = "21"
CMD_RUN_DUMMY = "22"
CMD_STOP_DUMMY = "23"
CMD_IPERF_CLIENT_TO_N4_B = "24"
CMD_UPDATE_ROOTFS = "25"



str_normal_ip_mng0 = ""
str_normal_ip_eth0 = ""
str_normal_ip_eth1 = ""
str_normal_ip_stg0 = ""

log = f_path_powermng_log
    
def pingExynos1NICB():
    global ip_addr_ex1    
    
    print("Starting ping NIC A -> NIC B Factory Test procedure...")    
    
    str_id = getMyID()
    
    if (str_id=="1"):
        print("Exynos #1 allowing only traffic to/from NIC B...")
        mac_b = getMACBAddr()
        test_ip_eth0 = "192.168.4.10" + str_id
        os.system("ifconfig eth1 down")
        print("Enforcing NIC B MAC address = '" + mac_b + "' to eth0...")
        os.system("ifconfig eth0 hw ether " + mac_b)
        test_ip_eth0 = "192.168.4.10" + str_id
        print("Enforcing IP address = '" + test_ip_eth0 + "' to eth0...")
        os.system("ifconfig eth0 " + test_ip_eth0)        
        test_ip_mng0 = "10.0.0.1" + str_id
        os.system("ifconfig mng0 " + test_ip_mng0)
        print("Ready to get ping from external NIC B transceiver...")
    else:
        print("Exynos #" + str_id + " allowing only traffic to/from NIC A...")
        print("Disabling eth1...")
        os.system("ifconfig eth1 down")        
        test_ip_eth0 = "192.168.4.10" + str_id
        print("Enforcing IP address = '" + test_ip_eth0 + "' to eth0...")
        os.system("ifconfig eth0 " + test_ip_eth0)   
        test_ip_mng0 = "10.0.0.1" + str_id
        os.system("ifconfig mng0 " + test_ip_mng0)                
        print("Pinging Exynos #1 -> 192.168.4.101 NIC A -> NIC B..")
        response = os.system("ping -c 1 192.168.4.101")
        if (response == 0):
            print("Exynos #1 and/or NICA/NICB transceivers are up\n")        
            return "0"
        else:
            print("Exynos #1 and/or NIAC/NICB transceivers are down\n")
            return "ffffffff"                     
        
                    

def pingNPU():
    global ip_addr_npu    
    
    print("Starting ping NPU A9 Factory Test procedure...")
    
    configureFactoryTest()
 
    response = os.system("ping -c 1 " + ip_addr_npu_test)
    if (response == 0):
        print("NPU is up\n")        
        return "0"
    else:
        print("NPU is down\n")
        return "ffffffff"    
   
   
def configureFactoryTest():    
    global str_id    
    global str_normal_ip_eth0
    global str_normal_ip_eth1
    global str_normal_ip_mng0
    global str_normal_ip_stg0
    
    str_id = getMyID()
        
    test_ip_eth0 = "192.168.3.10" + str_id
    test_ip_eth1 = "192.168.4.10" + str_id
    test_ip_mng0 = "10.0.0.1" + str_id    
    test_ip_stg0 = "192.168.5.10" + str_id
    
    print("Generated Factory Test static IPs:")
    print("eth0 = "+ test_ip_eth0)
    print("eth1 = "+ test_ip_eth1)
    print("mng0 = "+ test_ip_mng0)
    print("stg0 = "+ test_ip_stg0)
    print("NPU A9 mng IP = " + ip_addr_npu)
    
    print("Configuring all Ethernet Interface with Factory Test static IPs")
    os.system("ifconfig eth0 " + test_ip_eth0)
    os.system("ifconfig eth1 " + test_ip_eth1)
    os.system("ifconfig mng0 " + test_ip_mng0)         
        

def runIperfClient():
    global ip_addr_npu

    port = 5000 + int(str_id)

    print("Running iperf3 client\n")
    client = iperf3.Client()
    client.duration = 120
    client.server_hostname = ip_addr_npu
    client.protocol = "tcp"
    client.port = port
    print("Connecting to {0}:{1}".format(client.server_hostname, client.port))
    result = client.run()

    if (result.error):
        print("FAILED: Error: " + str(result.error))
        return "ffffffff"
    else:
        print("Test completed\n")
    if (result.sent_MBps == 0.0):
        print("FAILED: Low throughput\n")
        return "ffffffff"
    else:
        print("Mbps = {0}".format(result.sent_Mbps))

    return "0"

def runIperfCl(node):
    if node==2:
           proc = subprocess.Popen(['iperf3 -c 192.168.2.2 -t 80'],stdout=subprocess.PIPE,shell=True)
           reverse = "iperf3 -c 192.168.2.2 -R -t 80"
    elif node==3:
           proc = subprocess.Popen(['iperf3 -c 192.168.3.2 -t 80 '],stdout=subprocess.PIPE,shell=True)
           reverse = "iperf3 -c 192.168.3.2 -R -t 80"
    elif node==4:
           proc = subprocess.Popen(['iperf3 -c 192.168.2.4 -t 80'],stdout=subprocess.PIPE,shell=True)
           reverse = "iperf3 -c 192.168.2.4 -R -t 80"
    else:
           proc = subprocess.Popen(['iperf3 -c 192.168.3.4 -t 80'],stdout=subprocess.PIPE,shell=True)
           reverse = "iperf3 -c 192.168.3.4 -R -t 80"
    var=0x1
    while True:
          line = proc.stdout.readline()
          if line != '':
                if "error" in line:
                       var='ffffffff'
                       return var
          else:
                break;

    proc = subprocess.Popen([reverse],stdout=subprocess.PIPE,shell=True)
    while True:
          line = proc.stdout.readline()
          if line != '':
                if "error" in line:
                       var='ffffffff'
          else:
                break;

    return var

    

def runIperfServer():
    proc = subprocess.Popen(['stdbuf -oL iperf3 -s'],stdout=subprocess.PIPE,shell=True)
    print("i am on Server")
    var='ffffffff'
    while True:
          line = proc.stdout.readline()
          print(line)
          if line != '':                            
                if "Server listening on" in line:
                       var=0x1
                       return var
                                                    
          else:                                     
                break;                                               
    return var 


def forceFullUbuntuUpdate():
    runningMode = getRunningMode()
    if (runningMode != MODE_RECOVERY):
        prwarn(srv, "Cannot force full Ubunt update in other than recovery mode.", log)
        return "ffffffff"
    else:
        pr(srv, "Forcing full Ubuntu system update.", log)
        os.system("opkg update")
        os.system("opkg remove kmax-exynos* --force-depends")
        os.system("opkg update")
        os.system("opkg remove kmax-production-rootfs")
        os.system("opkg install kmax-production-rootfs")
        os.system("rm -rf  /var/cache/opkg/*kmax-exynos*")
        ret = Popen(["opkg", "install", "kmax-exynos"], stdout=PIPE)
	data = ret.communicate()[0]
        print("Installation of kmax-exynos completed status {}".format(ret.returncode))
        os.system("( sleep 6 ; poweroff -f ) &");
        if ret.returncode == 0:
             os.system("md5sum /var/cache/opkg/* > /root/version")
             os.system("opkg list-installed >> /root/version")
             return 0x1
        os.system("sync")
        return "ffffffff"

def forceFullUpdate():
    runningMode = getRunningMode()
    if (runningMode != MODE_RECOVERY):
        prwarn(srv, "Cannot force full Ubunt update in other than recovery mode.", log)
        return "ffffffff"
    else:
        pr(srv, "Forcing full Ubuntu system update.", log)
        statusAgent = os.path.isfile("/root/util/exynosAgent_exynos")
        if (statusAgent==False):
            return "ffffffff"
        os.system("/root/util/exynosAgent_exynos -lin -ler -bg &");
        return 0x1

def recoveryUpdate():
    runningMode = getRunningMode()
    if (runningMode != MODE_RECOVERY):
        prwarn(srv, "Cannot force full Ubunt update in other than recovery mode.", log)
        return "ffffffff"
    else:
        pr(srv, "Forcing full Ubuntu system update.", log)
        os.system("opkg update")
        os.system("opkg remove kmax-exynos* --force-depends")
        os.system("opkg update")
        os.system("opkg install kmax-production-rootfs")
        os.system("rm -rf  /var/cache/opkg/*kmax-exynos*")
        ret = Popen(["opkg", "install", "kmax-exynos"], stdout=PIPE)
	data = ret.communicate()[0]
        print("Installation of kmax-exynos completed with status {}".format(ret.returncode))
	os.system("( sleep 6 ; poweroff -f ) &");
        if ret.returncode == 0:
             os.system("md5sum /var/cache/opkg/* > /root/version")
             os.system("opkg list-installed >> /root/version")
             return 0x1
        os.system("sync")
        return "ffffffff" 

def setIpUpdate():
    str_id = getMyID()
    test_ip_mng0 = "10.1.1.1" + str_id
    print("mng0 = "+ test_ip_mng0)
    os.system("ifconfig mng0 " + test_ip_mng0)
def commandListener():       
    cmd = getCommand()

    if (cmd==CMD_IDLE):
        return
    
    if (cmd==CMD_POWER_OFF):
        pr(srv, "powermng: Got CMD_POWER_OFF. Shutting down... \n", log)
        os.system("halt")
        setCommand(CMD_IDLE)
        sys.exit(0)            

    if (cmd==CMD_PING_NPU):
        status = pingNPU()    
        setStatus(status)

    if (cmd==CMD_PING_EX1_NIC_B):
        status = pingExynos1NICB()    
        setStatus(status)
        
    if (cmd==CMD_IPERF_MNG):
        status = runIperfClient()
        setStatus(status)

    if (cmd==CMD_IPERF_SRV_NODE2):
        os.system("ifconfig eth0 192.168.2.2")
        os.system("ifconfig eth1 192.168.3.2")
        status = runIperfServer()
        print(status)
        setStatus(status)

    if (cmd==CMD_IPERF_SRV_NODE3):                                                     
        os.system("ifconfig eth0 192.168.2.3")
        status = runIperfServer()
        setStatus(status)

    if (cmd==CMD_IPERF_SRV_NODE4):
        os.system("ifconfig eth0 192.168.2.4")
        os.system("ifconfig eth1 192.168.3.4")
        status = runIperfServer()
        setStatus(status)

    if (cmd==CMD_IPERF_CLIENT_TO_N2_A):
        os.system("ifconfig eth0 192.168.2.1")
        status = runIperfCl(2)
        print(status)
        setStatus(status)

    if (cmd==CMD_IPERF_CLIENT_TO_N2_B):                                       
        os.system("ifconfig eth1 192.168.3.1") 
        status = runIperfCl(3)                                                         
        print(status)                                                                
        setStatus(status)

    if (cmd==CMD_IPERF_CLIENT_TO_N4_A):
        os.system("ifconfig eth0 192.168.2.3")                                           
        status = runIperfCl(4)
        print(status)          
        setStatus(status)

    if (cmd==CMD_IPERF_CLIENT_TO_N4_B):
        os.system("ifconfig eth1 192.168.3.3")
        status = runIperfCl(5)
        print(status)
        setStatus(status)

    if (cmd==CMD_UBUNTU_FULL_UPDATE):
        status = forceFullUbuntuUpdate()
        setStatus(status)

    if (cmd==CMD_UPDATE_ROOTFS):
        status = forceFullUpdate()
        setStatus(status)


    if (cmd==CMD_RECOVERY_UPDATE):
        status = recoveryUpdate()
        setStatus(status)

    if (cmd==CMD_SET_IP):
        status = setIpUpdate()
        setStatus(status)

    if (cmd==CMD_RUN_DUMMY):                                                             
        os.system("source ./root/util/run_thermal_load.sh 4 0 100000  &")                  
        setStatus("0")                                                               
                                                                                           
    if (cmd==CMD_STOP_DUMMY):                                                            
        os.system("killall run_dummy_load.sh")                                             
        setStatus("0")
                            
    setCommand(CMD_IDLE)
        
    
def getStatus():
    try:
        f_status = open(f_path_status, "r")
    except Exception:
        prerr(srv, "cannot open " + f_path_status, log)
        return ''
    
    line = f_status.readline()    
    f_status.close()
    return line
                
def getCommand():
    try:        
        f_command = open(f_path_command,"r")
    except Exception:
        prerr(srv, "cannot open " + f_path_command, log)
        return ''    
        
    line = f_command.readline()    
    f_command.close()
    return line

def setStatus(status):
    os.system("echo '" + str(status) + "' > " + f_path_status)
    return True

def setCommand(cmd):
    os.system("echo '" + cmd + "' > " + f_path_command)
    return True        
    
def isRunning():
    (stdout, stderr) = Popen(["pgrep", "-lf", "powermng"], stdout=PIPE).communicate()
    out = str(stdout)
    
    if (out.count("powermng")>1):
        return True
    
    return False

##Production reset
def resetProduction():
    status = os.path.isfile(f_ubuntu)
    if(status==False):
        prwarn(srv, "Ubuntu Image not found!", log)
        return False
    prev_dir = os.getcwd()
    os.system("cd /root/; rm -f ./ubuntu; sync; tar xf " + f_ubuntu + "; sync; cd /root/ubuntu; ./write_ubuntu.sh; sync; cd " + prev_dir)    
    os.system("echo updated > " + f_path_info)
    return True


def getMACBAddr():
    # TODO: derive my ID from my mng0 MAC address
    try:
        f_mac_b = open(f_path_mac_b,"r")
        line = f_mac_b.readline()
        f_mac_b.close()
        return line.strip()   
    except IOError as e:
        prerr(srv, "cannot read from " + f_path_mac_b, log)
        return ""

def main():
    global str_id
    
    if (isRunning()==True):
        prwarn(srv, "powermng service is already running. Exiting...", log)
        sys.exit(0)

    pr(srv, "", log)
    pr(srv, "started service", log)

    status = os.path.isfile(f_path_command)
    if (status==False):
        prwarn(srv, f_path_command + " does not exist. Maybe in SD emulation mode.", log)
        sys.exit(0)        
        
    status = os.path.isfile(f_path_command)
    if (status==False):
        prwarn(srv, f_path_command + " does not exist. Maybe in SD emulation mode.", log)
        sys.exit(0)
    
    str_id = getMyID()		
    pr(srv, "Running on Exynos #" + str_id, log)
    
    (stdout, stderr) = Popen(["cat", f_path_info], stdout=PIPE).communicate()
    out = str(stdout)
    
    if ("Production Reset required" in out):
        pr(srv, "Production System Reset required!!", log)
        if(resetProduction()==True):
            pr(srv, "Production Image Reset done!", log)
        else:
            prwarn(srv, "Production Image could not be reset!", log)

    setCommand(CMD_IDLE)

    # Check all PCIe components via lspci
    status = checkNPU_PCIe_Root()
    if (status==False):
        prwarn(srv, "NPU PCIe Root Complex is down", log)
    else:
        pr(srv, "NPU PCIe Root Complex is up", log)
    status = checkNPU_PCIe_Endpoint()
    if (status==False):
        prwarn(srv, "NPU PCIe Endpoint is down", log)
    else:
        pr(srv, "NPU PCIe Endpoint is up", log)
    status = checkSPU_PCIe_Root()
    if (status==False):
        prwarn(srv, "SPU PCIe Root Complex is down", log)
    else:
        pr(srv, "SPU PCIe Root Complex is up", log)
    status = checkSPU_PCIe_Endpoint()
    if (status==False):
        prwarn(srv, "SPU PCIe Endpoint is down", log)
    else:
        pr(srv, "SPU PCIe Endpoint is up", log)
        
    t = MngTimer(1, commandListener)
    t.start()    
    

if __name__=="__main__":
    main()
