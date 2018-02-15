#include "lib.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "TestFixture.h"
#include <iostream>

//------------------------------------------------------------------------------
int
main(int argc, char** argv)
{
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

//------------------------------------------------------------------------------
TEST_F(TestFixture, SingleSimpleVertex)
{
  const std::string VERTEX_SIMPLE_SINGLE = R"obj(
v  0.03264300152659416 0.056147500872612 -0.04995829984545708
)obj";

  const ObjParser::VertexPosition expected{
    0.03264300152659416, 0.056147500872612, -0.04995829984545708};

  {
    auto data = ObjParser::parseAsVariant(
      VERTEX_SIMPLE_SINGLE.begin(), VERTEX_SIMPLE_SINGLE.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(1u, data->size());
    ASSERT_EQ(typeid(ObjParser::VertexPosition), data->front().type())
      << "Expected VertexPosition type";

    const auto& v = boost::get<ObjParser::VertexPosition>(data->front());
    EXPECT_EQ(expected, v);
  }

  {
    auto data = ObjParser::parseAsAggregate(
      VERTEX_SIMPLE_SINGLE.begin(), VERTEX_SIMPLE_SINGLE.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(1u, data->positions.size());
    const auto& v = data->positions.front();
    EXPECT_EQ(expected, v);
  }
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

  {
    auto data = ObjParser::parseAsVariant(
      VERTEX_SIMPLE_LIST.begin(), VERTEX_SIMPLE_LIST.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expected.size(), data->size());
    int i = 0;
    for (auto& e : *data)
    {
      ASSERT_EQ(typeid(ObjParser::VertexPosition), e.type())
        << "Expected VertexPosition type";

      const auto& v = boost::get<ObjParser::VertexPosition>(e);
      EXPECT_EQ(expected[i++], v);
    }
  }

  {
    auto data = ObjParser::parseAsAggregate(
      VERTEX_SIMPLE_LIST.begin(), VERTEX_SIMPLE_LIST.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expected.size(), data->positions.size());
    int i = 0;
    for (auto& actual : data->positions)
    {
      EXPECT_EQ(expected[i++], actual);
    }
  }
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

  {
    auto data
      = ObjParser::parseAsVariant(NORMALS_LIST.begin(), NORMALS_LIST.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expected.size(), data->size());
    int i = 0;
    for (auto& e : *data)
    {
      ASSERT_EQ(typeid(ObjParser::VertexNormal), e.type())
        << "Expected VertexNormal type";

      const auto& v = boost::get<ObjParser::VertexNormal>(e);
      EXPECT_EQ(expected[i++], v);
    }
  }

  {
    auto data
      = ObjParser::parseAsAggregate(NORMALS_LIST.begin(), NORMALS_LIST.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expected.size(), data->normals.size());
    int i = 0;
    for (auto& actual : data->normals)
    {
      EXPECT_EQ(expected[i++], actual);
    }
  }
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

  {
    auto data = ObjParser::parseAsVariant(UV_LIST.begin(), UV_LIST.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expected.size(), data->size());
    int i = 0;
    for (auto& e : *data)
    {
      ASSERT_EQ(typeid(ObjParser::VertexTextureCoordinate), e.type())
        << "Expected VertexNormal type";

      const auto& v = boost::get<ObjParser::VertexTextureCoordinate>(e);
      EXPECT_EQ(expected[i++], v);
    }
  }

  {
    auto data = ObjParser::parseAsAggregate(UV_LIST.begin(), UV_LIST.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expected.size(), data->texCoords.size());
    int i = 0;
    for (auto& actual : data->texCoords)
    {
      EXPECT_EQ(expected[i++], actual);
    }
  }
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

  {
    auto data = ObjParser::parseAsVariant(FACE_LIST.begin(), FACE_LIST.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expected.size(), data->size());
    int i = 0;
    for (auto& e : *data)
    {
      ASSERT_EQ(typeid(ObjParser::Face), e.type())
        << "Expected VertexNormal type";

      const auto& v = boost::get<ObjParser::Face>(e);
      EXPECT_EQ(expected[i++], v);
    }
  }

  {
    auto data = ObjParser::parseAsAggregate(FACE_LIST.begin(), FACE_LIST.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expected.size(), data->faces.size());
    int i = 0;
    for (auto& actual : data->faces)
    {
      EXPECT_EQ(expected[i++], actual);
    }
  }
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

  ObjParser::ObjVariantVec expVariant;
  expVariant.insert(expVariant.end(), expVerts.begin(), expVerts.begin() + 3);
  expVariant.insert(expVariant.end(), expNormals.begin(), expNormals.end());
  expVariant.insert(expVariant.end(), expVerts.begin() + 3, expVerts.end());
  expVariant.insert(expVariant.end(), expUVs.begin(), expUVs.end());
  expVariant.insert(expVariant.end(), expFaces.begin(), expFaces.end());

  size_t sumSize
    = expVerts.size() + expNormals.size() + expUVs.size() + expFaces.size();
  ASSERT_EQ(expVariant.size(), sumSize);

  {
    auto data = ObjParser::parseAsVariant(
      MIXED_ALL_FEATURES.begin(), MIXED_ALL_FEATURES.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expVariant.size(), data->size());
    int i = 0;
    for (auto& actual : *data)
    {
      EXPECT_EQ(expVariant[i++], actual);
    }
  }

  {
    auto data = ObjParser::parseAsAggregate(
      MIXED_ALL_FEATURES.begin(), MIXED_ALL_FEATURES.end());
    ASSERT_TRUE(data.has_value()) << "Failed to parse";

    ASSERT_EQ(expVerts.size(), data->positions.size());
    int i = 0;
    for (auto& actual : data->positions)
    {
      EXPECT_EQ(expVerts[i++], actual);
    }

    ASSERT_EQ(expNormals.size(), data->normals.size());
    i = 0;
    for (auto& actual : data->normals)
    {
      EXPECT_EQ(expNormals[i++], actual);
    }

    ASSERT_EQ(expUVs.size(), data->texCoords.size());
    i = 0;
    for (auto& actual : data->texCoords)
    {
      EXPECT_EQ(expUVs[i++], actual);
    }

    ASSERT_EQ(expFaces.size(), data->faces.size());
    i = 0;
    for (auto& actual : data->faces)
    {
      EXPECT_EQ(expFaces[i++], actual);
    }
  }
}
//------------------------------------------------------------------------------
