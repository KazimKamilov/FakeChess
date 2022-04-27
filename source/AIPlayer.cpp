#include "AIPlayer.hpp"
#include "Field.hpp"
#include "FieldCell.hpp"
#include "FieldFigure.hpp"
#include "Game.hpp"

#include <random>

#include "SFML/Graphics.hpp"

inline size_t randomIndex(const size_t min, const size_t max)
{
	std::random_device device;
	std::mt19937_64 gena(device());
	std::uniform_int_distribution<size_t> uid(min, max);
	return uid(gena);
}

template<typename T>
inline const size_t randIndexIfCan(const std::vector<T>& vec)
{
	if (vec.size() > 1u)
		return randomIndex(0u, vec.size() - 1u);
	else if (vec.size() == 1u)
		return 0u;
	else // empty
		return std::numeric_limits<size_t>::max();
}

AIPlayer::AIPlayer(Game* game, const bool team_white) :
	m_pGame {game},
	m_pField {game->getField().get()},
	m_bTeamWhite {team_white},
	m_PathFinder {game->getField().get()}
{
}

AIPlayer::~AIPlayer()
{
}

void AIPlayer::think()
{
	m_StartMoveIndex = std::numeric_limits<size_t>::max();
	m_NearWinningPos = std::numeric_limits<size_t>::max();

	if (this->tryToComputeWinningCell()) // Get path to winner cell
	{
		this->searchPathToWin(m_StartMoveIndex, m_NearWinningPos);
		this->setMoveToCellAndSetDoneCallback(m_MovePath.front());
	}
	else if (this->tryToMoveToLessPriorityCell()) // Cannot get path to winning cell - move to lesser
	{
		this->searchPathToWin(m_StartMoveIndex, m_NearWinningPos);
		this->setMoveToCellAndSetDoneCallback(m_MovePath.front());
	}
	else if (this->tryToRandomMove()) // Cannot get path to winner cells - random move
	{
		this->makePathsForCells();

		this->setMoveToCellAndSetDoneCallback(m_MovePath.front());
	}
	else // No available moves for AI
	{
		m_pField->setMovingFigures(false);
		m_MovePath.clear();

		// Is no moves for AI - skip our move
		m_pGame->setState(Game::State::AI_MOVE);

		m_pGame->nextTurn();
	}
}

void AIPlayer::drawPaths()
{
	if constexpr (m_bDrawPaths)
	{
		if (m_PathEdges.empty())
			return;

		auto window = m_pField->getWindow();
		const auto& cells = m_pField->getCells();

		std::vector<sf::Vertex> vertices;

		for (const auto& pair : m_PathEdges)
		{
			const auto& pos = cells[pair.first]->getPosition();

			for (const auto i : pair.second)
			{
				vertices.push_back(sf::Vertex(cells[i]->getPosition()));
				vertices.push_back(sf::Vertex(pos));
			}
		}

		window->pushGLStates();
		window->resetGLStates();

		window->draw(vertices.data(), vertices.size(), sf::Lines);

		window->popGLStates();
	}
}

void AIPlayer::searchPathToWin(const size_t source, const size_t dest)
{
	this->makePathsForCells();

	m_PathFinder.searchPath(source, dest);

	m_MovePath = m_PathFinder.getPath();
}

void AIPlayer::makePathsForCells()
{
	m_PathEdges.clear();

	const auto& cells = m_pField->getCells();

	for (const auto& cell : cells)
	{
		if (!cell->hasFigureInside())
		{
			for (const auto neighbour : cell->getNeighbours())
			{
				const auto neighbour_index = neighbour->getIndex();

				if (!neighbour->hasFigureInside())
				{
					m_PathEdges[neighbour_index].insert(cell->getIndex());
					m_PathEdges[cell->getIndex()].insert(neighbour_index);
				}
			}
		}
	}
}

void AIPlayer::setMoveToCellAndSetDoneCallback(const size_t next_cell_index)
{
	const auto& cells = m_pField->getCells();
	auto current_cell = cells[m_StartMoveIndex].get();

	const auto& current_figure = current_cell->getFigureInside();

	m_pField->setSelectedData(current_figure, current_cell);

	auto next_cell = cells[next_cell_index].get();

	current_figure->setMoveDoneCallback([=]()
	{
		current_cell->setFigureInside(nullptr);

		next_cell->setFigureInside(current_figure);

		m_pField->setMovingFigures(false);

		m_pGame->nextTurn();

		m_MovePath.clear();
	});

	m_pField->moveSelectedFigureToCell(next_cell);

	m_pGame->setState(Game::State::AI_MOVE);
}

