#ifndef _FAKE_CHESS_BFS_HPP_
#define _FAKE_CHESS_BFS_HPP_

#include <map>
#include <vector>


class Field;

/**
	Damn pathfinders...
	This is Breadth First Search algorithm.
*/
class BFSPathFinder final
{
	Field* m_pField = nullptr;

	std::map<size_t, bool> m_Visited;
	std::map<size_t, size_t> m_Trace;

	size_t m_StartIndex = std::numeric_limits<size_t>::max();
	size_t m_EndIndex = std::numeric_limits<size_t>::max();

public:
	BFSPathFinder(Field* field);
	~BFSPathFinder() = default;

	void searchPath(const size_t start_index, const size_t finish_index);
	std::vector<size_t> getPath() const;
	bool hasPath(const size_t start_index, const size_t finish_index);

};

#endif
