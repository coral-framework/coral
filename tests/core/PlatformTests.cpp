#include <co/Platform.h>
#include <co/TypeKind.h>
#include <gtest/gtest.h>

TEST(PlatformTests, sizeOf) {
  EXPECT_EQ(1, sizeof(co::int8));
  EXPECT_EQ(2, sizeof(co::int16));
  EXPECT_EQ(4, sizeof(co::int32));
  EXPECT_EQ(8, sizeof(co::int64));
  EXPECT_EQ(8, sizeof(double));

  EXPECT_EQ(sizeof(co::int32), sizeof(co::TypeKind));
}
