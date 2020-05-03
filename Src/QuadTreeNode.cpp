#include "QuadTreeNode.hpp"

QuadTreeNode::QuadTreeNode(sf::FloatRect bounds, size_t level,
                          size_t const& maxLevel, size_t const& maxObjects)
: mBounds{ bounds }, mLevel{ level }, mMaxLevel{ maxLevel }, mMaxObjects{ maxObjects }
{}

size_t QuadTreeNode::countObjects() const
{
    if(!isSubdivided)
        return std::count_if(mObjects.begin(), mObjects.end(),
                             [](auto& obj)->bool{ return !obj.expired(); });


    size_t objCount = std::count_if(mObjects.begin(), mObjects.end(),
                             [](auto& obj)->bool{ return !obj.expired(); });

    for(auto const& child : mChildren)
        objCount += child->countObjects();

    return objCount;
}

void QuadTreeNode::subdivide()
{
    if(isSubdivided)
        return;

    float midWidth{ mBounds.width / 2.f }, midHeight{ mBounds.height / 2.f };
    float left{ mBounds.left }, top{ mBounds.top };

    mChildren[0] = std::make_unique<QuadTreeNode>(sf::FloatRect{ left, top,
                                                  midWidth, midHeight },
                                    mLevel + 1, mMaxLevel, mMaxObjects);

    mChildren[1] = std::make_unique<QuadTreeNode>(sf::FloatRect{ left + midWidth, top,
                                                  midWidth, midHeight },
                                    mLevel + 1, mMaxLevel, mMaxObjects);

    mChildren[2] = std::make_unique<QuadTreeNode>(sf::FloatRect{ left, top + midHeight,
                                                  midWidth, midHeight },
                                    mLevel + 1, mMaxLevel, mMaxObjects);

    mChildren[3] = std::make_unique<QuadTreeNode>(sf::FloatRect{ left + midWidth, top + midHeight,
                                                  midWidth, midHeight },
                                    mLevel + 1, mMaxLevel, mMaxObjects);

    for(auto& child : mChildren)
        child->mParent = this;

    isSubdivided = true;
}

void QuadTreeNode::insert(std::weak_ptr<Entity> ptr)
{
    if(isSubdivided) {
        // Try to insert it in a subnode
        auto i{ getIndex(ptr.lock()->getGlobalBounds()) };

        if(contains(mChildren[i]->mBounds, ptr.lock()->getGlobalBounds())) {
            mChildren[i]->insert(std::move(ptr));
            return;
        }

        // If it reaches this point, then it's between subnodes
        mStuckObjects.push_back(std::move(ptr));
        return;
   }

   mObjects.push_back(std::move(ptr));

   if(mLevel < mMaxLevel && mObjects.size() > mMaxObjects)
   {
       subdivide();

       for(auto& obj : mObjects)
            insert(std::move(obj));

       mObjects.clear();
   }
}

void QuadTreeNode::clear()
{
    mStuckObjects.clear();
    mObjects.clear();

    if(!isSubdivided)
        return;

    for(auto& child : mChildren) {
        child->clear();
        child.reset(nullptr);
    }

    isSubdivided = false;
}

void QuadTreeNode::drawBorders(sf::RenderTarget& t) const
{
    sf::RectangleShape r{ sf::Vector2f{mBounds.width, mBounds.height} };
    r.setPosition(mBounds.left, mBounds.top);
    r.setOutlineThickness(1.f);
    r.setOutlineColor(sf::Color::White);
    r.setFillColor(sf::Color::Transparent);

    t.draw(r);

    if(isSubdivided)
        for(auto& child : mChildren)
            child->drawBorders(t);
}

size_t QuadTreeNode::getIndex(sf::FloatRect objBounds) const
{
    float midWidth{ mBounds.left + (mBounds.width / 2.f) },
          midHeight{ mBounds.top + (mBounds.height / 2.f) };

    bool left{ objBounds.left <= midWidth },
         top{ objBounds.top <= midHeight };


    size_t index = NW;
    if (left) {
        if(!top)
            index = SW;
    }
    else {
        if (top) {
            index = NE;
        } else {
            index = SE;
        }
    }

    return index;
}

std::vector<std::weak_ptr<Entity>> QuadTreeNode::getCollisionable(sf::FloatRect const& bounds) const
{
    if(!contains(mBounds, bounds))
        return {};

    std::vector<std::weak_ptr<Entity>> ret;
    if(isSubdivided) {
        size_t possibleIndex{ getIndex(bounds) };
        if(contains(mChildren[possibleIndex]->mBounds, bounds)) {
            auto tmp{ mChildren[possibleIndex]->getCollisionable(bounds) };
            ret.insert(ret.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
        } else {
            if(bounds.left <= mChildren[NE]->mBounds.left) {
                if(bounds.top < mChildren[SW]->mBounds.top) {
                    auto tmp{ mChildren[NW]->getAllObjects() };
                    ret.insert(ret.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
                }

                if(bounds.top + bounds.height > mChildren[SW]->mBounds.top) {
                    auto tmp{ mChildren[SW]->getAllObjects() };
                    ret.insert(ret.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
                }
            }

            if(bounds.left + bounds.width > mChildren[NE]->mBounds.left) {
                if(bounds.top <= mChildren[SE]->mBounds.top) {
                    auto tmp{ mChildren[NE]->getAllObjects() };
                    ret.insert(ret.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
                }

                if(bounds.top + bounds.height > mChildren[SE]->mBounds.top) {
                    auto tmp{ mChildren[SE]->getAllObjects() };
                    ret.insert(ret.end(), std::make_move_iterator(tmp.begin()), std::make_move_iterator(tmp.end()));
                }
            }
        }
    }

    ret.insert(ret.end(), mStuckObjects.begin(), mStuckObjects.end());
    ret.insert(ret.end(), mObjects.begin(), mObjects.end());

    return std::move(ret);
}

std::vector<std::weak_ptr<Entity>> QuadTreeNode::getAllObjects() const
{
    std::vector<std::weak_ptr<Entity>> ret;

    if(isSubdivided)
        for(auto const& child : mChildren) {
            auto tmp{ child->getAllObjects() };
            ret.insert(ret.end(), tmp.begin(), tmp.end());
        }

    ret.insert(ret.end(), mObjects.begin(), mObjects.end());
    ret.insert(ret.end(), mStuckObjects.begin(), mStuckObjects.end());

    return ret;
}

bool QuadTreeNode::contains(sf::FloatRect const& first, sf::FloatRect const& second)
{
    return (first.left <= second.left && first.top <= second.top
            && first.left + first.width >= second.left + second.width
            && first.top + first.height >= second.top + second.height);
}
