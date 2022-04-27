#ifndef _FAKE_CHESS_FIELD_CELL_HPP_
#define _FAKE_CHESS_FIELD_CELL_HPP_

#include <vector>

#include "FieldElement.hpp"
#include "FieldCoords.hpp"

#include "SFML/Graphics/Color.hpp"


class FieldFigure;


class FieldCell final : public FieldElement
{
	FieldFigure* m_pFigureInside = nullptr;

	std::vector<FieldCell*> m_Neighbours;

	const FieldCoords m_Coords;

	// <is_winning, white>
	std::pair<bool, bool> m_WinningInfoPair {false, false};
	std::pair<int16_t, int16_t> m_WinningPriorityPair {std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::max()};

public:
	FieldCell(Field* field, const float size, const FieldCoords& coords);
	virtual ~FieldCell();

	virtual void update(const float delta) override;

	virtual const bool isCoordsInside(const sf::Vector2i& coords) const override;

	virtual void setPosition(const sf::Vector2f& position) noexcept override;

	void addNeighbour(FieldCell* cell) { return m_Neighbours.push_back(cell); }

	void setColor(const sf::Color& color);
	void setFigureInside(FieldFigure* figure);

	void setWinningInfo(const bool winning, const bool for_white) { m_WinningInfoPair = {winning, for_white}; }
	void setWinningPriority(const int16_t priority, const bool white) noexcept
	{ white ? (m_WinningPriorityPair.first = priority) : (m_WinningPriorityPair.second = priority); }

	void select(const bool selected = true);

	const FieldCoords& getCoords() const noexcept { return m_Coords; }
	const size_t getIndex() const noexcept;
	const int16_t getWinningPriority(const bool white) const noexcept
	{ return white ? m_WinningPriorityPair.first : m_WinningPriorityPair.second; }

	const std::vector<FieldCell*>& getNeighbours() const { return m_Neighbours; }

	const sf::Color& getColor() const;

	FieldFigure* getFigureInside() const noexcept { return m_pFigureInside; }

	const bool hasFigureInside() const noexcept { return (m_pFigureInside != nullptr); }

	const bool isWinning() const;

	// <winning, team>
	std::pair<bool, bool> getWinningInfo() const noexcept;
};

#endif
