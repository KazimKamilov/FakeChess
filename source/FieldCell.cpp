#include "FieldCell.hpp"
#include "FieldFigure.hpp"
#include "Field.hpp"

#include "SFML/Graphics.hpp"


FieldCell::FieldCell(Field* field, const float size, const FieldCoords& coords) :
	m_Coords {coords}
{
	m_pField = field;

	m_pFigure = std::make_unique<sf::RectangleShape>(sf::Vector2f{size, size});
	m_pFigure->setOutlineColor(Color::toSf(Field::FigureSelectColor));
}

FieldCell::~FieldCell()
{
}

void FieldCell::update(const float delta)
{
	// Do nothing
}

void FieldCell::setColor(const sf::Color& color)
{
	m_pFigure->setFillColor(color);
}

void FieldCell::setFigureInside(FieldFigure* figure)
{
	m_pFigureInside = figure;

	if (m_pFigureInside != nullptr)
	{
		m_pFigureInside->setPosition(this->getPosition());
	}
}

void FieldCell::setPosition(const sf::Vector2f& position) noexcept
{
	constexpr float figure_offset = (Field::BoardStep * 0.5f);

	m_Position = sf::Vector2f(figure_offset, figure_offset) + position;
	m_pFigure->setPosition(position);
}

void FieldCell::select(const bool selected)
{
	if (selected)
		m_pFigure->setOutlineThickness(-Field::BoardOutlineSize);
	else
		m_pFigure->setOutlineThickness(0);
}

const size_t FieldCell::getIndex() const noexcept
{
	return (static_cast<size_t>(m_Coords.x) * Field::BoardSegments + static_cast<size_t>(m_Coords.y));
};

const sf::Color& FieldCell::getColor() const
{
	return m_pFigure->getFillColor();
}

const bool FieldCell::isCoordsInside(const sf::Vector2i& coords) const
{
	const auto& pos = m_pFigure->getPosition();

	auto rect = dynamic_cast<sf::RectangleShape*>(m_pFigure.get());

	if (!rect)
		throw std::exception("Cannot cast figure pointer to target shape!");

	const auto& size = rect->getSize();

	return ((coords.x > pos.x) && (coords.x < (pos.x + size.x)) &&
		(coords.y > pos.y) && (coords.y < (pos.y + size.y)));
}

const bool FieldCell::isWinning() const
{
	return m_WinningInfoPair.first;
}

std::pair<bool, bool> FieldCell::getWinningInfo() const noexcept
{
	return m_WinningInfoPair;
}
