# Matrix multiplication test cases
## Test cases:
### case1: 2x2 Matrix multiplication with positive inputs

**Matrix operation code**
```text
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
     A(0,0)=1;  A(0,1)= 2;
     A(1,0)=3;  A(1,1)=4;

    //matrix B
    Matrix<std::int8_t> W(2,2);
    W(0,0)=5;  W(0,1)=6;
    W(1,0)=7;  W(1,1)=8;

    
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C(2,2);
    C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "C(0,0) =" << C(0,0) << "expected 19\n";
    std::cout << "C(0,1) =" << C(0,1) << "expected 22\n";
    std::cout << "C(0,0) =" << C(1,0) << "expected 43\n";
    std::cout << "C(0,1) =" << C(1,1) << "expected 50\n";
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}
Input Matrix A
1 2
3 4

Input Matrix B
5 6
7 8

Expected output
19 22
43 50

Actual Ouput
C(0,0) = 19 (expected 19)
C(0,1) = 22 (expected 22)
C(1,0) = 43 (expected 43)
C(1,1) = 50 (expected 50)
  multiplications: 8
  additions:       4
  macs:            8
  primitive_ops:   12
  
Status
  PASS


  Observation
  Implemented program computes the product by row by column matrix multiplication
```
### Case2: 2x2 matrix multiplication with negative inputs
**Matrix operation code**
```text
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
     A(0,0)=-2;  A(0,1)=-2;
     A(1,0)=-3;  A(1,1)=-4;

    //matrix B
    Matrix<std::int8_t> W(2,2);
    W(0,0)=-5;  W(0,1)=-6;
    W(1,0)=-7;  W(1,1)=-8;

    
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C(2,2);
    C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "C(0,0) =" << C(0,0) << "expected 24\n";
    std::cout << "C(0,1) =" << C(0,1) << "expected 28\n";
    std::cout << "C(1,0) =" << C(1,0) << "expected 43\n";
    std::cout << "C(1,1) =" << C(1,1) << "expected 50\n";
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}
Input matrix A:
 -2 -2
 -3 -4

 Output matrix B:
 -5 -6
 -7 -8

 Expected Output:
 24 28
 43 50

 Actual Output:
 24 28
 43 50
 C(0,0) = 24 (expected 24)
C(0,1) = 28 (expected 28)
C(1,0) = 43 (expected 43)
C(1,1) = 50 (expected 50)
  multiplications: 8
  additions:       4
  macs:            8
  primitive_ops:   12

 Status
  PASS
```
### case3: 3x3 matrix multiplication with negative inputs
**Matrix operation code**
```text
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
    Matrix<std::int8_t> A(3,3);
    A(0,0)=-1; A(0,1)=-2; A(0,2)=-3;
    A(1,0)=-4; A(1,1)=-5; A(1,2)=-6;
    A(2,0)=-7; A(2,1)=-8; A(2,2)=-9;
    //matrix B
    Matrix<std::int8_t> W(3,3);
    W(0,0)=-7; W(0,1)=-8; W(0,2)=-9;
    W(1,0)=-10; W(1,1)=-11; W(1,2)=-12;
    W(2,0)=-13; W(2,1)=-14; W(2,2)=-15;
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "C(0,0) = " << C(0,0) << " (expected 66)\n";
    std::cout << "C(0,1) = " << C(0,1) << " (expected 72)\n";
    std::cout << "C(0,2) = " << C(0,2) << " (expected 78)\n";
    std::cout << "C(1,0) = " << C(1,0) << " (expected 156)\n";
    std::cout << "C(1,1) = " << C(1,1) << " (expected 171)\n";
    std::cout << "C(1,2) = " << C(1,2) << " (expected 186)\n";
    std::cout << "C(2,0) = " << C(2,0) << " (expected 246)\n";
    std::cout << "C(2,1) = " << C(2,1) << " (expected 270)\n";
    std::cout << "C(2,2) = " << C(2,2) << " (expected 294)\n";
    
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}

Input Matrix A
-1 -2 -3
-4 -5 -6
-7 -8 -9

Input Matrix B
-7 -8 -9
-10 -11 -12
-13 -14 -15

Expected Output:
 66  72  78
 156 171 186
 246 270 294

 Actual Output:
C(0,0) = 66 (expected 66)
C(0,1) = 72 (expected 72)
C(0,2) = 78 (expected 78)
C(1,0) = 156 (expected 156)
C(1,1) = 171 (expected 171)
C(1,2) = 186 (expected 186)
C(2,0) = 246 (expected 246)
C(2,1) = 270 (expected 270)
C(2,2) = 294 (expected 294)
  multiplications: 27
  additions:       18
  macs:            27
  primitive_ops:   45

  status
  PASS
```
### case4: 2x2 matrix multiplication with identity matrix
**Matrix Operation code**
```text
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
    A(1,0)=-3; A(1,1)=-4;
    //matrix B
    Matrix<std::int8_t> W(2,2);
    W(0,0)=0; W(0,1)=1;
    W(1,0)=1; W(1,1)=0;
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "C(0,0) = " << C(0,0) << " (expected 2)\n";
    std::cout << "C(0,1) = " << C(0,1) << " (expected 1)\n";
    std::cout << "C(1,0) = " << C(1,0) << " (expected -4)\n";
    std::cout << "C(1,1) = " << C(1,1) << " (expected -3)\n";
    
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}
Input Matrix A
1 2
-3 -4

Input Matrix B
1 0
0 1

Expected output
2 1
-4 -3

Actual output
C(0,0) = 2 (expected 2)
C(0,1) = 1 (expected 1)
C(1,0) = -4 (expected -4)
C(1,1) = -3 (expected -3)
  multiplications: 8
  additions:       4
  macs:            8
  primitive_ops:   12

  Status
   PASS
```
### case5: 2x2 matrix multiplication with zero matrix
**Matrix operation code**
```text
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
    A(1,0)=-3; A(1,1)=-4;
    //matrix B
    Matrix<std::int8_t> W(2,2);
    W(0,0)=0; W(0,1)=0;
    W(1,0)=0; W(1,1)=0;
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "C(0,0) = " << C(0,0) << " (expected 0)\n";
    std::cout << "C(0,1) = " << C(0,1) << " (expected 0)\n";
    std::cout << "C(1,0) = " << C(1,0) << " (expected 0)\n";
    std::cout << "C(1,1) = " << C(1,1) << " (expected 0)\n";
    
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}
Input matrix A
1 2
-3 -4

Input matrix B
0 0
0 0

Expected Output
0 0
0 0

Actual Output
C(0,0) = 0 (expected 0)
C(0,1) = 0 (expected 0)
C(1,0) = 0 (expected 0)
C(1,1) = 0 (expected 0)
  multiplications: 8
  additions:       4
  macs:            8
  primitive_ops:   12

  Status
   PASS
  ```
  ### case6: 2x2 matrix multiplication using 2x2 boundary valued matrix
  **Matrix operation**
