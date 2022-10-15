# SDA (SENEC DATA ACQUISITION)

curl http://192.168.178.40/lala.cgi -H 'Content-Type: application/json' -d '{"PV1":{"POWER_RATIO":""},"ENERGY":{"GUI_HOUSE_POW":"","GUI_GRID_POW":"","GUI_INVERTER_POWER":"","GUI_BAT_DATA_POWER":"","GUI_BAT_DATA_FUEL_CHARGE":"","STAT_STATE":"","GUI_BAT_DATA_OA_CHARGING":""},"PM1OBJ1":{"P_TOTAL":""}}'

  - include observer to make senec data available
  - include observer to make solar forecast data available
  - include logging
  - include boost::serialisation for persistence
  - tests anpassen
  - add doxygen for documentation purposes


