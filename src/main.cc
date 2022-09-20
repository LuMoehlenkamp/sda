#include <array>
#include <iostream>
#include <string>

#include "senecDataAcquisition.hh"
#include "solarDataAcquisition.hh"

int main(int argc, char *argv[])
{
  SDA::SenecDataAcquisition sda;
  sda();
  
  SDA::SolarDataAcquisition solar_da;
  solar_da();

  return 0;
}