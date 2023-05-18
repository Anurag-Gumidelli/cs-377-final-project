#include <fs.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

using namespace std;

class FSTest : public ::testing::Test {
 protected:
  FSTest() {
    system("rm -f disk0");
    system("rm -f disk1");
    system("./create_fs disk0");
    system("./create_fs disk1");
  }
};

// test create_file return code 1 for success
TEST_F(FSTest, create_over_file_test) {
  myFileSystem f((char *)"disk0");
  int code0 = f.create_file((char *)"test.c", 1);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test0.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test1.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test2.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test3.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test4.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test5.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test6.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test7.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test8.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test9.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test10.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test11.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test12.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test13.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test14.c", 2);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test15.c", 2);
  ASSERT_EQ(-1, code0);
}

TEST_F(FSTest, create_file_no_free_block) {
  myFileSystem f((char *)"disk0");
  int code0 = f.create_file((char *)"test.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test0.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test1.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test2.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test3.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test4.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test5.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test6.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test7.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test8.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test9.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test10.c",8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test11.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test12.c", 8);
  ASSERT_EQ(1, code0);
  code0 = f.create_file((char *)"test13.c", 8);
  ASSERT_EQ(1, code0);
  cout << "reached here\n";
  code0 = f.create_file((char *)"test14.c", 8);
  ASSERT_EQ(-1, code0);
}

// test delete_file return code -1 for failure
TEST_F(FSTest, delete_file_test) {
  myFileSystem f((char *)"disk0");

  int code0 = f.create_file((char *)"test.c", 8);
  ASSERT_EQ(1, code0);

  int code = f.delete_file((char *)"test.c");
  ASSERT_EQ(1, code);
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}