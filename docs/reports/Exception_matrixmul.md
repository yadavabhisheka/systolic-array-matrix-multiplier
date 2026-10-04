# Matrix Multiplication Exception Test Cases

This report documents all exception cases and boundary conditions for matrix multiplication ($1 \times 1$ to $16 \times 16$).

---

## 1. Complete Executable Program (All Cases Combined)

### C++ Source Code
```cpp
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>

// Hardware capacity limit (1x1 to 16x16)
const int MAX_HARDWARE_DIM = 16;

// Matrix Structure
struct Matrix {
    int rows;
    int cols;
    std::vector<std::vector<int>> data;

    Matrix(int r, int c) : rows(r), cols(c) {
        if (r < 0 || c < 0) {
            throw std::invalid_argument("EXCEPTION ERROR: Invalid matrix size! Matrix dimensions cannot be negative.");
        }
        if (r == 0 || c == 0) {
            throw std::invalid_argument("EXCEPTION ERROR: Invalid matrix size! Rows and columns must be greater than zero.");
        }
        if (r > MAX_HARDWARE_DIM || c > MAX_HARDWARE_DIM) {
            throw std::out_of_range("EXCEPTION ERROR: Matrix size exceeds maximum hardware limit of 16x16.");
        }
        data.resize(r, std::vector<int>(c, 0));
    }

    Matrix(std::initializer_list<std::initializer_list<int>> list) {
        rows = list.size();
        cols = list.begin()->size();
        if (rows > MAX_HARDWARE_DIM || cols > MAX_HARDWARE_DIM) {
            throw std::out_of_range("EXCEPTION ERROR: Matrix size exceeds maximum hardware limit of 16x16.");
        }
        for (const auto& row : list) {
            data.push_back(row);
        }
    }
};

// Matrix Multiplication Function
Matrix multiply(const Matrix& A, const Matrix& B) {
    if (A.cols != B.rows) {
        throw std::logic_error("EXCEPTION ERROR: Inner dimension mismatch! Matrix A columns (" 
                                + std::to_string(A.cols) + ") must equal Matrix B rows (" 
                                + std::to_string(B.rows) + ").");
    }

    Matrix C(A.rows, B.cols);

    for (int i = 0; i < A.rows; ++i) {
        for (int j = 0; j < B.cols; ++j) {
            long long sum = 0;
            for (int k = 0; k < A.cols; ++k) {
                long long term = (long long)A.data[i][k] * B.data[k][j];
                
                if (term > std::numeric_limits<int>::max() || term < std::numeric_limits<int>::min()) {
                    throw std::overflow_error("EXCEPTION ERROR: Numeric overflow detected during calculation! Term exceeds maximum 32-bit integer range.");
                }

                sum += term;
                if (sum > std::numeric_limits<int>::max() || sum < std::numeric_limits<int>::min()) {
                    throw std::overflow_error("EXCEPTION ERROR: Numeric overflow detected during calculation! Result exceeds maximum 32-bit integer range.");
                }
            }
            C.data[i][j] = static_cast<int>(sum);
        }
    }
    return C;
}

// Function to parse input strings into a Matrix
Matrix parseMatrix(int r, int c, const std::string& inputStr) {
    Matrix M(r, c);
    std::stringstream ss(inputStr);

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            if (!(ss >> M.data[i][j])) {
                if (ss.eof()) {
                    throw std::runtime_error("EXCEPTION ERROR: Input stream failed! Expected elements, but reached end-of-file early.");
                } else {
                    throw std::runtime_error("EXCEPTION ERROR: Invalid data type encountered in matrix input stream! Expected integer value.");
                }
            }
        }
    }
    return M;
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "   MATRIX MULTIPLICATION ALL EXCEPTION TEST SUITE \n";
    std::cout << "==================================================\n\n";

    // TEST CASE 1: Incompatible Dimensions
    std::cout << "## Test Case 1 - Incompatible Dimensions\n";
    std::cout << "### Output\n";
    try {
        Matrix A = {{1, 2, 3}, {4, 5, 6}}; // 2x3
        Matrix B = {{1, 2}, {3, 4}, {5, 6}, {7, 8}}; // 4x2
        Matrix C = multiply(A, B);
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n\n";
    }

    // TEST CASE 2: Oversized Dimensions (>16x16)
    std::cout << "## Test Case 2 - Oversized Dimensions Exceeding Limit (>16x16)\n";
    std::cout << "### Output\n";
    try {
        Matrix A(17, 17);
        Matrix B(17, 17);
        Matrix C = multiply(A, B);
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n\n";
    }

    // TEST CASE 3: Zero Matrix Dimensions (0x0)
    std::cout << "## Test Case 3 - Zero Matrix Dimensions (0x0 Matrix)\n";
    std::cout << "### Output\n";
    try {
        Matrix A(0, 0);
        Matrix B(0, 0);
        Matrix C = multiply(A, B);
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n\n";
    }

    // TEST CASE 4: Negative Matrix Dimensions
    std::cout << "## Test Case 4 - Negative Matrix Dimensions\n";
    std::cout << "### Output\n";
    try {
        Matrix A(-2, 4);
        Matrix B(4, 2);
        Matrix C = multiply(A, B);
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n\n";
    }

    // TEST CASE 5: Numeric Integer Overflow
    std::cout << "## Test Case 5 - Numeric Integer Overflow\n";
    std::cout << "### Output\n";
    try {
        Matrix A = {{2000000000}};
        Matrix B = {{2000000000}};
        Matrix C = multiply(A, B);
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n\n";
    }

    // TEST CASE 6: Missing Input Data Elements
    std::cout << "## Test Case 6 - Missing Input Data Elements\n";
    std::cout << "### Output\n";
    try {
        std::string inputData = "1 2";
        Matrix A = parseMatrix(2, 2, inputData);
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n\n";
    }

    // TEST CASE 7: Non-Numeric Input Data
    std::cout << "## Test Case 7 - Non-Numeric Input Data (Invalid Data Types)\n";
    std::cout << "### Output\n";
    try {
        std::string inputData = "1 abc 3 4";
        Matrix A = parseMatrix(2, 2, inputData);
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n\n";
    }

    return 0;
}
```

