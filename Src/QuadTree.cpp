#include "QuadTree.hpp"

QuadTree::QuadTree(sf::FloatRect bounds, size_t maxLevel, size_t maxObjects)
: mBounds{ std::move(bounds) },
 mMaxLevel{ std::move(maxLevel) }, mMaxObjects{ std::move(maxObjects) },
 mRoot{ mBounds, 0U, mMaxLevel, mMaxObjects }
{

}

size_t QuadTree::countObjects() const
{
    return std::move(mRoot.countObjects());
}

void QuadTree::insert(std::vector<std::shared_ptr<Entity>> const& ptrArray)
{
    for(auto const& ptr : ptrArray)
        mRoot.insert(static_cast<std::weak_ptr<Entity>>(ptr));
}

void QuadTree::clear()
{
    mRoot.clear();
}

void QuadTree::drawBorders(sf::RenderTarget& t) const
{
    mRoot.drawBorders(t);
}

std::vector<std::weak_ptr<Entity>> QuadTree::getCollisionable(sf::FloatRect const& bounds) const
{
    return std::move(mRoot.getCollisionable(bounds));
}
