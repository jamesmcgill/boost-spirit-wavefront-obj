#pragma once

#include <iosfwd>
#include <boost/variant/variant_fwd.hpp>
#include <vector>
#include <string>
#include <optional>

namespace ObjParser
{
//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
struct VertexPosition
{
  float x;
  float y;
  float z;
  float w = 1.0f;

  friend std::ostream& operator<<(std::ostream& os, const VertexPosition& v);
};

//------------------------------------------------------------------------------
struct VertexNormal
{
  float i;
  float j;
  float k;

  friend std::ostream& operator<<(std::ostream& os, const VertexNormal& v);
};

//------------------------------------------------------------------------------
struct VertexTextureCoordinate
{
  float u;
  float v = 0.0f;
  float w = 0.0f;

  friend std::ostream&
  operator<<(std::ostream& os, const VertexTextureCoordinate& v);
};

//------------------------------------------------------------------------------
struct FaceTriplet
{
  int vertexIndex = 0;    // zero means undefined
  int uvIndex     = 0;    // zero means undefined
  int normalIndex = 0;    // zero means undefined

  friend std::ostream& operator<<(std::ostream& os, const FaceTriplet& f);
};

//------------------------------------------------------------------------------
using Face = std::vector<FaceTriplet>;
std::ostream& operator<<(std::ostream& os, const Face& face);

//------------------------------------------------------------------------------
struct ObjAggregate
{
  std::vector<VertexPosition> positions;
  std::vector<VertexNormal> normals;
  std::vector<VertexTextureCoordinate> texCoords;
  std::vector<Face> faces;
};

//------------------------------------------------------------------------------
using ObjVariant
  = boost::variant<VertexPosition, VertexNormal, VertexTextureCoordinate, Face>;

using ObjVariantVec = std::vector<ObjVariant>;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Library
//------------------------------------------------------------------------------
std::optional<ObjParser::ObjVariantVec>
loadAsVariant(const std::string& filename);

std::optional<ObjParser::ObjAggregate>
loadAsAggregate(const std::string& filename);

//------------------------------------------------------------------------------
}    // namespace ObjParser
