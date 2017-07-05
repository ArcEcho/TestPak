import configparser
import winreg
import subprocess
import os.path
import re

if __name__ == "__main__":
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("..\\..\\Saved\\Config\\Windows\\Game.ini")
    
    lastPackagedProjectOutputPathConfig = myConfigParser['/Script/UnrealEd.ProjectPackagingSettings']['StagingDirectory']
    
    pattern = re.compile(r'\(Path="(.*)"\)')
    m = pattern.search(lastPackagedProjectOutputPathConfig )
    lastPackagedProjectOutputPath = m.group(1) 
    
    targetMyPaksDir = os.path.join(lastPackagedProjectOutputPath, 'WindowsNoEditor\\TestPak\\MyPaks\\')
    targetMyPaksDir = os.path.normpath(targetMyPaksDir)

    subprocess.call( 'md "{}"\n'.format(targetMyPaksDir), shell=True)
    subprocess.call( 'copy /y ..\\TestOutput\\ "{}"'.format(targetMyPaksDir), shell=True)
