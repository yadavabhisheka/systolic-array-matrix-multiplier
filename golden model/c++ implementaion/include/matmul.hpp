#pragma once
#include<cstdint>
#include "matrix.hpp"
//getting the matrix multiplication statstics
struct MatmulStats{
    std::uint64_t multiplications;
    std::uint64_t additions;
    std::uint64_t macs;
    std::uint64_t primitive_ops;
};
//creating a Matrix multiplication function declaration
Matrix<std::int32_t> matmul(
    const Matrix<std::int8_t>& A, 
    const Matrix<std::int8_t>& W,
    MatmulStats* stats=nullptr
);
