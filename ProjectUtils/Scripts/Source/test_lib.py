import configparser
import winreg
import subprocess
import os.path
import os
import shutil
import re


def GetEngineRootDir():
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("PakTestConfig.ini")
    return os.path.normpath(myConfigParser['EnvironmentConfig']['EngineRootDir'])


def GetUnrealPakToolPath():
    return os.path.normpath(GetEngineRootDir() + "\\Engine\\Binaries\\Win64\\UnrealPak.exe")


def GetEditorCmdExePath():
    return os.path.normpath(GetEngineRootDir() + "\\Engine\\Binaries\\Win64\\UE4Editor-Cmd.exe")


def GetUATToolPath():
    return os.path.normpath(GetEngineRootDir() + "\\Engine\\Build\\BatchFiles\\RunUAT.bat")

# Try to open Unreal Engine reg key, this will fail if it has not been installed.


def DetectUnrealEngineRootDir(inExpectedVersion):
    try:
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
                             r"SOFTWARE\\EpicGames\\Unreal Engine")
    except WindowsError:
        raise Exception(
            "Error:No Unreal Engine was installed, check and retry!")

    try:
        expectedEngineKey = winreg.OpenKey(key, str(inExpectedVersion))
    except WindowsError:
        raise Exception('Unreal Engine {} was not installed, check and retry!'.format(
            inExpectedVersion))

    engineRootDir, _ = winreg.QueryValueEx(
        expectedEngineKey, "InstalledDirectory")

    return engineRootDir


class AssetPackage:
    def __init__(self, name):
        self.name = name
        self.associatedFiles = set([])

    def AddAssociatedFile(self, filepath):
        self.associatedFiles.add(os.path.normpath(os.path.abspath(filepath)))


def GetAllAssetPackagesInCookedContent(targetCookedContentDir):
    tempMap = {}
    for root, dirs, files in os.walk(targetCookedContentDir):
        for filename in files:
            filepath = os.path.join(root, filename)

            filepathWithoutExt, ext = os.path.splitext(filepath)
            fileDir, assetPackageName = os.path.split(filepathWithoutExt)

            m = re.match(r'(.*)_BuiltData', assetPackageName)
            if m:
                assetPackageAlias = os.path.join(fileDir , m.group(1))
            else:
                assetPackageAlias = filepathWithoutExt

            if tempMap.__contains__(assetPackageAlias):
                tempMap[assetPackageAlias].AddAssociatedFile(filepath)
            else:
                tempMap[assetPackageAlias] = AssetPackage(assetPackageAlias)
                tempMap[assetPackageAlias].AddAssociatedFile(filepath)
    results = []

    for key in tempMap:
        results.append(tempMap[key])
    return results

# This function accepts a short package filepath with out extension.
# And when generating pak file, it will add a dummy file to the reponse file list,
# in this way, the relative path to expected package root path will be saved in pak file.
# For example:
# If the input parameters targetPackagePathRootDir="F:\\AAA\\BBB\\" targetPackagePath="F:\\AAA\\BBB\\CCC\\*.*",
# the in the output pak file, the file's save path  will start with "\\CCC\\"
def GenerateSplitedPaks(outputPakFileDir, targetPackagePathRootDir, assetPackage):
    pakCmdTemplate = '"{}" "{}" {} "{}"'
    urealPakToolPath = GetUnrealPakToolPath()

    outputPakFileDir = os.path.normpath(outputPakFileDir)
    targetPackagePathRootDir = os.path.normpath(targetPackagePathRootDir)
    
    filesToWrite = ""
    for associatedFile in assetPackage.associatedFiles:
        filesToWrite += ' "{}"'.format(associatedFile)  
    
    #Use hash
    pakFilename = str(hash(assetPackage.name))

    # _, pakFilename = os.path.split(assetPackage.name)
    
    outputPakFilePath = os.path.join(outputPakFileDir, pakFilename + ".pak")
    dummyPackagePath = os.path.join(targetPackagePathRootDir, "dummy.uasset")

    pakCmd = pakCmdTemplate.format(urealPakToolPath, outputPakFilePath, filesToWrite, dummyPackagePath)

    subprocess.call(pakCmd, shell=True)

def GeneratePakFromResponseFile():
    # todo
    return


def GetCurrentProjectRootDir():
    return os.path.abspath("..\\..\\")


def GetProjectNameFromProjectRootDir(projectRootDir):
    _, projectName = os.path.split(projectRootDir)
    return projectName


def GetCurrentProjectName():
    return GetProjectNameFromProjectRootDir(GetCurrentProjectRootDir())


# It's not reliable to get this from windows registry table.
# So just read it from config.
def GetDesktopPathFromConfig():
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("PakTestConfig.ini")
    DesktopDir = myConfigParser['EnvironmentConfig']['DesktopDir']
    return os.path.normpath(DesktopDir)


def GetContentProviderProjectRootDir():
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("PakTestConfig.ini")
    return os.path.normpath(myConfigParser['ContentProviderProjectConfig']['ProjectRootDir'])


def GetContentProviderProjectName():
    return GetProjectNameFromProjectRootDir(GetContentProviderProjectRootDir())


def GetCurrentProjectPackagedProjectOutputDir():
    myConfigParser = configparser.ConfigParser()
    myConfigParser.read("PakTestConfig.ini")
    return os.path.normpath(myConfigParser['CurrentProjectConfig']['PackagedProjectOutputDir'])

def ListFilesInPak(pakFilepath):
    pakExtractCmdTemplate = '"{}" "{}" -extract -list"'
    urealPakToolPath = GetUnrealPakToolPath()
    targetDir = "C:\\Users\\zhoumy\Desktop\\TestPak\\ProjectUtils\\TestOutput\\"
  
    pakExtractCmd = pakExtractCmdTemplate.format(urealPakToolPath, pakFilepath)
    subprocess.call(pakExtractCmd, shell=True)
