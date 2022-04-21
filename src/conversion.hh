#pragma once

#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>


namespace SDA
{
  class Conversion
  {
    public:
      static float ConvertToFloat(const std::string& inString)
      {
        split_vector_type SplitVec;
        uint x; 
        std::stringstream ss;

        boost::split(SplitVec, inString, boost::is_any_of("_"), boost::token_compress_on);
        ss << std::hex << SplitVec[SplitVec.size() - 1];
        ss >> x;

        return reinterpret_cast<float&>(x);
      }

      static uint32_t ConvertToUint(const std::string& inString);

      static ConversionResult Convert(const std::string& inString)
      {
        split_vector_type SplitVec;
        boost::split(SplitVec, inString, boost::is_any_of("_"), boost::token_compress_on);
        std::stringstream ss;
        ss << std::hex << SplitVec[SplitVec.size() - 1];
        uint x; 
        ss >> x;

        if (SplitVec[0] == "fl")
        {
          return reinterpret_cast<float&>(x);
        }
        if (SplitVec[0] == "u8")
        {
          return x;
        }
        // ToDo: return default
      }
  };


}