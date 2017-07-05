import configparser
import winreg
import subprocess
import os.path
import os
import shutil

def GetUnrealPakToolPath():
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("PakTestConfig.ini")
    EngineRootDir = myConfigParser['BaseConfig']['EngineRootDir']
    toolPath = EngineRootDir + "\\Engine\\Binaries\\Win64\\UnrealPak.exe" 
    return os.path.normpath(toolPath)


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
    urealPakToolPath = GetUnrealPakToolPath()

    outputPakFileDir = os.path.normpath(outputPakFileDir)
    targetPackagePathRootDir = os.path.normpath(targetPackagePathRootDir)
    targetPackagePathWithWildcard = os.path.normpath(targetPackagePath) + ".*"

    relativePackagePath = targetPackagePathWithWildcard.replace(targetPackagePathRootDir, "")
    hashStr = str(hash(relativePackagePath))
    outputPakFilePath = os.path.join(outputPakFileDir, hashStr + ".pak")
    dummyPackagePath = os.path.join(targetPackagePathRootDir, "dummy.uasset")



    pakCmd = pakCmdTemplate.format(urealPakToolPath, outputPakFilePath, targetPackagePathWithWildcard, dummyPackagePath )
    subprocess.call(pakCmd, shell=True)


if __name__ == "__main__":
    targetCookedContentDir = os.path.abspath("..\\..\\CookedContentFromAnotherProject\\")
    outputPakFileDir =  os.path.abspath("..\\TestOutput")
    outputPakFileDirPendingDelete =  os.path.abspath("..\\TestOutput_pending_delete")


    os.rename(outputPakFileDir, outputPakFileDirPendingDelete)
    shutil.rmtree(outputPakFileDirPendingDelete)
    
    assets = GetAllAssetsInCookedContent(targetCookedContentDir)
    for assetName in assets:
        PakFile(outputPakFileDir, targetCookedContentDir, assetName)
    
    


