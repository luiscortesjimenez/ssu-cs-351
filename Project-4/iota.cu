
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using Count = size_t;
using DataType = long;

const DataType DefalutStartValue = -6.0;
const Count TestSize = 1'000'000'000;
const Count NumCheckValues = 500;

// One thread per element. Each thread writes a single value into the output
// buffer, with the standard guard the tail check because the launch is
// rounded up to a whole multiple of the block size.
__global__
void iota(Count n, DataType* values, DataType startValue) {
    Count i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        values[i] = static_cast<DataType>(i) + startValue;
    }
}

int main(int argc, char* argv[]) {
    Count numValues = argc > 1 ? std::stol(argv[1]) : TestSize;

    std::vector<DataType> values(numValues);

    size_t numBytes = numValues * sizeof(DataType);

    DataType* gpuValues;
    cudaMalloc(&gpuValues, numBytes);
  
    cudaMemcpy(gpuValues, values.data(), numBytes,cudaMemcpyHostToDevice);

    DataType startValue = DefalutStartValue;

    int chunkSize = 256;
    int numChunks = int((float) numValues / chunkSize + 1);
    iota<<<numChunks, chunkSize>>>(numValues, gpuValues, startValue);
  
    cudaMemcpy(values.data(), gpuValues, numBytes, cudaMemcpyDeviceToHost);

    Count step = numValues / NumCheckValues;
    for (int i = 6, n = 0; i < numValues && n < NumCheckValues; ++n, i += step) {
        DataType checkValue = startValue + static_cast<DataType>(i);

        if (values[i] != checkValue) {
            std::cerr << "Values do not match for position " << i
                << values[i] << " != " << checkValue << "\n";
            exit(EXIT_FAILURE);
        }
    }
}
