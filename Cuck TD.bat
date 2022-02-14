@ECHO OFF
set BIN_PATH=Bin32
IF /I "%PROCESSOR_ARCHITECTURE%" == "amd64" (
    set BIN_PATH=Bin64
)
IF /I "%PROCESSOR_ARCHITEW6432%" == "amd64" (
    set BIN_PATH=Bin64
)
start %BIN_PATH%"\Cuck TD.exe" %*