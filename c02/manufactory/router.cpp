#include "router.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

namespace {
// faze nacitani souboru
enum class NLoading_Phase { Unknown, Nodes, Edges };
} // namespace

bool CRouter::Load_Graph(const std::string &file) {
  std::ifstream ifs(file);

  if (!ifs.is_open()) {
    std::cout << "Cannot be opened..." << std::endl;
    return false;
  }

  NLoading_Phase phase = NLoading_Phase::Unknown;

  std::string line;
  while (std::getline(ifs, line)) {
    if (line == "NODES") {
      phase = NLoading_Phase::Nodes;
      continue;
    } else if (line == "EDGES") {
      phase = NLoading_Phase::Edges;
      continue;
    }

    if (phase == NLoading_Phase::Nodes) {
      std::string token;
      std::istringstream iss(line);

      try {
        Graph_Node node;

        if (!std::getline(iss, token, ';')) {
          std::cout << "node #1" << std::endl;
          return false;
        }

        node.id = std::stoull(token);

        if (!std::getline(iss, token, ';')) {
          std::cout << "node #2" << std::endl;
          return false;
        }

        node.x = std::stod(token);

        if (!std::getline(iss, token, ';')) {
          std::cout << "node #3" << std::endl;
          return false;
        }

        node.y = std::stod(token);

        if (!std::getline(iss, token, ';')) {
          std::cout << "node #4" << std::endl;
          return false;
        }

        node.name = token;

        m_nodes[node.id] = node;
      } catch (std::exception &ex) {
        std::cerr << "Chyba pri nacitani uzlu: " << ex.what() << std::endl;
        return false;
      }
    } else if (phase == NLoading_Phase::Edges) {
      std::string token;
      std::istringstream iss(line);

      try {
        Graph_Edge edge;

        if (!std::getline(iss, token, ';')) {
          std::cout << "edge #1" << std::endl;
          return false;
        }

        edge.src_id = std::stoull(token);

        if (!std::getline(iss, token, ';')) {
          std::cout << "edge #2" << std::endl;
          return false;
        }

        edge.dest_id = std::stoull(token);

        if (!std::getline(iss, token, ';')) {
          std::cout << "edge #3" << std::endl;
          return false;
        }

        edge.cost = std::stod(token);

        m_nodes[edge.src_id].edges.push_back(edge);

        std::swap(edge.src_id, edge.dest_id);

        m_nodes[edge.src_id].edges.push_back(edge);
      } catch (std::exception &ex) {
        std::cerr << "Chyba pri nacitani hrany: " << ex.what() << std::endl;
        return false;
      }
    } else if (phase == NLoading_Phase::Unknown) {
      std::cout << "Unknown phase" << std::endl;
      return false;
    }
  }

  return true;
}

bool CRouter::Optimize_Node_Order(const std::list<size_t> &nodeIds,
                                  size_t inputNode) {
  std::set<size_t> pending(nodeIds.begin(), nodeIds.end());

  m_optimizedNodeOrder.clear();

  auto get_distance = [this](size_t nsrc, size_t ndst) {
    return std::sqrt(std::pow(m_nodes[nsrc].x - m_nodes[ndst].x, 2.0) +
                     std::pow(m_nodes[nsrc].y - m_nodes[ndst].y, 2.0));
  };

  size_t currentNode = inputNode;
  while (!pending.empty()) {
    size_t bestNode = InvalidNode;
    double bestMetric = std::numeric_limits<double>::max();
    for (auto &id : pending) {
      auto dist = get_distance(currentNode, id);
      if (dist < bestMetric) {
        bestNode = id;
        bestMetric = dist;
      }
    }

    if (bestNode == InvalidNode)
      return false;

    pending.erase(bestNode);
    m_optimizedNodeOrder.push_back(bestNode);

    currentNode = bestNode;
  }

  return true;
}

