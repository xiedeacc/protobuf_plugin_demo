/*******************************************************************************
 * Copyright (c) 2015, 2018 Copyright 2015-2018 pinduoduo.com.
 * All rights reserved.
 *******************************************************************************/
#ifndef FG_GENERATOR_VOID_DATA_MAKER_H_
#define FG_GENERATOR_VOID_DATA_MAKER_H_
#include "absl/time/clock.h"
#include "fg/common/base_type_memory.h"
#include "src/demo.pb.h"
#include <vector>
namespace fg {
namespace generator {

class VoidDataMaker {
public:
  static void Build(const std::vector<int> &features_case_NOs,
                    const ::feature::FgCommonFeatures &fg_feature,
                    std::vector<const void *> *void_data,
                    fg::common::BaseTypeMemory *memory_holder);

  static void Build(const std::vector<int> &feature_ids,
                    const ::feature::FgCandidateFeatures &fg_feature,
                    std::vector<const void *> *void_data,
                    fg::common::BaseTypeMemory *memory_holder);

  static void
  Build(const std::vector<int> &feature_ids,
        const std::vector<::feature::FgCandidateFeatures> &fg_features,
        std::vector<std::vector<const void *>> *candidates_void_data,
        fg::common::BaseTypeMemory *memory_holder);
};

} /* namespace generator */
} /* namespace fg */

#endif /* FG_GENERATOR_VOID_DATA_MAKER_H_ */
