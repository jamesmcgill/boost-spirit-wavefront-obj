#include "ObjParser.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "TestFixture.h"

namespace
{
//------------------------------------------------------------------------------
TEST_F(TestFixture, SingleSimpleVertex)
{
  const std::string VERTEX_SIMPLE_SINGLE = R"obj(
v  0.03264300152659416 0.056147500872612 -0.04995829984545708
)obj";

  const std::vector<ObjParser::VertexPosition> expected
    = {{0.03264300152659416, 0.056147500872612, -0.04995829984545708}};

  ObjParser::ObjAggregate agg;
  agg.positions = expected;

  performTestVariant(VERTEX_SIMPLE_SINGLE, expected);
  performTestAggregate(VERTEX_SIMPLE_SINGLE, agg);
}

//------------------------------------------------------------------------------
TEST_F(TestFixture, SimpleVertexList)
{
  const std::string VERTEX_SIMPLE_LIST = R"obj(
v  0.03264300152659416 0.056147500872612 -0.04995829984545708
v  0.03251679986715317 0.05801349878311157 -0.050050999969244
v  0.03080499917268753 0.0559782013297081 -0.04991229996085167 1.5
v  0.03264300152659416 0.056147500872612 -0.04995829984545708
v  0.03080499917268753 0.0559782013297081 -0.04991229996085167
)obj";

  const std::vector<ObjParser::VertexPosition> expected = {
    {0.03264300152659416f, 0.056147500872612f, -0.04995829984545708f, 1.0f},
    {0.03251679986715317f, 0.05801349878311157f, -0.050050999969244f, 1.0f},
    {0.03080499917268753f, 0.0559782013297081f, -0.04991229996085167f, 1.5f},
    {0.03264300152659416f, 0.056147500872612f, -0.04995829984545708f, 1.0f},
    {0.03080499917268753f, 0.0559782013297081f, -0.04991229996085167f, 1.0f}};

  ObjParser::ObjAggregate agg;
  agg.positions = expected;

  performTestVariant(VERTEX_SIMPLE_LIST, expected);
  performTestAggregate(VERTEX_SIMPLE_LIST, agg);
}

//------------------------------------------------------------------------------
TEST_F(TestFixture, NormalsList)
{
  static const std::string NORMALS_LIST = R"obj(
vn -0.4538693428039551 0.3681831955909729 0.8114454746246338
vn -0.15167635679245 0.9458057284355164 -0.2871338427066803
vn  0.03080499917268753 0.0559782013297081 -0.04991229996085167
)obj";

  const std::vector<ObjParser::VertexNormal> expected
    = {{-0.453869342803951f, 0.3681831955909729f, 0.8114454746246338f},
       {-0.15167635679245f, 0.9458057284355164f, -0.2871338427066803f},
       {0.03080499917268753f, 0.0559782013297081f, -0.04991229996085167f}};

  ObjParser::ObjAggregate agg;
  agg.normals = expected;

  performTestVariant(NORMALS_LIST, expected);
  performTestAggregate(NORMALS_LIST, agg);
}

//------------------------------------------------------------------------------
TEST_F(TestFixture, UVList)
{
  static const std::string UV_LIST = R"obj(
vt 0.089608
vt 0.089608 0.023837
vt 0.5 0.5
vt 0.987654 0.246802 0.123456
vt 0.5 0.5 0.5
)obj";

  const std::vector<ObjParser::VertexTextureCoordinate> expected
    = {{0.089608f, 0.0f, 0.0f},
       {0.089608f, 0.023837f, 0.0f},
       {0.5f, 0.5f, 0.0f},
       {0.987654f, 0.246802f, 0.123456f},
       {0.5f, 0.5f, 0.5f}};

  ObjParser::ObjAggregate agg;
  agg.texCoords = expected;

  performTestVariant(UV_LIST, expected);
  performTestAggregate(UV_LIST, agg);
}

