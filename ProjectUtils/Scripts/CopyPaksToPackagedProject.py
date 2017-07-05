import configparser
import winreg
import subprocess
import os.path
import re

def GetLast():
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("..\\..\\Saved\\Config\\Windows\\Game.ini")
    lastPackagedProjectOutputPathConfig = myConfigParser['/Script/UnrealEd.ProjectPackagingSettings']['StagingDirectory']
    pattern = re.compile(r'\(Path="(.*)"\)')
    m = pattern.search(lastPackagedProjectOutputPathConfig )
    lastPackagedProjectOutputPath = m.group(1) 
    return lastPackagedProjectOutputPath

def GetDesktop():
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("PakTestConfig.ini")
    DesktopDir = myConfigParser['BaseConfig']['DesktopDir']
    return os.path.normpath(DesktopDir)

if __name__ == "__main__":
    try:
        lastPackagedProjectOutputPath = GetLast()
    except:
        lastPackagedProjectOutputPath =  GetDesktop()
        pass 

    targetMyPaksDir = os.path.join(lastPackagedProjectOutputPath, 'WindowsNoEditor\\TestPak\\SplitedPaks\\')
    targetMyPaksDir = os.path.normpath(targetMyPaksDir)

    subprocess.call( 'md "{}"\n'.format(targetMyPaksDir), shell=True)
    subprocess.call( 'copy /y ..\\TestOutput\\ "{}"'.format(targetMyPaksDir), shell=True)
