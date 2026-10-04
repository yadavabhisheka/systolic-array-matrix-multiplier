# Non-Square Matrix Multiplication Test

## Test

Tested the C++ Golden Model with non-square matrices.

### Matrix Dimensions

```text
A = 2 × 3
W = 3 × 4
C = 2 × 4

The multiplication is:

A(2 × 3) × W(3 × 4) = C(2 × 4)

The inner dimensions are equal:

3 = 3

Therefore, the matrix multiplication is valid.


Core Logic

The matrix multiplication follows the standard row-by-column multiplication method.

For each row i of A
    For each column j of W
        accumulator = 0

        For k = 0 to K-1
            accumulator = accumulator + A(i,k) × W(k,j)

        C(i,j) = accumulator

For this test:

M = 2
K = 3
N = 4

Therefore:

A(M × K) × W(K × N) = C(M × N)

A(2 × 3) × W(3 × 4) = C(2 × 4)
Example Calculation

For the first element of the output matrix:

C(0,0) = A(0,0) × W(0,0)
       + A(0,1) × W(1,0)
       + A(0,2) × W(2,0)

       = 1 × 7 + 2 × 11 + 3 × 15

       = 7 + 22 + 45

       = 74

The same row-by-column operation is performed for every element of the output matrix.

Test Code

#include "matrix.hpp"
#include "matmul.hpp"
#include <iostream>

void printMatrix(const Matrix<std::int32_t>& C) {
    for (std::size_t i = 0; i < C.rows(); i++) {
        for (std::size_t j = 0; j < C.cols(); j++) {
            std::cout << C(i, j) << "\t";
        }
        std::cout << "\n";
    }
}

void printStats(const MatmulStats& stats) {
    std::cout << "\nmultiplications: " << stats.multiplications << "\n";
    std::cout << "additions:       " << stats.additions << "\n";
    std::cout << "macs:            " << stats.macs << "\n";
    std::cout << "primitive_ops:   " << stats.primitive_ops << "\n";
}

int main() {
    Matrix<std::int8_t> A(2, 3);

    A(0,0) = 1;
    A(0,1) = 2;
    A(0,2) = 3;

    A(1,0) = 4;
    A(1,1) = 5;
    A(1,2) = 6;

    Matrix<std::int8_t> W(3, 4);

    W(0,0) = 7;
    W(0,1) = 8;
    W(0,2) = 9;
    W(0,3) = 10;

    W(1,0) = 11;
    W(1,1) = 12;
    W(1,2) = 13;
    W(1,3) = 14;

    W(2,0) = 15;
    W(2,1) = 16;
    W(2,2) = 17;
    W(2,3) = 18;

    MatmulStats stats{};

    Matrix<std::int32_t> C = matmul(A, W, &stats);

    std::cout << "Result Matrix C (2 x 4):\n";
    printMatrix(C);

    printStats(stats);

    return 0;
}

Input Matrix A
A =
[ 1  2  3
  4  5  6 ]

Input Matrix W
W =
[  7   8   9  10
  11  12  13  14
  15  16  17  18 ]

  Output

  Result Matrix C (2 x 4):
74    80    86    92
173   188   203   218

multiplications: 24
additions:       16
macs:            24
primitive_ops:   40

Operation Statistics

For:

M = 2
K = 3
N = 4

The expected operation counts are:

Multiplications = M × K × N
                 = 2 × 3 × 4
                 = 24

Additions = M × N × (K - 1)
          = 2 × 4 × 2
          = 16

MACs = M × K × N
     = 24

Primitive Operations = Multiplications + Additions
                     = 24 + 16
                     = 40

The observed values from the C++ Golden Model match these expected values.

Result

The C++ Golden Model successfully performed the non-square matrix multiplication:

(2 × 3) × (3 × 4) = (2 × 4)

The calculated output matrix was:

[  74   80   86   92
  173  188  203  218 ]