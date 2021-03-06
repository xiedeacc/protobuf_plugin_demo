#ifndef PB_CODE_GENERATOR_H_
#define PB_CODE_GENERATOR_H_
#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/io/printer.h"
#include <set>
#include <string>
#include <vector>
namespace protobuf_plugin_demo {

class PBCodeGenerator : public google::protobuf::compiler::CodeGenerator {
public:
  explicit PBCodeGenerator(const std::string &template_dir_);

public:
  bool Generate(const google::protobuf::FileDescriptor *file,
                const std::string &parameter,
                google::protobuf::compiler::GeneratorContext *generator_context,
                std::string *error) const;

private:
  bool GeneratePBFeatureData() const;

  bool GenerateVoidDataCodeH() const;

  std::string
  ToPbTypeString(const google::protobuf::FieldDescriptor &field) const;
  std::string
  ToTypeString(const google::protobuf::FieldDescriptor &field) const;
  std::string
  ToTypeDefString(const google::protobuf::FieldDescriptor &field) const;
  std::string ToTypeName(const google::protobuf::FieldDescriptor &field) const;
  std::string
  ToTypeFullName(const google::protobuf::FieldDescriptor &field) const;

  void GenerateOutStr2(std::string &store_content, // NOLINT
                       const google::protobuf::FieldDescriptor &level2_field,
                       const google::protobuf::FieldDescriptor &level3_field,
                       int &case_num) const; // NOLINT
private:
  bool Print(const std::string &file_name, const std::string &content) const;
  std::string GetFullPath(const std::string &file_name) const;

private:
  const mutable google::protobuf::FileDescriptor *file_;
  mutable google::protobuf::compiler::GeneratorContext *generator_context_;
  std::string template_dir_;
};

} // namespace protobuf_plugin_demo

#endif /* PB_CODE_GENERATOR_H_ */
