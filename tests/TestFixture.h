//------------------------------------------------------------------------------
template <typename T>
void
checkExpectedVariant(const ObjParser::ObjVariant& actual, const T& expected)
{
  ASSERT_EQ(typeid(T), actual.type()) << "Wrong type";
  EXPECT_EQ(expected, boost::get<T>(actual));
}

//------------------------------------------------------------------------------
template <>
void
checkExpectedVariant(
  const ObjParser::ObjVariant& actual, const ObjParser::ObjVariant& expected)
{
  ASSERT_EQ(expected.type(), actual.type()) << "Wrong type";
  EXPECT_EQ(expected, actual);
}

//------------------------------------------------------------------------------
template <typename T>
void
performTestVariant(const std::string& input, const std::vector<T>& expected)
{
  auto data = ObjParser::parseAsVariant(input.begin(), input.end());
  ASSERT_TRUE(data.has_value()) << "Failed to parse";

  ASSERT_EQ(expected.size(), data->size());
  int i = 0;
  for (const auto& actual : *data)
  {
    checkExpectedVariant(actual, expected[i++]);
  }
}

//------------------------------------------------------------------------------
void
performTestAggregate(
  const std::string& input, const ObjParser::ObjAggregate& expected)
{
  auto data = ObjParser::parseAsAggregate(input.begin(), input.end());
  ASSERT_TRUE(data.has_value()) << "Failed to parse";

  ASSERT_EQ(expected.positions.size(), data->positions.size());
  int i = 0;
  for (auto& actual : data->positions)
  {
    EXPECT_EQ(expected.positions[i++], actual);
  }

  ASSERT_EQ(expected.normals.size(), data->normals.size());
  i = 0;
  for (auto& actual : data->normals)
  {
    EXPECT_EQ(expected.normals[i++], actual);
  }

  ASSERT_EQ(expected.texCoords.size(), data->texCoords.size());
  i = 0;
  for (auto& actual : data->texCoords)
  {
    EXPECT_EQ(expected.texCoords[i++], actual);
  }

  ASSERT_EQ(expected.faces.size(), data->faces.size());
  i = 0;
  for (auto& actual : data->faces)
  {
    EXPECT_EQ(expected.faces[i++], actual);
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class TestFixture : public ::testing::Test
{
public:
  TestFixture() {}
  virtual ~TestFixture() {}
};
//------------------------------------------------------------------------------
