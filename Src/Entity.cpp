#include "Entity.hpp"

Entity::Entity(sf::Vector2f position)
{
    setPosition(position);
    mSprite.setFillColor(sf::Color::Red);
}

void Entity::update(sf::Time const& dt)
{
    move(mVelocity * dt.asSeconds());
}

void Entity::setVelocity(sf::Vector2f v)
{
    mVelocity = std::move(v);
}

sf::Vector2f const& Entity::getVelocity() const
{
    return mVelocity;
}

void Entity::setColor(sf::Color color)
{
    mSprite.setFillColor(color);
}

sf::FloatRect Entity::getGlobalBounds() const noexcept
{
    return getTransform().transformRect(mSprite.getLocalBounds());
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept
{
    states.transform.combine(getTransform());

    target.draw(mSprite, states);
}
