# WIKI ARDUINO

Semplice progetto Arduino che mostra come prendere da un database Redis una stringa e accendere o spegnere i relativi led.
Questo progetto è collegato con therickys93/wikicontroller per la creazione di una casa domotica.

## Upload

Per uplodare il file .hex in arduino usare il seguente comando: ```avrdude -Cavrdude.conf -v -patmega328p -carduino -P<device> -b115200 -D -Uflash:w:src.ino.hex:i```.

## Interfaccia Seriale

Per collegarsi all'interfaccia seriale usare il seguente comando: ```screen <device> 57600```