#include "void_data_maker.h"
#include "glog/logging.h"
#include <vector>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
using absl::GetCurrentTimeNanos;
using std::vector;
using foo::Foo;
using bar::Bar;
namespace protobuf_plugin_demo{

void VoidDataMaker::Build(const vector<int> &field_ids,
                          const Foo& foo_fields,
                          vector<const void *> *void_data,
                          BaseTypeMemory *memory_holder) {
  /**CandicateBoolFileData**/
  if (void_data->size() != field_ids.size()) {
    void_data->resize(field_ids.size());
  }
  for (size_t i = 0; i < field_ids.size(); ++i) {
    switch (field_ids[i]) { /**CandicateFieldsFileData**/
    }
  }
}

} /* namespace protobuf_plugin_demo*/
