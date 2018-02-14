#include "lib.h"

#include <boost/variant.hpp>
#include <sstream>
#include <chrono>
#include <iostream>

#include "Log.h"
#include "fmt/format.h"

//------------------------------------------------------------------------------
void
printVariant(const ObjParser::ObjVariantVec& lines)
{
  LOG_INFO("-------------------------");
  LOG_INFO("Parsed %u entries", lines.size());
  LOG_INFO("-------------------------");

  for (auto& line : lines)
  {
    std::stringstream ss;
    ss << line;
    LOG_INFO("%s", ss.str().c_str());
  }
}

//------------------------------------------------------------------------------
void
printAggregate(const ObjParser::ObjAggregate& data)
{
  LOG_INFO("-------------------------");
  LOG_INFO(
    "Parsed %u vertices, %u normals, %u uvs, %u faces",
    data.positions.size(),
    data.normals.size(),
    data.texCoords.size(),
    data.faces.size());
  LOG_INFO("-------------------------");

  for (auto& vertex : data.positions)
  {
    std::stringstream ss;
    ss << vertex;
    LOG_INFO("%s", ss.str().c_str());
  }

  for (auto& normal : data.normals)
  {
    std::stringstream ss;
    ss << normal;
    LOG_INFO("%s", ss.str().c_str());
  }

  for (auto& texCoord : data.texCoords)
  {
    std::stringstream ss;
    ss << texCoord;
    LOG_INFO("%s", ss.str().c_str());
  }

  for (auto& face : data.faces)
  {
    std::stringstream ss;
    ss << face;
    LOG_INFO("%s", ss.str().c_str());
  }
}

//------------------------------------------------------------------------------
void
printSuccess(std::chrono::nanoseconds::rep elapsed)
{
  LOG_INFO("-------------------------");
  LOG_INFO("Parsing succeeded in %fms", elapsed / 1000000.0f);
  LOG_INFO("-------------------------");
};

//------------------------------------------------------------------------------
void
printFailure()
{
  LOG_INFO("-------------------------");
  LOG_INFO("Parsing failed");
  LOG_INFO("-------------------------");
};

//------------------------------------------------------------------------------
int
main()
{
  static const char* TEST_FILE   = "assets/test.obj";
  static const char* DRAGON_FILE = "assets/dragon_vrip_res3.obj";

  {
    auto start = std::chrono::high_resolution_clock::now();
    if (auto data = ObjParser::loadAsVariant(std::string(TEST_FILE)))
    {
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      printVariant(*data);
      printSuccess(elapsed.count());
    }
    else
    {
      printFailure();
    }
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    if (auto data = ObjParser::loadAsAggregate(std::string(TEST_FILE)))
    {
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      printAggregate(*data);
      printSuccess(elapsed.count());
    }
    else
    {
      printFailure();
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
