#pragma once

#include <chrono>
#include <string>
#include <vector>
namespace worker {

// return value = program return value
// run the server
int master();

// decide based on rank, N and M
// if A or B should run
void non_master(int rank);

void A(int rank);

void B(int rank);

namespace _detail {

// === TAGS used for MPI communication

constexpr int TAG_URL = 1;
constexpr int TAG_RESULT_A = 2;
constexpr int TAG_RESULT_B = 3;
constexpr int TAG_KILL = 999;

struct Website_Graph {
  std::vector<std::string> uris;
  std::vector<std::tuple<std::string, std::string>> refs;
};

struct Page_Content {
  std::string uri;
  int imgs;
  int links;
  int forms;
  // depth, text
  std::vector<std::tuple<int, std::string>> headings;
};

struct Log_Entry {
  std::chrono::time_point<std::chrono::system_clock> time;
  std::string msg;
};

struct Result_A {
  Website_Graph graph;
  std::vector<Page_Content> contents;
  std::vector<Log_Entry> log;
};

struct Result_B {
  std::string url;
  int imgs;
  int links;
  int forms;
  // links to pages with the same prefix
  std::vector<std::string> found_pages;
};

// process list of URLs using MPI and write informative html into output
void process_master(const std::vector<std::string> &urls, std::string &output);

Result_A process_A(int rank, const std::string &url);

Result_B process_B(int rank, const std::string &url);

} // namespace _detail

} // namespace worker
