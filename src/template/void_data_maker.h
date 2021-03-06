#ifndef VOID_DATA_MAKER_H_
#define VOID_DATA_MAKER_H_
#include "absl/time/clock.h"
#include "src/base_type_memory.h"
#include "foo/foo.pb.h"
#include "bar/bar.pb.h"
#include <vector>
namespace protobuf_plugin_demo {

class VoidDataMaker {
public:


  static void Build(const std::vector<int> &field_ids,
                          const ::foo::Foo &foo_fields,
                          std::vector<const void *> *void_data,
                          ::protobuf_plugin_demo::BaseTypeMemory *memory_holder);
};

} /* namespace protobuf_plugin_demo */

#endif /* VOID_DATA_MAKER_H_ */
