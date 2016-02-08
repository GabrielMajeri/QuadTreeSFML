#pragma once
#include "Common.hpp"

class Entity : public sf::Transformable, public sf::Drawable,
                private NonCopyable
{
public:
    explicit Entity(sf::Vector2f position);

    sf::FloatRect getGlobalBounds() const noexcept;

    void update(sf::Time const& dt);

    void setVelocity(sf::Vector2f velocity);
    sf::Vector2f const& getVelocity() const;

    void setColor(sf::Color color);
protected:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;

    sf::CircleShape mSprite{ 10.f };
    sf::Vector2f mVelocity{ 0.f, 0.f };
};
