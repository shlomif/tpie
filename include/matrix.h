// Copyright (c) 1994 Darren Vengroff
//
// File: matrix.h
// Author: Darren Vengroff <darrenv@eecs.umich.edu>
// Created: 11/4/94
//
// $Id: matrix.h,v 1.1 1994-12-16 21:48:45 darrenv Exp $
//
#ifndef MATRIX_H
#define MATRIX_H

#include <iostream.h>

#include <tpie_assert.h>

// Enable exceptions if the compiler supports them.
#ifndef HANDLE_EXCEPTIONS
#define HANDLE_EXCEPTIONS 0
#endif

// References to rows and colums and submatrices.
template<class T> class rowref;
template<class T> class colref;

// Matrices and submatrices.
template<class T> class matrix_base;
template<class T> class matrix;
template<class T> class submatrix;

// A base class for matrices and submatrices.
template<class T> class matrix_base
{
protected:
    unsigned int r,c;
public:
#if HANDLE_EXCEPTIONS    
    // Exception class.
    class range { };
#endif

    matrix_base(unsigned int rows, unsigned int cols);
    virtual ~matrix_base(void);

    // What is the size of the matrix?
    unsigned int rows(void) const;
    unsigned int cols(void) const;
    
    // Access to the contents of the matrix.
    virtual T &elt(unsigned int row, unsigned int col) const = 0;

    rowref<T> row(unsigned int row);
    colref<T> col(unsigned int col);

    rowref<T> operator[](unsigned int row) const;
        
    // Assignement.
    matrix_base<T> &operator=(const matrix_base<T> &rhs);
    matrix_base<T> &operator=(const rowref<T> &rhs);
    matrix_base<T> &operator=(const colref<T> &rhs);
    
    // Addition in place.
    matrix_base<T> &operator+=(const matrix_base<T> &rhs);
};


// References to rows and columns.
template<class T>
class rowref
{
private:
    matrix_base<T> &m;
    unsigned int r;
public:
    rowref(matrix_base<T> &matrix, unsigned int row);
    ~rowref(void);

    T &operator[](unsigned int col) const;

    friend class matrix_base<T>;
    friend class matrix<T>;
};

template<class T>
class colref
{
private:
    matrix_base<T> &m;
    unsigned int c;
public:
    colref(matrix_base<T> &matrix, unsigned int col);
    ~colref(void);

    T &operator[](unsigned int row) const;

    friend class matrix_base<T>;
    friend class matrix<T>;
};


template<class T>
matrix_base<T>::matrix_base(unsigned int rows, unsigned int cols) :
        r(rows),
        c(cols)
{
}

template<class T>
matrix_base<T>::~matrix_base(void)
{
}

template<class T>
unsigned int matrix_base<T>::rows(void)
{
    return r;
}

template<class T>
unsigned int matrix_base<T>::cols(void)
{
    return c;
}

template<class T>
rowref<T> matrix_base<T>::row(unsigned int row)
{
    if (row >= r) {
#if HANDLE_EXCEPTIONS    
        throw range();
#else
        tp_assert(0, "Range error.");
#endif
    }

    
    return rowref<T>(*this, row);
}

template<class T>
colref<T> matrix_base<T>::col(unsigned int col)
{
    if (col >= c) {
#if HANDLE_EXCEPTIONS        
        throw range();
#else
        tp_assert(0, "Range error.");
#endif
    }

    
    return colref<T>(*this, col);
}

template<class T>
rowref<T> matrix_base<T>::operator[](unsigned int row)
{
    return this->row(row);
}


template<class T>
matrix_base<T> &matrix_base<T>::operator=(const matrix_base<T> &rhs)
{
    if ((rows() != rhs.rows()) || (cols() != rhs.cols())) {
#if HANDLE_EXCEPTIONS    
        throw range();
#else
        tp_assert(0, "Range error.");
#endif
    }

    
    unsigned int ii,jj;
    
    for (ii = rows(); ii--; ) {
        for (jj = cols(); jj--; ) {
            elt(ii,jj) = rhs.elt(ii,jj);
        }
    }

    return *this;
}


template<class T>
matrix_base<T> &matrix_base<T>::operator=(const rowref<T> &rhs)
{
    if ((rows() != 1) || (cols() != rhs.m.cols())) {
#if HANDLE_EXCEPTIONS    
        throw range();
#else
        tp_assert(0, "Range error.");
#endif
    }
    
    unsigned int ii;
    
    for (ii = cols(); ii--; ) {
        elt(0,ii) = rhs[ii];
    }

    return *this;
}


