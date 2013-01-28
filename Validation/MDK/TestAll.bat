@ECHO off

REM Usage: TestAll.bat [PATH TO UV4.exe] 

IF "%1"=="" (
SET UV4_EXE=C:\Keil\UV4\UV4.exe
) ELSE (
SET UV4_EXE=%1
)
ECHO Using %UV4_EXE%

ECHO. 
ECHO Building application...
if exist .\Flash\Blinky.axf del .\Flash\Blinky.axf

%UV4_EXE% -b Blinky.uvproj

if exist .\Flash\Blinky.axf (
echo Build succeded) else (
echo Build failed
goto :done
)



ECHO. 
ECHO Loading application to hardware target...
%UV4_EXE% -f Blinky.uvproj -t"DAP - Flash"

if ERRORLEVEL 1 (
echo Flash download failed
goto :done
) 


ECHO. 
ECHO Debugging hardware target...
if exist .\Flash.test_results del .\Flash.test_results

%UV4_EXE% -d Blinky.uvproj -t"DAP - Flash"

if exist .\Flash.test_results (
type .\Flash.test_results ) else (
echo. Test ended abnormally - file Flash.test_results is not produced
goto :done
)
)

ECHO. 
ECHO All tests completed

:done


