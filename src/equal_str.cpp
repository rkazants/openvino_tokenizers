// Copyright (C) 2018-2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "equal_str.hpp"
#include "utils.hpp"

using namespace ov;


void EqualStr::validate_and_infer_types() {
    OPENVINO_ASSERT(get_input_size() == 6);

    auto begins_type1 = this->get_input_element_type(0);
    auto ends_type1 = this->get_input_element_type(1);
    auto begins_type2 = this->get_input_element_type(3);
    auto ends_type2 = this->get_input_element_type(4);

    OPENVINO_ASSERT(begins_type1 == element::i32 && begins_type2 == element::i32,
        "Expected an i32 begins for string tensor representation.");
    OPENVINO_ASSERT(ends_type1 == element::i32 && ends_type2 == element::i32,
        "Expected an i32 ends for string tensor representation.");

    set_output_type(0, ov::element::boolean, PartialShape({ Dimension::dynamic() }));
}

bool EqualStr::evaluate(ov::TensorVector& outputs, const ov::TensorVector& inputs) const {
    auto begins1 = inputs[0].data<const int32_t>();
    auto ends1 = inputs[1].data<const int32_t>();
    auto chars1 = inputs[2].data<const uint8_t>();
    auto begins2 = inputs[3].data<const int32_t>();
    auto ends2 = inputs[4].data<const int32_t>();
    auto chars2 = inputs[5].data<const uint8_t>();

    size_t num_elems1 = inputs[0].get_size();
    size_t num_elems2 = inputs[3].get_size();
    size_t num_elems = std::max(num_elems1, num_elems2);
    outputs[0].set_shape(ov::Shape{ num_elems });
    auto result = outputs[0].data<bool>();

    for (size_t idx = 0; idx < num_elems; ++idx) {
        // handle indices due to broadcasting case
        size_t idx1 = (idx < num_elems1) ? idx : 0;
        size_t idx2 = (idx < num_elems2) ? idx : 0;
        
        std::vector<uint8_t> op1(chars1 + begins1[idx1], chars1 + ends1[idx1]);
        std::vector<uint8_t> op2(chars2 + begins2[idx2], chars2 + ends2[idx2]);
        if (op1 == op2) {
            result[idx] = true;
        }
        else {
            result[idx] = false;
        }
    }
    return true;
}