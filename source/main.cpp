#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <chrono>

#include "Log.h"
#include "fmt/format.h"

namespace client
{
namespace qi      = boost::spirit::qi;
namespace phoenix = boost::phoenix;

//------------------------------------------------------------------------------
// v
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
};

//------------------------------------------------------------------------------
// vn
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
};

//------------------------------------------------------------------------------
// vt
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
};

//------------------------------------------------------------------------------
// f
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
struct ObjData
{
  std::vector<VertexPosition> positions;
  std::vector<VertexNormal> normals;
  std::vector<VertexTextureCoordinate> texCoords;
  std::vector<Face> faces;
};

using ObjLine
  = boost::variant<VertexPosition, VertexNormal, VertexTextureCoordinate, Face>;

}    // namespace client
//------------------------------------------------------------------------------

// clang-format off
BOOST_FUSION_ADAPT_STRUCT(
  client::VertexPosition,
  (float, x)
  (float, y)
  (float, z)
  (float, w)
)

BOOST_FUSION_ADAPT_STRUCT(
  client::VertexNormal,
  (float, i)
  (float, j)
  (float, k)
)

BOOST_FUSION_ADAPT_STRUCT(
  client::VertexTextureCoordinate,
  (float, u)
  (float, v)
  (float, w)
)

BOOST_FUSION_ADAPT_STRUCT(
  client::FaceTriplet,
  (int, vertexIndex)
  (int, uvIndex)
  (int, normalIndex)
)
// clang-format on

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
namespace client
{
//------------------------------------------------------------------------------
void
printVariant(const std::vector<ObjLine>& lines)
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
printAggregate(const ObjData& data)
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
template <typename Iterator, typename Skipper = SkipParser<Iterator>>
struct obj_parser_variant
    : qi::grammar<Iterator, std::vector<ObjLine>(), Skipper>
{
  obj_parser_variant()
      : obj_parser_variant::base_type(start)
  {
    using qi::double_;
    using qi::eol;
    using qi::int_;

    position
      = 'v' >> qi::double_ >> qi::double_ >> qi::double_ >> -(qi::double_);
    normal   = "vn" >> qi::double_ >> qi::double_ >> qi::double_;
    texCoord = "vt" >> qi::double_ >> -(qi::double_) >> -(qi::double_);

    faceDuo     = qi::int_ >> -('/' >> qi::int_);
    faceTriplet = qi::int_ >> '/' >> -(qi::int_) >> '/' >> qi::int_;
    face        = 'f' >> +(faceTriplet | faceDuo);

    start = *(position | normal | texCoord | face);
  }

  qi::rule<Iterator, client::VertexPosition(), Skipper> position;
  qi::rule<Iterator, client::VertexNormal(), Skipper> normal;
  qi::rule<Iterator, client::VertexTextureCoordinate(), Skipper> texCoord;

  qi::rule<Iterator, client::FaceTriplet(), Skipper> faceDuo;
  qi::rule<Iterator, client::FaceTriplet(), Skipper> faceTriplet;
  qi::rule<Iterator, client::Face(), Skipper> face;

  qi::rule<Iterator, std::vector<ObjLine>(), Skipper> start;
};

//------------------------------------------------------------------------------
template <typename Iterator, typename Skipper = SkipParser<Iterator>>
struct obj_parser : qi::grammar<Iterator, ObjData(), Skipper>
{
  obj_parser()
      : obj_parser::base_type(start)
  {
    using phoenix::push_back;
    using qi::_1;
    using qi::_val;
    using qi::double_;
    using qi::int_;

    auto _positions = phoenix::bind(&ObjData::positions, qi::_val);
    auto _normals   = phoenix::bind(&ObjData::normals, qi::_val);
    auto _texCoords = phoenix::bind(&ObjData::texCoords, qi::_val);
    auto _faces     = phoenix::bind(&ObjData::faces, qi::_val);

    position
      = 'v' >> qi::double_ >> qi::double_ >> qi::double_ >> -(qi::double_);
    normal   = "vn" >> qi::double_ >> qi::double_ >> qi::double_;
    texCoord = "vt" >> qi::double_ >> -(qi::double_) >> -(qi::double_);

    faceDuo     = qi::int_ >> -('/' >> qi::int_);
    faceTriplet = qi::int_ >> '/' >> -(qi::int_) >> '/' >> qi::int_;
    face        = 'f' >> +(faceTriplet | faceDuo);

    // clang-format off
    start = *(
      position[push_back(_positions, _1)]
      | normal[push_back(_normals, _1)]
      | texCoord[push_back(_texCoords, _1)]
      | face[push_back(_faces, _1)]
    );
    // clang-format on

    BOOST_SPIRIT_DEBUG_NODE(start);
  }

  qi::rule<Iterator, client::VertexPosition(), Skipper> position;
  qi::rule<Iterator, client::VertexNormal(), Skipper> normal;
  qi::rule<Iterator, client::VertexTextureCoordinate(), Skipper> texCoord;

  qi::rule<Iterator, client::FaceTriplet(), Skipper> faceDuo;
  qi::rule<Iterator, client::FaceTriplet(), Skipper> faceTriplet;
  qi::rule<Iterator, client::Face(), Skipper> face;

  qi::rule<Iterator, ObjData(), Skipper> start;
};

//------------------------------------------------------------------------------
template <typename P, typename Data, typename Iterator>
bool
objParse(Iterator first, Iterator last, Data& data)
{
  P parser;
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
}

}    // namespace client

//------------------------------------------------------------------------------
int
main()
{
  namespace qi = boost::spirit::qi;

  static const char* TEST_FILE   = "assets/test.obj";
  static const char* DRAGON_FILE = "assets/dragon_vrip_res3.obj";

  std::ifstream in(TEST_FILE, std::ios::binary);
  if (!in.is_open())
  {
    LOG_ERROR("File open failed");
    return 1;
  }
  in.unsetf(std::ios::skipws);

  using It = boost::spirit::istream_iterator;
  It begin(in);
  It end;

  //client::ObjData data;
  std::vector<client::ObjLine> data;		// variant

  using P        = client::obj_parser<It>;
  using VariantP = client::obj_parser_variant<It>;

  auto start = std::chrono::high_resolution_clock::now();
  if (client::objParse<VariantP>(begin, end, data))
  {
    auto elapsed = std::chrono::high_resolution_clock::now() - start;

    printVariant(data);
    //printAggregate(data);

    LOG_INFO("-------------------------");
    LOG_INFO("Parsing succeeded in %fms", elapsed.count() / 1000000.0f);
    LOG_INFO("-------------------------");
  }
  else
  {
    LOG_INFO("-------------------------");
    LOG_INFO("Parsing failed");
    LOG_INFO("-------------------------");
  }

  return 0;
}

//------------------------------------------------------------------------------
