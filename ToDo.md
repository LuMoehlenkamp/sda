# SDA (SENEC DATA ACQUISITION)

curl http://192.168.178.40/lala.cgi -H 'Content-Type: application/json' -d '{"PV1":{"POWER_RATIO":""},"ENERGY":{"GUI_HOUSE_POW":"","GUI_GRID_POW":"","GUI_INVERTER_POWER":"","GUI_BAT_DATA_POWER":"","GUI_BAT_DATA_FUEL_CHARGE":"","STAT_STATE":"","GUI_BAT_DATA_OA_CHARGING":""},"PM1OBJ1":{"P_TOTAL":""}}'

## ToDo
  - cmake project structure:
    - exe 1
      - src
      - include
      - linux64 (?)
      - test
        - src
        - include
    - exe 2
      - ...

  - include boost::asio for network communication
    - resolve host adress
    - open tcp socket
    - establish connection
    - generate http request
    - evaluate http response

    - types: 
      u8
      fl 

  - include boost::serialisation for persistence

  - add doxygen for documentation purposes

  - USE GIT

