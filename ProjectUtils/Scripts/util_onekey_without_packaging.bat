@echo off
call .\a_cook_content_provider_project.bat

call .\b_handle_content_provider_project_cooked_content.bat

call .\d_copy_paks_to_packaged_project.bat

echo.
call .\extra_check_splited_paks.bat