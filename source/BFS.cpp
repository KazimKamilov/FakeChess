#include "BFS.hpp"
#include "Field.hpp"

#include <queue>


BFSPathFinder::BFSPathFinder(Field* field) :
	m_pField {field}
{
	for (size_t i = 0u; i < Field::BoardCellsCount; ++i)
		m_Visited[i] = false;
}

void BFSPathFinder::searchPath(const size_t start_index, const size_t finish_index)
{
	m_StartIndex = start_index;
	m_EndIndex = finish_index;
	m_Trace.clear();

	for (auto& vis : m_Visited)
		vis.second = false;

	std::queue<size_t> indices;

	bool finish = false;

	m_Visited[start_index] = true;

	indices.push(start_index);

	while (!indices.empty() && !finish)
	{
		const size_t index = indices.front();

		indices.pop();

		// If we use reverse iterator - path will be compute from another side
		for (const size_t neighbour_index : m_pField->getFreeCellsNeighboursIndices(index))
		{
			if (!m_Visited[neighbour_index])
			{
				m_Visited[neighbour_index] = true;

				indices.push(neighbour_index);

				m_Trace[neighbour_index] = index;

				if (neighbour_index == finish_index)
				{
					finish = true;
					break;
				}
			}
		}
	}
}

std::vector<size_t> BFSPathFinder::getPath() const
{
	std::vector<size_t> path;

	size_t index = m_EndIndex;

	while (index != m_StartIndex)
	{
		path.insert(path.cbegin(), index);
		index = m_Trace.at(index);
	}

	return path;
}

bool BFSPathFinder::hasPath(const size_t start_index, const size_t finish_index)
{
	this->searchPath(start_index, finish_index);

	std::pair<bool, bool> found {false, false};

	// here i use a little trick - finish index is
	// not pushed to trace - because it a neighbour.
	for (const auto& pair : m_Trace)
	{
		if (pair.second == start_index)
			found.first = true;

		if (pair.first == finish_index)
			found.second = true;
	}

	return (found.first && found.second);
}
