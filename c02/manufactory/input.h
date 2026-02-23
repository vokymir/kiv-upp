#pragma once

#include <list>
#include <string>
#include <vector>

/*
 * Trida obstaravajici prijem pozadavku
 */
class CInput
{
	public:
		CInput() = default;

		// prijima pozadavek, vraci "true" pokud se pozadavek podarilo prijmout
		bool Accept_Request();

		// ziska seznam uzlu
		const std::list<size_t>& Get_Target_Nodes() const
		{
			return m_targetNodes;
		}

		// ziska pocet hran soucastky
		size_t Get_Line_Count() const
		{
			return m_lineCount;
		}

		// ziska vektor polomeru pro generovani soucastky
		const std::vector<double>& Get_Diameters() const
		{
			return m_diameters;
		}

	private:
		// ulozene cilove sklady
		std::list<size_t> m_targetNodes;
		// pocet hran soucastky
		size_t m_lineCount = 0;
		// vektor polomeru pro generovani soucastky
		std::vector<double> m_diameters;
};
