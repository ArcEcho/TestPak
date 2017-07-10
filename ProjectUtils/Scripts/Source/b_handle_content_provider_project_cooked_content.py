# import configparser
# import winreg
import subprocess
import os.path
import os
import shutil

import test_lib

if __name__ == "__main__":
    
    contentProviderProjectCookedContentDir= os.path.join(
        test_lib.GetContentProviderProjectRootDir(), "Saved\\Cooked\\WindowsNoEditor\\" +
        test_lib.GetContentProviderProjectName() + "\\Content\\")

    splitedPaksTempDir =  os.path.abspath(os.path.join(test_lib.GetCurrentProjectRootDir(), "Saved\\ProjectUtils\\SplitedPaksTemp\\"))
    splitedPaksTempDirPendingDelete =  os.path.abspath(os.path.join(test_lib.GetCurrentProjectRootDir(), "Saved\\ProjectUtils\\SplitedPaksTemp_pending_delete\\"))

    #Because deleting files is an asynchoronus operation, here is a trick to 
    # avoid this side effect.
    if os.path.exists(splitedPaksTempDir):
        os.rename(splitedPaksTempDir, splitedPaksTempDirPendingDelete)
        shutil.rmtree(splitedPaksTempDirPendingDelete)
    
    assetPackages = test_lib.GetAllAssetPackagesInCookedContent(contentProviderProjectCookedContentDir)

    generatePakLogFilepath = os.path.abspath(os.path.join(test_lib.GetCurrentProjectRootDir(), "Saved\\ProjectUtils\\GeneratePaks.log"))
    logFileHandle = open(generatePakLogFilepath, 'w')
    for assetPackage in assetPackages:
        outputPakFilePath = test_lib.GenerateSplitedPaks(splitedPaksTempDir, contentProviderProjectCookedContentDir, assetPackage)
        
        # If the pak file size is less than 64KB, when pack and regenerate pak file to meet the PAK_PRECAHCE_GRUNULARITY. 
        if os.path.getsize(outputPakFilePath) < 64 * 1024:
            os.remove(outputPakFilePath)
            outputPakFilePath = test_lib.GenerateSplitedPaks(splitedPaksTempDir, contentProviderProjectCookedContentDir, assetPackage,  inShouldPackTo64KB = True)
            logFileHandle.write(assetPackage.longName + " --> " + outputPakFilePath + " packed \n" )        
        else:
            logFileHandle.write(assetPackage.longName + " --> " + outputPakFilePath + " unpacked \n" )
       
    logFileHandle.close()
    