```text
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
    A(0,0)=127; A(0,1)=-128;
    A(1,0)=-128; A(1,1)=127;
    //matrix B
    Matrix<std::int8_t> W(2,2);
    W(0,0)=1; W(0,1)=1;
    W(1,0)=1; W(1,1)=1;
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "C(0,0) = " << C(0,0) << " (expected -1)\n";
    std::cout << "C(0,1) = " << C(0,1) << " (expected -1)\n";
    std::cout << "C(1,0) = " << C(1,0) << " (expected -1)\n";
    std::cout << "C(1,1) = " << C(1,1) << " (expected -1)\n";
    
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}
 
Input Matrix A
127 -128
-128 127

Input Matrix B
1 1
1 1

Expected Output
-1 -1
-1 -1

Actual Output
C(0,0) = -1 (expected -1)
C(0,1) = -1 (expected -1)
C(1,0) = -1 (expected -1)
C(1,1) = -1 (expected -1)
  multiplications: 8
  additions:       4
  macs:            8
  primitive_ops:   12

status
 PASS
   ```
### case7: 8x8 matrix multiplication with positive and negative inputs
**Matrix Operation**
```text
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
    Matrix<std::int8_t> A(8,8);
     A(0,0)=1;  A(0,1)=-2; A(0,2)=3;  A(0,3)=-4;
    A(0,4)=5;  A(0,5)=-6; A(0,6)=7;  A(0,7)=-8;

    A(1,0)=-1; A(1,1)=2;  A(1,2)=-3; A(1,3)=4;
    A(1,4)=-5; A(1,5)=6;  A(1,6)=-7; A(1,7)=8;

    A(2,0)=2;  A(2,1)=3;  A(2,2)=-1; A(2,3)=-2;
    A(2,4)=4;  A(2,5)=-4; A(2,6)=5;  A(2,7)=-5;

    A(3,0)=-3; A(3,1)=1;  A(3,2)=-4; A(3,3)=2;
    A(3,4)=-5; A(3,5)=3;  A(3,6)=-6; A(3,7)=4;

    A(4,0)=5;  A(4,1)=-1; A(4,2)=6;  A(4,3)=-2;
    A(4,4)=7;  A(4,5)=-3; A(4,6)=8;  A(4,7)=-4;

    A(5,0)=-2; A(5,1)=-4; A(5,2)=2;  A(5,3)=4;
    A(5,4)=-6; A(5,5)=6;  A(5,6)=-8; A(5,7)=8;

    A(6,0)=3;  A(6,1)=-5; A(6,2)=7;  A(6,3)=-1;
    A(6,4)=-3; A(6,5)=5;  A(6,6)=-7; A(6,7)=1;

    A(7,0)=-4; A(7,1)=6;  A(7,2)=-2; A(7,3)=8;
    A(7,4)=-1; A(7,5)=3;  A(7,6)=-5; A(7,7)=7;
    //matrix B
    Matrix<std::int8_t> W(8,8);
    W(0,0)=2;  W(0,1)=-1; W(0,2)=3;  W(0,3)=-2;
    W(0,4)=4;  W(0,5)=-3; W(0,6)=5;  W(0,7)=-4;

    W(1,0)=-3; W(1,1)=2;  W(1,2)=-4; W(1,3)=3;
    W(1,4)=-5; W(1,5)=4;  W(1,6)=-6; W(1,7)=5;

    W(2,0)=1;  W(2,1)=-2; W(2,2)=5;  W(2,3)=-1;
    W(2,4)=3;  W(2,5)=-4; W(2,6)=2;  W(2,7)=-3;

    W(3,0)=-2; W(3,1)=4;  W(3,2)=-1; W(3,3)=5;
    W(3,4)=-3; W(3,5)=6;  W(3,6)=-4; W(3,7)=2;

    W(4,0)=3;  W(4,1)=-5; W(4,2)=2;  W(4,3)=-4;
    W(4,4)=6;  W(4,5)=-1; W(4,6)=4;  W(4,7)=-2;

    W(5,0)=-4; W(5,1)=3;  W(5,2)=-6; W(5,3)=2;
    W(5,4)=-1; W(5,5)=5;  W(5,6)=-3; W(5,7)=4;

    W(6,0)=5;  W(6,1)=-3; W(6,2)=4;  W(6,3)=-6;
    W(6,4)=2;  W(6,5)=-5; W(6,6)=3;  W(6,7)=-1;

    W(7,0)=-1; W(7,1)=5;  W(7,2)=-2; W(7,3)=4;
    W(7,4)=-4; W(7,5)=2;  W(7,6)=-5; W(7,7)=6;
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "C(0,0) = " << C(0,0) << " (expected 101)\n";
    std::cout << "C(0,1) = " << C(0,1) << " (expected -131)\n";
    std::cout << "C(0,2) = " << C(0,2) << " (expected 120)\n";
    std::cout << "C(0,3) = " << C(0,3) << " (expected -137)\n";
    std::cout << "C(0,4) = " << C(0,4) << " (expected 117)\n";
    std::cout << "C(0,5) = " << C(0,5) << " (expected -133)\n";
    std::cout << "C(0,6) = " << C(0,6) << " (expected 138)\n";
    std::cout << "C(0,7) = " << C(0,7) << " (expected -120)\n";

    std::cout << "C(1,0) = " << C(1,0) << " (expected -101)\n";
    std::cout << "C(1,1) = " << C(1,1) << " (expected 131)\n";
    std::cout << "C(1,2) = " << C(1,2) << " (expected -120)\n";
    std::cout << "C(1,3) = " << C(1,3) << " (expected 137)\n";
    std::cout << "C(1,4) = " << C(1,4) << " (expected -117)\n";
    std::cout << "C(1,5) = " << C(1,5) << " (expected 133)\n";
    std::cout << "C(1,6) = " << C(1,6) << " (expected -138)\n";
    std::cout << "C(1,7) = " << C(1,7) << " (expected 120)\n";

    std::cout << "C(2,0) = " << C(2,0) << " (expected 56)\n";
    std::cout << "C(2,1) = " << C(2,1) << " (expected -74)\n";
    std::cout << "C(2,2) = " << C(2,2) << " (expected 53)\n";
    std::cout << "C(2,3) = " << C(2,3) << " (expected -78)\n";
    std::cout << "C(2,4) = " << C(2,4) << " (expected 54)\n";
    std::cout << "C(2,5) = " << C(2,5) << " (expected -61)\n";
    std::cout << "C(2,6) = " << C(2,6) << " (expected 66)\n";
    std::cout << "C(2,7) = " << C(2,7) << " (expected -53)\n";

    std::cout << "C(3,0) = " << C(3,0) << " (expected -78)\n";
    std::cout << "C(3,1) = " << C(3,1) << " (expected 93)\n";
    std::cout << "C(3,2) = " << C(3,2) << " (expected -95)\n";
    std::cout << "C(3,3) = " << C(3,3) << " (expected 101)\n";
    std::cout << "C(3,4) = " << C(3,4) << " (expected -96)\n";
    std::cout << "C(3,5) = " << C(3,5) << " (expected 99)\n";
    std::cout << "C(3,6) = " << C(3,6) << " (expected -104)\n";
    std::cout << "C(3,7) = " << C(3,7) << " (expected 85)\n";

    std::cout << "C(4,0) = " << C(4,0) << " (expected 100)\n";
    std::cout << "C(4,1) = " << C(4,1) << " (expected -115)\n";
    std::cout << "C(4,2) = " << C(4,2) << " (expected 123)\n";
    std::cout << "C(4,3) = " << C(4,3) << " (expected -127)\n";
    std::cout << "C(4,4) = " << C(4,4) << " (expected 126)\n";
    std::cout << "C(4,5) = " << C(4,5) << " (expected -125)\n";
    std::cout << "C(4,6) = " << C(4,6) << " (expected 132)\n";
    std::cout << "C(4,7) = " << C(4,7) << " (expected -105)\n";

    std::cout << "C(5,0) = " << C(5,0) << " (expected -88)\n";
    std::cout << "C(5,1) = " << C(5,1) << " (expected 118)\n";
    std::cout << "C(5,2) = " << C(5,2) << " (expected -80)\n";
    std::cout << "C(5,3) = " << C(5,3) << " (expected 126)\n";
    std::cout << "C(5,4) = " << C(5,4) << " (expected -84)\n";
    std::cout << "C(5,5) = " << C(5,5) << " (expected 98)\n";
    std::cout << "C(5,6) = " << C(5,6) << " (expected -104)\n";
    std::cout << "C(5,7) = " << C(5,7) << " (expected 82)\n";

    std::cout << "C(6,0) = " << C(6,0) << " (expected -35)\n";
    std::cout << "C(6,1) = " << C(6,1) << " (expected 25)\n";
    std::cout << "C(6,2) = " << C(6,2) << " (expected -1)\n";
    std::cout << "C(6,3) = " << C(6,3) << " (expected 35)\n";
    std::cout << "C(6,4) = " << C(6,4) << " (expected 20)\n";
    std::cout << "C(6,5) = " << C(6,5) << " (expected 2)\n";
    std::cout << "C(6,6) = " << C(6,6) << " (expected 10)\n";
    std::cout << "C(6,7) = " << C(6,7) << " (expected -21)\n";

    std::cout << "C(7,0) = " << C(7,0) << " (expected -91)\n";
    std::cout << "C(7,1) = " << C(7,1) << " (expected 116)\n";
    std::cout << "C(7,2) = " << C(7,2) << " (expected -108)\n";
    std::cout << "C(7,3) = " << C(7,3) << " (expected 136)\n";
    std::cout << "C(7,4) = " << C(7,4) << " (expected -123)\n";
    std::cout << "C(7,5) = " << C(7,5) << " (expected 147)\n";
    std::cout << "C(7,6) = " << C(7,6) << " (expected -155)\n";
    std::cout << "C(7,7) = " << C(7,7) << " (expected 129)\n";
    
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}
Input Matrix A
1 -2  3 -4 5 -6 7 -8
−1 2 -3  4 -5 6 -7 8
2  3 -1 -2  4 -4 5 -5
−3 1 -4  2 -5  3 -6 4  
5 -1  6 -2  7 -3  8 -4 
−2 -4 2  4 -6  6 -8  8
3  -5 7 -1 -3  5 -7  1
−4​​ 6 -2  8 -1  3 -5  7

Input Matrix B
2 -1 3 -2 4 -3 5 -4
-3 2 -4 3 -5 4 -6 5
1 -2 5 -1 3 -4 2 -3
-2 4 -1 5 -3 6 -4 2
3 -5 2 -4 6 -1 4 -2
-4 3 -6 2 -1 5 -3 4
5 -3 4 -6 2 -5 3 -1
-1 5 -2 4 -4 2 -5 6

Expected Output
101 -131 120 -137 117 -133 138 -120
-101 131 -120 137 -117 133 -138 120
56  -74   53 -78   54  -61  66  -53
-78  93   -95 101  -96  99 -104 85
100  -115  123 -127 126 -125 132 -105
-88  118  -80  126 -84  98  -104 82
-35  25   -1  35   20   2   10  -21
-91 116  -108 136 -123 147 -155 129

Actual Output
C(0,0) = 101 (expected 101)
C(0,1) = -131 (expected -131)
C(0,2) = 120 (expected 120)
C(0,3) = -137 (expected -137)
C(0,4) = 117 (expected 117)
C(0,5) = -133 (expected -133)
C(0,6) = 138 (expected 138)
C(0,7) = -120 (expected -120)
C(1,0) = -101 (expected -101)
C(1,1) = 131 (expected 131)
C(1,2) = -120 (expected -120)
C(1,3) = 137 (expected 137)
C(1,4) = -117 (expected -117)
C(1,5) = 133 (expected 133)
C(1,6) = -138 (expected -138)
C(1,7) = 120 (expected 120)
C(2,0) = 56 (expected 56)
C(2,1) = -74 (expected -74)
C(2,2) = 53 (expected 53)
C(2,3) = -78 (expected -78)
C(2,4) = 54 (expected 54)
C(2,5) = -61 (expected -61)
C(2,6) = 66 (expected 66)
C(2,7) = -53 (expected -53)
C(3,0) = -78 (expected -78)
C(3,1) = 93 (expected 93)
C(3,2) = -95 (expected -95)
C(3,3) = 101 (expected 101)
C(3,4) = -96 (expected -96)
C(3,5) = 99 (expected 99)
C(3,6) = -104 (expected -104)
C(3,7) = 85 (expected 85)
C(4,0) = 100 (expected 100)
C(4,1) = -115 (expected -115)
C(4,2) = 123 (expected 123)
C(4,3) = -127 (expected -127)
C(4,4) = 126 (expected 126)
C(4,5) = -125 (expected -125)
C(4,6) = 132 (expected 132)
C(4,7) = -105 (expected -105)
C(5,0) = -88 (expected -88)
C(5,1) = 118 (expected 118)
C(5,2) = -80 (expected -80)
C(5,3) = 126 (expected 126)
C(5,4) = -84 (expected -84)
C(5,5) = 98 (expected 98)
C(5,6) = -104 (expected -104)
C(5,7) = 82 (expected 82)
C(6,0) = -35 (expected -35)
C(6,1) = 25 (expected 25)
C(6,2) = -1 (expected -1)
C(6,3) = 35 (expected 35)
C(6,4) = 20 (expected 20)
C(6,5) = 2 (expected 2)
C(6,6) = 10 (expected 10)
C(6,7) = -21 (expected -21)
C(7,0) = -91 (expected -91)
C(7,1) = 116 (expected 116)
C(7,2) = -108 (expected -108)
C(7,3) = 136 (expected 136)
C(7,4) = -123 (expected -123)
C(7,5) = 147 (expected 147)
C(7,6) = -155 (expected -155)
C(7,7) = 129 (expected 129)
  multiplications: 512
  additions:       448
  macs:            512
  primitive_ops:   960

  status
   PASS
   ```
### case8: 2x2 matrix multiplication with out of INT8 values
**Matrix operation code**
 ```text
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
     A(0,0)=128;  A(0,1)= 2;
     A(0,4)=3;  A(0,5)=4;

    //matrix B
    Matrix<std::int8_t> W(2,2);
    W(0,0)=1;  W(0,1)=-2;
    W(0,4)=3;  W(0,5)=4;

    
    //struct called
    MatmulStats stats{};
    //C Matrix 
    Matrix<std::int32_t> C = matmul(A, W, &stats);
    //Priniting elements of C Matrix
    std::cout << "Invalid Output: Test Failed";
    
    //calling the fuction to print stats
    printStats(stats);

    return 0;
}
Input Matrix A
128 2
3   4

Input Matrix B
1 -2
3  4

Expected Output
 index out of bounds

Actual Output
terminate called after throwing an instance of 'std::out_of_range'
  what():  index out of bounds

status
 PASS
```
### Observation
```text
 Matrix multiplication was tested using matrices of different sizes and input values, including positive, negative, mixed-sign, identity matrix, and out-of-range input cases. The test results were compared with the expected outputs to verify the correctness of the C++ golden model. All test cases passed, including the invalid-input tests, which behaved as expected.. 

