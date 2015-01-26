@echo off
set ANDROID_TARGET=android-20

echo "Cleaning up / removing build folders..."
rd /s /q .\_gen
mkdir _gen

echo "Build axvoice java code..."
javac -d .\_gen -bootclasspath %ANDROID_SDK_ROOT%\platforms\%ANDROID_TARGET%\android.jar;.\libs\unity-classes.jar;..\libifly\android\Msc.jar -encoding gbk .\src\com\axia\*.java

echo "Creating axvoice.jar..."
pause
cd _gen
jar cvfM ..\axvoice.jar .
cd ..

echo "Cleaning up / removing build folders..."
pause
rd /s /q .\_gen


echo "Copy to test project"
mkdir ..\test.android\libs
mkdir ..\test.android\libs\armeabi
mkdir ..\test.android\libs\armeabi-v7a

copy axvoice.jar ..\test.android\libs
copy .\libs\armeabi\libamr-codec.so ..\test.android\libs\armeabi\
copy .\libs\armeabi-v7a\libamr-codec.so ..\test.android\libs\armeabi-v7a\
copy ..\libifly\android\Msc.jar ..\test.android\libs\
copy ..\libifly\android\armeabi\libmsc.so ..\test.android\libs\armeabi\
copy ..\libifly\android\armeabi-v7a\libmsc.so ..\test.android\libs\armeabi-v7a\


copy axvoice.jar ..\test.unity\Assets\Plugins\Android