template<class T>
matrix_base<T> &matrix_base<T>::operator=(const colref<T> &rhs)
{
    if ((cols() != 1) || (rows() != rhs.m.rows())) {
#if HANDLE_EXCEPTIONS    
        throw range();
#else
        tp_assert(0, "Range error.");
#endif
    }

    unsigned int ii;
    T t;
    
    for (ii = rows(); ii--; ) {
        t = rhs[ii];
        elt(ii,0) = t;
    }

    return *this;
}


template<class T>
matrix_base<T> &matrix_base<T>::
        operator+=(const matrix_base<T> &rhs)
{
    if ((rows() != rhs.rows()) || (cols() != rhs.cols())) {
#if HANDLE_EXCEPTIONS        
        throw range();
#else
        tp_assert(0, "Range error.");
#endif
    }

    
    unsigned int ii,jj;
    
    for (ii = rows(); ii--; ) {
        for (jj = cols(); jj--; ) {
            elt(ii,jj) += rhs.elt(ii,jj);
        }
    }
    
    return *this;
}


template<class T>
matrix<T> operator+(const matrix_base<T> &op1,
                         const matrix_base<T> &op2)
{
    if ((op1.rows() != op2.rows()) || (op1.cols() != op2.cols())) {
#if HANDLE_EXCEPTIONS        
        throw matrix_base<T>::range();
#else
        tp_assert(0, "Range error.");
#endif
    }


    matrix<T> temp(op1);

    return temp += op2;
}


template<class T>
void perform_mult_in_place(const matrix_base<T> &op1,
                           const matrix_base<T> &op2,
                           matrix_base<T> &res)
{
    if ((op1.cols() != op2.rows()) ||
        (op1.rows() != res.rows()) ||
        (op2.cols() != res.cols())) {
#if HANDLE_EXCEPTIONS        
        throw matrix_base<T>::range();
#else
        tp_assert(0, "Range error.");
#endif
    }

    unsigned int ii,jj,kk;
    T t;
    
    // Iterate over rows of op1.
    for (ii = op1.rows(); ii--; ) {
        // Iterate over colums of op2.
        for (jj = op2.cols(); jj--; ) {
            // Iterate through the row of r1 and the column of r2.
            t = op1.elt(ii,op1.cols()-1) * op2.elt(op2.rows()-1,jj);
            for (kk = op2.rows() - 1; kk--; ) {
                t += op1.elt(ii,kk) * op2.elt(kk,jj);
            }
            // Assign into the result.
            res.elt(ii,jj) = t;
        }
    }    
}                      


template<class T>
void perform_mult_add_in_place(const matrix_base<T> &op1,
                               const matrix_base<T> &op2,
                               matrix_base<T> &res)
{
    if ((op1.cols() != op2.rows()) ||
        (op1.rows() != res.rows()) ||
        (op2.cols() != res.cols())) {
#if HANDLE_EXCEPTIONS        
        throw matrix_base<T>::range();
#else
        tp_assert(0, "Range error.");
#endif
    }

    unsigned int ii,jj,kk;
    T t;
    
    // Iterate over rows of op1.
    for (ii = op1.rows(); ii--; ) {
        // Iterate over colums of op2.
        for (jj = op2.cols(); jj--; ) {
            // Iterate through the row of r1 and the column of r2.
            t = op1.elt(ii,op1.cols()-1) * op2.elt(op2.rows()-1,jj);
            for (kk = op2.rows() - 1; kk--; ) {
                t += op1.elt(ii,kk) * op2.elt(kk,jj);
            }
            // Add into the result.
            res.elt(ii,jj) += t;
        }
    }    
}                      


template<class T>
matrix<T> operator*(const matrix_base<T> &op1,
                    const matrix_base<T> &op2)
{
    if (op1.cols() != op2.rows()) {
#if HANDLE_EXCEPTIONS        
        throw matrix_base<T>::range();
#else
        tp_assert(0, "Range error.");
#endif
    }
    
    matrix<T> temp(op1.rows(),op2.cols());

    perform_mult_in_place(op1, op2, (matrix_base<T> &)temp);
    
    return temp;
}

template<class T>
ostream &operator<<(ostream &s, const matrix_base<T> &m)
{
    unsigned int ii,jj;
    
    // Iterate over rows
    for (ii = 0; ii < m.rows(); ii++) {
        // Iterate over cols
        s << m.elt(ii,0);
        for (jj = 1; jj < m.cols(); jj++) {
            if (jj) (s << ' ');
            s << m.elt(ii,jj);
        }
        s << '\n';
    }
            
    return s;
}

// Member functions for row and column reference classes.

template<class T>
rowref<T>::rowref(matrix_base<T> &matrix, unsigned int row) :
        m(matrix),
        r(row)
{
}

