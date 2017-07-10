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
        test_lib.GenerateSplitedPaks(splitedPaksTempDir, contentProviderProjectCookedContentDir, assetPackage, logFileHandle)
    logFileHandle.close()
    


