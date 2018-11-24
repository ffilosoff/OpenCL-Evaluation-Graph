#include "Node.h"
#include <iostream>
#include <algorithm>

int main()
{
	// construct graph
	cl_graph::Node data1 = cl_graph::Data({ 1.0f, 1.0f, 1.0f, 1.0f }, { 2, 2 });
	cl_graph::Node data2 = cl_graph::Data({ 1.0f, 1.0f, 3.0f, 1.0f }, { 2, 2 });
	auto devices = cl_graph::Device::get_all_devices();
	const auto it = std::find_if(devices.begin(), devices.end(), [] (const auto & d) { return d.get_type() == cl_graph::Device::NOT_CL_CPU; });
	if (it == devices.end()) {
		std::cerr << "No needed device" << std::endl;
		return -1;
	}
	cl_graph::Node addNode = cl_graph::Node::add_node(data1, data2, *it);

	// get result
	cl_graph::Data result = addNode.evaluate();
	std::vector<float> resultVector;
	std::vector<size_t> resultShape;
	result.download(resultVector, resultShape);

	// output result
	size_t rows = resultShape[0];
	size_t cols = resultShape[1];
	std::cout << "result matrix:" << std::endl;
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < rows; ++col)	{
			std::cout << resultVector[row * cols + col] << " ";
		}
		std::cout << std::endl;
	}
}