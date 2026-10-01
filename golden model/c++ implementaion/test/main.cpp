#include "matrix.hpp"
#include "matmul.hpp"
#include <iostream>
//function to print all the stats at once
void printStats(const MatmulStats& stats) {
    std::cout << "  multiplications: " << stats.multiplications << "\n";
    std::cout << "  additions:       " << stats.additions << "\n";
    std::cout << "  macs:            " << stats.macs << "\n";
    std::cout << "  primitive_ops:   " << stats.primitive_ops << "\n";
}

int main(){
    //matrix A
    Matrix<std::int8_t> A(2,2);
    A(0,0)=1; A(0,1)=2;
    A(1,0)=3; A(1,1)=4;
    //matrix B
    Matrix<std::int8_t> W(2,2);
    W(0,0)=5; W(0,1)=6;
    W(1,0)=7; W(1,1)=8;
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "C(0,0) = " << C(0,0) << " (expected 19)\n";
    std::cout << "C(0,1) = " << C(0,1) << " (expected 22)\n";
    std::cout << "C(1,0) = " << C(1,0) << " (expected 43)\n";
    std::cout << "C(1,1) = " << C(1,1) << " (expected 50)\n";
    
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}