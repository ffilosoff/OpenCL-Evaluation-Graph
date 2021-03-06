#pragma once

#include "cl_graph/Definitions.h"
#include <cstddef>
#include <vector>

#include "cl_graph/Device.h"
#include "helpers/opencl.h"

namespace cl_graph {

struct ClMem{
	ClMem() = default;
	ClMem(cl_mem _mem, cl_event _event) : mem(_mem), event(_event) {}

	cl_device_id device = nullptr;
	cl_command_queue queue = nullptr;
	cl_mem mem = nullptr;
	cl_event event = nullptr;
};

class DataImpl
{
public:
    DataImpl() = default;
    DataImpl(ClMem clmem) : m_cl_mem(clmem) {};
	DataImpl(const DataImpl &) = delete;

	bool download(std::vector<float> & data, std::vector<size_t> & shape) const;
	bool upload(std::vector<float> data, std::vector<size_t> shape = {});

	const std::vector<float> & get_data() const { return m_data; }
	const std::vector<size_t> & get_shape() const { return m_shape; }

	const bool empty() { return m_data.empty(); }

	void resize(size_t sz);
	bool set_shape(std::vector<size_t> shape);
	void squeeze(); // removes one's from the end

	ClMem get_cl_data(const Device & device, size_t broadcast_size = 0);

	void print(std::ostream & strm) const;

private:
	ClMem m_cl_mem;
    std::vector<float> m_data;
    std::vector<size_t> m_shape;
};

}