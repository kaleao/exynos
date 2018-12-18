#!/usr/bin/python

import os
import sys
import threading
from subprocess import Popen, PIPE
import time
import xml
import xml.etree.ElementTree as ET
import wget
import hashlib
import common
from common import *

debug = False

LEVEL_1 = "1"
LEVEL_2 = "2"

level = LEVEL_1
ip_mng_npu = "192.168.1.3"
update_file = "update.xml"
packages_file = "packages.xml"
dir_download = "download"
dir_abs_download = ""
working_dir = ""
pkg_install_script = "install.sh"

def pr(message):
    print("updatemng: " + message)
    
def prerr(message):
    
    print("updatemng: Error: " + message)
    
def prwarn(message):
    print("updatemng: Warning: " + message)
    
def prierr(message):
    print("updatemng: Internal Error: " + message)
    
def dbg(message):
    if (debug==True):
        pr(message)    

class PackageDesc:    
    def __init__(self, name, file, sum, url):
        self.name = name
        self.file = file        
        self.sum = sum
        self.url = url
        self.downloaded = False          
        self.installed = False            
                        
update_packages = []
installed_packages = []
to_update = []

def getXMLNodeByName(root, name):
    for child in root:
        if (child.attrib["name"] == name):
            return child
    
    return None

def lenDownloadedPackages(lst):
    cnt = 0
    for pkg in lst:
        if (pkg.downloaded == True):
            cnt = cnt +1
            
    return cnt

def lenInstalledPackages(lst):
    cnt = 0
    for pkg in lst:
        if (pkg.installed == True):
            cnt = cnt +1
            
    return cnt

def getPackageByName(lst, name):
    if ((lst==None) or (lst is None)):
        prierr("lst==None")
        return None
    
    for pkg in lst:
        if (pkg.name == name):
            return pkg
    
    return None
    
def printPackageDescList(lst):
    pr("Package list:")
    if ((lst==None) or (lst is None)):
        pr(" [None]")
        return
    
    if (len(lst)==0):
        pr(" [Empty]")
    
    i = 0
    for pkg in lst:        
        if ((pkg==None) or (pkg is None)):
            pr(" #"+str(i) + ": None")
        else:
            pr(" #"+str(i) + ": '"+pkg.name+"' '"+pkg.file+"' '"+pkg.sum+"' '"+pkg.url+"'")
        i = i +1        

def getPackageDescList(xmlFile):
    packages = []
    pr("asasasasa  " + xmlFile)
    try:
        xmlTree = ET.parse(xmlFile)
    except Exception as ex:
        prerr("Cannot parse '" + xmlFile + "' : " + str(ex.strerror))   
        return packages
    
    rootNode = xmlTree.getroot()    
    for childNode in rootNode:        
        pkgName = childNode.attrib['name']
        pkgSum = childNode.attrib['sum']
        pkgUrl = childNode.attrib['pkgurl']
        pkgFile = os.path.basename(pkgUrl)                                        
        package = PackageDesc(pkgName, pkgFile, pkgSum, pkgUrl)              
        packages.append(package) 
        dbg("red package " + pkgName +" "+pkgSum+" "+pkgFile + " "+pkgUrl)
    
    return packages

def getPackagesToUpdate(installed, update):
    lst = []
    
    if ((installed==None) or (installed==None) or (len(installed)==0)):
        pr("There are not any recently installed packages.")
        
    if ((update==None) or (update is None) or (len(update)==0)):
        pr("No updates are available.")
        return lst
        
    for pkg in update:
        found = False
        for ipkg in installed:
            if (pkg.name == ipkg.name):                              
                if (pkg.sum != ipkg.sum):                         
                    pr("  Package '"+ipkg.name+"' will be updated")
                    lst.append(pkg)                
                else:
                    pr("  Package '"+ipkg.name+"' is already in latest version")                                
                found = True  
                break              
                        
        if (found == False):
            pr("New package '"+pkg.name+"' will be installed")
            lst.append(pkg)
                
    return lst
                                        

def getSingleLine(filepath):
    file = None
    try:
        file = open(filepath, "r")
    except IOError as ex:
        prerr(" " + e.errno +" "+ e.strerror)
        return None
    
    line = file.readline()
    file.close()
    return line

def installPackage(pkg):    
    if ((pkg==None) or (pkg is None)):
        prierr("pkg==None")
        return False

    # cd into package directory
    dir_parent = os.getcwd() 
    os.chdir(pkg.name)            
    
    # Untar package .tar.gz archive
    pr("  Unpacking '"+pkg.file+"' ...")
    #os.system("tar -xf "+pkg.file +" --strip 1")
    (stdout, stderr) = Popen(["tar", "-xf", "./"+pkg.file], stdout=PIPE).communicate()
    out = str(stdout)  
    print(out)
    os.system("sync")        
    
    # Find and run install.sh script
    status = os.path.isfile(pkg_install_script)
    if (status==False):
        sys.stdout.write(RED)
        prwarn("  Package '"+pkg.name+"' does not contain a valid install.sh script.")
        sys.stdout.write(RESET)
        os.chdir(dir_parent)
        return False
    
    os.system("chmod +x "+pkg_install_script)
    
    (stdout, stderr) = Popen(["./"+pkg_install_script], stdout=PIPE).communicate()
    out = str(stdout)    
    print(out)
    cnt = out.find("INSTALLED")
    if (cnt==-1):
        os.chdir(dir_parent)
        return False
     
    os.chdir(dir_parent)    
    return True

