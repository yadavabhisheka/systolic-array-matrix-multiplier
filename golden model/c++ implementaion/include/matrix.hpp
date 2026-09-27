#pragma once
#include<vector>
#include<cstddef>
#include<stdexcept>
//template because we require different types of matrix (int_8,int_16,int_32)
template <typename T>
//class Matrix Starts
class Matrix{
    //std:size_t because return the datatype as we want(int_8/int_16/int_32)
    //private variables (row_,col_)and a vector for storing the elements named data_
    private:
    std::size_t row_;
    std::size_t col_;
    std::vector<T> data_;
    public:
    //Matrix constructor (which condition checking row and col should not be equal to zero)
    Matrix(std::size_t row,std::size_t col):row_(row),col_(col),data_(row*col){
    if (row == 0 ||col == 0){
        throw std::invalid_argument("Row and Column values cannot be zero.");
    }
    }
    std::size_t rows() const{
        return row_;
    }
    //define row() and col() accessor forthe accessing of row and col conut 
    std::size_t cols() const{
        return col_;

    }
    //getting a element by operator() with index formula devloped while the matrix data is rowwise storing in a vector 
    T& operator()(std::size_t row,std::size_t col){
        if(row>=row_||col>=col_){
            throw std::out_of_range("index out of bounds");
        }
            return data_[row * col_ + col];

    }
    //same operator() in const form
    const T& operator()(std::size_t row,std::size_t col)const{
        if(row>=row_||col>=col_){
            throw std::out_of_range("index out of bounds");
        }
            return data_[row * col_ + col];
    }
    //getting the sizeof data_ vector
    std::size_t size() const{
        return data_.size();
    }
    //pointer to change the value as exact pointer
    //only drawbackis out_of_bonds value can be changed but they are never initalized
    T* data(){
        return data_.data();
    }
    //const so value can only be read not changed
    const T* data()const{
        return data_.data();
    }
    
};