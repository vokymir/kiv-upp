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

// only master can assign work to A
// i is some iteration, for fair distribution
inline int assign_A(int i) { return idx_A_first() + (i % N); }

// only A can assign work to B
// i is some iteration, so it is fairly distributed
inline int assign_B(int rank_A, int i) {
  // how much offset because of A
  int offset_of_A_group = (rank_A - idx_A_first()) * M;
  // start index of A's group
  int start_of_group = idx_B_first() + offset_of_A_group;
  // don't overshoot the group
  int employee = start_of_group + (i % M);

  return employee;
}

// who to send direct message if I am done? whom to listen to?
inline int employer(int rank) {
  if (is_master(rank)) {
    return -1;

  } else if (is_worker_A(rank)) {
    return idx_master();

  } else if (is_worker_B(rank)) {
    int idx = rank - idx_B_first();
    // integer division = floor
    int group = idx / M;
    // group is 0-indexed, so it works
    int employer = group + idx_A_first();

    return employer;

  } else {
    return -2;
  }
}

} // namespace cfg
