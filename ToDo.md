# SDA (SENEC DATA ACQUISITION) - current ToDos

## control algorithm
- stage 1
  - proof of concept
    - simple but deterministic setup to check power consumption
    - start application using: ssh -X 192.168.178.38 -f 'cd ~/sda/sda/bin && sudo ./sda'
    - power grid's sign has to be inverted
    - build binary from dir ../build using: 'cmake --build . --config Release --target sda -- -j 1'
    - calc clock setting: 19.200.000MHz / pwmClock / pwmRange =>  19.200.000MHz / 1920 / 1000
    - autostart using sudo update-rc.d NameDesSkripts defaults
    - remove autostart using sudo update-rc.d -f NameDesSkripts remove
    - show cron logs using sudo nano /var/log/syslog
    - local starten (test): sudo /usr/sbin/mosquitto -c /etc/mosquitto/mosquitto.conf -v
    - mosquitto_sub -t 'openWB/#' -v -p 1883
    - curl -k https://192.168.178.40/lala.cgi -H 'Content-Type: application/json' -d '{"ENERGY":{"GUI_HOUSE_POW":"","GUI_GRID_POW":"","GUI_INVERTER_POWER":"","GUI_BAT_DATA_POWER":"","GUI_BAT_DATA_FUEL_CHARGE":"","STAT_STATE":""},"STATISTIC":{"LIVE_GRID_IMPORT":""},"PM1OBJ1":{"P_AC":""}}'

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
