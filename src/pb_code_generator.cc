/*******************************************************************************
 * Copyright (c) 2015, 2018 Copyright 2015-2018 pinduoduo.com.
 * All rights reserved.
 *******************************************************************************/
#include "src/pb_code_generator.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "glog/logging.h"
#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "src/file_util.h"
#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

using demo::FileUtil;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::FileDescriptor;
using google::protobuf::compiler::GeneratorContext;
using google::protobuf::io::Printer;
using google::protobuf::io::ZeroCopyOutputStream;
using std::map;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

namespace demo {

PBCodeGenerator::PBCodeGenerator(const string &template_dir) {
  this->file_ = nullptr;
  this->template_dir_ = template_dir;
  this->generator_context_ = nullptr;
}

bool PBCodeGenerator::Generate(const FileDescriptor *file,
                               const string &parameter,
                               GeneratorContext *generator_context,
                               string *error) const {
  generator_context_ = generator_context;
  file_ = file;
  bool ret = true;
  ret = ret && GeneratePBFeatureData();
  ret = ret && GenerateVoidDataCodeH();
  return ret;
}
bool PBCodeGenerator::GenerateVoidDataCodeH() const {
  string t_data = FileUtil::LoadContent(GetFullPath("void_data_maker.h"));
  return Print("void_data_maker.h", t_data);
}

void PBCodeGenerator::BuildConfigName(const string &full_name,
                                      string *config_name, bool *is_common) {
  static map<string, string> config_prefix_map = {
      {"ContextFeature", "context"},
      {"AdFeature", "item"},
      {"CatFeature", "cat"},
      {"CreativeFeature", "cat"},
      {"GoodsFeature", "item"},
      {"ItemContextFeature", "context"},
      {"MallFeature", "item"},
      {"UserFeature", "user"},
      {"QueryFeature", "context"},
      {"ItemFeature", "item"},
      {"UserRealtimeFeature", "context"},
      {"PackContext", "context"},
      {"Item2Ad", "context"},
      {"ServerGeneratedFeature", "payload"},
  };

  *is_common = false;
  if (full_name == "GoodsFeature.cat_id") {
    *config_name = "goods.cat_id";
    return;
  }

  for (auto &pair : config_prefix_map) {
    auto pos = full_name.find(pair.first + ".");
    if (pos == string::npos) {
      continue;
    }
    if (pair.second == "user") {
      *is_common = true;
    }

    *config_name = pair.second + full_name.substr(pos + pair.first.size());
    return;
  }

  *config_name = full_name;
}

void PBCodeGenerator::GenerateOutStr2(string &store_content,
                                      const FieldDescriptor &level2_field,
                                      const FieldDescriptor &level3_field,
                                      int &case_num) const {
  if (level3_field.is_map() || level3_field.is_repeated()) {
    store_content.append(string("\n    "));
    store_content.append(string("case ") + absl::AlphaNum(case_num).data() +
                         ":\n");
    if (level2_field.name() == "item_feature" &&
        level3_field.name() == "cat_id") {
      const char *json_str =
          R"V0G0N(
    {
      auto& x_vec_item = (*void_data)[i];
      if (likely(has_item_feature)) {
         auto& x_item_feature = fg_feature.item_feature();
         auto& x_o = *memory_holder->GetInt64();
         x_vec_item = (const void*)&x_o;
         x_o = 0;
         for (int _xx_ = x_item_feature.cat_id_size() - 1; _xx_ >= 0; --_xx_) {
           if (x_item_feature.cat_id()[_xx_] != 0) {
              x_o = x_item_feature.cat_id()[_xx_];
              break;
           }
        }
      } else {
        x_vec_item = nullptr;
      }
      break;
    }
   )V0G0N";
      store_content.append(json_str);
    } else {
      store_content.append(string("    "));
      store_content.append(string("(*void_data)[i] = ") + "(const void*)(");
      store_content.append(string("likely(has_") + level2_field.name() +
                           ") ? ");
      store_content.append("(&(fg_feature." + level2_field.name() + "()." +
                           level3_field.name() + "()))" + " : NULL" + ");\n");
      store_content.append(string("    ") + "break;\n");
    }
  } else if (level3_field.cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
    store_content.append(string("\n    "));
    store_content.append(string("case ") + absl::AlphaNum(case_num).data() +
                         ":\n");
    store_content.append(string("    ") + "(*void_data)[i] = ");
    store_content.append("(const void*)(");
    store_content.append(string("likely(has_") + level2_field.name() + ") ? ");
    store_content.append("(fg_feature." + level2_field.name() + "().");
    store_content.append("has_" + level3_field.name() + "() ? ");
    store_content.append("&fg_feature." + level2_field.name() + "().");
    store_content.append(level3_field.name() + "() : NULL)" + " : NULL" +
                         ");\n");
    store_content.append(string("    ") + "break;\n");
  } else if (level3_field.cpp_type() == FieldDescriptor::CPPTYPE_STRING) {
    store_content.append(string("\n    "));
    store_content.append(string("case "));
    store_content.append(string(absl::AlphaNum(case_num).data()) + ":\n");
    store_content.append(string("    ") + "(*void_data)[i] = ");
    store_content.append(string("(const void*)(") + "likely(has_");
    store_content.append(level2_field.name() + ") ? ");
    store_content.append("(fg_feature." + level2_field.name() + "().");
    store_content.append(level3_field.name() + "_oneof_case() ? ");
    store_content.append("&fg_feature." + level2_field.name() + "().");
    store_content.append(level3_field.name() + "() : NULL)");
    store_content.append(string(" : NULL);\n") + string("    ") + "break;\n");
  } else if (level3_field.cpp_type() == FieldDescriptor::CPPTYPE_FLOAT) {
    store_content.append(string("\n    "));
    store_content.append(string("case ") + absl::AlphaNum(case_num).data());
    store_content.append(":\n");
    store_content.append(string("    ") + "(*void_data)[i] = ");
    store_content.append("(const void*)(");
    store_content.append(string("likely(has_"));
    store_content.append(string(level2_field.name()) + ") ? ");
    store_content.append("(fg_feature." + level2_field.name() + "().");
    store_content.append(string(level3_field.name()) + "_oneof_case() ? ");
    store_content.append("&(*memory_holder->GetFloat() = fg_feature.");
    store_content.append(string(level2_field.name()) + "().");
    store_content.append(string(level3_field.name()));
    store_content.append(string("()) : NULL)") + " : NULL" + ");\n");
    store_content.append(string("    ") + "break;\n");
  } else {
    store_content.append(string("\n    "));
    store_content.append("case ");
    store_content.append(string(absl::AlphaNum(case_num).data()) + ":\n");
    if (level2_field.name() == "goods_feature" &&
        (level3_field.name() == "cat_id" || level3_field.name() == "cat_id_1" ||
         level3_field.name() == "cat_id_2")) {
      if (level3_field.name() == "cat_id") {
        const char *json_str =
            R"V0G0N(
      {
        auto& x_vec_item = (*void_data)[i];
        x_vec_item = nullptr;
        if (likely(has_goods_feature)) {
            if (fg_feature.goods_feature().cat_id_oneof_case()) {
               auto& value = *memory_holder->GetInt64();
               value = fg_feature.goods_feature().cat_id();
               x_vec_item = (const void*)&value;
            }
        } else {
            if (likely(has_item_feature)) {
              auto& x_item_feature = fg_feature.item_feature();
              auto& x_o = *memory_holder->GetInt64();
              x_vec_item = (const void*)&x_o;
              x_o = 0;
              for (int _xx_ = x_item_feature.cat_id_size() - 1; _xx_ >= 0; --_xx_) {
                 if (x_item_feature.cat_id()[_xx_] != 0) {
                    x_o = x_item_feature.cat_id()[_xx_];
                    break;
                  }
               }
            }
        }
        break;
      }
     )V0G0N";
        store_content.append(json_str);
      } else if (level3_field.name() == "cat_id_1") {
        const char *json_str =
            R"V0G0N(
      {
        auto& x_vec_item = (*void_data)[i];
        x_vec_item = nullptr;
        if (likely(has_item_feature)) {
            auto& x_item_feature = fg_feature.item_feature();
            if (x_item_feature.cat_id_size() > 0 &&
              x_item_feature.cat_id(0) > 0) {
              auto& x_o = *memory_holder->GetInt64();
              x_o = x_item_feature.cat_id(0);
              x_vec_item = (const void*)&x_o;
            }
        }
        if (x_vec_item == nullptr && has_goods_feature) {
            if (fg_feature.goods_feature().cat_id_1_oneof_case()) {
               auto& value = *memory_holder->GetInt64();
               value = fg_feature.goods_feature().cat_id_1();
               x_vec_item = (const void*)&value;
            }
        }
        break;
      }
     )V0G0N";
        store_content.append(json_str);
      } else if (level3_field.name() == "cat_id_2") {
        const char *json_str =
            R"V0G0N(
      {
        auto& x_vec_item = (*void_data)[i];
        x_vec_item = nullptr;
        if (likely(has_item_feature)) {
              auto& x_item_feature = fg_feature.item_feature();
              if (x_item_feature.cat_id_size() > 1 &&
                 x_item_feature.cat_id(1) > 0) {
                 auto& x_o = *memory_holder->GetInt64();
                 x_o = x_item_feature.cat_id(1);
                 x_vec_item = (const void*)&x_o;
              }
        }
        if (x_vec_item == nullptr && has_goods_feature) {
            if (fg_feature.goods_feature().cat_id_2_oneof_case()) {
               auto& value = *memory_holder->GetInt64();
               value = fg_feature.goods_feature().cat_id_2();
               x_vec_item = (const void*)&value;
            }
        }
        break;
      }
     )V0G0N";
        store_content.append(json_str);
      }
    } else {
      store_content.append(string("    ") + "(*void_data)[i] = ");
      store_content.append("(const void*)(");
      store_content.append(string("likely(has_") + level2_field.name() +
                           ") ? ");
      store_content.append(string("(fg_feature.") + level2_field.name() +
                           "().");
      store_content.append(string(level3_field.name()) + "_oneof_case() ? ");
      store_content.append(
          string("&(*memory_holder->GetInt64() = fg_feature."));
      store_content.append(string(level2_field.name()) + "().");
      store_content.append(string(level3_field.name()) + "()) : NULL)");
      store_content.append(string(" : NULL") + ");\n");
      store_content.append(string("    ") + "break;\n");
    }
  }
  ++case_num;
}

