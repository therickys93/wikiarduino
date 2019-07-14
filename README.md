# WIKI ARDUINO

Semplice progetto Arduino che mostra come prendere da un database Redis una stringa e accendere o spegnere i relativi led.
Questo progetto è collegato con therickys93/wikicontroller per la creazione di una casa domotica.

## Upload

Per uplodare il file .hex in arduino usare il seguente comando: ```avrdude -Cavrdude.conf -v -patmega328p -carduino -P<device> -b115200 -D -Uflash:w:src.ino.hex:i```.

## Interfaccia Seriale

Per collegarsi all'interfaccia seriale usare il seguente comando: ```screen <device> 57600```

## Sensore Temperatura

Il sensore di temperatura va collegato al pin a0 e come input alla corrente a 5V.

## Compilazione

Per compilare il progetto usare si può usare Arduino IDE importando le librerie oppure si può usare docker.
Per docker seguire questa procedura:
```
docker run -it --rm -v $PWD:/arduino therickys93/ubuntu14arduino
```
```
cd /arduino
```
```
arduino-builder -libraries ./lib -verbose -build-path $HOME -build-options-file $ARDUINO_PATH/build.options.json src/src.ino
```
```
cd $HOME
```

i file hex saranno presenti in tale directory.