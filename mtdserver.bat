@ECHO OFF
set BIN_PATH=bin32
IF /I "%PROCESSOR_ARCHITECTURE%" == "amd64" (
    set BIN_PATH=bin64
)
IF /I "%PROCESSOR_ARCHITEW6432%" == "amd64" (
    set BIN_PATH=bin64
)
start %BIN_PATH%"\server.exe" data\server\server.conf %*