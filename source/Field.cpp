#include "Field.hpp"
#include "Log.hpp"

#include "FieldCell.hpp"
#include "FieldFigure.hpp"

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"


Field::Field(sf::RenderWindow* window) :
	m_pWindow {window}
{
}

Field::~Field()
{
}

void Field::update(const float delta)
{
	for (const auto& cell : m_Cells)
	{
		//cell->update(delta);
		cell->draw();
	}

	for (const auto& figure : m_Figures)
	{
		figure->update(delta);
		figure->draw();
	}
}

sf::Vector2f Field::getCellPosByCoords(const size_t x, const size_t y) const
{
	const float step_x = BoardStep * static_cast<float>(x);
	const float step_y = BoardStep * static_cast<float>(y);

	return (m_FieldPosition + sf::Vector2f {step_x, step_y});
}

FieldCell* Field::getCellByCoords(const size_t x, const size_t y) const
{
	return m_Cells[y * BoardSegments + x].get();
}

std::vector<size_t> Field::getFreeCellsNeighboursIndices(const size_t cell_index) const
{
	std::vector<size_t> neighbours;

	for (const auto& cell : m_Cells[cell_index]->getNeighbours())
	{
		if (!cell->hasFigureInside())
			neighbours.push_back(cell->getIndex());
	}

	return neighbours;
}

std::vector<FieldCell*> Field::getFreeCellsNeighbours(const size_t cell_index) const
{
	std::vector<FieldCell*> neighbours;

	for (const auto& cell : m_Cells[cell_index]->getNeighbours())
	{
		if (!cell->hasFigureInside())
			neighbours.push_back(cell);
	}

	return neighbours;
}

const size_t Field::getFreeNearestLowPriorityWinningCell(const size_t from_cell_index, const bool white) const
{
	const auto& cell_from = m_Cells[from_cell_index];

	float max_dist = 0.0f;
	int16_t min_priority = std::numeric_limits<int16_t>::max();
	size_t index = std::numeric_limits<size_t>::max();

	for (const auto& win_cell : white ? m_WinningCells.first : m_WinningCells.second)
	{
		if (win_cell->hasFigureInside())
			continue;

		const float dist = distance(cell_from->getPosition(), win_cell->getPosition());

		if (const auto priority = win_cell->getWinningPriority(white);
			(priority < cell_from->getWinningPriority(white)) &&
			(dist >= max_dist))
		{
			max_dist = dist;

			index = win_cell->getIndex();
		}
	}

	return index;
}

const size_t Field::getFreeLowPriorityNeighbourCell(const size_t from_cell_index, const bool white) const
{
	const auto& cell_from = m_Cells[from_cell_index];

	int16_t min_priority = std::numeric_limits<int16_t>::max();
	size_t index = std::numeric_limits<size_t>::max();

	for (const auto& cell : this->getFreeCellsNeighbours(from_cell_index))
	{
		if (const auto priority = cell->getWinningPriority(white);
			!cell->hasFigureInside() && (priority < min_priority))
		{
			min_priority = priority;
			index = cell->getIndex();
		}
	}

	return index;
}

std::vector<std::pair<FieldFigure*, size_t>> Field::getFiguresCanMove(const bool white) const
{
	std::vector<std::pair<FieldFigure*, size_t>> figures_can_move;

	const auto& winning_cells = this->getWinningCells(white);

	for (size_t i = 0u; i < m_Cells.size(); ++i)
	{
		const auto& cell = m_Cells[i];

		if (auto figure = cell->getFigureInside();
			(figure != nullptr) &&
			(figure->isWhite() == white) &&
			(!this->getFreeCellsNeighbours(i).empty()))
		{
			figures_can_move.push_back({figure, i});
		}
	}

	return figures_can_move;
}

void Field::computeAvailablePaths(const size_t cell_index)
{
	m_AvailablePathCells = this->getFreeCellsNeighbours(cell_index);
}

void Field::clearAvailableCellsSelection()
{
	for (const auto& cell : m_AvailablePathCells)
		cell->select(false);

	m_AvailablePathCells.clear();
}

void Field::moveSelectedFigureToCell(FieldCell* cell)
{
	m_SelectedData.first->setMovePath(m_SelectedData.second->getPosition(), cell->getPosition());

	if (const auto& diff = (cell->getPosition() - m_SelectedData.second->getPosition());
		diff.x == 0.0f && diff.y < 0.0f)
	{
		m_SelectedData.first->startMove(FieldFigure::MoveDirection::UP);
	}
	else if (diff.x == 0.0f && diff.y > 0.0f)
	{
		m_SelectedData.first->startMove(FieldFigure::MoveDirection::DOWN);
	}
	else if (diff.x < 0.0f && diff.y == 0.0f)
	{
		m_SelectedData.first->startMove(FieldFigure::MoveDirection::LEFT);
	}
	else if (diff.x > 0.0f && diff.y == 0.0f)
	{
		m_SelectedData.first->startMove(FieldFigure::MoveDirection::RIGHT);
	}

	this->clearAvailableCellsSelection();

	m_bMovingFigures = true;
}

