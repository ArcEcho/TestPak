import configparser
import winreg
import subprocess
import os.path
import json
import shutil
import hashlib
import glob

myConfigparser = configparser.ConfigParser()

def InitConfigParser(iniFilepath):
    myConfigparser.read(iniFilepath)

def GetEngineRootDir():
    return os.path.normpath(myConfigparser['EnvironmentConfig']['EngineRootDir'])

def GetUnrealPakToolPath():
    return os.path.normpath(GetEngineRootDir() + "\\Engine\\Binaries\\Win64\\UnrealPak.exe")

def GetTargetPorjectRootDir():
    return os.path.normpath(myConfigparser['TargetProjectConfig']['ProjectRootDir'])

def GetTargetPorjectName():
    _, porjectName = os.path.split(GetTargetPorjectRootDir())
    return porjectName

def GetTargetProjectCookedGameDir():
    return os.path.normpath(os.path.join(GetTargetPorjectRootDir(), "Saved\\Cooked\\WindowsNoEditor", GetTargetPorjectName()))

def GetTargetProjectCookedRootDir():
    return os.path.normpath(os.path.join(GetTargetPorjectRootDir(), "Saved\\Cooked\\WindowsNoEditor"))

def GetTargetProjectCookedContentDir():
    return os.path.normpath(os.path.join(GetTargetPorjectRootDir(), "Saved\\Cooked\\WindowsNoEditor", GetTargetPorjectName(), "Content" ))

def GetSha1OfLongPackageName(longPackageName):
    sha1Obj = hashlib.sha1()
    sha1Obj.update(str.encode(longPackageName))
    return sha1Obj.hexdigest()

def GeneratePak(outputPakFileDir, targetPackagePathRootDir, longPackageName, inShouldPackTo64KB = False):
    pakCmdTemplate = '"{}" "{}" -create="{}" -encryptionini -platform=Windows -installed -UTF8Output -multiprocess -patchpaddingalign=2048 -abslog="{}" -projectdir={} -enginedir={}'
    urealPakToolPath = GetUnrealPakToolPath()
    outputPakFileDir = os.path.normpath(outputPakFileDir)
    targetPackagePathRootDir = os.path.normpath(targetPackagePathRootDir)
    outputPakFilename = GetSha1OfLongPackageName(longPackageName)
    # Generate pak response file.
    pakReponseFilepath = os.path.join(GetTargetPorjectRootDir(), "Saved\\ProjectUtils\\paklist.txt")
    with open(pakReponseFilepath, 'w') as pakReponseFileHandle:
        filesToWrite = []
        fileToWriteEntry = longPackageName.replace('/Game', "Content" )
        fileToWriteEntry = os.path.normpath(os.path.join(targetPackagePathRootDir , fileToWriteEntry))
        searchStr = '{}.*'.format(fileToWriteEntry)
        searchResult = glob.glob(searchStr)
        rootProjectDirInSavedContent = GetTargetPorjectRootDir
        for r in searchResult:
            # Convert to relative
            tt = GetTargetProjectCookedRootDir()
            relativePath = r.replace(tt, "..\\..\\..")
            relativePath = relativePath.replace("\\", "/")
            filesToWrite.append('"{}" "{}"\n'.format(r, relativePath))

        # Add dummy file
        # dummyPackagePath = os.path.join(targetPackagePathRootDir, "dummy.uasset")  
        # filesToWrite.append('"{}"\n'.format(dummyPackagePath))

        # Check pack
        packDataFilepath = None
        if inShouldPackTo64KB:
            targetProjectCookedContentDir = GetTargetProjectCookedContentDir()
            packDataFilepath = os.path.join(targetProjectCookedContentDir, "pack_data.dat")
            if not os.path.exists(packDataFilepath):
                shutil.copy('.\\pack_data.dat', targetProjectCookedContentDir)
                
        if packDataFilepath != None:
            filesToWrite.append('"{}" "{}"\n'.format(packDataFilepath, "../../../" + GetTargetPorjectName()  + "/Content/pack_data.dat"))
    
        pakReponseFileHandle.writelines(filesToWrite)

    absPakLogFilepath = os.path.join(GetTargetPorjectRootDir(), "Saved\\ProjectUtils\\Log\\", outputPakFilename) + ".log"
      
    outputPakFilePath = os.path.join(outputPakFileDir, outputPakFilename + ".pak")

    projectDir = GetTargetPorjectRootDir()
    engineDir = GetEngineRootDir()

    pakCmd = pakCmdTemplate.format(urealPakToolPath, outputPakFilePath, pakReponseFilepath, absPakLogFilepath, projectDir, engineDir)

    subprocess.check_call(pakCmd, shell=True)

    return outputPakFilePath

