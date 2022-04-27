#include "FieldElement.hpp"
#include "Field.hpp"

#include "SFML/Graphics/Shape.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics/RenderWindow.hpp"


FieldElement::FieldElement()
{
}

FieldElement::~FieldElement()
{
}

void FieldElement::draw()
{
	auto window = m_pField->getWindow();

	window->pushGLStates();
	window->resetGLStates();

	window->draw(*m_pFigure);

	window->popGLStates();
}

void FieldElement::setPosition(const sf::Vector2f& position) noexcept
{
	m_Position = position;

	m_pFigure->setPosition(m_Position);
}

const bool FieldElement::isClicked() const
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		const auto& mouse_pos = sf::Mouse::getPosition(*m_pField->getWindow());

		return this->isCoordsInside(mouse_pos);
	}

	return false;
}
