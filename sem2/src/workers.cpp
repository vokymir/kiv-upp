
#include "workers.h"
#include "config.h"
#include "server.h"
#include "utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <queue>
#include <string_view>
#include <unordered_set>
namespace worker {

int master() {

  // inicializace serveru
  CServer svr;
  if (!svr.Init("./data", "0.0.0.0", 8001)) {
    std::cerr << "Nelze inicializovat server!" << std::endl;
    return EXIT_FAILURE;
  }

  // registrace callbacku pro zpracovani odeslanych URL
  // these shananigans are only to avoid global state/config which would hold
  // M and N
  svr.RegisterFormCallback(_detail::process_master);

  // spusteni serveru
  return svr.Run() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void non_master(int rank) {

  if (cfg::is_worker_A(rank)) {
    A(rank);

  } else {
    B(rank);
  }
}

void A(int rank) {
  int employer = cfg::employer(rank);

  while (true) {

    std::string url = utils::mpi::recv_string(employer, _detail::TAG_URL);

    _detail::Result_A result = _detail::process_A(rank, url);

    utils::mpi::send_result_A(result, employer);
  }
}

void B(int rank) {
  int employer = cfg::employer(rank);

  while (true) {
    std::string url = utils::mpi::recv_string(employer, _detail::TAG_URL);

    _detail::Result_B result = _detail::process_B(rank, url);

    utils::mpi::send_result_B(result, employer);
  }
}

namespace _detail {

void process_master(const std::vector<std::string> &urls, std::string &output) {

  // divide the work
  for (int i = 0; i < static_cast<int>(urls.size()); ++i) {
    int worker = cfg::assign_A(i);
    utils::mpi::send_string(urls[i], worker, TAG_URL);
  }

  // conquer the results
  std::vector<Result_A> results;
  output += "<ul>";

  for (int i = 0; i < urls.size(); i++) {
    int worker = cfg::assign_A(i);
    Result_A res = utils::mpi::recv_result_A(worker);
    render_html(res, output);
    results.push_back(res);
  }

  output += "</ul>";

  save_to_file(results);
}

void render_html(const Result_A &r, std::string &output) {
  output += "<li>";
  output += "<h3>Root URL: " + r.contents[0].url + "</h3>";

  // pages
  output += "<h4>Pages</h4><ul>";

  for (const auto &page : r.contents) {
    output += "<li>";
    output += "<b>" + page.url + "</b><br>";

    output += "Images: " + std::to_string(page.imgs) + "<br>";
    output += "Links: " + std::to_string(page.links) + "<br>";
    output += "Forms: " + std::to_string(page.forms) + "<br>";

    // headings
    output += "<u>Headings:</u><br>";
    for (const auto &h : page.headings) {
      output += std::to_string(h.depth);
      output += ": " + h.text + "<br>";
    }

    output += "</li>";
  }

  output += "</ul>";

  // Graph
  output += "<h4>Graph</h4>";

  // nodes
  output += "<b>Nodes:</b><ul>";
  for (const auto &uri : r.graph.uris) {
    output += "<li>" + uri + "</li>";
  }
  output += "</ul>";

  // edges
  output += "<b>Edges:</b><ul>";
  for (const auto &ref : r.graph.refs) {
    output += "<li>" + ref.origin + " -- " + ref.target + "</li>";
  }
  output += "</ul>";

  // log
  output += "<h4>Log</h4><ul>";
  for (const auto &l : r.log) {
    output += "<li>" + l.msg + "</li>";
  }
  output += "</ul>";

  output += "</li>";
}

std::string make_timestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);

  std::tm tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y_%m_%d_%H_%M");
  return oss.str();
}

std::string sanitize(const std::string &url) {
  std::string out;
  for (char c : url) {
    if (std::isalnum(c)) {
      out += c;
    } else {
      out += '_';
    }
  }
  return out;
}

std::string
format_time(const std::chrono::time_point<std::chrono::system_clock> &tp) {
  std::time_t t = std::chrono::system_clock::to_time_t(tp);
  std::tm tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

void save_to_file(const std::vector<Result_A> &websites) {
  namespace fs = std::filesystem;

  fs::create_directories("results");

  for (const auto &site : websites) {
    // == create dir
    std::string timestamp = make_timestamp();

    std::string base = "unknown";
    if (!site.contents.empty()) {
      base = sanitize(site.contents.front().url);
    }

    std::string dir = "results/" + timestamp + "_" + base;
    fs::create_directories(dir);

    // == map.txt
    {
      std::ofstream f(dir + "/map.txt");

      // nodes
      for (const auto &uri : site.graph.uris) {
        f << "\"" << uri << "\"\n";
      }

      // edges
      for (const auto &ref : site.graph.refs) {
        f << "\"" << ref.origin << "\" \"" << ref.target << "\"\n";
      }
    }

    // == content.txt
    {
      std::ofstream f(dir + "/content.txt");

      for (const auto &page : site.contents) {
        f << "\"" << page.url << "\"\n";
        f << "IMAGES " << page.imgs << "\n";
        f << "LINKS " << page.links << "\n";
        f << "FORMS " << page.forms << "\n";

        for (const auto &h : page.headings) {
          for (int i = 0; i < h.depth; ++i)
            f << "-";
          f << " " << h.text << "\n";
        }

        f << "\n";
      }
    }

    // == log.txt
    {
      std::ofstream f(dir + "/log.txt");

      if (site.log.empty()) {
        f << "EMPTY LOG\n";

      } else {
        bool has_error = false;
        for (const auto &entry : site.log) {
          f << format_time(entry.time) << " " << entry.msg << "\n";
          if (entry.msg.find("ERROR") != std::string::npos) {
            has_error = true;
          }
        }

        if (!has_error) {
          f << "OK\n";
        }
      }
    }
  }
}

Result_A process_A(int rank, const std::string &original_url) {
  Result_A r;
  std::unordered_map<std::string, Result_B> processed;
  std::unordered_set<std::string> in_progress;
  std::queue<std::string> queue;

  std::string base_url = trim(original_url);
  base_url = base_url.ends_with('/') ? base_url : base_url + "/";
  queue.push(base_url);
  log(r.log, LOG::INFO,
      std::format("[A {}] Start processing {}", rank, base_url));

  int sent = 0;
  int done = 0;
  // don't end on first run, wait until queue is empty and no B is working
  while (sent == 0 || sent != done) {

    // receive all enqueued work and fill the queue with new
    while (done < sent) {
      int worker = cfg::assign_B(rank, done++);
      Result_B res = utils::mpi::recv_result_B(worker);
      log(r.log, LOG::INFO,
          std::format("[A {}] Received done work from {}, the status is now "
                      "done/sent: {}/{}",
                      rank, worker, done, sent));

      // remove from in_progress, add to processed and get reference to it
      in_progress.erase(res.page.url);
      auto [it, inserted] = processed.emplace(res.page.url, std::move(res));
      if (!inserted) {
        log(r.log, LOG::ERROR,
            std::format("[A {}] The page already exist: {}", rank,
                        res.page.url));
        continue;
      }
      auto &stored = it->second;

      // filter found pages and enque them for further search
      std::vector<std::string> filtered;
      for (const auto &found_url : stored.found_pages) {
        std::string abs_url = resolve_link(stored.page.url, found_url);

        bool already_seen =
            (processed.contains(abs_url) || in_progress.contains(abs_url));
        bool valid = valid_link(base_url, stored.page.url, abs_url);

        if (!already_seen && valid) {
          log(r.log, LOG::INFO,
              std::format("[A {}] Enqueuing {}", rank, abs_url));

          queue.push(abs_url);
          filtered.push_back(abs_url);

        } else {
          log(r.log, LOG::INFO,
              std::format("[A {}] NOT enqueuing {}: {} {}", rank, abs_url,
                          already_seen ? "already seen" : "",
                          !valid ? "invalid" : ""));
        }
      }

      stored.found_pages = std::move(filtered);
    }

    // flush the queue to workers
    while (!queue.empty()) {
      std::string url = queue.front();
      queue.pop();

      // must be here, because we don't watch what is in queue
      bool already_seen =
          (processed.contains(url) || in_progress.contains(url));
      if (already_seen) {
        continue;
      }

      int worker = cfg::assign_B(rank, sent++);
      utils::mpi::send_string(url, worker, TAG_URL);
      in_progress.insert(url);
      log(r.log, LOG::INFO,
          std::format("[A {}] Sent to worker {} page {}", rank, worker, url));
    }
  }

  join_results_A(r, processed);

  log(r.log, LOG::INFO,
      std::format("[A {}] Ending page processing", rank)); // end

  return r;
}

std::string trim(const std::string &s) {
  const std::string whitespace = " \t\n\r\f\v";

  size_t start = s.find_first_not_of(whitespace);
  if (start == std::string::npos) {
    return "";
  }

  size_t end = s.find_last_not_of(whitespace);

  return s.substr(start, end - start + 1);
}

std::string get_domain(const std::string &url) {
  size_t pos = url.find("://");

  if (pos == std::string::npos) {
    return ""; // must have protocol
  }

  // the first slash after "://"
  pos = url.find('/', pos + 3);

  if (pos == std::string::npos) {
    return url; // eg. "https://google.com" - without trailing slash
  }

  return url.substr(0, pos); // no trailing slash
}

std::string normalize_path(const std::string &path) {
  std::vector<std::string> parts;
  size_t pos = 0;

  while (pos < path.size()) {
    size_t next_pos = path.find('/', pos);
    if (next_pos == std::string::npos) {
      next_pos = path.size();
    }

    std::string part = path.substr(pos, next_pos - pos);

    // increment for next round
    pos = next_pos + 1;

    // allow double slash in path
    if (part.empty()) {
      continue;
    }

    if (part == "..") {
      parts.pop_back();

    } else if (part != ".") {
      parts.push_back(part);
    }
  }

  std::string result = "/";
  for (size_t k = 0; k < parts.size(); ++k) {
    result += parts[k];

    if (k + 1 < parts.size()) {
      result += "/";
    }
  }

  return result;
}

std::string resolve_link(const std::string &current_url,
                         const std::string &link) {
  // skip empty links, anchors, or javascript
  if (link.empty() || link.starts_with("#") ||
      link.starts_with("javascript:")) {
    return "";
  }

  // ABSOLUTE

  // already absolute URL
  if (link.starts_with("http://") || link.starts_with("https://")) {
    return link;
  }

  // RELATIVE

  std::string domain = get_domain(current_url);
  std::string full;

  // relative link from root (starts with '/')
  // example:
  // current =  http://test.cz/portal/info
  // link =     /about
  // ->         http://test.cz/about
  if (link.starts_with("/")) {
    full = domain + link;

    // relative link
    // example:
    // current =  http://test.cz/portal/info
    // link =     contact.html
    // ->         http://test.cz/portal/contact.html
  } else {
    size_t last_slash = current_url.rfind('/');

    // have trailing slash
    if (current_url.ends_with('/')) {
      full = current_url + link;

      // the url has a path, but no trailing slash
      // example:
      // current =  http://test.cz/portal/info
      // link =     contact.html
      // ->         http://test.cz/portal/contact.html
    } else if (last_slash != std::string::npos && last_slash >= domain.size()) {
      full = current_url.substr(0, last_slash + 1) + link;

      // the only slashes are in "://"
      // example: https://github.com
    } else {
      full = current_url + "/" + link;
    }
  }

  // NORMALIZE the path, no matter its origin

  size_t path_start = full.find('/', domain.size());
  if (path_start == std::string::npos) {
    return full;
  }

  std::string path = full.substr(path_start);
  return domain + normalize_path(path);
}

bool valid_link(const std::string &base_url, const std::string &current_url,
                const std::string &found_url) {
  std::string absolute_link = resolve_link(current_url, found_url);

  return !absolute_link.empty() && absolute_link.starts_with(base_url);
}

void join_results_A(
    Result_A &r, const std::unordered_map<std::string, Result_B> &processed) {
  r.contents.reserve(processed.size());
  r.graph.uris.reserve(processed.size());

  for (const auto &[url, res] : processed) {

    // Page_Content already done by B
    r.contents.push_back(res.page);

    // Website_Graph almost done by B
    r.graph.uris.push_back(url);

    for (const auto &target : res.found_pages) {
      r.graph.refs.push_back({url, target});
    }

    // get all logs
    for (const auto &log : res.log) {
      r.log.push_back(log);
    }
  }

  // SORT everything

  // sort contents by URL
  std::sort(r.contents.begin(), r.contents.end(),
            [](const Page_Content &a, const Page_Content &b) {
              return a.url < b.url;
            });

  // sort graph as strings
  std::sort(r.graph.uris.begin(), r.graph.uris.end());
  std::sort(r.graph.refs.begin(), r.graph.refs.end(),
            [](const Reference &a, const Reference &b) {
              if (a.origin != b.origin) {
                return a.origin < b.origin;
              }
              return a.target < b.target;
            });

  // sort logs by time
  std::sort(
      r.log.begin(), r.log.end(),
      [](const Log_Entry &a, const Log_Entry &b) { return a.time < b.time; });
}

Result_B process_B(int rank, const std::string &url) {
  Result_B r;
  log(r.log, LOG::INFO,
      std::format("[B {}] Starts working now, url = {}", rank, url));

  const std::string contents = utils::downloadHTML(url);
  const std::string_view contents_sv{contents.data(), contents.size()};

  if (contents.empty()) {
    log(r.log, LOG::ERROR,
        std::format("[B {}] Cannot download HTML page {}", rank, url));
    return r;
  }

  r.page.url = url;
  r.page.imgs = find_occurences(contents_sv, "<img").size();
  r.page.forms = find_occurences(contents_sv, "<form").size();

  auto links = find_occurences(contents_sv, "<a");
  r.page.links = links.size();

  // fill all nonempty found links
  // filtering is done on A
  for (const auto &link_pos : links) {
    std::string link = find_href(contents_sv, link_pos);

    if (!link.empty()) {
      r.found_pages.push_back(link);
    }
  }

  // fill all found headings
  auto headings = find_occurences(contents_sv, "<h");
  log(r.log, LOG::INFO,
      std::format("[B {}] Found {} headings", rank, headings.size()));
  for (const auto &heading_pos : headings) {
    auto heading = find_heading(contents_sv, heading_pos);
    if (heading.text.empty()) {
      continue; // skip invalid headings
    }
    log(r.log, LOG::INFO,
        std::format("[B {}] Found heading: h{}: {}", rank, heading.depth,
                    heading.text));
    r.page.headings.push_back(heading);
  }

  log(r.log, LOG::INFO,
      std::format("[B {}] Done working now, url = {}", rank, url));
  return r;
}

std::vector<size_t> find_occurences(std::string_view s, std::string_view word) {
  std::vector<size_t> occs;

  size_t pos = 0;
  size_t len = word.size();

  while ((pos = s.find(word, pos)) != std::string_view::npos) {
    occs.push_back(pos);
    pos += len; // skip the word
  }

  return occs;
}

std::string escape_html(std::string_view str) {
  std::string buffer;
  buffer.reserve(str.size());
  for (size_t pos = 0; pos != str.size(); ++pos) {
    switch (str[pos]) {
    case '&':
      buffer.append("&amp;");
      break;
    case '\"':
      buffer.append("&quot;");
      break;
    case '\'':
      buffer.append("&apos;");
      break;
    case '<':
      buffer.append("&lt;");
      break;
    case '>':
      buffer.append("&gt;");
      break;
    default:
      buffer.push_back(str[pos]);
      break;
    }
  }
  return buffer;
}

std::string find_href(std::string_view s, size_t pos) {
  size_t end_pos = s.find(">", pos);
  size_t href_pos = s.find("href=", pos);

  if (end_pos == std::string_view::npos || href_pos == std::string_view::npos ||
      href_pos > end_pos) {
    return {};
  }

  size_t link_start_pos = href_pos + 5 + 1; // 5 for href=, 1 for "
  size_t link_end_pos = s.find('"', link_start_pos);
  if (link_end_pos == std::string_view::npos) {
    return {};
  }

  return std::string(s.substr(link_start_pos, link_end_pos - link_start_pos));
}

Heading find_heading(std::string_view s, size_t pos) {
  Heading h{};
  size_t depth_pos = pos + 2;

  // must have "<hX"
  if (depth_pos >= s.size()) {
    return h;
  }

  char d = s[depth_pos];
  if ('1' > d || d > '6') {
    return h; // invalid, may be eg <html, <header, ...
  }

  h.depth = d - '0';

  // find end of opening tag
  size_t tag_end = s.find('>', depth_pos);
  if (tag_end == std::string_view::npos) {
    return h;
  }

  size_t start_pos = tag_end + 1;

  // find specific closing tag
  size_t end_pos = s.find(std::format("</h{}", d), start_pos);
  if (end_pos == std::string_view::npos) {
    return h;
  }

  // extract text
  h.text = std::string(s.substr(start_pos, end_pos - start_pos));

  return h;
}

} // namespace _detail

} // namespace worker
