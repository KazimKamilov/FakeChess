#ifndef _FAKE_CHESS_FIELD_HPP_
#define _FAKE_CHESS_FIELD_HPP_

#include <inttypes.h>
#include <memory>
#include <array>
#include <vector>
#include <cmath>

#include "FieldCoords.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Color.hpp"

namespace sf
{
class RectangleShape;
class RenderWindow;
}

class FieldCell;
class FieldFigure;

inline float distance(const sf::Vector2f& a, const sf::Vector2f& b)
{
	const auto& vec = b - a;

	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

struct Color final
{
	uint8_t r = 0x00u;
	uint8_t g = 0x00u;
	uint8_t b = 0x00u;
	uint8_t a = 0xFFu;

	static sf::Color toSf(const Color& color) { return {color.r, color.g, color.b, color.a}; }
};


class Field final
{
public:
	static constexpr float BoardSize {500.0f};
	static constexpr size_t BoardSegments {8u};
	static constexpr size_t BoardWinningColumns {3u};
	static constexpr size_t BoardCellsCount {BoardSegments * BoardSegments};

	static constexpr float BoardStep = (BoardSize / static_cast<float>(BoardSegments));
	static constexpr float FigureDiameter = BoardStep * 0.8f;

	static constexpr Color FigureWhiteColor { 0xFFu, 0xFFu, 0xFFu, 0xFFu };
	static constexpr Color FigureBlackColor {};
	static constexpr Color FigureWhiteOutlineColor { 0xAFu, 0xAFu, 0xAFu, 0xFFu };
	static constexpr Color FigureBlackOutlineColor { 0x50u, 0x50u, 0x50u, 0xFFu };

	static constexpr Color FigureSelectColor { 0x20u, 0xFFu, 0x20u, 0xFFu };

	static constexpr Color BoardWhiteColor { 0x94u, 0x5Eu, 0x33u, 0xFFu };
	static constexpr Color BoardBlackColor { 0x41u, 0x2Du, 0x06u, 0xFFu };
	static constexpr Color BoardSelectColor { 0x20u, 0xFFu, 0x20u, 0xFFu };
	static constexpr float BoardOutlineSize {2.0f};

private:
	sf::RenderWindow* m_pWindow;

	const sf::Vector2f m_FieldPosition {40.0f, 40.0f};

	std::array<std::unique_ptr<FieldCell>, BoardSegments * BoardSegments> m_Cells;
	std::vector<std::unique_ptr<FieldFigure>> m_Figures;

	std::pair<FieldFigure*, FieldCell*> m_SelectedData {nullptr, nullptr};
	std::vector<FieldCell*> m_AvailablePathCells;

	// <white, black>
	std::pair<std::vector<FieldCell*>, std::vector<FieldCell*>> m_WinningCells;

	bool m_bMovingFigures = false;

public:
	Field(sf::RenderWindow* window);
	~Field();

	void update(const float delta);

	void setSelectedData(FieldFigure* figure, FieldCell* cell) { m_SelectedData = {figure, cell}; }
	const auto& getSelectedData() const { return m_SelectedData; }
	FieldFigure* getSelectedFigure() const { return m_SelectedData.first; }
	const bool haveSelectedFigure() const { return (m_SelectedData.first != nullptr); }

	sf::RenderWindow* getWindow() const noexcept { return m_pWindow; }

	const sf::Vector2f& getPosition() const noexcept { return m_FieldPosition; }

	const auto& getCells() const { return m_Cells; }
	const auto& getFigures() const { return m_Figures; }

	sf::Vector2f getCellPosByCoords(const size_t x, const size_t y) const;
	FieldCell* getCellByCoords(const size_t x, const size_t y) const;

	const std::vector<FieldCell*>& getAvailablePaths() const { return m_AvailablePathCells; }
	const auto& getWinningCells(const bool white) const { return (white ? m_WinningCells.first : m_WinningCells.second); }

	void computeAvailablePaths(const size_t cell_index);
	std::vector<FieldCell*> getFreeCellsNeighbours(const size_t cell_index) const;
	std::vector<size_t> getFreeCellsNeighboursIndices(const size_t cell_index) const;

	const size_t getFreeNearestLowPriorityWinningCell(const size_t from_cell_index, const bool white) const;
	const size_t getFreeLowPriorityNeighbourCell(const size_t from_cell_index, const bool white) const;

	// <figure_ptr, place index>
	std::vector<std::pair<FieldFigure*, size_t>> getFiguresCanMove(const bool white) const;

	void clearAvailableCellsSelection();

	void moveSelectedFigureToCell(FieldCell* cell);

	// <is winner white, is winner black>
	std::pair<bool, bool> getWinnerTeam() const;

	const bool hasMovingFigures() const { return m_bMovingFigures; }
	void setMovingFigures(const bool value) { m_bMovingFigures = value; }

	const bool isCellWinning(const size_t cell_index, const bool white) const;

	void createCells();
	void createFigures();
	void createWinningCells();

	void selectAllCells();

private:

};

#endif
