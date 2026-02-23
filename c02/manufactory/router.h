#pragma once

#include <string>
#include <list>
#include <map>
#include <limits>
#include <set>

/*
 * Trida obstaravajici praci s mapou a uzly, hledani cesty, apod.
 */
class CRouter final
{
	private:
		// konstanta pro "neplatny uzel"
		static constexpr size_t InvalidNode = ~static_cast<size_t>(0);

	public:
		// struktura hrany grafu
		struct Graph_Edge
		{
			size_t src_id;
			size_t dest_id;
			double cost;
		};

		// struktura uzlu grafu
		struct Graph_Node
		{
			// ID uzlu
			size_t id;
			// X souradnice uzlu
			double x;
			// Y souradnice uzlu
			double y;
			// nazev uzlu
			std::string name;

			// hrany z tohoto uzlu vychazejici
			std::list<Graph_Edge> edges;

			// pomocna promenna - odkud byl uzel objeven? InvalidNode pokud objeven nebyl
			size_t discovered_from = InvalidNode;
			// pomocna promenna - s jakou vahou byl uzel objeven? max() pokud objeven nebyl
			double path_metric = std::numeric_limits<double>::max();
		};

		// struktura cesty v grafu
		struct Path
		{
			// ID uzlu, kterymi cesta prochazi
			std::list<size_t> node_ids;
			// celkova cena cesty
			double total_cost;

			// pro hezci retezeni cesty
			Path& operator+=(const Path& other)
			{
				for (auto node : other.node_ids)
					node_ids.push_back(node);
				total_cost += other.total_cost;

				return *this;
			}

			// predradi uzel do cesty (hodi se pro rekonstrukci cesty z cile)
			void prepend(size_t id)
			{
				node_ids.push_front(id);
			}

			// je cesta prazdna?
			bool empty() const
			{
				return node_ids.empty();
			}
		};

	public:
		CRouter() = default;

		// nacte graf ze souboru
		bool Load_Graph(const std::string& file);

		// optimalizuje poradi uzlu tak, aby byla sance, ze vysledna cesta je co nejkratsi
		bool Optimize_Node_Order(const std::list<size_t>& nodeIds, size_t inputNode = 1);

		// vygeneruje cestu od vychoziho uzlu skrze vsechny zadane
		bool Generate_Path(const std::list<size_t> nodeIds, size_t inputNode = 1);

		// vykresli cestu do souboru
		bool Draw_Path();

		// ziska vygenerovanou cestu (ID po sobe jdoucich uzlu)
		const std::list<size_t> Get_Path() const
		{
			return m_path;
		}

		// ziska celkovou cenu cesty
		const double Get_Total_Cost() const
		{
			return m_totalCost;
		}

		// ziska vykreslene SVG cesty
		const std::string& Get_Path_Drawing() const
		{
			return m_pathSvg;
		}

		// ziska nazev uzlu dle jeho ID
		const std::string& Get_Node_Name(size_t id) const noexcept(false)
		{
			return m_nodes.at(id).name;
		}

		// ziska seznam uzlu v lepsim poradi
		const std::list<size_t>& Get_Optimized_Node_Order() const
		{
			return m_optimizedNodeOrder;
		}

	protected:

	private:
		// uzly grafu
		std::map<size_t, Graph_Node> m_nodes;
		// lepsi poradi uzlu nez to, ktere zadal uzivatel (ma sanci na kratsi cestu)
		std::list<size_t> m_optimizedNodeOrder;

		// vygenerovana cesta
		std::list<size_t> m_path;
		// celkova cena cesty
		double m_totalCost = 0;
		// ID skladu
		std::set<size_t> m_warehouses;
		// ID vsech uzlu, skrze ktere prochazi cesta
		std::set<size_t> m_internodes;

		// vygenerovane SVG cesty
		std::string m_pathSvg;
};
