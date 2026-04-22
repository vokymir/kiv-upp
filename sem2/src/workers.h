#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
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

// === STRUCTS to store info

// helper for Website_Graph
struct Reference {
  std::string origin;
  std::string target;
};

struct Website_Graph {
  std::vector<std::string> uris;
  std::vector<Reference> refs;
};

// helper for Page_Content
struct Heading {
  int depth;
  std::string text;
};

struct Page_Content {
  std::string url;
  int imgs;
  int links;
  int forms;
  std::vector<Heading> headings;
};

struct Log_Entry {
  std::chrono::time_point<std::chrono::system_clock> time;
  std::string msg;
};

inline void log(std::vector<Log_Entry> &log, std::string msg) {
  log.push_back({std::chrono::system_clock::now(), std::move(msg)});
}

struct Result_A {
  Website_Graph graph;
  std::vector<Page_Content> contents;
  std::vector<Log_Entry> log;
};

struct Result_B {
  Page_Content page;
  // links to pages with the same prefix
  std::vector<std::string> found_pages;
};

// === PROCESSES

// process list of URLs using MPI and write informative html into output
void process_master(const std::vector<std::string> &urls, std::string &output);

// == PROCESS A

// actually do the work in A
Result_A process_A(int rank, const std::string &url);

// after the work in A is done, join all the results into Result_A
void join_results_A(Result_A &r,
                    const std::unordered_map<std::string, Result_B> &processed);

// == PROCESS B

Result_B process_B(int rank, const std::string &url);

// find positions of all occurences of word inside s
std::vector<size_t> find_occurences(std::string_view s,
                                    const std::string &word);

// find the contents of href="..." inside a tag
std::string find_href(std::string_view s, size_t pos);

// classify the heading, pos it the position of "<h"
Heading find_heading(std::string_view s, size_t pos);

} // namespace _detail

} // namespace worker