bool PBCodeGenerator::GeneratePBFeatureData() const {
  set<string> unique_store_strs;
  vector<string> store_strs;
  set<string> pb_type_strs;
  set<string> op_type_def_str;
  set<string> unique_messages;
  string store_content_common;
  string store_content_candidates;
  string bool_common_str;
  string bool_candidate_str;
  volatile bool is_first = true;
  int case_num = 0;
  pb_type_strs.insert("PBTYPE_BASE_INT32");
  pb_type_strs.insert("PBTYPE_BASE_INT64");
  pb_type_strs.insert("PBTYPE_BASE_UINT32");
  pb_type_strs.insert("PBTYPE_BASE_UINT64");
  pb_type_strs.insert("PBTYPE_BASE_DOUBLE");
  pb_type_strs.insert("PBTYPE_BASE_FLOAT");
  pb_type_strs.insert("PBTYPE_BASE_BOOL");
  pb_type_strs.insert("PBTYPE_BASE_STRING");

  for (int level1_idx = 0; level1_idx < file_->message_type_count();
       ++level1_idx) {
    auto &level1_msg = *(file_->message_type(level1_idx));
    for (int level2_idx = 0; level2_idx < level1_msg.field_count();
         ++level2_idx) {
      auto &level2_field = *(level1_msg.field(level2_idx));
      if (level2_field.type() != FieldDescriptor::Type::TYPE_MESSAGE) {
        LOG(ERROR) << "level2 is not msg";
        continue;
      }
      auto &level2_msg = *(level2_field.message_type());
      if (unique_messages.count(level2_msg.name()) != 0) {
        continue;
      }
      if (!is_first) {
        bool_candidate_str.append("  ");
      }
      is_first = false;
      bool_candidate_str.append("bool has_");
      bool_candidate_str.append(level2_field.name());
      bool_candidate_str.append(" = ");
      bool_candidate_str.append("fg_feature.has_");
      bool_candidate_str.append(level2_field.name());
      bool_candidate_str.append("() ? true : false;\n");
      for (int level3_idx = 0; level3_idx < level2_msg.field_count();
           ++level3_idx) {
        auto &level3_field = *(level2_msg.field(level3_idx));
        string pb_type_str = ToPbTypeString(level3_field);
        if (level2_field.name() == "item_feature" &&
            level3_field.name() == "cat_id") {
          // TO make compatible. My God!
          pb_type_str = "PBTYPE_BASE_INT64";
        }
        pb_type_strs.insert(pb_type_str);
        op_type_def_str.insert(ToTypeDefString(level3_field));
        string full_path = level2_msg.name();
        full_path.append(".");
        full_path.append(level3_field.name());
        string store_data;
        string config_name;
        bool is_common;
        BuildConfigName(full_path, &config_name, &is_common);
        string common_str = "false";
        if (is_common) {
          common_str = "true";
        }
        store_data.append("  Add(\"");
        store_data.append(full_path);
        store_data.append("\", \"");
        store_data.append(config_name);
        store_data.append("\", ");
        store_data.append(pb_type_str);
        store_data.append(", ");
        store_data.append(common_str);
        store_data.append(", ");
        store_data.append(std::to_string(case_num));
        store_data.append(");");
        if (unique_store_strs.find(store_data) != unique_store_strs.end()) {
          LOG(FATAL) << "found duplicated field: " << store_data;
        }
        store_strs.push_back(store_data);
        unique_store_strs.insert(store_data);
        GenerateOutStr2(store_content_candidates, level2_field, level3_field,
                        case_num);
      }
      // LOG(INFO) << bool_candidate_str;
      // this field not defined in pb
      unique_messages.insert(level2_msg.name());
    }
  }
  string void_data_maker_cc_data =
      FileUtil::LoadContent(GetFullPath("void_data_maker.cc"));
  FileUtil::Replace(string("/**CommonBoolFileData**/"), bool_common_str,
                    &void_data_maker_cc_data);
  FileUtil::Replace(string("/**CommonFeatureFileData**/"), store_content_common,
                    &void_data_maker_cc_data);
  FileUtil::Replace(string("/**CandicateBoolFileData**/"), bool_candidate_str,
                    &void_data_maker_cc_data);
  FileUtil::Replace(string("/**CandicateFeatureFileData**/"),
                    store_content_candidates, &void_data_maker_cc_data);
  Print("void_data_maker.cc", void_data_maker_cc_data);
  return true;
}