//------------------------------------------------------------------------------
TEST_F(TestFixture, FacesList)
{
  static const std::string FACE_LIST = R"obj(
f  5 1 6
f  8/8 6/6 9/9
f  1//1 2//2 3//3
f  11/11/11 12/12/12 2/2/2
)obj";

  const std::vector<ObjParser::Face> expected = {
    {{5, 0, 0}, {1, 0, 0}, {6, 0, 0}},
    {{8, 8, 0}, {6, 6, 0}, {9, 9, 0}},
    {{1, 0, 1}, {2, 0, 2}, {3, 0, 3}},
    {{11, 11, 11}, {12, 12, 12}, {2, 2, 2}},
  };

  ObjParser::ObjAggregate agg;
  agg.faces = expected;

  performTestVariant(FACE_LIST, expected);
  performTestAggregate(FACE_LIST, agg);
}

//------------------------------------------------------------------------------
TEST_F(TestFixture, AllFeatures)
{
  static const std::string MIXED_ALL_FEATURES = R"obj(
mtllib	test.mtl
# Vertices here
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
f  11/11/11 12/12/12 2/2/2
f  5 1 6
f  1//1 3//3 6//6
f  8/8 6/6 9/9
)obj";

  const std::vector<ObjParser::VertexPosition> expVerts = {
    {0.03264300152659416f, 0.056147500872612f, -0.04995829984545708f, 1.0f},
    {0.03080499917268753f, 0.0559782013297081f, -0.04991229996085167f, 1.5f},
    {0.03251679986715317f, 0.05801349878311157f, -0.050050999969244f, 1.0f},
    {0.03264300152659416f, 0.056147500872612f, -0.04995829984545708f, 1.0f},
    {0.03080499917268753f, 0.0559782013297081f, -0.04991229996085167f, 1.0f}};

  const std::vector<ObjParser::VertexNormal> expNormals = {
    {-0.4538693428039551f, 0.3681831955909729f, 0.8114454746246338f},
    {-0.15167635679245f, 0.9458057284355164f, -0.2871338427066803f},
  };

  const std::vector<ObjParser::VertexTextureCoordinate> expUVs
    = {{0.089608f, 0.023837f, 0.0f}, {-0.07313f, 0.023837f, 0.0f}};

  const std::vector<ObjParser::Face> expFaces = {
    {{1, 0, 1}, {2, 0, 2}, {3, 0, 3}},
    {{11, 11, 11}, {12, 12, 12}, {2, 2, 2}},
    {{5, 0, 0}, {1, 0, 0}, {6, 0, 0}},
    {{1, 0, 1}, {3, 0, 3}, {6, 0, 6}},
    {{8, 8, 0}, {6, 6, 0}, {9, 9, 0}},
  };

  // Variant Test
  ObjParser::ObjVariantVec expVariant;
  expVariant.insert(expVariant.end(), expVerts.begin(), expVerts.begin() + 3);
  expVariant.insert(expVariant.end(), expNormals.begin(), expNormals.end());
  expVariant.insert(expVariant.end(), expVerts.begin() + 3, expVerts.end());
  expVariant.insert(expVariant.end(), expUVs.begin(), expUVs.end());
  expVariant.insert(expVariant.end(), expFaces.begin(), expFaces.end());
  size_t sumSize
    = expVerts.size() + expNormals.size() + expUVs.size() + expFaces.size();
  ASSERT_EQ(expVariant.size(), sumSize);

  performTestVariant(MIXED_ALL_FEATURES, expVariant);

  // Aggregate Test
  ObjParser::ObjAggregate expected;
  expected.positions = expVerts;
  expected.normals   = expNormals;
  expected.texCoords = expUVs;
  expected.faces     = expFaces;

  performTestAggregate(MIXED_ALL_FEATURES, expected);
}

//------------------------------------------------------------------------------
}    // namespace

//------------------------------------------------------------------------------
int
main(int argc, char** argv)
{
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

//------------------------------------------------------------------------------
