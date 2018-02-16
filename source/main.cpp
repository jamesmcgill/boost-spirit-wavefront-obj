#include "ObjParser.h"

#include <boost/variant.hpp>
#include <sstream>
#include <chrono>
#include <iostream>

#include "log.h"
#include "fmt/format.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

//------------------------------------------------------------------------------
#ifndef __WIN32
#define UNREFERENCED_PARAMETER(P) (P)
#endif

constexpr bool printFullData = false;

//------------------------------------------------------------------------------
void
printVariant(const ObjParser::ObjVariantVec& lines)
{
  LOG_INFO("-------------------------");
  LOG_INFO("VARIANT: Parsed %u entries", lines.size());
  LOG_INFO("-------------------------");

  if (!printFullData)
  {
    return;
  }

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
    "AGGREGATE: Parsed %u vertices, %u normals, %u uvs, %u faces",
    data.positions.size(),
    data.normals.size(),
    data.texCoords.size(),
    data.faces.size());
  LOG_INFO("-------------------------");

  if (!printFullData)
  {
    return;
  }

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
printSuccess(const char* type, std::chrono::nanoseconds::rep elapsed)
{
  LOG_INFO("-------------------------");
  LOG_INFO("%s Parsing succeeded in %fms", type, elapsed / 1000000.0f);
  LOG_INFO("-------------------------");
};

//------------------------------------------------------------------------------
void
printFailure(const char* type)
{
  LOG_INFO("-------------------------");
  LOG_INFO("%s Parsing failed", type);
  LOG_INFO("-------------------------");
};

//------------------------------------------------------------------------------
bool
loadWithTinyObj(
  const std::string& filename,
  tinyobj::attrib_t& attrib,
  std::vector<tinyobj::shape_t>& shapes,
  std::vector<tinyobj::material_t>& materials)
{
  std::string err;
  bool ret
    = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
  if (!ret)
  {
    LOG_ERROR("tinyobjloader failed to open file");
  }

  if (!err.empty())
  {
    LOG_ERROR(err.c_str());
  }

  return ret;
}

//------------------------------------------------------------------------------
void
printTinyObj(
  tinyobj::attrib_t& attrib,
  std::vector<tinyobj::shape_t>& shapes,
  std::vector<tinyobj::material_t>& materials)
{
  UNREFERENCED_PARAMETER(materials);

  LOG_INFO("-------------------------");
  LOG_INFO(
    "TINYOBJ Parsed %u shapes, %u vertices, %u normals, %u uvs,",
    shapes.size(),
    attrib.vertices.size(),
    attrib.normals.size(),
    attrib.texcoords.size());
  LOG_INFO("-------------------------");

  if (!printFullData)
  {
    return;
  }

  for (auto& vertex : attrib.vertices)
  {
    std::stringstream ss;
    ss << vertex;
    LOG_INFO("%s", ss.str().c_str());
  }

  for (auto& normal : attrib.normals)
  {
    std::stringstream ss;
    ss << normal;
    LOG_INFO("%s", ss.str().c_str());
  }

  for (auto& texCoord : attrib.texcoords)
  {
    std::stringstream ss;
    ss << texCoord;
    LOG_INFO("%s", ss.str().c_str());
  }
}

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
      printSuccess("VARIANT", elapsed.count());
    }
    else
    {
      printFailure("VARIANT");
    }
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    if (auto data = ObjParser::loadAsAggregate(std::string(TEST_FILE)))
    {
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      printAggregate(*data);
      printSuccess("AGGREGATE", elapsed.count());
    }
    else
    {
      printFailure("AGGREGATE");
    }
  }

  {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    auto start = std::chrono::high_resolution_clock::now();
    if (loadWithTinyObj(std::string(TEST_FILE), attrib, shapes, materials))
    {
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      printTinyObj(attrib, shapes, materials);
      printSuccess("TINYOBJ", elapsed.count());
    }
    else
    {
      printFailure("TINYOBJ");
    }
  }

  std::string TEST_STRING = R"obj(
v  0.03264300152659416 0.056147500872612 -0.04995829984545708
v  0.03080499917268753 0.0559782013297081 -0.04991229996085167 1.5
v  0.03251679986715317 0.05801349878311157 -0.050050999969244
vn -0.4538693428039551 0.3681831955909729 0.8114454746246338
vn -0.15167635679245 0.9458057284355164 -0.2871338427066803
v  0.03264300152659416 0.056147500872612 -0.04995829984545708
v  0.03080499917268753 0.0559782013297081 -0.04991229996085167
vt 0.089608 0.023837
vt -0.07313 0.023837

# Mesh '' with 47794 faces
usemtl $Material_0
f  1//1 2//2 3//3
f  4//4 3//3 2//2
f  11/11/11 12/12/12 2/2/2
f  5//5 1//1 6//6
f  5 1 6
f  1//1 3//3 6//6
f  8/8 6/6 9/9
f  10//10 8//8 9//9
)obj";

  {
    auto start = std::chrono::high_resolution_clock::now();
    if (
      auto data
      = ObjParser::parseAsVariant(TEST_STRING.begin(), TEST_STRING.end()))
    {
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      printVariant(*data);
      printSuccess("VARIANT", elapsed.count());
    }
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    if (
      auto data
      = ObjParser::parseAsAggregate(TEST_STRING.begin(), TEST_STRING.end()))
    {
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      printAggregate(*data);
      printSuccess("AGGREGATE", elapsed.count());
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
