#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
namespace worker {

// run the server, register callback for when URLs submitted
// return value = program return value
int master();

// decide based on rank, N and M
// if A or B should run
void non_master(int rank);

// main A loop where it waits for message and send result back
// inside is called another function which takes care about the work
void A(int rank);

// main B loop where it waits for message and send result back
// inside is called another function which takes care about the work
void B(int rank);

namespace _detail {

// === TAGS used for MPI communication
enum TAG {
  URL,
  RESULT_A,
  RESULT_B,
  KILL // not used (yet)
};

// === STRUCTS to store info

// found reference on any page
struct Reference {
  std::string origin; // page on which it was found
  std::string target; // where the link leads
};

// store all pages found on website (urls)
// and all crossreferences
struct Website_Graph {
  std::vector<std::string> urls;
  std::vector<Reference> refs;
};

// represent any heading
struct Heading {
  int depth; // 1-6, eg <h1>
  std::string text;
};

// what is present on the page
struct Page_Content {
  std::string url;               // url of this page
  int imgs;                      // images count
  int links;                     // <a ...> count (even if have invalid href)
  int forms;                     // forms count
  std::vector<Heading> headings; // all headings in order from top to bottom
};

// what happened and when
struct Log_Entry {
  std::chrono::time_point<std::chrono::system_clock> time;
  std::string msg;
};

// helper section for logging - predefined severity
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

// helper for storing log entry - where to put it, severity and message
inline void log(std::vector<Log_Entry> &log, LOG severity, std::string msg) {
  std::string m = std::format("{}: {}", LOG_str(severity), std::move(msg));
  log.push_back({std::chrono::system_clock::now(), m});
}

// represent one crawled website, the result of worker A work
struct Result_A {
  std::string base_url;
  Website_Graph graph;
  std::vector<Page_Content> contents;
  std::vector<Log_Entry> log;
};

// represent one crawled page
struct Result_B {
  Page_Content page;
  std::vector<std::string> found_pages; // all found links
  std::vector<Log_Entry> log; // logs what happened when gathering Result_B
};

// === PROCESSES

// == MASTER PROCESS

// the callback for processing list of URLs
// divide work between workers A, save results into files (as per assignment)
// and store them also in output as HTML
void process_master(const std::vector<std::string> &urls, std::string &output);

// write nicely styled HTML information from r to output
void render_html(const Result_A &r, std::string &output);

// create a timesptamp string
// useful when creating directory which needs to have current time
std::string make_timestamp();

// sanitize url to be safe as filepath
std::string sanitize(const std::string &url);

// format time to be same as in the assignment
std::string
format_time(const std::chrono::time_point<std::chrono::system_clock> &tp);

// save all websites into the filestructure as per assignment
// - results/
//   - <datetime>_<input_url>/
//     - map.txt
//     - content.txt
//     - log.txt
void save_to_file(const std::vector<Result_A> &websites);

// == PROCESS A

// actually do the work in A
// in loop assign work to B, get their results and enqueue newly found links
// when all work of B is done, join and sort the results and return them
Result_A process_A(int rank, const std::string &url);

// remove whitespaces from both sides of string
std::string trim(const std::string &s);

// extract just the domain from URL
// "http://test.cz/portal/info" -> "http://test.cz"
// return EMPTY STRING on failure
std::string get_domain(const std::string &url);

// from url path, resolve . and ..
// https://website/portal/./paper/../dog
// -> https://website/portal/dog
std::string normalize_path(const std::string &path);

// converts ANY href link into absolute URL based on the current page
// return EMPTY STRING on failure
std::string resolve_link(const std::string &current_url,
                         const std::string &href);

// is the found link valid and should it be probed later?
// valid means, if it starts with base_url and other small thingies
bool valid_link(const std::string &base_url, const std::string &current_url,
                const std::string &found_url);

// after the work in A is done, join all the results into Result_A
// also sort them to be sensible
void join_results_A(Result_A &r,
                    const std::unordered_map<std::string, Result_B> &processed);

// == PROCESS B

// download page, find all tags, save and count them
// return Result_B, if error happened, it can be found in log
Result_B process_B(int rank, const std::string &url);

// find positions of all occurences of word inside s
std::vector<size_t> find_occurences(std::string_view s, std::string_view word);

// rewrite eg '<' to &lt, so it renders correctly in web browser
std::string escape_html(std::string_view str);

// find the contents of href="..." inside a <a> tag
// pos is the position of "<a"
std::string find_href(std::string_view s, size_t pos);

// classify the heading, pos it the position of "<h"
Heading find_heading(std::string_view s, size_t pos);

} // namespace _detail

} // namespace worker
