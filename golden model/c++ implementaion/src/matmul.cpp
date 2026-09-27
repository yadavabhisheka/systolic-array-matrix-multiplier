#include "matmul.hpp"
#include "matrix.hpp"
#include<stdexcept>
//defination of the funtion
Matrix<std::int32_t> matmul(
    const Matrix<std::int8_t>& A, 
    const Matrix<std::int8_t>& W,
    MatmulStats* stats
){ 
    //making three variables out of the matrix order
    std::size_t M=A.rows();
    std::size_t K=A.cols();
    std::size_t N=W.cols();
    //checking is matrix multiplication actually possible
    if(A.cols()!=W.rows()){
        throw std::invalid_argument("For two matrix to multiplied the number of column in first Matrix should equal to the number of row of second.");
    }
    //checking is M,K,N is not out range of our program which calculate matrix from order 1x1 to 16x16
    if(M>16 || K>16 || N>16){
        throw std::out_of_range("For our application we allow the Matrix mulitplication from 1x1 to 16x16 only.");
    }
    
    Matrix<std::int32_t> C(M,N);//declaring a matrix C the output of multiplication
    //looping thorugh the A Matrix rows
    for(std::size_t i=0;i<M;i++){
        //looping through the W Matrix column
        for(std::size_t j=0;j<N;j++){
            std::int32_t accumulator=0;//declaring a accumulator for addition of product values
            //looping through the A Matrix column
            for(std::size_t k=0;k<K;k++){
                //static cast and get the product
                std::int16_t product=static_cast<std::int16_t>(A(i,k))*static_cast<std::int16_t>(W(k,j));
                //accumulating the product and casting that too
                accumulator+=static_cast<std::int32_t>(product);
            }
            //assigning value in the C matrix
            C(i,j)=accumulator;
        }
    }
    //calculating the total operations happening 
    if (stats != nullptr) {
    stats->multiplications = M * K * N;//total multiplication happening
    stats->additions = M * N * (K - 1);//total addition happening 
    stats->macs = M * K * N;//total macs required
    stats->primitive_ops = stats->multiplications + stats->additions; //total operations
    }
    
    return C;
}