#include "FieldFigure.hpp"
#include "Field.hpp"

#include "SFML/Graphics.hpp"


FieldFigure::FieldFigure(Field* field, const float diameter, const bool white) :
	m_bWhite {white}
{
	m_pField = field;

	m_pFigure = std::make_unique<sf::CircleShape>(diameter * 0.5f);
	m_pFigure->setFillColor(m_bWhite ? Color::toSf(Field::FigureWhiteColor) : Color::toSf(Field::FigureBlackColor));
	m_pFigure->setOutlineColor(m_bWhite ? Color::toSf(Field::FigureWhiteOutlineColor) : Color::toSf(Field::FigureBlackOutlineColor));
	m_pFigure->setOutlineThickness(-3.0f);
}

FieldFigure::~FieldFigure()
{
}

void FieldFigure::update(const float delta)
{
	const float anim_step = delta * 3.0f;

	const auto StopMoveAndRunCallback = [this]()
	{
		m_MoveDirection = MoveDirection::NONE;

		if (m_MoveDoneCallback)
		{
			m_MoveDoneCallback();

			m_MoveDoneCallback = nullptr;
		}
	};

	switch (m_MoveDirection)
	{
		case MoveDirection::UP:
		{
			m_Position.y -= anim_step;

			if (m_Position.y <= m_MovePath.second.y)
			{
				m_Position.y = m_MovePath.second.y;

				StopMoveAndRunCallback();
			}

			this->setPosition(m_Position);
		}
		break;

		case MoveDirection::DOWN:
		{
			m_Position.y += anim_step;

			if (m_Position.y >= m_MovePath.second.y)
			{
				m_Position.y = m_MovePath.second.y;

				StopMoveAndRunCallback();
			}

			this->setPosition(m_Position);
		}
		break;

		case MoveDirection::LEFT:
		{
			m_Position.x -= anim_step;

			if (m_Position.x <= m_MovePath.second.x)
			{
				m_Position.x = m_MovePath.second.x;

				StopMoveAndRunCallback();
			}

			this->setPosition(m_Position);
		}
		break;

		case MoveDirection::RIGHT:
		{
			m_Position.x += anim_step;

			if (m_Position.x >= m_MovePath.second.x)
			{
				m_Position.x = m_MovePath.second.x;

				StopMoveAndRunCallback();
			}

			this->setPosition(m_Position);
		}
		break;

		default:
		break;
	}
}

void FieldFigure::select(const bool selected)
{
	if (selected)
		m_pFigure->setOutlineColor(Color::toSf(Field::FigureSelectColor));
	else
		m_pFigure->setOutlineColor(m_bWhite ?
		Color::toSf(Field::FigureWhiteOutlineColor) :
		Color::toSf(Field::FigureBlackOutlineColor));
}

const bool FieldFigure::isCoordsInside(const sf::Vector2i& coords) const
{
	const auto& pos = m_pFigure->getPosition();

	auto circle = dynamic_cast<sf::CircleShape*>(m_pFigure.get());

	if (!circle)
		throw std::exception("Cannot cast figure pointer to target shape!");

	const float radius = circle->getRadius();

	return (((coords.x - (pos.x + radius)) * (coords.x - (pos.x + radius)) +
		(coords.y - (pos.y + radius)) * (coords.y - (pos.y + radius))) <= (radius * radius));
}

void FieldFigure::setPosition(const sf::Vector2f& position) noexcept
{
	constexpr float figure_offset = (Field::FigureDiameter * 0.5f);

	m_Position = position;
	m_pFigure->setPosition(m_Position - sf::Vector2f {figure_offset, figure_offset});
}
