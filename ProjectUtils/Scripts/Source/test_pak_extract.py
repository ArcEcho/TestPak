import configparser
import winreg
import subprocess
import os.path
from hashlib import blake2b

def DetectUnrealEngineRootDir(inExpectedVersion):
    #Try to open Unreal Engine reg key, this will fail if it has not been installed.
    try:   
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,r"SOFTWARE\\EpicGames\\Unreal Engine")
    except WindowsError:
        raise Exception("Error:No Unreal Engine was installed, check and retry!")
    
    try:   
       expectedEngineKey = winreg.OpenKey(key, str(inExpectedVersion))
    except WindowsError:
        raise Exception('Unreal Engine {} was not installed, check and retry!'.format(inExpectedVersion))
    
    engineRootDir, _ = winreg.QueryValueEx(expectedEngineKey, "InstalledDirectory")
    
    return engineRootDir

def GetAllAssetsInCookedContent(targetCookedContentDir):
    results = set([])
    for root, dirs, files in os.walk(targetCookedContentDir):     
        for filename in files:
            filenameWithoutExt,_  = os.path.splitext(filename) 
            results.add(os.path.join(root, filenameWithoutExt))
    return results

def PakFile(outputPakFileDir, targetPackagePathRootDir, targetPackagePath ):
    pakCmdTemplate = '"{}" "{}" "{}" "{}"'
    urealPakToolPath = "C:\\Program Files\\Epic Games\\UE_4.16\Engine\\Binaries\\Win64\\UnrealPak.exe"

    outputPakFileDir = os.path.normpath(outputPakFileDir)
    targetPackagePathRootDir = os.path.normpath(targetPackagePathRootDir)
    targetPackagePathWithWildcard = os.path.normpath(targetPackagePath) + ".*"

    relativePackagePath = targetPackagePathWithWildcard.replace(targetPackagePathRootDir, "")
    hashStr = str(hash(relativePackagePath))
    outputPakFilePath = os.path.join(outputPakFileDir, hashStr + ".pak")
    dummyPackagePath = os.path.join(targetPackagePathRootDir, "dummy.uasset")

    # print(outputPakFilePath)
    # print(targetPackagePath)
    # print(dummyPackagePath)

    pakCmd = pakCmdTemplate.format(urealPakToolPath, outputPakFilePath, targetPackagePathWithWildcard, dummyPackagePath )
    print(pakCmd)
    subprocess.call(pakCmd, shell=True)


if __name__ == "__main__":
    pakExtractCmdTemplate = '"{}" "{}" -extract -list"'
    urealPakToolPath = "C:\\Program Files\\Epic Games\\UE_4.16\Engine\\Binaries\\Win64\\UnrealPak.exe"
    targetDir = "C:\\Users\\zhoumy\Desktop\\TestPak\\ProjectUtils\\TestOutput\\"
    for root, dirs, files in os.walk(targetDir):     
        for filename in files: 
            pakExtractCmd = pakExtractCmdTemplate.format(urealPakToolPath, os.path.join(root, filename))
            subprocess.call(pakExtractCmd, shell=True)


