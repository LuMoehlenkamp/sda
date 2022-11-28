# SDA (SENEC DATA ACQUISITION)

## include logging

## include observer to make solar forecast data available

## Dockerfile erstellen
  - apt search libboost-all-dev
  - apt install -y libboost-all-dev
  - apt install -y wiringpi
  - hypriot/rpi-gpio

## Container starten
  - docker run --privileged -d whatever
  - docker run --device /dev/gpiomem -d whatever
  - docker run -v /sys:/sys -d whatever

## include boost::serialisation for persistence

## tests anpassen

## add doxygen for documentation purposes


