#pragma once

#include <map>

#include "NvInfer.h"
#include "core/conversion/conversionctx/ConversionCtx.h"
#include "torch/csrc/jit/ir/ir.h"

namespace trtorch {
namespace core {
namespace conversion {

struct InputRange {
  nvinfer1::Dims min;
  nvinfer1::Dims max;
  nvinfer1::Dims opt;
  nvinfer1::Dims input_shape;
  bool input_is_dynamic = false;
  // Should we restrict to unsigned?
  InputRange(std::vector<int64_t> d);
  InputRange(std::vector<int64_t> min_shape, std::vector<int64_t> opt_shape, std::vector<int64_t> max_shape);
};

struct ConversionInfo {
  std::vector<InputRange> input_ranges;
  BuilderSettings engine_settings;
  ConversionInfo(std::vector<InputRange> input_ranges)
      : input_ranges(std::move(input_ranges)), engine_settings(BuilderSettings()) {}
};

// TODO: REMOVE GRAPH AND PARAMS AND MOVE FULLY TO INLINED CONSTANTS

using GraphParams = std::map<torch::jit::Value*, torch::jit::IValue>;

GraphParams get_named_params(c10::ArrayRef<torch::jit::Value*> inputs, std::vector<torch::jit::IValue> params);

// Converts a already lowered block (blocks with no sub blocks) to
// a serialized TensorRT engine that can be deserialized and run
std::string ConvertBlockToEngine(const torch::jit::Block* b, ConversionInfo build_info, GraphParams& static_params);

bool OpSupported(const torch::jit::Node* n);

bool VerifyConverterSupportForBlock(const torch::jit::Block* b);

c10::optional<torch::jit::IValue> EvaluateNode(
    ConversionCtx* ctx,
    const torch::jit::Node* n,
    int level = 0,
    int limit = 10);

} // namespace conversion
} // namespace core
} // namespace trtorch