template<class T>
rowref<T>::~rowref(void)
{
}

template<class T>
T &rowref<T>::operator[](unsigned int col) 
{
    return m.elt(r,col);
}

template<class T>
colref<T>::colref(matrix_base<T> &matrix, unsigned int col) :
        m(matrix),
        c(col)
{
}

template<class T>
colref<T>::~colref(void)
{
}

template<class T>
T &colref<T>::operator[](unsigned int row)
{
    return m.elt(row,c);
}


// A submatrix class.
template<class T>
class submatrix : public matrix_base<T>
{
private:
    matrix_base<T> &m;
    unsigned int r1,r2,c1,c2;
public:
    // Construction/destruction.
    submatrix(matrix_base<T> &matrix,
                 unsigned int row1, unsigned int row2,
                 unsigned int col1, unsigned int col2);

    virtual ~submatrix(void);

    // We need an assignement operator that copies data by explicitly
    // calling the base class's assignment operator to do elementwise
    // copying.  Otherwise, m, r1, r2, c1, and c2 are just copied.
    submatrix<T> &operator=(const submatrix<T> &rhs);

    // We also want to be able to assign from matrices.
    submatrix<T> &operator=(const matrix<T> &rhs);
    
    // Access to elements.
    T& elt(unsigned int row, unsigned int col) const;
};

template<class T>
submatrix<T>::submatrix(matrix_base<T> &matrix,
                              unsigned int row1, unsigned int row2,
                              unsigned int col1, unsigned int col2) :
                                      matrix_base<T>(row2 - row1 + 1,
                                                          col2 - col1 + 1),
                                      m(matrix),
                                      r1(row1), r2(row2),
                                      c1(col1), c2(col2)
{
}

template<class T>
submatrix<T>::~submatrix(void)
{
}

template<class T>
submatrix<T> &submatrix<T>::operator=(const submatrix<T> &rhs)
{
    // Call the assignement operator from the base class to do range
    // checking and elementwise assignment.
    (matrix_base<T> &)(*this) = (const matrix_base<T> &)rhs;
    
    return *this;
}

template<class T>
submatrix<T> &submatrix<T>::operator=(const matrix<T> &rhs)
{
    // Call the assignement operator from the base class to do range
    // checking and elementwise assignment.
    (matrix_base<T> &)(*this) = (const matrix_base<T> &)rhs;
    
    return *this;
}

template<class T>
T& submatrix<T>::elt(unsigned int row, unsigned int col)
{
    if ((row >= rows()) || (col >= cols())) {
#if HANDLE_EXCEPTIONS
        throw matrix_base<T>::range();
#else
        tp_assert(0, "Range error.");
#endif
    }
    return m.elt(row + r1, col + c1);
}



// The matrix class itself.
template<class T>
class matrix : public matrix_base<T> {
private:
    T *data;
public:
    // Construction/destruction.
    matrix(unsigned int rows, unsigned int cols);
    matrix(const matrix<T> &rhs);
    matrix(const matrix_base<T> &rhs);
    matrix(const submatrix<T> &rhs);
    matrix(const rowref<T> umrr);
    matrix(const colref<T> umcr);

    virtual ~matrix(void);

    // We need an assignement operator that copies data by explicitly
    // calling the base class's assignment operator to do elementwise
    // copying.  Otherwise, the data pointer is just copied.
    matrix<T> &operator=(const matrix<T> &rhs);
    
    // We also want to be able to assign from submatrices.
    matrix<T> &operator=(const submatrix<T> &rhs);
    
    // Access to elements.
    T &elt(unsigned int row, unsigned int col) const;
};


template<class T>
matrix<T>::matrix(unsigned int rows, unsigned int cols) :
        matrix_base<T>(rows,cols)
{
    data = new T[rows * cols];
}

template<class T>
matrix<T>::matrix(const matrix<T> &rhs) :
        matrix_base<T>(rhs.rows(), rhs.cols())
{
    unsigned int ii;
    
    data = new T[r*c];

    for (ii = r*c; ii--; ) {
        data[ii] = rhs.data[ii];
    }
}

template<class T>
matrix<T>::matrix(const matrix_base<T> &rhs) :
        matrix_base<T>(rhs.rows(), rhs.cols())
{
    unsigned int ii,jj;
    
    data = new T[r*c];

    for (ii = r; ii--; ) {
        for (jj = c; jj--; ) {
            data[c*ii+jj] = rhs.elt(ii,jj);
        }
    }
}

