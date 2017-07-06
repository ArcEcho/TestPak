import configparser
import winreg
import subprocess
import os.path
import re

import test_lib



if __name__ == "__main__":

    packagedProjectOutputDir = test_lib.GetCurrentProjectPackagedProjectOutputDir()
    packagingCmdTemplate = '"{}" BuildCookRun -nocompile -nocompileeditor -installed -nop4 -project="{}" -cook -stage -archive -archivedirectory="{}" -package -clientconfig=Development -ue4exe=UE4Editor-Cmd.exe -pak -prereqs -nodebuginfo -targetplatform=Win64 -build -utf8output'
    
    srcDir =  os.path.abspath(os.path.join(test_lib.GetCurrentProjectRootDir(), "Saved\\ProjectUtils\\SplitedPaksTemp\\"))
    destDir =  os.path.normpath(os.path.join(packagedProjectOutputDir, 'WindowsNoEditor\\TestPak\\SplitedPaks\\'))
    print(srcDir, destDir)

    subprocess.call( 'md "{}"\n'.format(destDir), shell=True)
    subprocess.call( 'copy /y "{}" "{}"'.format(srcDir, destDir), shell=True)
