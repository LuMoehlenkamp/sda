# SDA (SENEC DATA ACQUISITION) - current ToDos

## control algorithm
- stage 1
  - proof of concept
    - simple but deterministic setup to check power consumption
    - start application using: ssh -X 192.168.178.38 -f 'cd ~/sda/sda/bin && sudo ./sda'

## make temperature sensor data available
  - get more info regarding 1-wire interface
  - pull up setting in /boot/config.txt
  - read existing sensor ids
  - check their type (group) -> look for ids starting with 28-
  - read data from existing sensors
  - __testing__
    - test using: cat /sys/devices/w1_bus_master1/28-072261372670/w1_slave
    - test using: cat /sys/devices/w1_bus_master1/28-072261372670/temperature
    - test using: cat /sys/bus/w1/devices/28-072261372670/temperature
  - __interface__
    - GetAllTemperatures
    - GetReservoirTemperature
    - GetControlsTemperature
## logging
- multiple log levels are needed
## include observer to make solar forecast data available

## include boost::serialisation for persistence

## tests anpassen

## add doxygen for documentation purposes

## Dockerfile erstellen
  - apt search libboost-all-dev
  - apt install -y libboost-all-dev
  - apt install -y wiringpi
  - hypriot/rpi-gpio
  - /usr/bin/cmake --build /home/ludger/source/sda/build --config Release --target sda -j 10
  - copy params.json

## Container starten
  - docker run --privileged -d whatever
  - docker run --device /dev/gpiomem -d whatever
  - docker run -v /sys:/sys -d whatever

  - docker images
  - docker rm
  - docker run -it --name SDA 9d36 /bin/bash
