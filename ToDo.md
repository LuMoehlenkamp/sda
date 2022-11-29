# SDA (SENEC DATA ACQUISITION)

## include logging

## include observer to make solar forecast data available

## Dockerfile erstellen
  - apt search libboost-all-dev
  - apt install -y libboost-all-dev
  - apt install -y wiringpi
  - hypriot/rpi-gpio
  - /usr/bin/cmake --build /home/ludger/source/sda/build --config Release --target sda -j 10
  - copy params.json

## einarbeitung gpiod
  - segmentation fault when starting

## Container starten
  - docker run --privileged -d whatever
  - docker run --device /dev/gpiomem -d whatever
  - docker run -v /sys:/sys -d whatever

  - docker images
  - docker rm
  - docker run -it --name SDA 9d36 /bin/bash

## include boost::serialisation for persistence

## tests anpassen

## add doxygen for documentation purposes