bool AIPlayer::tryToComputeWinningCell()
{
	// Temporary data
	struct MoveData final
	{
		FieldCell* winning_cell = nullptr;
		FieldFigure* figure = nullptr;
		size_t start_index = std::numeric_limits<size_t>::max();
		float distance = 0.0f;
		int16_t priority = -1;
	};

	const auto& figures_can_move = m_pField->getFiguresCanMove(m_bTeamWhite);
	const auto& cells = m_pField->getCells();

	// <from ,to>
	std::vector<std::pair<size_t, size_t>> priority_moves;
	std::vector<MoveData> move_choice;

	/**
		So, what are we doing here?
		For all figures that can make a move, we find
		a way to the furthest winning free cell.
		If the AI hits a winning cell, it tries to move the
		figure to the nearest one with a lower priority.
	*/
	for (const auto& figure_pair : figures_can_move)
	{
		const auto& from_cell = cells[figure_pair.second];
		const bool team = figure_pair.first->isWhite();

		if (const auto& pair = from_cell->getWinningInfo();
			pair.first && (pair.second == team))
		{
			for (const auto& cell : m_pField->getFreeCellsNeighbours(figure_pair.second))
			{
				if (cell->getWinningPriority(team) < from_cell->getWinningPriority(team))
					priority_moves.push_back({figure_pair.second, cell->getIndex()});
			}
		}

		if (const size_t index = m_pField->getFreeNearestLowPriorityWinningCell(figure_pair.second, figure_pair.first->isWhite());
			index != std::numeric_limits<size_t>::max() &&
			m_PathFinder.hasPath(figure_pair.second, index))
		{
			auto& data = move_choice.emplace_back();

			data.winning_cell = cells[index].get();
			data.figure = figure_pair.first;
			data.priority = from_cell->getWinningPriority(team) - from_cell->getWinningPriority(team);
			data.start_index = figure_pair.second;
			data.distance = distance(from_cell->getPosition(), cells[index]->getPosition());
		}
	}

	if (!priority_moves.empty())
	{
		const auto index = randIndexIfCan(priority_moves);

		const auto& move_data = priority_moves[index];

		m_StartMoveIndex = move_data.first;
		m_NearWinningPos = move_data.second;

		return true;
	}
	else if (const auto index = randIndexIfCan(move_choice);
		index != std::numeric_limits<size_t>::max())
	{
		const auto& move_data = move_choice[index];

		m_StartMoveIndex = move_data.start_index;
		m_NearWinningPos = move_data.winning_cell->getIndex();

		return true;
	}
	else
	{
		return false;
	}
}

bool AIPlayer::tryToMoveToLessPriorityCell()
{
	std::vector<std::pair<size_t, size_t>> nearest_cells;

	for (const auto& figure_pair : m_pField->getFiguresCanMove(m_bTeamWhite))
	{
		if (const size_t index = m_pField->getFreeLowPriorityNeighbourCell(figure_pair.second, m_bTeamWhite);
			index != std::numeric_limits<size_t>::max())
		{
			nearest_cells.push_back({figure_pair.second, index});
		}
	}

	if (const auto index = randIndexIfCan(nearest_cells);
		index != std::numeric_limits<size_t>::max())
	{
		const auto& move_data = nearest_cells[index];

		m_StartMoveIndex = move_data.first;
		m_NearWinningPos = move_data.second;

		return true;
	}
	else
	{
		return false;
	}
}

bool AIPlayer::tryToRandomMove()
{
	const auto& cells = m_pField->getCells();

	std::vector<size_t> free_figures;

	for (size_t i = 0u; i < cells.size(); ++i)
	{
		const auto& cell = cells[i];

		if (auto figure = cell->getFigureInside();
			(figure != nullptr) &&
			(figure->isWhite() == m_bTeamWhite) &&
			!m_pField->getFreeCellsNeighbours(i).empty())
		{
			free_figures.push_back(i);
		}
	}

	if (free_figures.empty())
		return false;

	const auto SetMoveData = [this](const size_t start_index) -> void
	{
		m_StartMoveIndex = start_index;

		const auto& neighbours = m_pField->getFreeCellsNeighbours(m_StartMoveIndex);

		if (neighbours.size() > 1u)
		{
			const auto& cell_to_move = neighbours[randomIndex(0u, neighbours.size() - 1u)];

			m_MovePath.push_back(cell_to_move->getIndex());
		}
		else
		{
			m_MovePath.push_back(neighbours.back()->getIndex());
		}
	};

	if (free_figures.size() > 1u)
	{
		const size_t random_cell_index = randomIndex(0u, free_figures.size() - 1u);

		SetMoveData(free_figures[random_cell_index]);
	}
	else // just one
	{
		SetMoveData(free_figures.back());
	}

	return true;
}
