import configparser
import winreg
import subprocess
import os.path

def DetectUnrealEngineRootDir(inExpectedVersion):
    #Try to open Unreal Engine reg key, this will fail if it has not been installed.
    try:   
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,r"SOFTWARE\EpicGames\Unreal Engine")
    except WindowsError:
        raise Exception("Error:No Unreal Engine was installed, check and retry!")
    
    try:   
       expectedEngineKey = winreg.OpenKey(key, str(inExpectedVersion))
    except WindowsError:
        raise Exception('Unreal Engine {} was not installed, check and retry!'.format(inExpectedVersion))
    
    engineRootDir, _ = winreg.QueryValueEx(expectedEngineKey, "InstalledDirectory")
    
    return engineRootDir


if __name__ == "__main__":
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("PakTestConfig.ini")
    
    expectedEngineVersion = myConfigParser['PakConfig']['EngineVersion']
    
    try:
      engineRootDir =  DetectUnrealEngineRootDir(expectedEngineVersion) 
    except Exception as e:
        print(e)
        exit()

        

    # pak section
    pakCmdTemplate = '"{}" {} -create="{}" -encryptionini -enginedir="{}" -projectdir="{}" -platform=Windows -abslog="{}" -installed -order="{}" -UTF8Output -multiprocess -patchpaddingalign=2048'
    urealPakToolPath = engineRootDir + "\\Engine\\Binaries\\Win64\\UnrealPak.exe"
    outputPakFilePath = myConfigParser['PakConfig']['OutputPakFilePath']
    pakResponseFilePath = myConfigParser['PakConfig']['PakResponsibleFilePath']
    engineDir=engineRootDir + "\\Engine\\
    projectDir="C:\Users\zhoumy\Desktop\TestPak"
    pakLogOutputPath="C:\Users\zhoumy\Desktop\TestPak\\pak.log"
    openOrderFilePath="C:\Users\zhoumy\Desktop\TestPak\\order.log"

    pakCmd = pakCmdTemplate.format(urealPakToolPath, outputPakFilePath, pakResponseFilePath )
    print(pakCmd)
    # subprocess.call(pakCmd, shell=True)

    #cook section
    cookCmdTemplate =  '"{}" "{}" -run=Cook  -NoLogTimes -TargetPlatform=WindowsNoEditor -fileopenlog -unversioned -abslog="{}" -stdout -CrashForUAT -unattended'  
    editorCmdExePath = engineRootDir + "\\Engine\\Binaries\\Win64\\UE4Editor-Cmd.exe"
    targetProjectPath = "C:\\Users\\zhoumy\\Desktop\\TestPak\\TestPak.uproject"
    cookLogOutputPath = "C:\\Users\\zhoumy\\Desktop\\cook.log"

    cookCmd = cookCmdTemplate.format(editorCmdExePath, targetProjectPath, cookLogOutputPath)
    print(cookCmd)
    subprocess.call(cookCmd, shell=True)


