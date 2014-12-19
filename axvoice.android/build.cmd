@echo off
set ANDROID_TARGET=android-20

echo "Cleaning up / removing build folders..."
rd /s /q .\_gen
mkdir _gen

echo "Build axvoice java code..."
javac -d .\_gen -bootclasspath %ANDROID_SDK_ROOT%\platforms\%ANDROID_TARGET%\android.jar;.\libs\unity-classes.jar -classpath .\libs\axtrace.jar .\src\com\axia\*.java

echo "Creating axvoice.jar..."
pause
cd _gen
jar cvfM ..\axvoice.jar .
cd ..

echo "Cleaning up / removing build folders..."
pause
rd /s /q .\_gen


echo "Copy to test project"
copy axvoice.jar ..\test.unity\Assets\Plugins\Android
