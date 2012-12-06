/* ----------------------------------------------------------------------- *//**
 *
 * @file lmfigd.hpp
 *
 * Specific implementaion of incremental gradient descent for low rank matrix
 * factorization, in the fashion of user-definied aggregates.
 * They should be called by actually database functions,
 * after arguments are properly parsed.
 *
 *//* ----------------------------------------------------------------------- */
#ifndef MADLIB_MODULES_CONVEX_ALGO_LMFIGD_HPP_
#define MADLIB_MODULES_CONVEX_ALGO_LMFIGD_HPP_

#include "igd.hpp"

namespace madlib {

namespace modules {

namespace convex {

// Use Eigen
using namespace dbal::eigen_integration;

// The reason for using ConstState instead of const State to reduce the
// template type list: flexibility to high-level for mutability control
// More: cast<ConstState>(MutableState) may not always work
template <class State, class ConstState, class Task>
class LMFIGD: public IGD<State, ConstState, Task> {
public:
    typedef State state_type;
    typedef ConstState const_state_type;
    typedef typename Task::tuple_type tuple_type;
    typedef typename HandleTraits<MutableArrayHandle<double> >
                    ::MatrixTransparentHandleMap MutableMatrix;
    typedef typename HandleTraits<ArrayHandle<double> >
                    ::MatrixTransparentHandleMap ImmutableMatrix;
    typedef typename HandleTraits<MutableArrayHandle<double> >
                    ::ColumnVectorTransparentHandleMap MutableColumnVector;
    typedef typename HandleTraits<ArrayHandle<double> >
                    ::ColumnVectorTransparentHandleMap ImmutableColumnVector;

    static void transition(state_type &state, const tuple_type &tuple);
    static void merge(state_type &state, const_state_type &otherState);
    static void final(state_type &state);

    static void combine_param_matrix(MutableColumnVector &updated_rows,
                                MutableMatrix &state_matrix,
                                ImmutableColumnVector &other_updated_rows,
                                ImmutableMatrix &other_matrix );
    static void update_vector_counts(state_type &state,
                                     const_state_type &otherState);
};

template <class State, class ConstState, class Task>
void
LMFIGD<State, ConstState, Task>::transition(state_type &state,
                                         const tuple_type &tuple) {
    IGD<State, ConstState, Task>::transition(state, tuple);
    state.algo.updated_U_rows[tuple.indVar.i]++;
    state.algo.updated_V_rows[tuple.indVar.j]++;
}

template <class State, class ConstState, class Task>
void
LMFIGD<State, ConstState, Task>::merge(state_type &state,
        const_state_type &otherState) {
    /* Overriding the parent merge function to incorporate model averaging
        using updated row counts for each row of the model instead of the
        number of updates for complete model
    */
    // Having zero checking here to reduce dependency to the caller.
    // This can be removed if it affects performance in the future,
    // with the expectation that callers should do the zero checking.
    if (otherState.algo.numRows == 0) {
        return;
    }
    if (state.algo.numRows == 0) {
        state.algo.incrModel = otherState.algo.incrModel;
        return;
    }
    // model averaging
    combine_param_matrix(   state.algo.updated_U_rows
                            ,state.algo.incrModel.matrixU
                            ,otherState.algo.updated_U_rows
                            ,otherState.algo.incrModel.matrixU);
                            // ,state.algo.u_mutex_merges);

    combine_param_matrix(   state.algo.updated_V_rows
                            ,state.algo.incrModel.matrixV
                            ,otherState.algo.updated_V_rows
                            ,otherState.algo.incrModel.matrixV);
                            // ,state.algo.u_mutex_merges);
    update_vector_counts(state, otherState);
}

template <class State, class ConstState, class Task>
void
LMFIGD<State, ConstState, Task>::final(state_type &state) {
   IGD<State, ConstState, Task>::final(state);
}

template <class State, class ConstState, class Task>
void
LMFIGD<State, ConstState, Task>::combine_param_matrix(
                                MutableColumnVector &updated_rows,
                                MutableMatrix &state_matrix,
                                ImmutableColumnVector &other_updated_rows,
                                ImmutableMatrix &other_matrix ){
                                /* DEBUG */
                                // ,utils::MutableReference<double, uint64_t> mutex_count){

    // Output = (w1 * m1 + w2 * m2) / (w1 + w2): where w1, w2 are vectors
    // with no non-zero elements.
    // Since m2 is immutable, we change it to
    // (m1 * w1 / w2  + m2) * w2 / (w1 + w2).
    ColumnVector weight_ratio = updated_rows.cwiseQuotient(
                                    other_updated_rows);   // w1/w2
    // Use .asDiagonal() to perform broadcast product between matrix and vector.
    //  eg. 'm * v.asDiagonal()' multiplies m[:, 0] with v[0], m[:, 1] with v[1]
    //      Similarly, 'v.asDiagonal() * m' multiplies m[0, :] with v[0], ...
    state_matrix = weight_ratio.asDiagonal() * state_matrix;
    state_matrix += other_matrix;
    ColumnVector total_updated_rows = updated_rows +
                                        other_updated_rows;    // w1 + w2
    ColumnVector other_weight_ratio =
                            other_updated_rows.cwiseQuotient(
                                        total_updated_rows);  // w2/(w1 + w2)
    state_matrix = other_weight_ratio.asDiagonal() * state_matrix;
    // mutex_count += (total_updated_rows.count() == 2);
}

template <class State, class ConstState, class Task>
void
LMFIGD<State, ConstState, Task>::update_vector_counts(
                                        state_type &state,
                                        const_state_type &otherState) {

    /* Since we are taking a weighted average to merge model matrices,
     *  it is not clear what is an appropriate form of merge for the weights.
     *  For now, we take the max of the weights as the merged weight.
    */
    state.algo.updated_U_rows = state.algo.updated_U_rows.cwiseMax(
                                            otherState.algo.updated_U_rows);

    state.algo.updated_V_rows = state.algo.updated_V_rows.cwiseMax(
                                            otherState.algo.updated_V_rows);
}

} // namespace convex

} // namespace modules

} // namespace madlib

#endif