bool CRouter::Generate_Path(const std::list<size_t> nodeIds, size_t inputNode) {
  auto clear_temps = [this]() {
    for (auto &n : m_nodes) {
      n.second.discovered_from = InvalidNode;
      n.second.path_metric = std::numeric_limits<double>::max();
    }
  };

  auto find_path = [this, &clear_temps](size_t from, size_t to) {
    clear_temps();

    std::list<size_t> discovered{from};
    m_nodes[from].path_metric = 0;

    while (!discovered.empty()) {
      size_t selected = discovered.front();
      size_t next = InvalidNode;
      double nextcost = std::numeric_limits<double>::max();

      for (size_t s : discovered) {
        for (auto &e : m_nodes[s].edges) {
          if (m_nodes[e.dest_id].discovered_from == InvalidNode &&
              m_nodes[s].path_metric + e.cost < nextcost) {
            selected = s;
            next = e.dest_id;
            nextcost = m_nodes[s].path_metric + e.cost;
            break;
          }
        }
      }

      for (auto itr = discovered.begin(); itr != discovered.end();) {
        bool hasedges = false;
        for (auto &e : m_nodes[*itr].edges) {
          if (m_nodes[e.dest_id].discovered_from == InvalidNode) {
            hasedges = true;
            break;
          }
        }

        if (!hasedges)
          itr = discovered.erase(itr);
        else
          ++itr;
      }

      if (next != InvalidNode) {
        discovered.push_back(next);
        m_nodes[next].discovered_from = selected;
        m_nodes[next].path_metric = nextcost;

        if (next == to)
          break;
      }
    }

    if (m_nodes[to].discovered_from == InvalidNode)
      return Path();

    Path pt;
    pt.total_cost = m_nodes[to].path_metric;

    pt.prepend(to);

    while (pt.node_ids.front() != from)
      pt.prepend(m_nodes[pt.node_ids.front()].discovered_from);

    return pt;
  };

  Path total;
  total.total_cost = 0;

  size_t curNode = inputNode;
  for (size_t node : nodeIds) {
    auto res = find_path(curNode, node);
    curNode = node;

    if (res.empty())
      continue;

    total += res;
  }

  Path best = total;

  m_path = best.node_ids;
  m_totalCost = best.total_cost;

  m_warehouses = std::set<size_t>(nodeIds.begin(), nodeIds.end());
  m_internodes = std::set<size_t>(m_path.begin(), m_path.end());

  return true;
}

bool CRouter::Draw_Path() {
  if (m_warehouses.empty() || m_internodes.empty() || m_path.empty())
    return false;

  auto is_warehouse = [this](const size_t id) {
    return (m_warehouses.find(id) != m_warehouses.end());
  };
  auto is_internode = [this](const size_t id) {
    return (m_internodes.find(id) != m_internodes.end());
  };

  std::ostringstream svg_out;

  constexpr size_t Grid_Size_X = 100;
  constexpr size_t Grid_Size_Y = 100;

  constexpr double Grid_Spacing_X = 1.0;
  constexpr double Grid_Spacing_Y = 1.0;

  constexpr double Grid_Margin_X = 0.05;
  constexpr double Grid_Margin_Y = 0.05;

  constexpr double Scaling = 10.0;

  svg_out << "<svg width=\"" << Grid_Size_X * Grid_Spacing_X * Scaling
          << "\" height=\"" << Grid_Size_Y * Grid_Spacing_Y * Scaling
          << "\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;
  {
    svg_out << "<g>" << std::endl;
    for (const auto &n : m_nodes) {
      const std::string &col =
          is_warehouse(n.second.id)
              ? "red"
              : (is_internode(n.second.id) ? "blue" : "black");

      svg_out << "<circle cx=\"" << static_cast<int>(n.second.x * Scaling)
              << "\" cy=\"" << static_cast<int>(n.second.y * Scaling)
              << "\" r=\"" << (is_warehouse(n.second.id) ? 3 : 2)
              << "\" fill=\"" << col << "\"><title>" << n.second.id << " - "
              << n.second.name << "</title></circle>" << std::endl;

      for (const auto &e : n.second.edges) {
        const Graph_Node &n1 = m_nodes[e.src_id];
        const Graph_Node &n2 = m_nodes[e.dest_id];

        svg_out << "<line x1=\"" << static_cast<int>(n1.x * Scaling)
                << "\" y1=\"" << static_cast<int>(n1.y * Scaling) << "\" x2=\""
                << static_cast<int>(n2.x * Scaling) << "\" y2=\""
                << static_cast<int>(n2.y * Scaling)
                << "\" stroke-width=\"0.5\" stroke=\"" << col << "\" />"
                << std::endl;
      }
    }
    svg_out << "</g>" << std::endl;
  }
  svg_out << "</svg>" << std::endl;

  m_pathSvg = svg_out.str();

  return true;
}
