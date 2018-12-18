#!/usr/bin/python

import os
import sys
from subprocess import Popen, PIPE

from common import *
from pcie import *
from netif import *
from repo import *

srv = "netmng"

eth0 = "eth0"
eth1 = "eth1"
mng0 = "mng0"

mac_a = "00:00:00:00:00:00"
mac_b = "00:00:00:00:00:00"
mac_mng = "00:00:00:00:00:00"
mac_stg = "00:00:00:00:00:00"

t_repo = None
log = f_path_netmng_log


def findUpdateServer():
    global f_path_recovery_repo 
    global f_path_update_server

    updateIface = "mng0"
    
    mode = getRunningMode()
    if (mode==MODE_RECOVERY):
        if (checkIface(updateIface)==True):
            if (ifaceIsUp(updateIface)==True):
                if (ifaceHasIP(updateIface)==True):

                    status = os.path.isdir("/etc/opkg")
                    if (status==False):
                        pr(srv, "/etc/opkg does not exist. Creating it...", log)
                        os.system("mkdir -p /etc/opkg")
                        os.system("sync")

                    status = os.path.isfile(f_path_recovery_repo)
                    if (status==False):
                        pr(srv, f_path_recovery_repo + " does not exist. Creating it...", log)
                        os.system('echo "" > ' + f_path_recovery_repo)
                        os.system("sync")

                    content = ""
                    try:
                        f_recovery_repo = open(f_path_recovery_repo, "r")
                        content = f_recovery_repo.read()
                        f_recovery_repo.close()
                    except Exception:
                        prwarn(srv, "cannot read '"+ f_path_recovery_repo +"'")


                    if (content.find("kmax")==-1):
                        update_srv_ip = getUpdateSrvIpFromMem()
                        update_url = "src/gz kmaxsrv0 http://" + update_srv_ip + "/kaleao/exynos/recovery"
                        pr(srv, "Got update repository from shared memory: '"+update_url+"'", log)
                        status = appendRecoveryRepo(update_url)
                        if (status==False):
                            prerr(srv, "cannot write to '" + f_path_recovery_repo + "'", log)
                    else:
                        pr(srv, "Kmax repo already set up.", log)

                    t_repo.stop()
    elif (mode==MODE_PRODUCTION):
        if (checkIface(updateIface)==True):
            if (ifaceIsUp(updateIface)==True):
                if (ifaceHasIP(updateIface)==True):
                    content = ""
                    try:
                        f_recovery_repo = open(f_path_ubuntu_repo, "r")
                        content = f_recovery_repo.read()
                        f_recovery_repo.close()
                    except Exception:
                        prwarn(srv, "cannot read '" + f_path_ubuntu_repo + "'", log)

                    if (content.find("kmax") == -1):
                        update_srv_ip = getUpdateSrvIpFromMem()
                        update_url = "deb http://" + update_srv_ip + "/kaleao/exynos/ubuntu kmax/"
                        pr(srv, "Got update repository from shared memory: '" + update_url + "'", log)
                        status = appendUbuntuRepo(update_url)
                        if (status == False):
                            prerr(srv, "cannot write to '" + f_path_ubuntu_repo + "'", log)
                    else:
                        pr(srv, "Kmax repo already set up.", log)

                    t_repo.stop()
    else:
        pr(srv, "unknown running mode. Neither ubuntu or recovery.", log)
        t_repo.stop()

def sample():
    getsetMacAddresses()

def checkIFaces():
    
    status = checkIface(eth0)
    if (status==False):
        pr(srv, eth0 + " not found", log)
        return -1
        
    status = checkIface(eth1)
    if (status==False):
        pr(srv, eth1 + " not found", log)
        return -1    
    
    status = checkIface(mng0)
    if (status==False):
        pr(srv, mng0 + " not found", log)
        return -1            
    
    return 0
    
def getsetMacAddresses():
    global f_path_mac_a
    global f_path_mac_b
    global f_path_mac_mng
    global f_path_mac_stg
    global mac_a
    global mac_b
    global mac_mng
    global mac_stg

    status = checkIFaces()
    if (status==-1):
        return

    try:        
        f_mac_a = open(f_path_mac_a,"r")
    except IOError as e:
        prerr(srv, "Cannot open " + f_path_mac_a, log)
        return
    
    try:
        f_mac_b = open(f_path_mac_b, "r")
    except Exception as e:
        prerr(srv, "Cannot open " + f_path_mac_b, log)
        return

    try:        
        f_mac_mng = open(f_path_mac_mng, "r")
    except IOError as e:
        prerr(srv, "Cannot open " + f_path_mac_mng, log)
        return
    
    try:        
        f_mac_stg = open(f_path_mac_stg, "r")
    except IOError as e:
        prerr(srv, "Cannot open " + f_path_mac_stg, log)
        return

    new_mac_a = f_mac_a.readline()
    new_mac_b = f_mac_b.readline()
    new_mac_mng = f_mac_mng.readline()
    new_mac_stg = f_mac_stg.readline()
    
    if (new_mac_a!=mac_a):
#        print "mac a changed to '" + new_mac_a + "'"
        mac_a = new_mac_a
        os.system("ifconfig eth0 hw ether " + mac_a) 

    if (new_mac_b!=mac_b):
#        print "mac b changed to '" + new_mac_b + "'"
        mac_b = new_mac_b
        os.system("ifconfig eth1 hw ether " + mac_b)

    if (new_mac_mng!=mac_mng):
#        print "mac mng changed to '" + new_mac_mng + "'"
        mac_mng = new_mac_mng
        os.system("ifconfig mng0 down")
        os.system("ifconfig mng0 hw ether " + mac_mng)
        os.system("ifconfig mng0 up")

    if (new_mac_stg!=mac_stg):
#        print "mac b changed to '" + new_mac_b + "'"
        mac_stg = new_mac_stg
        os.system("ifconfig stg0 hw ether " + mac_stg)

    f_mac_a.close()
    f_mac_b.close()
    f_mac_mng.close()
    f_mac_stg.close()
       
       
def isRunning():
    (stdout, stderr) = Popen(["pgrep", "-lf", "netmng"], stdout=PIPE).communicate()
    out = str(stdout)
    
    if (out.count("netmng")>1):
        return True
    
    return False
   

def main():
    global f_path_mac_a
    global f_path_mac_b
    global f_path_mac_mng
    global mac_a
    global mac_b
    global mac_mng
    global t_repo
        
    if (isRunning()==True):
        prwarn(srv, "netmng service is already running. Exiting...", log)
        sys.exit(0)

    pr(srv, "", log)
    pr(srv, "started service", log)

    status = os.path.isfile(f_path_mac_a)
    if (status==False):
        prwarn(srv, f_path_mac_a + " does not exist. Maybe in SD emulation mode.", log)
        sys.exit(0)
    
    status = os.path.isfile(f_path_mac_b)
    if (status==False):
        prwarn(srv, f_path_mac_b + " does not exist. Maybe in SD emulation mode.", log)
        sys.exit(0)

    status = os.path.isfile(f_path_mac_mng)
    if (status==False):
        prwarn(srv, f_path_mac_mng + " does not exist. Maybe in SD emulation mode.", log)
        sys.exit(0)
        
    status = checkIFaces()
    if (status>=0):
        getsetMacAddresses()
    
    t = MngTimer(2, sample)
    t.start()
    
    
if __name__=="__main__":
    main()
