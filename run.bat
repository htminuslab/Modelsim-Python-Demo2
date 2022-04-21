@echo off

set TCLLIBPATH=C:/utils/Python38/tcl/

vdel -all
vlib work

cl -c /Zi -I%MTI_HOME%\include -IC:\utils\Python38\include src\fli_python.c
link -dll /EXPORT:call_python_function /EXPORT:call_python_module /EXPORT:call_python_cleanup fli_python.obj %MTI_HOME%\win64pe\mtipli.lib C:\utils\Python38\libs\python38.lib  /out:fli_python.dll

vcom -quiet -2008 rtl\tb.vhd
vsim  -quiet -c work.tb -do "run 1 us; quit -f"