def getSum(filepath):
    try:
        file = open(filepath, "r")
    except Exception as ex:
        prerr(" " + ex.errno +" "+ ex.strerror)
        return None
    
    data = file.read()
    sha1sum = hashlib.sha1(data)
    sha1hex = sha1sum.hexdigest()    
    return sha1hex
        

def downloadPackage(pkg):            
    if ((pkg==None) or (pkg is None)):
        prierr("pkg==None")
        return False
            
    status = os.path.isdir(pkg.name)
    if (status==True):
        prwarn("Directory '"+pkg.name+"' exists, deleting it...")
        os.system("rm -rf " + pkg.name)
        
    os.system("mkdir " + pkg.name)
    os.system("sync")    
    
    dir_parent = os.getcwd() 
    os.chdir(pkg.name)    
    
    integrity = False
    i = 0
    while (integrity==False):                            
        try:
            file = wget.download(pkg.url)                                    
        except Exception as ex:
            prerr("Cannot get '"+pkg.url+"'"+" : "+str(ex.strerror))            
            os.chdir(dir_parent)
            os.system("rm -rf "+pkg.name)
            os.system("sync")
            return False
        
        print("")
        os.system("sync")
        sum = getSum(file)        
        if (sum.strip()!=pkg.sum):
            prwarn("Package '"+pkg.name+"' is corrupted. Retrying "+str(i)+ " ...")            
            integrity = False            
            os.system("rm -rf " + pkg.file)
            os.system("sync")            
        else:            
            integrity = True 
        i = i + 1
        if (i>=5):
            pr("Aborting ...")
            os.chdir(dir_parent)  
            os.system("rm -rf "+pkg.name)
            os.system("sync")                        
            return False                       
    
    os.chdir(dir_parent)                
    return True
    

def getUpdateLevel():
    status = os.path.isfile("/etc/init.d/netexy")
    if (status==True):
        lvl = LEVEL_2
    else:
        status = os.path.isfile("/etc/init.d/S41kmaxnet")
        if (status==True):
            lvl = LEVEL_1
        else:
            prerr("Unknown update level. Exiting...")
            return None
    return lvl

def getUpdateUrl():
    global ip_mng_npu
    global update_file
    global ip_mng_npu    
    global to_update
    
    #ip_mng_npu = common.getNPUMngIP()
    ip_mng_npu = "192.168.1.3"    
    if ((ip_mng_npu==None) or (ip_mng_npu is None)):
        return None
           
    lvl = getUpdateLevel()    
    if ((lvl == None) or (lvl is None)):
        return None
    url =  "http://" + ip_mng_npu + "/" + "level"+ lvl +"/" + update_file 
    return url
    

