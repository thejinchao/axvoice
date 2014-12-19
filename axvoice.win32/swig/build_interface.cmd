@echo off

del *.h
del *.cxx
del *.cs

swig -csharp -outcurrentdir -namespace com.axia -c++ ..\axvoice.i

copy *.h ..
copy *.cxx ..

copy axvoicePINVOKE.cs D:\app_unity\voice_test\Assets\Plugins
copy axvoice*Callback.cs D:\app_unity\voice_test\Assets\Plugins
