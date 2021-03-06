#ifndef BASE_TYPE_MEMORY_H_
#define BASE_TYPE_MEMORY_H_
#include <string>
#include <vector>
#include "glog/logging.h"
#include "gtest/gtest_prod.h"
#include "attr.h"
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

namespace protobuf_plugin_demo {

template<typename T>
class BaseTypeMemoryItem {
 public:
  explicit BaseTypeMemoryItem(size_t size = 100) {
    idx_ = 0;
    size_ = size;
    data = new T[size];
  }

  virtual ~BaseTypeMemoryItem() {
    Reset();
    delete[] data;
  }

  void Reset() {
    idx_ = 0;
    if (unlikely(small_datas.size() > 0)) {
      for (size_t i = 0; i < small_datas.size(); ++i) {
        delete[] small_datas[i];
      }
      small_datas.clear();
    }
  }

  T* Get() {
    if (likely(idx_ < size_)) {
      return data + (idx_++);
    }
    small_datas.push_back(data);
    size_ = 2 * size_;
    data = new T[size_];
    idx_ = 0;
    return data + (idx_++);
  }

 private:
  size_t idx_;
  size_t size_;

 public:
  T* data;
  std::vector<T*> small_datas;
};

template <>
class BaseTypeMemoryItem<std::string> {
 public:
  explicit BaseTypeMemoryItem(size_t size = 10) {
    idx_ = 0;
    size_ = size;
    data = CreateBuffString(size);
  }

  virtual ~BaseTypeMemoryItem() {
    Reset();
    delete[] data;
  }

  void Reset() {
    for (size_t i = 0; i < idx_; ++i) {
      data[i].clear();
    }
    idx_ = 0;
    if (unlikely(small_datas.size() > 0)) {
      for (size_t i = 0; i < small_datas.size(); ++i) {
        delete[] small_datas[i];
      }
      small_datas.clear();
    }
  }

  std::string* Get() {
    if (likely(idx_ < size_)) {
      return data + (idx_++);
    }
    small_datas.push_back(data);
    size_ = 2 * size_;
    data = CreateBuffString(size_);
    idx_ = 0;
    return data + (idx_++);
  }

 private:
  std::string* CreateBuffString(size_t size) {
    std::string* ret = new std::string[size];
    for (size_t i = 0; i < size; ++i) {
      (ret + i)->reserve(100);
    }
    return ret;
  }

 private:
  size_t idx_;
  size_t size_;

 public:
  std::string* data;
  std::vector<std::string*> small_datas;
};

template <>
class BaseTypeMemoryItem<FormatAttr> {
  FRIEND_TEST(BaseTypeMemoryItem, FormatAttr);

 public:
  explicit BaseTypeMemoryItem(size_t size = 100) {
    idx_ = 0;
    bucket_idx = 0;
    datas.reserve(100);
    profile.reserve(100);
    profile_count = 0;
    for (size_t i = 0; i < size; i += kBucketSize) {
      NewBucket();
    }

    if (datas.size() <= 0) {
      NewBucket();
    }

    data = datas[0];
  }

  virtual ~BaseTypeMemoryItem() {
    for (auto& pdata : datas) {
      delete[] pdata;
    }
    datas.clear();
  }

  void Reset() {
    for (size_t i = 0; i < idx_; ++i) {
      data[i].Reset();
    }

    for (size_t j = 0; j < bucket_idx; ++j) {
      auto& b = datas[j];
      for (size_t bN = 0; bN < kBucketSize; ++bN) {
        b[bN].Reset();
      }
    }

    // TryRelease();
    bucket_idx = 0;
    idx_ = 0;
    data = datas[0];
  }

  FormatAttr* Get() {
    if (unlikely(idx_ >= kBucketSize)) {
      ++bucket_idx;
      if (unlikely(bucket_idx >= datas.size())) {
        NewBucket();
      }
      data = datas[bucket_idx];
      idx_ = 0;
    }
    return data + (idx_++);
  }

  size_t GetBucketCount() {
    return datas.size();
  }

 private:
  void NewBucket() {
    datas.push_back(new FormatAttr[kBucketSize]);
    profile.push_back(0U);
  }

  void TryRelease() {
    profile[bucket_idx]++;
    if (likely(++profile_count < kMaxProfileCount)) {
      return;
    }

    size_t expire_count = 0;
    size_t k_max = profile.size() - 1;
    for (; k_max >= 0; --k_max) {
      auto& c_profile_count = profile[k_max];
      size_t x_c = expire_count + c_profile_count;
      if (x_c > kExpireCount) {
        break;
      }
      delete[] datas[k_max];
      datas[k_max] = nullptr;
      expire_count = x_c;
    }

    datas.resize(k_max + 1);
    profile_count = 0;
    profile.clear();
    for (size_t i = 0; i < datas.size(); ++i) {
      profile.push_back(0U);
    }
  }

 private:
  size_t idx_;
  size_t bucket_idx;
  std::vector<FormatAttr*> datas;
  FormatAttr* data;
  size_t profile_count;
  std::vector<size_t> profile;

  static const size_t kBucketSize = 100;
  static const size_t kMaxProfileCount = 10000;
  static constexpr double kExpireRatio = 0.0001;
  static const size_t kExpireCount = kMaxProfileCount * kExpireRatio;
};

class BaseTypeMemory {
 private:
  BaseTypeMemoryItem<int32_t> I32;
  BaseTypeMemoryItem<int64_t> I64;
  BaseTypeMemoryItem<uint32_t> U32;
  BaseTypeMemoryItem<uint64_t> U64;

  BaseTypeMemoryItem<double> d;
  BaseTypeMemoryItem<float> f;
  BaseTypeMemoryItem<bool> b;
  BaseTypeMemoryItem<std::string> s;

 public:
  int32_t* GetInt32() {
    return I32.Get();
  }
  int64_t* GetInt64() {
    return I64.Get();
  }
  uint32_t* GetUInt32() {
    return U32.Get();
  }
  uint64_t* GetUInt64() {
    return U64.Get();
  }
  double* GetDouble() {
    return d.Get();
  }
  float* GetFloat() {
    return f.Get();
  }
  bool* GetBool() {
    return b.Get();
  }

  std::string* GetString() {
    return s.Get();
  }

 public:
  void Reset() {
    I32.Reset();
    I64.Reset();
    U32.Reset();
    U64.Reset();
    d.Reset();
    f.Reset();
    b.Reset();
    s.Reset();
  }
};

} /* namespace protobuf_plugin_demo */

#endif /* BASE_TYPE_MEMORY_H_ */