def GenerateBatchPak(outputPakFileDir, targetPackagePathRootDir, longPackageNameList, outputPakFilename, inShouldPackTo64KB = False):
    pakCmdTemplate = '"{}" "{}" -create="{}" -encryptionini -platform=Windows -installed -UTF8Output -multiprocess -patchpaddingalign=2048 -abslog="{}"'
    urealPakToolPath = GetUnrealPakToolPath()

    outputPakFileDir = os.path.normpath(outputPakFileDir)
    targetPackagePathRootDir = os.path.normpath(targetPackagePathRootDir)
   
    # Generate pak response file.
    pakReponseFilepath = os.path.join(GetTargetPorjectRootDir(), "Saved\\ProjectUtils\\paklist.txt")
    with open(pakReponseFilepath, 'w') as pakReponseFileHandle:
        filesToWrite = []
        for longPackageName in longPackageNameList:
            fileToWriteEntry = longPackageName.replace('/Game', "Content" )
            fileToWriteEntry = os.path.normpath(os.path.join(targetPackagePathRootDir , fileToWriteEntry))
            searchStr = '{}.*'.format(fileToWriteEntry)
            searchResult = glob.glob(searchStr)
            rootProjectDirInSavedContent = GetTargetPorjectRootDir
            for r in searchResult:
                # Convert to relative
                tt = GetTargetProjectCookedRootDir()
                relativePath = r.replace(tt, "..\\..\\..")
                relativePath = relativePath.replace("\\", "/")
                filesToWrite.append('"{}" "{}"\n'.format(r, relativePath))

        # Add dummy file
        # dummyPackagePath = os.path.join(targetPackagePathRootDir, "dummy.uasset")  
        # filesToWrite.append('"{}"\n'.format(dummyPackagePath))

        # Check pack
        packDataFilepath = None
        if inShouldPackTo64KB:
            packDataFilepath = os.path.join(targetPackagePathRootDir, "pack_data.dat")
            if not os.path.exists(packDataFilepath):
                shutil.copy('.\\pack_data.dat', targetPackagePathRootDir)
            packDataFilepath = os.path.join(targetPackagePathRootDir, "pack_data.dat")
   
        if packDataFilepath != None:
            filesToWrite.append('"{}" "{}"\n'.format(packDataFilepath, "../../../" + GetTargetPorjectName()  + "/Content/pack_data.dat"))
    
        pakReponseFileHandle.writelines(filesToWrite)

    absPakLogFilepath = os.path.join(GetTargetPorjectRootDir(), "Saved\\ProjectUtils\\Log\\", outputPakFilename) + ".log"
      
    outputPakFilePath = os.path.join(outputPakFileDir, outputPakFilename + ".pak")

    pakCmd = pakCmdTemplate.format(urealPakToolPath, outputPakFilePath, pakReponseFilepath, absPakLogFilepath)

    subprocess.check_call(pakCmd, shell=True)

    return outputPakFilePath



