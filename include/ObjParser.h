#pragma once

#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/config/warning_disable.hpp>
#include <boost/variant.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>

#include "log.h"

//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------
namespace ObjParser
{
//------------------------------------------------------------------------------
struct VertexPosition
{
  float x;
  float y;
  float z;
  float w = 1.0f;

  friend std::ostream& operator<<(std::ostream& os, const VertexPosition& v)
  {
    return os << "v(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w
              << ")";
  }

  friend bool operator==(const VertexPosition& lhs, const VertexPosition& rhs)
  {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
  }
};

//------------------------------------------------------------------------------
struct VertexNormal
{
  float i;
  float j;
  float k;

  friend std::ostream& operator<<(std::ostream& os, const VertexNormal& v)
  {
    return os << "n(" << v.i << ", " << v.j << ", " << v.k << ")";
  }

  friend bool operator==(const VertexNormal& lhs, const VertexNormal& rhs)
  {
    return lhs.i == rhs.i && lhs.j == rhs.j && lhs.k == rhs.k;
  }
};

//------------------------------------------------------------------------------
struct VertexTextureCoordinate
{
  float u;
  float v = 0.0f;
  float w = 0.0f;

  friend std::ostream&
  operator<<(std::ostream& os, const VertexTextureCoordinate& v)
  {
    return os << "uv(" << v.u << ", " << v.v << ", " << v.w << ")";
  }

  friend bool operator==(
    const VertexTextureCoordinate& lhs, const VertexTextureCoordinate& rhs)
  {
    return lhs.u == rhs.u && lhs.v == rhs.v && lhs.w == rhs.w;
  }
};

//------------------------------------------------------------------------------
struct FaceTriplet
{
  int vertexIndex = 0;    // zero means undefined
  int uvIndex     = 0;    // zero means undefined
  int normalIndex = 0;    // zero means undefined

  friend std::ostream& operator<<(std::ostream& os, const FaceTriplet& f)
  {
    return os << "(" << f.vertexIndex << ", " << f.uvIndex << ", "
              << f.normalIndex << ") ";
  }

  friend bool operator==(const FaceTriplet& lhs, const FaceTriplet& rhs)
  {
    return lhs.vertexIndex == rhs.vertexIndex && lhs.uvIndex == rhs.uvIndex
           && lhs.normalIndex == rhs.normalIndex;
  }
};

//------------------------------------------------------------------------------
using Face = std::vector<FaceTriplet>;
std::ostream&
operator<<(std::ostream& os, const Face& face)
{
  os << "f: ";
  for (auto& elem : face)
  {
    os << elem;
  }
  return os;
}

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
}    // namespace ObjParser

//------------------------------------------------------------------------------
// clang-format off
BOOST_FUSION_ADAPT_STRUCT(
  ObjParser::VertexPosition,
  (float, x)
  (float, y)
  (float, z)
  (float, w)
)

BOOST_FUSION_ADAPT_STRUCT(
  ObjParser::VertexNormal,
  (float, i)
  (float, j)
  (float, k)
)

BOOST_FUSION_ADAPT_STRUCT(
  ObjParser::VertexTextureCoordinate,
  (float, u)
  (float, v)
  (float, w)
)

BOOST_FUSION_ADAPT_STRUCT(
  ObjParser::FaceTriplet,
  (int, vertexIndex)
  (int, uvIndex)
  (int, normalIndex)
)
// clang-format on
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------
namespace ObjParser
{
namespace qi      = boost::spirit::qi;
namespace phoenix = boost::phoenix;

//------------------------------------------------------------------------------
template <typename Iterator>
struct SkipParser : public qi::grammar<Iterator>
{
  SkipParser()
      : SkipParser::base_type(skipper)
  {
    using qi::char_;
    using qi::eol;

    comment = "#" >> *(char_ - eol);

    // TODO(James): handle materials
    materialLib = "mtllib" >> *(char_ - eol);
    useMaterial = "usemtl" >> *(char_ - eol);

    skipper = qi::space | comment | materialLib | useMaterial;
  }
  qi::rule<Iterator> skipper;
  qi::rule<Iterator> comment;
  qi::rule<Iterator> materialLib;
  qi::rule<Iterator> useMaterial;
};

//------------------------------------------------------------------------------
template <
  typename Data,
  typename Iterator,
  typename Skipper = SkipParser<Iterator>>
struct ObjParserCommon : public qi::grammar<Iterator, Data(), Skipper>
{
  ObjParserCommon()
      : ObjParserCommon::base_type(start)
  {
    using qi::double_;
    using qi::int_;

    position = 'v' >> double_ >> double_ >> double_ >> -(double_);
    normal   = "vn" >> double_ >> double_ >> double_;
    texCoord = "vt" >> double_ >> -(double_) >> -(double_);

    faceDuo     = int_ >> -('/' >> int_);
    faceTriplet = int_ >> '/' >> -(int_) >> '/' >> int_;
    face        = 'f' >> +(faceTriplet | faceDuo);
  }

  qi::rule<Iterator, VertexPosition(), Skipper> position;
  qi::rule<Iterator, VertexNormal(), Skipper> normal;
  qi::rule<Iterator, VertexTextureCoordinate(), Skipper> texCoord;

