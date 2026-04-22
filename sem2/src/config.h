#pragma once

// global configuration - because passing N,M was messy
namespace cfg {

inline int N = 0;
inline int M = 0;

// Example for N=3, M=2
// rank:      0  1  2  3  4  5  6  7  8  9
// role:      M  A1 A2 A3 B1 B2 B3 B4 B5 B6
// owner:     -  M  M  M  A1 A1 A2 A2 A3 A3

// == HARD CONFIG ==

inline int idx_master() { return 0; }

inline int idx_A_first() { return 1; }
inline int idx_A_last() { return N; }

inline int idx_B_first() { return N + 1; }
inline int idx_B_last() { return N + N * M; }

// == USEFUL GETTERS ==

inline bool is_master(int rank) { return rank == idx_master(); }

inline bool is_worker_A(int rank) {
  return idx_A_first() <= rank && rank <= idx_A_last();
}

inline bool is_worker_B(int rank) {
  return idx_B_first() <= rank && rank <= idx_B_last();
}

// == ACTIONABLE GETTERS ==

inline int assign_A(int i) { return idx_A_first() + (i % N); }

} // namespace cfg
