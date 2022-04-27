#ifndef _FAKE_CHESS_AI_PLAYER_HPP_
#define _FAKE_CHESS_AI_PLAYER_HPP_

#include <set>
#include <vector>

#include "BFS.hpp"
#include "Field.hpp"

class Game;

class AIPlayer final
{
	Field* m_pField = nullptr;
	Game* m_pGame = nullptr;

	BFSPathFinder m_PathFinder;

	const bool m_bTeamWhite = false;

	std::map<size_t, std::set<size_t>> m_PathEdges;
	std::vector<size_t> m_MovePath;

	size_t m_StartMoveIndex = std::numeric_limits<size_t>::max();
	size_t m_NearWinningPos = std::numeric_limits<size_t>::max();

	// DEBUG
	static constexpr bool m_bDrawPaths = false;

public:
	AIPlayer(Game* game, const bool team_white);
	~AIPlayer();

	void think();

	void drawPaths();

private:
	void searchPathToWin(const size_t source, const size_t dest);
	void makePathsForCells();
	void setMoveToCellAndSetDoneCallback(const size_t next_cell_index);

	bool tryToComputeWinningCell();
	bool tryToMoveToLessPriorityCell();
	bool tryToRandomMove();
};

#endif
