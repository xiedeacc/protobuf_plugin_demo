#include <vector>
#include "src/base_type_memory.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
using std::string;
using std::vector;

namespace protobuf_plugin_demo {
TEST(BaseTypeMemoryItem, Get) {
  BaseTypeMemoryItem<int> item(1);
  for (int x = 0; x < 10; ++x) {
    item.Reset();
    vector<int*> addrs;
    for (int i = 0; i < 1000; ++i) {
      int* addr = item.Get();
      *addr = i;
      addrs.push_back(addr);
    }
    for (int i = 0; i < 1000; ++i) {
      ASSERT_EQ(i, *addrs[i]);
    }
  }
}

TEST(BaseTypeMemoryItem, string) {
  BaseTypeMemoryItem<string> item(1);
  for (int x = 0; x < 10; ++x) {
    item.Reset();
    vector<string*> addrs;
    for (int i = 0; i < 10000; ++i) {
      auto addr = item.Get();
      *addr = std::to_string(i);
      addrs.push_back(addr);
    }
    for (int i = 0; i < 10000; ++i) {
      ASSERT_EQ(std::to_string(i), *addrs[i]);
    }
  }
}

}  // namespace protobuf_plugin_demo