if __name__ == "__main__":
    
    InitConfigParser("ProjectUtilsConfig.ini")    
    
    #Delete old project utils saved dir if exists.
    projectUtilsSavedDir =  os.path.join(GetTargetPorjectRootDir(), "Saved\\ProjectUtils\\")
    if os.path.exists(projectUtilsSavedDir):
        subprocess.check_call("rd /q /s {}".format(projectUtilsSavedDir), shell = True)
    
    subprocess.check_call("md {}".format(projectUtilsSavedDir), shell = True)

    generatePakLogFilepath = os.path.join(projectUtilsSavedDir, "GeneratePaks.log")
    assetDependenciesFileFilpath = os.path.join(GetTargetPorjectRootDir(), "Saved\\ExportAssetDependecies\\AssetDependencies.json")

    targetProjectCookedGameDir = GetTargetProjectCookedGameDir()

    with open(generatePakLogFilepath, 'w') as generatePakLogFileHandle:
        with open(assetDependenciesFileFilpath, 'r') as assetDependenciesFileHandle:
            data = json.load(assetDependenciesFileHandle)
            for (targetPackage,dependentData) in  data.items():
                sha1OfLongTargetPackageName = GetSha1OfLongPackageName(targetPackage)

                # Now just handle dependencies in game content dir.
                packagesToHandle = dependentData['DependenciesInGameContentDir']
                packagesToHandle.append(targetPackage)

                # Make output dir with sha1 value 
                targetOutputDir = os.path.join(projectUtilsSavedDir, sha1OfLongTargetPackageName)
                os.mkdir(targetOutputDir)

                # Fill json object and generate pak.
                jsonObject = {}
                jsonObject["long_package_name"] = targetPackage
                jsonObject["dependent_pak_file_list"] = []

                bShouldGenerateBatchPak = myConfigparser.getboolean("TargetProjectConfig", "UseBatchPakFile")
                if not bShouldGenerateBatchPak:
                    for longPackageName in packagesToHandle:
                        outputPakFilePath = GeneratePak(targetOutputDir, targetProjectCookedGameDir, longPackageName)
                        # If the pak file size is less than 64KB, when pack and regenerate pak file to meet the PAK_PRECAHCE_GRUNULARITY. 
                        if os.path.getsize(outputPakFilePath) < 64 * 1024:
                            os.remove(outputPakFilePath)
                            outputPakFilePath = GeneratePak(targetOutputDir, targetProjectCookedGameDir, longPackageName,  inShouldPackTo64KB = True)
                            isPacked = True       
                        else:
                            isPacked = False
                        _, outputFilename = os.path.split(outputPakFilePath) 

                        if longPackageName != targetPackage:
                            #  
                            entry = {}
                            entry["file_name"] = outputFilename
                            entry["file_size"] = os.path.getsize(outputPakFilePath)
                            entry["long_package_name"] = longPackageName
                            entry["is_packed_to_64KB"] = isPacked
                            jsonObject["dependent_pak_file_list"].append(entry)
                        else:
                            # Dependencies
                            filename = longPackageName.replace('/Game', "" )
                            filename = os.path.normpath(targetProjectCookedGameDir + filename)
                            jsonObject["file_size"] = os.path.getsize(outputPakFilePath)
                            jsonObject["pak_file"] = outputFilename
                            jsonObject["is_packed_to_64KB"] = isPacked
                            jsonObject["is_level"] = os.path.exists(filename + ".umap")

                            # Write json object to info file.
                            generatePakLogFileHandle.write('"{}" --> "{}"\n'.format(longPackageName, sha1OfLongTargetPackageName ))
                else:
                    outputPakFilePath = GenerateBatchPak(targetOutputDir, targetProjectCookedGameDir, packagesToHandle, sha1OfLongTargetPackageName)
                    
                    # If the pak file size is less than 64KB, when pack and regenerate pak file to meet the PAK_PRECAHCE_GRUNULARITY. 
                    if os.path.getsize(outputPakFilePath) < 64 * 1024:
                        os.remove(outputPakFilePath)
                        outputPakFilePath =  GenerateBatchPak(targetOutputDir, targetProjectCookedGameDir, packagesToHandle, sha1OfLongTargetPackageName, inShouldPackTo64KB = True)
                        isPacked = True       
                    else:
                        isPacked = False
                    _, outputFilename = os.path.split(outputPakFilePath) 
                    
                    # Dependencies
                    filename = targetPackage.replace('/Game', "content" )
                    filename = os.path.normpath(os.path.join(targetProjectCookedGameDir,filename))
                    jsonObject["file_size"] = os.path.getsize(outputPakFilePath)
                    jsonObject["pak_file"] = outputFilename
                    jsonObject["is_packed_to_64KB"] = isPacked
                    jsonObject["is_level"] = os.path.exists(filename + ".umap")

                    # Write json object to info file.
                    generatePakLogFileHandle.write('"{}" --> "{}"\n'.format(targetPackage, sha1OfLongTargetPackageName ))

                outputInfoFilepath = os.path.join(targetOutputDir, '{}.json'.format(sha1OfLongTargetPackageName))
                with open(outputInfoFilepath, 'w') as outputInfoFileHandle:
                    outputInfoFileHandle.write(json.dumps(jsonObject, indent=4))

    # Explore to projectUtilsSavedDir for convenience.
    os.system("explorer.exe %s" % projectUtilsSavedDir)
                    