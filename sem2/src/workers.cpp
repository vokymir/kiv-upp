
#include "workers.h"
#include "config.h"
#include "server.h"
#include "utils.h"
#include <iostream>
#include <mpi.h>
#include <queue>
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

  while (true) {

    int employer = cfg::employer(rank);
    std::string url = utils::mpi::recv_string(employer, _detail::TAG_URL);

    _detail::Result_A result = _detail::process_A(rank, url);

    // TODO: send result of A to master
    // utils::mpi::send_string(result, employer, _detail::TAG_RESULT_A);
  }
}

void B(int rank) {

  while (true) {
    int employer = cfg::employer(rank);
    std::string url = utils::mpi::recv_string(employer, _detail::TAG_URL);

    _detail::Result_B result = _detail::process_B(rank, url);

    // TODO: send result of B to A
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
  output = "Zadali jste: <ul>";

  for (int i = 0; i < urls.size(); i++) {
    int src = MPI_ANY_SOURCE; // might be cfg::assign_A(i) - and that is precise
    std::string res = utils::mpi::recv_string(src, TAG_RESULT_A);
    output += res + "<br/>";
  }

  output += "</ul>";
}

Result_A process_A(int rank, const std::string &original_url) {
  std::unordered_map<std::string, Result_B> processed;
  std::queue<std::string> queue;

  queue.push(original_url);

  int sent = 0;
  int done = 0;
  // don't end on first run, wait until queue is empty and no B is working
  while (sent == 0 || sent != done) {

    // receive all enqueued work and fill the queue with new
    while (done < sent) {
      int worker = cfg::assign_B(rank, done++);
      // TODO: receive from worker
      Result_B res = {};

      // if one page is processed multiple times, don't do duplicates
      // (it might happen if links to that page are found before it is
      // processed)
      if (processed.contains(res.url)) {
        continue;
      }
      processed[res.url] = res;

      // enqueue found pages
      for (const auto &found_url : res.found_pages) {
        // only if it already isn't done
        if (!processed.contains(found_url)) {
          queue.push(found_url);
        }
      }
    }

    // flush the queue to workers
    while (!queue.empty()) {
      std::string url = queue.front();
      queue.pop();

      int worker = cfg::assign_B(rank, sent++);
      utils::mpi::send_string(url, worker, TAG_URL);
    }
  }

  return {};
}

Result_B process_B(int rank, const std::string &url) { return {}; }

} // namespace _detail

} // namespace worker
