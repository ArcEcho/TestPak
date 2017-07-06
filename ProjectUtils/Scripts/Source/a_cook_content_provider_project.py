import test_lib
import subprocess
import os.path

if __name__ == "__main__":
    cookCmdTemplate =  '"{}" "{}" -run=Cook  -NoLogTimes -TargetPlatform=WindowsNoEditor -fileopenlog -unversioned -abslog="{}" -stdout -CrashForUAT -unattended'  
    
    editorCmdExePath = test_lib.GetEditorCmdExePath()
    contentProviderProjectFileFilepath = os.path.abspath( os.path.join(test_lib.GetContentProviderProjectRootDir(), test_lib.GetContentProviderProjectName() + ".uproject"))
    cookLogOutputPath = os.path.abspath(os.path.join(test_lib.GetContentProviderProjectRootDir(), "Saved\\Logs\\Cook.log"))

    cookCmd = cookCmdTemplate.format(editorCmdExePath, contentProviderProjectFileFilepath, cookLogOutputPath)
    subprocess.call(cookCmd, shell=True)