std::string PBCodeGenerator::ToTypeString(const FieldDescriptor &field) const {
  string ret;
  auto type = field.cpp_type();
  if (type != FieldDescriptor::CPPTYPE_MESSAGE &&
      type != FieldDescriptor::CPPTYPE_ENUM &&
      type != FieldDescriptor::MAX_CPPTYPE) {
    ret = field.cpp_type_name();
    std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
    return ret;
  }
  if (type == FieldDescriptor::CPPTYPE_MESSAGE) {
    return field.message_type()->name();
  }
  if (type == FieldDescriptor::CPPTYPE_ENUM) {
    return field.enum_type()->name();
  }

  return "HAHAHA";
}

string PBCodeGenerator::ToPbTypeString(const FieldDescriptor &field) const {
  auto type = field.cpp_type();
  string ret = "PBTYPE_";
  if (field.is_map()) {
    auto *key = field.message_type()->FindFieldByName("key");
    auto *val = field.message_type()->FindFieldByName("value");
    ret.append("MAP_");
    ret.append(ToTypeString(*key));
    ret.append("_");
    ret.append(ToTypeString(*val));
  } else if (field.is_repeated()) {
    ret.append("REPEAT_");
    ret.append(ToTypeString(field));
  } else if (type != FieldDescriptor::CPPTYPE_MESSAGE &&
             type != FieldDescriptor::CPPTYPE_ENUM) {
    ret.append("BASE_");
    ret.append(field.cpp_type_name());
    std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
  } else if (type == FieldDescriptor::CPPTYPE_MESSAGE ||
             type == FieldDescriptor::CPPTYPE_ENUM) {
    ret.append("MESSAGE_");
    ret.append(field.message_type()->name());
  } else if (type == FieldDescriptor::CPPTYPE_ENUM) {
    ret.append("ENUM_");
    ret.append(field.enum_type()->name());
  } else {
    LOG(INFO) << "should never be here: " << field.name();
  }
  return ret;
}

