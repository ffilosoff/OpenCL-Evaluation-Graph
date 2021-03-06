//
// Created by pav on 2019-02-17.
//

#include "NodeTwoOp.h"

#include "DataImpl.h"
#include "DeviceImpl.h"

#include <assert.h>

namespace cl_graph {

NodeTwoOp::NodeTwoOp(Node left, Node right, const Device & device)
    : m_left(std::move(left)),
      m_right(std::move(right)),
      m_device(device)
{

}

Data NodeTwoOp::perform_opencl_operation(const std::pair<const char *, const char *> & op) {
    Data left_data = m_left.evaluate();
    Data right_data = m_right.evaluate();

    if (left_data.empty() || right_data.empty()) {
        return {};
    }
    const auto & data_left = left_data.get_impl()->get_data();
    const auto & data_right = right_data.get_impl()->get_data();
    const auto & left_shape = left_data.get_impl()->get_shape();
    const auto & right_shape = right_data.get_impl()->get_shape();
    assert((data_left.size() == data_right.size() || data_left.size() == 1 || data_right.size() == 1) && "matrixes are not of equal size and not a scalar");
    assert(((left_shape == right_shape) || (left_shape.size() == 1 && left_shape[0] == 1) || (right_shape.size() == 1 && right_shape[0] == 1)) && "shapes are missmatch and not a one is  scalar");


    auto kernel = m_device.get_impl()->get_kernel(op);
    Data result;
    result.get_impl()->resize(data_left.size());
    auto res_cl = result.get_impl()->get_cl_data(m_device);
    auto a = left_data.get_impl()->get_cl_data(m_device, data_right.size());
    auto b = right_data.get_impl()->get_cl_data(m_device, data_left.size());
    cl_int err;
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&res_cl.mem);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&a.mem);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&b.mem);
    size_t local_work_size[2], global_work_size[2];
    local_work_size[0] = 1;
    local_work_size[1] = 1;
    global_work_size[0] = left_data.get_impl()->get_data().size(); //m_device.get_impl()->get_max_global_work_size();
    global_work_size[1] = 1;//m_device.get_impl()->get_max_global_work_size();
    err |= clEnqueueNDRangeKernel(
        m_device.get_impl()->get_queue(),
        kernel,
        1,
        nullptr,
        global_work_size,
        local_work_size,
        0,
        nullptr,
        nullptr);
    if (err == CL_SUCCESS) {
        result.get_impl()->set_shape(left_data.get_impl()->get_shape());
        return result;
    }
    return {};
}

}
