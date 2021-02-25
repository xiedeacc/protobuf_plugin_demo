/*******************************************************************************
 * Copyright (c) 2015, 2018 Copyright 2015-2018 pinduoduo.com.
 * All rights reserved.
 *******************************************************************************/
#include "fg/feature/void_data_maker.h"
#include "glog/logging.h"
#include <vector>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
using absl::GetCurrentTimeNanos;
using ::feature::FgCandidateFeatures;
using ::feature::FgCommonFeatures;
using fg::common::BaseTypeMemory;
using std::vector;

namespace fg {
namespace generator {
void VoidDataMaker::Build(const vector<int> &feature_ids,
                          const FgCommonFeatures &fg_feature,
                          vector<const void *> *void_data,
                          BaseTypeMemory *memory_holder) {
  /**CommonBoolFileData**/
  if (void_data->size() != feature_ids.size()) {
    void_data->resize(feature_ids.size());
  }
  for (size_t i = 0; i < feature_ids.size(); ++i) {
    switch (feature_ids[i]) { /**CommonFeatureFileData**/
    }
  }
}

void VoidDataMaker::Build(const vector<int> &feature_ids,
                          const FgCandidateFeatures &fg_feature,
                          vector<const void *> *void_data,
                          BaseTypeMemory *memory_holder) {
  /**CandicateBoolFileData**/
  if (void_data->size() != feature_ids.size()) {
    void_data->resize(feature_ids.size());
  }
  for (size_t i = 0; i < feature_ids.size(); ++i) {
    switch (feature_ids[i]) { /**CandicateFeatureFileData**/
    }
  }
}

} /* namespace generator */
} /* namespace fg */
