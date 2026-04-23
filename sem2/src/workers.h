#pragma once

#include <chrono>
#include <print>
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

enum Log_Severity { INFO, WARN, ERROR };
typedef Log_Severity LOG;
inline std::string LOG_str(LOG severity) {
  switch (severity) {
  case LOG::INFO:
    return "INFO";
  case LOG::WARN:
    return "WARN";
  case LOG::ERROR:
    return "ERROR";
  }

  return "UNKNOWN";
}

inline void log(std::vector<Log_Entry> &log, LOG severity, std::string msg) {
  std::string m = std::format("{}: {}", LOG_str(severity), std::move(msg));
  log.push_back({std::chrono::system_clock::now(), m});
  std::println("{}", m);
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
  std::vector<Log_Entry> log;
};

// === PROCESSES

// == MASTER PROCESS

void process_master(const std::vector<std::string> &urls, std::string &output);

// process list of URLs using MPI and write informative html into output
void render_html(const Result_A &r, std::string &output);

// create a timesptamp string
std::string make_timestamp();

// sanitize url to be safe as filepath
std::string sanitize(const std::string &url);

std::string
format_time(const std::chrono::time_point<std::chrono::system_clock> &tp);

// final stage of master process
void save_to_file(const std::vector<Result_A> &websites);

// == PROCESS A

// actually do the work in A
Result_A process_A(int rank, const std::string &url);

// remove whitespaces from both sides
std::string trim(const std::string &s);

// extract just the domain from URL
// "http://test.cz/portal/info" -> "http://test.cz"
// return EMPTY STRING on failure
std::string get_domain(const std::string &url);

// from url path, resolve . and ..
std::string normalize_path(const std::string &path);

// converts ANY href link into absolute URL based on the current page
// return EMPTY STRING on failure
std::string resolve_link(const std::string &current_url,
                         const std::string &href);

// is the found link valid and should it be probed later?
bool valid_link(const std::string &base_url, const std::string &current_url,
                const std::string &found_url);

// after the work in A is done, join all the results into Result_A
void join_results_A(Result_A &r,
                    const std::unordered_map<std::string, Result_B> &processed);

// == PROCESS B

Result_B process_B(int rank, const std::string &url);

// find positions of all occurences of word inside s
std::vector<size_t> find_occurences(std::string_view s, std::string_view word);

// rewrite eg '<' to &lt, so it renders correctly
std::string escape_html(std::string_view str);

// find the contents of href="..." inside a tag
std::string find_href(std::string_view s, size_t pos);

// classify the heading, pos it the position of "<h"
Heading find_heading(std::string_view s, size_t pos);

} // namespace _detail

} // namespace worker