string PBCodeGenerator::ToTypeName(const FieldDescriptor &field) const {
  auto type = field.cpp_type();
  if (field.is_map()) {
    return "";
  }

  string ret;
  if (type == FieldDescriptor::CPPTYPE_MESSAGE) {
    ret = field.message_type()->name();
  } else {
    ret = field.cpp_type_name();
  }
  ret[0] = std::toupper(ret[0]);
  return ret;
}

string PBCodeGenerator::ToTypeFullName(const FieldDescriptor &field) const {
  auto type = field.cpp_type();
  if (field.is_map()) {
    return "";
  }

  if (type == FieldDescriptor::CPPTYPE_MESSAGE ||
      type == FieldDescriptor::CPPTYPE_ENUM) {
    return string("::feature::") + field.message_type()->name();
  }
  if (type == FieldDescriptor::CPPTYPE_INT32 ||
      type == FieldDescriptor::CPPTYPE_INT64 ||
      type == FieldDescriptor::CPPTYPE_UINT32 ||
      type == FieldDescriptor::CPPTYPE_UINT64 ||
      type == FieldDescriptor::CPPTYPE_STRING) {
    return string("::google::protobuf::") + field.cpp_type_name();
  }
  return field.cpp_type_name();
}

string PBCodeGenerator::ToTypeDefString(const FieldDescriptor &field) const {
  if (field.is_map()) {
    string ret = "typedef ::google::protobuf::Map<";
    auto &key = *(field.message_type()->FindFieldByName("key"));
    auto &val = *(field.message_type()->FindFieldByName("value"));
    ret += ToTypeFullName(key);
    ret += ", ";
    ret += ToTypeFullName(val);
    ret += "> Map";
    ret += ToTypeName(key);
    ret += ToTypeName(val);
    return ret;
  } else if (field.is_repeated()) {
    string ret = "typedef ::google::protobuf::";
    auto type = field.cpp_type();
    if (type == FieldDescriptor::CPPTYPE_MESSAGE ||
        type == FieldDescriptor::CPPTYPE_STRING) {
      ret += "RepeatedPtrField";
    } else {
      ret += "RepeatedField";
    }
    ret += "<";
    ret += ToTypeFullName(field);
    ret += "> Repeated";
    ret += ToTypeName(field);
    return ret;
  }

  return "";
}

