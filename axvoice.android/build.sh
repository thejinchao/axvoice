#!/bin/sh

ANDROID_TARGET=android-20

echo "Cleaning up / removing build folders..."
rm -rf ./_gen
mkdir _gen


echo "Build axvoice java code..."
javac -d ./_gen -bootclasspath $ANDROID_SDK_ROOT/platforms/$ANDROID_TARGET/android.jar:./libs/unity-classes.jar -encoding gbk ./src/com/axia/*.java

echo "Creating axvoice.jar..."
read -n 1 -p "Press any key to continue..."
cd _gen
jar cvfM ../axvoice.jar .
cd ..

echo "Cleaning up / removing build folders..."
read -n 1 -p "Press any key to continue..."
rm -rf _gen

echo "Copy to test project"
cp axvoice.jar ../test.unity/Assets/Plugins/Android
cp axvoice.jar ../test.android/libs

