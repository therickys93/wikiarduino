#!/bin/sh

arduino-builder -libraries ./lib -verbose -build-path $HOME -build-options-file $ARDUINO_PATH/build.options.json src/src.ino