def main():    
    global working_dir
    global dir_download
    global dir_abs_download
    global installed_packages
    
    working_dir = os.path.dirname(os.path.realpath(__file__))    
    os.chdir(working_dir)    
    dir_abs_download = working_dir + "/" + dir_download
    pr("Set working directory to '"+os.getcwd()+"'  and download directory to '"+dir_abs_download+"'")    
    
    os.system("rm -rf " + dir_download)
    
    urlUpdate = getUpdateUrl()
    if ((urlUpdate==None) or (urlUpdate is None)):
        prerr("Failed to get update URL. Exiting... ")
        sys.exit(0)
    
    pr("Got update URL '" + urlUpdate + "'")    
    
    #urlUpdate = "http://" + ip_mng_npu + "/" + update_file    
    
    status = os.path.isfile(update_file)
    if (status==True):
        pr(update_file+" exists, deleting... ")
        os.remove(update_file)
        
    pr("Downloading update information...")  
    try:
        filename = wget.download(url=urlUpdate, out=update_file)        
    except Exception as ex:
        prerr("Cannot get '"+url+"'"+" : "+str(ex.strerror))
        pr("Exiting...")
        sys.exit(0)
          
    print("")          
    dbg("Downloaded update information '"+filename+"' from " + ip_mng_npu)
    
    # Get update packages description list
    update_packages = getPackageDescList(filename)
    if ((update_packages == None) or (update_packages is None)):
        prerr("Cannot get update packages information. Exiting...")
        sys.exit(0)
                
    dbg("CWD = '"+os.getcwd()+"'")
    # Get installed packages description list
    status = os.path.isfile(packages_file)
    if (status==False):
        prwarn("There is no information about currently installed packages")
        installed_packages = []
    else:        
        installed_packages = getPackageDescList(packages_file)
        if ((installed_packages == None) or (installed_packages is None)):
            prerr("Cannot get installed packages information. Exiting...")
            sys.exit(0)
    
    dbg("CWD = '"+os.getcwd()+"'")
    # Check if update packages are needed       
    to_update = getPackagesToUpdate(installed_packages, update_packages)
    if ((to_update==None) or (to_update is None)):            
        prierr("to_update==None. Exiting...")
        sys.exit(0)        
    
    nrpkg = len(to_update)
    
    pr(str(nrpkg) + " packages will be downloaded")
    if (nrpkg==0):      
        pr("Exiting...")
        sys.exit(0)        

    
    dbg("CWD = '"+os.getcwd()+"'")
    
    # Download update packages    
    os.system("mkdir " + dir_download)

    dbg("CWD = '"+os.getcwd()+"'")    
    os.chdir(dir_download)        
    for pkg in to_update:
        pr("Downloading package '"+pkg.name+"'")        
        pkg.downloaded = downloadPackage(pkg)
        if(pkg.downloaded==False):                        
            sys.stdout.write(RED)
            prwarn("   Failed to download package '"+pkg.name+"'. It will not be installed. Continuing...")
            sys.stdout.write(RESET)                      
        else:            
            sys.stdout.write(GREEN)
            pr("   Downloaded package '"+pkg.name+"'")     
            sys.stdout.write(RESET)            
        
    
    nrpkg = lenDownloadedPackages(to_update)
    pr(str(nrpkg) + " update packages will be installed")
    if (nrpkg==0):      
        pr("Exiting...")
        sys.exit(0)        
        
    # Install all successfully downloaded packages
    for pkg in to_update:
        if (pkg.downloaded == True):
            pkg.installed = installPackage(pkg)
            if (pkg.installed == False):                
                sys.stdout.write(RED)
                prwarn(" Package '"+pkg.name+"' failed to install. Continuing... ")
                sys.stdout.write(RESET) 
                continue
            else:
                sys.stdout.write(GREEN)
                pr("   Package '"+pkg.name+"' was successfully installed")     
                sys.stdout.write(RESET)               
     
    nrpkg = lenInstalledPackages(to_update)
    pr(str(nrpkg) + " update packages were successfully installed")   
    
    pr("Summary:")
    for pkg in to_update:
        if (pkg.downloaded == True):
            if (pkg.installed ==  True):
                sys.stdout.write(GREEN)
                pr(" " + pkg.name + ": INSTALLED" )
                sys.stdout.write(RESET) 
            else:
                sys.stdout.write(RED)
                pr(" " + pkg.name + ": NOT INSTALLED" )
                sys.stdout.write(RESET)           
        else:
                 sys.stdout.write(RED)
                 pr(" " + pkg.name + ": NOT DOWNLOADED" )
                 sys.stdout.write(RESET)              
    
         
               
    pr("aaaaaaaaa " + str(len(installed_packages)))
    # Update installed_packages list with latest values    
    for pkg in to_update:
        if (pkg.installed == True):
            ipkg = getPackageByName(installed_packages, pkg.name)            
            if ((ipkg==None) or (ipkg is None)):
                # It is a new package add it
                installed_packages.append(pkg)
            else:
                # It is an update of an already installed package, modify it
                ipkg.sum = pkg.sum
                ipkg.donwloaded = True      
                ipkg.installed = True
                ipkg.url = pkg.url        
                
    pr("aaaaaaaaa2 " + str(len(installed_packages)))
            
    # Update local packages.xml according to installed_packages list
    os.chdir(working_dir)
    pr("CWD = '"+os.getcwd()+"'")      
    
    status = os.path.isfile(packages_file)
    if (status==False):
        pr("First time to write "+packages_file)
        os.system("cp -f "+update_file+" "+packages_file)        
    else:        
        try:
            xmlTree = ET.parse(packages_file)
        except IOError as ex:
            prerr("Cannot parse '" + packages_file + "' : " + ex.strerror)   
        
        rootNode = xmlTree.getroot()
        
        for ipkg in installed_packages:
            node = getXMLNodeByName(rootNode, ipkg.name)
            if ((node==None) or (node is None)):
                # This is a newly installed package, add a new XML entry
                pr("Add new XML entry for '"+ipkg.name+"'")
                new_node = ET.Element("item")
                new_node.set("name", ipkg.name)
                new_node.set("pkgurl", ipkg.url)
                new_node.set("sum", ipkg.sum)
                rootNode.append(new_node)
            else:
                # This an updated package, modify its existing XML entry
                pr("Modify XML entry for '"+ipkg.name+"'")                                
                node.set("pkgurl", ipkg.url)
                node.set("sum", ipkg.sum)         
        
        xmlTree.write(packages_file)    
    
#    os.system("rm -f packages.xml")
#    os.system("mv -f " + update_file + " packages.xml")
    os.system("sync")
    pr("Updated installed packages information")
                   
if __name__=="__main__":
    main()

