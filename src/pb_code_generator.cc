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

using protobuf_plugin_demo::FileUtil;
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

namespace protobuf_plugin_demo {

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

    void PBCodeGenerator::GenerateOutStr2(string &store_content,
        const FieldDescriptor &level2_field,
        const FieldDescriptor &level3_field,
        int &case_num) const {
        if (level3_field.is_map() || level3_field.is_repeated()) {
            store_content.append(string("\n    "));
            store_content.append(string("case ") + absl::AlphaNum(case_num).data() +
                ":\n");

            store_content.append(string("    "));
            store_content.append(string("(*void_data)[i] = ") + "(const void*)(");
            store_content.append(string("likely(has_") + level2_field.name() +
                ") ? ");
            store_content.append("(&(foo_fields." + level2_field.name() + "()." +
                level3_field.name() + "()))" + " : NULL" + ");\n");
            store_content.append(string("    ") + "break;\n");
        } else if (level3_field.cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
            store_content.append(string("\n    "));
            store_content.append(string("case ") + absl::AlphaNum(case_num).data() +
                ":\n");
            store_content.append(string("    ") + "(*void_data)[i] = ");
            store_content.append("(const void*)(");
            store_content.append(string("likely(has_") + level2_field.name() + ") ? ");
            store_content.append("(foo_fields." + level2_field.name() + "().");
            store_content.append("has_" + level3_field.name() + "() ? ");
            store_content.append("&foo_fields." + level2_field.name() + "().");
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
            store_content.append("(foo_fields." + level2_field.name() + "().");
            store_content.append(level3_field.name() + "_oneof_case() ? ");
            store_content.append("&foo_fields." + level2_field.name() + "().");
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
            store_content.append("(foo_fields." + level2_field.name() + "().");
            store_content.append(string(level3_field.name()) + "_oneof_case() ? ");
            store_content.append("&(*memory_holder->GetFloat() = foo_fields.");
            store_content.append(string(level2_field.name()) + "().");
            store_content.append(string(level3_field.name()));
            store_content.append(string("()) : NULL)") + " : NULL" + ");\n");
            store_content.append(string("    ") + "break;\n");
        }
        else {
            store_content.append(string("\n    "));
            store_content.append("case ");
            store_content.append(string(absl::AlphaNum(case_num).data()) + ":\n");
            store_content.append(string("    ") + "(*void_data)[i] = ");
            store_content.append("(const void*)(");
            store_content.append(string("likely(has_") + level2_field.name() +
                ") ? ");
            store_content.append(string("(foo_fields.") + level2_field.name() +
                "().");
            store_content.append(string(level3_field.name()) + "_oneof_case() ? ");
            store_content.append(
                string("&(*memory_holder->GetInt64() = foo_fields."));
            store_content.append(string(level2_field.name()) + "().");
            store_content.append(string(level3_field.name()) + "()) : NULL)");
            store_content.append(string(" : NULL") + ");\n");
            store_content.append(string("    ") + "break;\n");
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
                bool_candidate_str.append("foo_fields.has_");
                bool_candidate_str.append(level2_field.name());
                bool_candidate_str.append("() ? true : false;\n");
                for (int level3_idx = 0; level3_idx < level2_msg.field_count();
                    ++level3_idx) {
                    auto &level3_field = *(level2_msg.field(level3_idx));
                    string pb_type_str = ToPbTypeString(level3_field);
                    pb_type_strs.insert(pb_type_str);
                    op_type_def_str.insert(ToTypeDefString(level3_field));
                    string full_path = level2_msg.name();
                    full_path.append(".");
                    full_path.append(level3_field.name());
                    LOG(INFO) << full_path;
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

} // namespace protobuf_plugin_demo

int main(int argc, char *argv[]) {
    string template_base_dir = FgFileUtil::GetRealPath(argv[0]);
    template_base_dir = FgFileUtil::TruncatePath(template_base_dir, "bazel-out");
    LOG(ERROR) << template_base_dir;
    fg::feature::PBFeatureCodeGenerator generator(template_base_dir);
    protobuf_plugin_demo::PBCodeGenerator generator(real_full_path);
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
