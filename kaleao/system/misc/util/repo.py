
from common import*

def appendUbuntuRepo(str_repo):
    global f_path_ubuntu_repo

    try:
        f_ubuntu_repo = open(f_path_ubuntu_repo, "a")
        f_ubuntu_repo.write("\n" + str_repo + "\n")
        f_ubuntu_repo.close()
        return True
    except Exception:
        return False


def appendRecoveryRepo(str_repo):
    global f_path_recovery_repo
    try:
        f_recovery_repo = open(f_path_recovery_repo, "a")
        f_recovery_repo.write("\n"+str_repo+"\n")
        f_recovery_repo.close()
        return True
    except Exception:
        return False

def getUpdateSrvIpFromMem():
    global f_path_update_server
    try:        
        f_update_server = open(f_path_update_server, "r")
        line = f_update_server.readline()
        f_update_server.close()  
    except Exception:        
        return None            
    return line.strip() 
