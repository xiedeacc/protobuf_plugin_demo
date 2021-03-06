#ifndef ATTR_H_
#define ATTR_H_
#include <string>
#include <vector>

#include "absl/container/inlined_vector.h"

#include "glog/logging.h"
namespace protobuf_plugin_demo{
enum FeaturesType {
  DENSE,
  SPARSE,
  // tf feature types
  INT64,
  FLOAT,
  STRING,
  UNKNOWN,
};

class AttrDiscription {
 public:
  void Reset() {
    //  op_name.clear();
    //  op_feature_id.clear();
    //  ext.clear();
    std::string("").swap(op_name);
    std::string("").swap(ext);
    std::string("").swap(op_feature_id);
  }

 public:
  std::string op_name;
  std::string ext;
  std::string op_feature_id;
};

template <typename T>
using InlinedVector = absl::InlinedVector<T, 4>;

class FormatAttr {
 public:
  FormatAttr() {
    column_name.reserve(108);
    feature_values.reserve(1);
  }

 public:
  void Reset() {
    if (column_name.capacity() > 108) {
      std::string("").swap(column_name);
      column_name.reserve(108);
    }
    column_name.clear();
    column_values.clear();
    feature_values.clear();
    type = UNKNOWN;
    discription.Reset();
    hash_code = -1;
    is_replicative = false;
    INT64_VALUES.clear();
    FLOAT_VALUES.clear();
    STRING_VALUES.clear();
    STRING_HASHS.clear();
  }

 public:
  std::string column_name;
  std::vector<std::string> column_values;
  std::vector<double> feature_values;
  // tf column values
  InlinedVector<int64_t> INT64_VALUES;
  InlinedVector<float> FLOAT_VALUES;
  InlinedVector<std::string*> STRING_VALUES;
  InlinedVector<uint64_t> STRING_HASHS;

  FeaturesType type;
  AttrDiscription discription;

  // Use for performance
 public:
  int64_t hash_code;
  bool is_replicative;
};

}  // namespace protobuf_plugin_demo

#endif /* ATTR_H_ */
