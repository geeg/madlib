/* ----------------------------------------------------------------------- *//**
 *
 * @file independent_variables.hpp
 *
 * Types of independent variables can be simply vectors/matrices that are
 * offered by standard or third party libraries. Classes in this files are
 * for some special ones.
 *
 *//* ----------------------------------------------------------------------- */

#ifndef MADLIB_MODULES_CONVEX_TYPE_INDEPENDENT_VARIABLES_HPP_
#define MADLIB_MODULES_CONVEX_TYPE_INDEPENDENT_VARIABLES_HPP_

namespace madlib {

namespace modules {

namespace convex {

struct SmallMatrixIndex { //For matrices limited by max_value(INT2)
    uint16_t i;
    uint16_t j;
};

struct MatrixIndex { //For matrices limited by max_value(INT4)
    uint32_t i;
    uint32_t j;
};

} // namespace convex

} // namespace modules

} // namespace madlib

#endif

