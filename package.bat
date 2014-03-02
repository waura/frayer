rmdir /s /q package

md package
md package\frayer
md package\frayer\log
md package\frayer\config
md package\frayer_x64
md package\frayer_x64\log
md package\frayer_x64\config

cd .\Win32\Release
.\Test_UtilOpenCV.exe
.\Test_libImgEdit.exe
.\Test_frayer.exe
cd ..\..
PAUSE

copy /y README.txt package\frayer
copy /y ChangeLog.txt package\frayer
copy /y license.txt package\frayer
copy /y clean_registry.bat package\frayer
xcopy /y /s /i brush_texture package\frayer\brush_texture
copy /y Win32\Release\frayer.exe package\frayer
copy /y Win32\Release\libImgEdit.dll package\frayer
copy /y Win32\Release\UtilOpenCV.dll package\frayer
xcopy /y /s /i Win32\Release\data package\frayer\data

cd .\x64\Release
.\Test_UtilOpenCV.exe
.\Test_libImgEdit.exe
.\Test_frayer.exe
cd ..\..
PAUSE

copy /y README.txt package\frayer_x64
copy /y ChangeLog.txt package\frayer_x64
copy /y license.txt package\frayer_x64
copy /y clean_registry.bat package\frayer_x64
xcopy /y /s /i brush_texture package\frayer_x64\brush_texture
copy /y x64\Release\frayer_x64.exe package\frayer_x64
copy /y x64\Release\libImgEdit.dll package\frayer_x64
copy /y x64\Release\UtilOpenCV.dll package\frayer_x64
xcopy /y /s /i x64\Release\data package\frayer_x64\data