bool PBCodeGenerator::Print(const string &file_name,
                            const string &content) const {
  if (file_name.size() <= 0 || content.size() <= 0) {
    LOG(ERROR) << "Empty file or context.file_name=" << file_name;
    return false;
  }
  unique_ptr<ZeroCopyOutputStream> output(generator_context_->Open(file_name));
  Printer printer(output.get(), '$', nullptr);
  printer.Print(content.c_str());
  return true;
}

string PBCodeGenerator::GetFullPath(const string &file_name) const {
  return template_dir_ + "/" + file_name;
}

} // namespace demo

int main(int argc, char *argv[]) {
  string template_dir = argv[0];
  template_dir += ".runfiles";
  vector<string> all_files;
  if (!FileUtil::ListDirRecursive(template_dir, &all_files)) {
    LOG(ERROR) << "Fail to list directory of " << template_dir;
    return -1;
  }
  string real_full_path;

  for (auto &path : all_files) {
    string truncated_path = path.substr(template_dir.size() + 1);
    auto pos = truncated_path.find("/");
    if (pos == string::npos) {
      continue;
    }
    string dir_name = truncated_path.substr(0, pos);
    string x_path = truncated_path.substr(pos + 1);
    string prefix = "src/template";
    if (x_path.compare(0, prefix.size(), prefix) == 0) {
      real_full_path = template_dir + "/" + dir_name + "/" + prefix;
    }
  }
  if (real_full_path.size() <= 0) {
    LOG(ERROR) << "Miss path under " << template_dir;
    return -1;
  }
  LOG(INFO) << "Template diectory=" << real_full_path;
  demo::PBCodeGenerator generator(real_full_path);
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
