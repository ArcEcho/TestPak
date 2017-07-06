import subprocess
import os.path
import shutil

import test_lib

if __name__ == "__main__":
    UATToolPath = test_lib.GetUATToolPath()
    currentProjectFileFilepath = os.path.join(test_lib.GetCurrentProjectRootDir(), test_lib.GetCurrentProjectName() + ".uproject")
    packagedProjectOutputDir = test_lib.GetCurrentProjectPackagedProjectOutputDir()
    packagingCmdTemplate = '"{}" BuildCookRun -nocompile -nocompileeditor -installed -nop4 -project="{}" -cook -stage -archive -archivedirectory="{}" -package -clientconfig=Development -ue4exe=UE4Editor-Cmd.exe -pak -prereqs -nodebuginfo -targetplatform=Win64 -build -utf8output'
    
    packagingCmd = packagingCmdTemplate.format(UATToolPath, currentProjectFileFilepath, packagedProjectOutputDir)
    subprocess.call(packagingCmd, shell=True)
    