### Full Program Output
```text
==================================================
   MATRIX MULTIPLICATION ALL EXCEPTION TEST SUITE 
==================================================

## Test Case 1 - Incompatible Dimensions
### Output
EXCEPTION ERROR: Inner dimension mismatch! Matrix A columns (3) must equal Matrix B rows (4).

## Test Case 2 - Oversized Dimensions Exceeding Limit (>16x16)
### Output
EXCEPTION ERROR: Matrix size exceeds maximum hardware limit of 16x16.

## Test Case 3 - Zero Matrix Dimensions (0x0 Matrix)
### Output
EXCEPTION ERROR: Invalid matrix size! Rows and columns must be greater than zero.

## Test Case 4 - Negative Matrix Dimensions
### Output
EXCEPTION ERROR: Invalid matrix size! Matrix dimensions cannot be negative.

## Test Case 5 - Numeric Integer Overflow
### Output
EXCEPTION ERROR: Numeric overflow detected during calculation! Term exceeds maximum 32-bit integer range.

## Test Case 6 - Missing Input Data Elements
### Output
EXCEPTION ERROR: Input stream failed! Expected elements, but reached end-of-file early.

## Test Case 7 - Non-Numeric Input Data (Invalid Data Types)
### Output
EXCEPTION ERROR: Invalid data type encountered in matrix input stream! Expected integer value.
```

---

## 2. Individual Test Case Snippets (For Main.cpp Copy-Pasting)

### Test Case 1: Incompatible Dimensions
#### Code
```cpp
Matrix A = {{1, 2, 3}, {4, 5, 6}}; // 2x3
Matrix B = {{1, 2}, {3, 4}, {5, 6}, {7, 8}}; // 4x2
Matrix C = multiply(A, B);
```
#### Output
```text
EXCEPTION ERROR: Inner dimension mismatch! Matrix A columns (3) must equal Matrix B rows (4).
```

---

### Test Case 2: Oversized Dimensions Exceeding Limit (>16x16)
#### Code
```cpp
Matrix A(17, 17);
Matrix B(17, 17);
Matrix C = multiply(A, B);
```
#### Output
```text
EXCEPTION ERROR: Matrix size exceeds maximum hardware limit of 16x16.
```

---

### Test Case 3: Zero Matrix Dimensions (0x0 Matrix)
#### Code
```cpp
Matrix A(0, 0);
Matrix B(0, 0);
Matrix C = multiply(A, B);
```
#### Output
```text
EXCEPTION ERROR: Invalid matrix size! Rows and columns must be greater than zero.
```

---

### Test Case 4: Negative Matrix Dimensions
#### Code
```cpp
Matrix A(-2, 4);
Matrix B(4, 2);
Matrix C = multiply(A, B);
```
#### Output
```text
EXCEPTION ERROR: Invalid matrix size! Matrix dimensions cannot be negative.
```

---

### Test Case 5: Numeric Integer Overflow
#### Code
```cpp
Matrix A = {{2000000000}};
Matrix B = {{2000000000}};
Matrix C = multiply(A, B);
```
#### Output
```text
EXCEPTION ERROR: Numeric overflow detected during calculation! Term exceeds maximum 32-bit integer range.
```

---

### Test Case 6: Missing Input Data Elements
#### Code
```cpp
std::string inputData = "1 2";
Matrix A = parseMatrix(2, 2, inputData);
```
#### Output
```text
EXCEPTION ERROR: Input stream failed! Expected elements, but reached end-of-file early.
```

---

### Test Case 7: Non-Numeric Input Data
#### Code
```cpp
std::string inputData = "1 abc 3 4";
Matrix A = parseMatrix(2, 2, inputData);
```
#### Output
```text
EXCEPTION ERROR: Invalid data type encountered in matrix input stream! Expected integer value.
```
