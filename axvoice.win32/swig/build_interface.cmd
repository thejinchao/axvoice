@echo off

del *.h
del *.cxx
del *.cs

swig -csharp -outcurrentdir -namespace com.axia -c++ ..\axvoice.i

copy *.h ..
copy *.cxx ..
