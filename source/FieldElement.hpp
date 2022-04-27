#ifndef _FAKE_CHESS_FIELD_ELEMENT_HPP_
#define _FAKE_CHESS_FIELD_ELEMENT_HPP_

#include <memory>

#include "SFML/System/Vector2.hpp"


namespace sf { class Shape; }

class Field;


class FieldElement
{
protected:
	std::unique_ptr<sf::Shape> m_pFigure;

	Field* m_pField = nullptr;

	sf::Vector2f m_Position {0.0f, 0.0f};

public:
	FieldElement();
	virtual ~FieldElement();

	virtual void draw();
	virtual void update(const float delta) = 0;

	virtual const bool isCoordsInside(const sf::Vector2i& coords) const = 0;

	// Position must be always a center of figure element
	virtual void setPosition(const sf::Vector2f& position) noexcept = 0;

	// Return element position(center of element)
	const sf::Vector2f& getPosition() const noexcept { return m_Position; }

	const bool isClicked() const;

};

#endif
