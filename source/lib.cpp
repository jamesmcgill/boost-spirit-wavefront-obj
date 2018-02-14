#include "lib.h"

#include <boost/config/warning_disable.hpp>
#include <boost/variant.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <iostream>
#include <fstream>

#include "Log.h"

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

namespace ObjParser
{
namespace qi      = boost::spirit::qi;
namespace phoenix = boost::phoenix;

//------------------------------------------------------------------------------
// Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::ostream&
operator<<(std::ostream& os, const VertexPosition& v)
{
  return os << "v(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}

//------------------------------------------------------------------------------
std::ostream&
operator<<(std::ostream& os, const VertexNormal& v)
{
  return os << "n(" << v.i << ", " << v.j << ", " << v.k << ")";
}

//------------------------------------------------------------------------------
std::ostream&
operator<<(std::ostream& os, const VertexTextureCoordinate& v)
{
  return os << "uv(" << v.u << ", " << v.v << ", " << v.w << ")";
}

//------------------------------------------------------------------------------
std::ostream&
operator<<(std::ostream& os, const FaceTriplet& f)
{
  return os << "(" << f.vertexIndex << ", " << f.uvIndex << ", "
            << f.normalIndex << ") ";
}

//------------------------------------------------------------------------------
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
// Library
//------------------------------------------------------------------------------

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
struct obj_parser_variant : qi::grammar<Iterator, Data(), Skipper>
{
  obj_parser_variant()
      : obj_parser_variant::base_type(start)
  {
    using qi::double_;
    using qi::int_;

    position = 'v' >> double_ >> double_ >> double_ >> -(double_);
    normal   = "vn" >> double_ >> double_ >> double_;
    texCoord = "vt" >> double_ >> -(double_) >> -(double_);

    faceDuo     = int_ >> -('/' >> int_);
    faceTriplet = int_ >> '/' >> -(int_) >> '/' >> int_;
    face        = 'f' >> +(faceTriplet | faceDuo);

    start = *(position | normal | texCoord | face);
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
  typename Skipper = SkipParser<Iterator>>
struct obj_parser : qi::grammar<Iterator, Data(), Skipper>
{
  obj_parser()
      : obj_parser::base_type(start)
  {
    using phoenix::bind;
    using phoenix::push_back;
    using qi::_1;
    using qi::_val;
    using qi::double_;
    using qi::int_;

    auto _positions = bind(&Data::positions, _val);
    auto _normals   = bind(&Data::normals, _val);
    auto _texCoords = bind(&Data::texCoords, _val);
    auto _faces     = bind(&Data::faces, _val);

    position = 'v' >> double_ >> double_ >> double_ >> -(double_);
    normal   = "vn" >> double_ >> double_ >> double_;
    texCoord = "vt" >> double_ >> -(double_) >> -(double_);

    faceDuo     = int_ >> -('/' >> int_);
    faceTriplet = int_ >> '/' >> -(int_) >> '/' >> int_;
    face        = 'f' >> +(faceTriplet | faceDuo);

    // clang-format off
    start = *(
      position      [push_back(_positions, _1)]
      | normal      [push_back(_normals, _1)]
      | texCoord    [push_back(_texCoords, _1)]
      | face        [push_back(_faces, _1)]
    );
    // clang-format on

    BOOST_SPIRIT_DEBUG_NODE(start);
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

  Data data;
  if (parseObj<Parser>(begin, end, data))
  {
    return data;
  }
  return std::nullopt;
}

//------------------------------------------------------------------------------
std::optional<ObjParser::ObjVariantVec>
loadAsVariant(const std::string& filename)
{
  using It     = boost::spirit::istream_iterator;
  using Data   = ObjVariantVec;
  using Parser = ObjParser::obj_parser_variant<Data, It>;

  return loadAndParseImpl<Parser, Data, It>(filename);
}

//------------------------------------------------------------------------------
std::optional<ObjParser::ObjAggregate>
loadAsAggregate(const std::string& filename)
{
  using It     = boost::spirit::istream_iterator;
  using Data   = ObjAggregate;
  using Parser = ObjParser::obj_parser<Data, It>;

  return loadAndParseImpl<Parser, Data, It>(filename);
}

//------------------------------------------------------------------------------
}    // namespace ObjParser