template<class T>
matrix<T>::matrix(const submatrix<T> &rhs) :
        matrix_base<T>(rhs.rows(), rhs.cols())
{
    unsigned int ii,jj;
    
    data = new T[r*c];

    for (ii = r; ii--; ) {
        for (jj = c; jj--; ) {
            data[c*ii+jj] = rhs.elt(ii,jj);
        }
    }
}

template<class T>
matrix<T>::matrix(const rowref<T> umrr) :
        matrix_base<T>(1, umrr.m.cols())
{
    data = new T[c];

    matrix_base<T>::operator=(umrr);
}

template<class T>
matrix<T>::matrix(const colref<T> umcr) :
        matrix_base<T>(umcr.m.rows(),1)        
{
    data = new T[r];

    matrix_base<T>::operator=(umcr);
}

template<class T>
matrix<T>::~matrix(void) {
    delete[] data;
}


template<class T>
matrix<T> &matrix<T>::operator=(const matrix<T> &rhs)
{
    // Call the assignement operator from the base class to do range
    // checking and elementwise assignment.
    (matrix_base<T> &)(*this) = (const matrix_base<T> &)rhs;
    
    return *this;
}

template<class T>
matrix<T> &matrix<T>::operator=(const submatrix<T> &rhs)
{
    // Call the assignement operator from the base class to do range
    // checking and elementwise assignment.
    (matrix_base<T> &)(*this) = (const matrix_base<T> &)rhs;
    
    return *this;
}


template<class T>
T& matrix<T>::elt(unsigned int row, unsigned int col)
{
    if ((row >= rows()) || (col >= cols())) {
#if HANDLE_EXCEPTIONS
        throw matrix_base<T>::range();
#else
        tp_assert(0, "Range error.");
#endif
    }
    return data[row*cols()+col];
}


// These are needed since template functions accept only exact argument
// type matches.  Base class promotion is not done as it is for
// ordinary functions.

#define MAT_DUMMY_OP(TM1,TM2,OP)					\
template<class T>							\
matrix<T> operator OP (const TM1 &op1,					\
                       const TM2 &op2)					\
{									\
    return ((const matrix_base<T> &)op1) OP				\
        ((const matrix_base<T> &)op2);					\
}

MAT_DUMMY_OP(matrix<T>,matrix<T>,+)
MAT_DUMMY_OP(matrix<T>,submatrix<T>,+)
MAT_DUMMY_OP(submatrix<T>,matrix<T>,+)
MAT_DUMMY_OP(submatrix<T>,submatrix<T>,+)
    
MAT_DUMMY_OP(matrix<T>,matrix<T>,*)
MAT_DUMMY_OP(matrix<T>,submatrix<T>,*)
MAT_DUMMY_OP(submatrix<T>,matrix<T>,*)
MAT_DUMMY_OP(submatrix<T>,submatrix<T>,*)
    
template<class T>
ostream &operator<<(ostream &s, const matrix<T> &m)
{
    return s << (const matrix_base<T> &)m;
}

template<class T>
ostream &operator<<(ostream &s, const submatrix<T> &m)
{
    return s << (const matrix_base<T> &)m;
}


#ifdef NO_IMPLICIT_TEMPLATES

#define TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,TM1,TM2,OP)			\
template matrix<T> operator OP (const TM1 &op1, const TM2 &op2);

#define TEMPLATE_INSTANTIATE_MATRIX(T)					\
template class matrix_base<T>;						\
template class colref<T>;						\
template class rowref<T>;						\
template class submatrix<T>;						\
template class matrix<T>;						\
									\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,matrix_base<T>,			\
                                  matrix_base<T>,+)			\
									\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,matrix<T>,matrix<T>,+)		\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,matrix<T>,submatrix<T>,+)		\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,submatrix<T>,matrix<T>,+)		\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,submatrix<T>,submatrix<T>,+)	\
									\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,matrix_base<T>,			\
                                  matrix_base<T>,*)			\
									\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,matrix<T>,matrix<T>,*)		\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,matrix<T>,submatrix<T>,*)		\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,submatrix<T>,matrix<T>,*)		\
TEMPLATE_INSTANTIATE_MAT_DUMMY_OP(T,submatrix<T>,submatrix<T>,*)	\
									\
template void perform_mult_in_place(const matrix_base<T> &op1,		\
                                    const matrix_base<T> &op2,		\
                                    matrix_base<T> &res);		\
									\
template void perform_mult_add_in_place(const matrix_base<T> &op1,	\
                                        const matrix_base<T> &op2,	\
                                        matrix_base<T> &res);		\
									\
template ostream &operator<<(ostream &s, matrix_base<T> &m);		\
template ostream &operator<<(ostream &s, matrix<T> &m);			\
template ostream &operator<<(ostream &s, submatrix<T> &m);

#endif

#endif // MATRIX_H 
