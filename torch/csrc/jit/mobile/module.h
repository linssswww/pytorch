#pragma once
//#include <ATen/core/function_schema.h>
#include <torch/csrc/jit/mobile/function.h>

namespace torch {
namespace jit {
namespace mobile {
using Stack = std::vector<c10::IValue>;

class CompilationUnit {
 public:
  void register_function(std::unique_ptr<Function> fn);
  std::vector<std::unique_ptr<Function>>& methods() {
    return methods_;
  }
  Function* find_function(const c10::QualifiedName& qn);

 private:
  std::vector<std::unique_ptr<Function>> methods_;
};

class TORCH_API Module {
 public:
  Module(
      c10::intrusive_ptr<c10::ivalue::Object> object,
      std::shared_ptr<CompilationUnit> cu)
      : object_(object), cu_(std::move(cu)){};
  Module() {}
  c10::IValue run_method(const std::string& method_name, Stack stack);
  c10::IValue forward(std::vector<c10::IValue> inputs) {
    return run_method("forward", std::move(inputs));
  }
  Function* find_method(const std::string& basename) const;
  std::string name() {
    return object_->name();
  }
  const std::vector<at::IValue>& slots() const {
    return object_->slots();
  }
  const std::vector<at::Tensor> parameters() const;
  const std::map<std::string, at::Tensor> named_parameters() const;
  void save_data(std::ostream& out) const;
  void save_data(const std::string& filename) const;

 private:
  c10::intrusive_ptr<c10::ivalue::Object> object_;
  std::shared_ptr<CompilationUnit> cu_;
};
} // namespace mobile
} // namespace jit
} // namespace torch