std::pair<bool, bool> Field::getWinnerTeam() const
{
	std::pair<bool, bool> result {true, true};

	const auto CheckWinning = [&](const bool white) -> void
	{
		for (const auto& cell : white ? m_WinningCells.first : m_WinningCells.second)
		{
			if (const auto figure = cell->getFigureInside(); figure != nullptr)
			{
				if (const bool win = (cell->getWinningInfo().second == figure->isWhite());
					win && (white ? result.first : result.second))
				{
					white ? (result.first = true) : (result.second = true);
				}
				else
				{
					white ? (result.first = false) : (result.second = false);
				}
			}
			else
			{
				white ? (result.first = false) : (result.second = false);
			}
		}
	};

	CheckWinning(true);
	CheckWinning(false);

	return result;
}

const bool Field::isCellWinning(const size_t cell_index, const bool white) const
{
	const auto& pair = m_Cells[cell_index]->getWinningInfo();

	return (pair.first && (pair.second == white));
}

void Field::createCells()
{
	for (size_t x = 0u; x < BoardSegments; ++x)
	{
		for (size_t y = 0u; y < BoardSegments; ++y)
		{
			auto& cell = m_Cells[x * BoardSegments + y];
			cell = std::make_unique<FieldCell>(this, BoardStep, FieldCoords {x, y});
		}
	}

	const auto IsMultipleOfTwo = [](const size_t value) -> bool
	{
		return ((value % 2u) == 0u);
	};

	for (size_t x = 0u; x < BoardSegments; ++x)
	{
		for (size_t y = 0u; y < BoardSegments; ++y)
		{
			auto cell = this->getCellByCoords(x, y);

			const auto& cell_pos = this->getCellPosByCoords(x, y);

			// Here we set center of cell position(needed to set figures inside cells)
			cell->setPosition(cell_pos + sf::Vector2f{BoardStep * 0.5f, BoardStep * 0.5f});

			cell->setColor(
				(IsMultipleOfTwo(IsMultipleOfTwo(y) ? (x + 1u) : x)) ?
				Color::toSf(BoardBlackColor) : Color::toSf(BoardWhiteColor));

			cell->setWinningPriority(static_cast<int16_t>(x + y), true);
			cell->setWinningPriority(static_cast<int16_t>((BoardSegments - x - 1u) +
				(BoardSegments - y - 1u)), false);
		}
	}

	// Fill cells neighbours
	for (const auto& cell : m_Cells)
	{
		const auto& coords = cell->getCoords();

		constexpr int segments = static_cast<int>(BoardSegments);

		// left
		if (coords.x > 0u)
			cell->addNeighbour(m_Cells[(coords.x - 1) * segments + coords.y].get());

		// top
		if (coords.y > 0u)
			cell->addNeighbour(m_Cells[coords.x * segments + (coords.y - 1)].get());

		// right
		if (coords.x < (BoardSegments - 1u))
			cell->addNeighbour(m_Cells[(coords.x + 1) * segments + coords.y].get());

		// bottom
		if (coords.y < (BoardSegments - 1u))
			cell->addNeighbour(m_Cells[coords.x * segments + (coords.y + 1)].get());
	}
}

void Field::createFigures()
{
	constexpr size_t columnsXcolumns = BoardWinningColumns * BoardWinningColumns;

	for (size_t i = 0u; i < (columnsXcolumns * 2u); ++i)
		m_Figures.emplace_back(std::make_unique<FieldFigure>(this, FigureDiameter, (i >= columnsXcolumns)));

	const auto MakeFiguresByColumns = [&](bool white)
	{
		constexpr size_t black_start_index = (BoardSegments - BoardWinningColumns);
		const auto begin = (white ? 0u : black_start_index);
		const auto end = (white ? BoardWinningColumns : BoardSegments);

		for (size_t x = begin; x < end; ++x)
		{
			for (size_t y = begin; y < end; ++y)
			{
				const size_t index = white ?
					(x * BoardWinningColumns + y) :
					((x - begin) * BoardWinningColumns + (y - begin) + columnsXcolumns);

				auto figure = m_Figures[index].get();

				auto cell = this->getCellByCoords(x, y);

				figure->setPosition(cell->getPosition());

				cell->setFigureInside(figure);

				cell->setWinningInfo(true, white);

				cell->setColor(white ?
					cell->getColor() + sf::Color(0x20u, 0x20u, 0x20u, 0x00u) :
					cell->getColor() - sf::Color(0x20u, 0x20u, 0x20u, 0x00u));
			}
		}
	};

	// first player
	MakeFiguresByColumns(true);

	// second player
	MakeFiguresByColumns(false);
}

void Field::createWinningCells()
{
	for (const auto& cell : m_Cells)
	{
		if (cell->isWinning())
		{
			if (cell->getWinningInfo().second) // for white
				m_WinningCells.first.push_back(cell.get());
			else // for black
				m_WinningCells.second.push_back(cell.get());
		}
	}
}

void Field::selectAllCells()
{
	for (const auto& cell : m_Cells)
		cell->select();
}