  qi::rule<Iterator, FaceTriplet(), Skipper> faceDuo;
  qi::rule<Iterator, FaceTriplet(), Skipper> faceTriplet;
  qi::rule<Iterator, Face(), Skipper> face;

  qi::rule<Iterator, Data(), Skipper> start;
};

//------------------------------------------------------------------------------
template <
  typename Data,
  typename Iterator,
  typename Skipper = SkipParser<Iterator>,
  typename Common  = ObjParserCommon<Data, Iterator, Skipper>>
struct ObjParserVariant : public Common
{
  ObjParserVariant()
  {
    Common::start
      = *(Common::position | Common::normal | Common::texCoord | Common::face);
  }
};

//------------------------------------------------------------------------------
template <
  typename Data,
  typename Iterator,
  typename Skipper = SkipParser<Iterator>,
  typename Common  = ObjParserCommon<Data, Iterator, Skipper>>
struct ObjParserSemanticActions : public Common
{
  ObjParserSemanticActions()
  {
    using phoenix::bind;
    using phoenix::push_back;
    using qi::_1;
    using qi::_val;

    auto _positions = bind(&Data::positions, _val);
    auto _normals   = bind(&Data::normals, _val);
    auto _texCoords = bind(&Data::texCoords, _val);
    auto _faces     = bind(&Data::faces, _val);

    // clang-format off
    Common::start = *(
      Common::position      [push_back(_positions, _1)]
      | Common::normal      [push_back(_normals, _1)]
      | Common::texCoord    [push_back(_texCoords, _1)]
      | Common::face        [push_back(_faces, _1)]
    );
    // clang-format on
  }
};

//------------------------------------------------------------------------------
template <typename Parser, typename Data, typename Iterator>
bool
parseObj(Iterator first, Iterator last, Data& data)
{
  Parser parser;
  SkipParser<Iterator> skipper;

  try
  {
    using boost::spirit::qi::space;
    if (!qi::phrase_parse(first, last, parser, skipper, data))
    {
      LOG_ERROR("parse failed: %s\n", std::string(first, last).c_str());
      return false;
    }
    if (first != last)
    {
      LOG_ERROR("unparsed: %s\n", std::string(first, last).c_str());
      return false;
    }
    return true;
  }
  catch (const qi::expectation_failure<Iterator>& e)
  {
    LOG_ERROR(
      "parse exception: %s %s\n", e.what(), std::string(first, last).c_str());
  }
  catch (...)
  {
    LOG_ERROR("misc exception\n");
  }

  return false;
}

//------------------------------------------------------------------------------
template <typename Parser, typename Data, typename Iterator>
std::optional<Data>
parseImpl(Iterator& begin, Iterator& end)
{
  Data data;
  if (parseObj<Parser>(begin, end, data))
  {
    return data;
  }
  return std::nullopt;
}

//------------------------------------------------------------------------------
template <typename Parser, typename Data, typename Iterator>
std::optional<Data>
loadAndParseImpl(const std::string& filename)
{
  std::ifstream in(filename, std::ios::binary);
  if (!in.is_open())
  {
    LOG_ERROR("File open failed");
    return std::nullopt;
  }
  in.unsetf(std::ios::skipws);

  Iterator begin(in);
  Iterator end;

  return parseImpl<Parser, Data, Iterator>(begin, end);
}

//------------------------------------------------------------------------------
}    // namespace ObjParser

//------------------------------------------------------------------------------
// Library Interface
//------------------------------------------------------------------------------
namespace ObjParser
{
//------------------------------------------------------------------------------
template <typename Iterator>
std::optional<ObjParser::ObjVariantVec>
parseAsVariant(Iterator begin, Iterator end)
{
  using Data   = ObjVariantVec;
  using Parser = ObjParser::ObjParserVariant<Data, Iterator>;

  return parseImpl<Parser, Data, Iterator>(begin, end);
}

//------------------------------------------------------------------------------
template <typename Iterator>
std::optional<ObjParser::ObjAggregate>
parseAsAggregate(Iterator begin, Iterator end)
{
  using Data   = ObjAggregate;
  using Parser = ObjParser::ObjParserSemanticActions<Data, Iterator>;

  return parseImpl<Parser, Data, Iterator>(begin, end);
}

//------------------------------------------------------------------------------
std::optional<ObjParser::ObjVariantVec>
loadAsVariant(const std::string& filename)
{
  using It     = boost::spirit::istream_iterator;
  using Data   = ObjVariantVec;
  using Parser = ObjParser::ObjParserVariant<Data, It>;

  return loadAndParseImpl<Parser, Data, It>(filename);
}

//------------------------------------------------------------------------------
std::optional<ObjParser::ObjAggregate>
loadAsAggregate(const std::string& filename)
{
  using It     = boost::spirit::istream_iterator;
  using Data   = ObjAggregate;
  using Parser = ObjParser::ObjParserSemanticActions<Data, It>;

  return loadAndParseImpl<Parser, Data, It>(filename);
}

//------------------------------------------------------------------------------
}    // namespace ObjParser
