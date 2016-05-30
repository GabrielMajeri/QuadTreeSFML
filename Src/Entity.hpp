#pragma once
#include "Common.hpp"

/// An entity is a circle that moves around the screen
class Entity : public sf::Transformable, public sf::Drawable,
                private NonCopyable
{
public:
    /// Construct a new entity at a given position
    explicit Entity(sf::Vector2f position);

    /// Get the bounds of this entity, relative to its transform
    sf::FloatRect getGlobalBounds() const noexcept;
    
    /// Integrate over some amount of time
    void update(sf::Time const& dt);

    /// Set/get the current velocity
    void setVelocity(sf::Vector2f velocity);
    sf::Vector2f const& getVelocity() const;

    /// Set the circle's color
    void setColor(sf::Color color);
    
protected:
    /// Draws to screen
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;

    /// The circle
    sf::CircleShape mSprite{ 10.f };
    
    /// The current velocity
    sf::Vector2f mVelocity{ 0.f, 0.f };
};
