#include "Entity.hpp"
#include "QuadTree.hpp"

sf::RenderWindow window;

// This will contain pointers to every circle
std::vector<std::shared_ptr<Entity>> entities;

// Construct a new quad tree
// It goes down to a maximum of 15 levels
// Each level holds up to 1 item before it spills
QuadTree qt{ sf::FloatRect{ 0U, 0U, 800U, 600U }, 15U, 1U };

void init()
{
    // Construct a new window
    window.create( sf::VideoMode{ 800U, 600U }, "Quad Tree" );


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(10, 700), disY(10, 500),
        vel(5, 150);

    // Randomly generate balls
    for(size_t i = 0; i < 100; ++i) {
        entities.emplace_back(std::make_shared<Entity>(
            sf::Vector2f{25 + (float)disX(gen), 25 + (float)disY(gen)}));
        entities.back()->setVelocity({50 + (float)vel(gen), 50 + (float)vel(gen)});
    }

    // Insert the balls' pointers to the quad tree
    qt.insert(entities);
}

void handleEvents(sf::Event const& e)
{
    switch(e.type) {
        // Close the window when user clicks the Close button
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            if(!entities.empty())
                entities.erase(entities.end());
            break;
        default:
            break;
    }
}

void handleCollisions()
{
    for(size_t i{ 0 }; i < entities.size() - 1; ++i) {
        auto tmp{ qt.getCollisionable(entities[i]->getGlobalBounds()) };
        // We do not care about self collision, so size must be bigger than 1
        if(tmp.size() > 1) {
            for(size_t j{ 0 }; j < tmp.size(); ++j) {
                // Skip self collision
                if((tmp[j].lock().get()) == (entities[i].get()))
                    continue;

                if(tmp[j].lock()->getGlobalBounds().intersects(entities[i]->getGlobalBounds())) {
                    tmp[j].lock()->setColor(sf::Color::Magenta);
                    entities[i]->setColor(sf::Color::Magenta);
                }
            }
        }
    }
}

void update(sf::Time const& dt)
{
    // Reset all entities' colors
    for(auto& entity : entities)
        entity->setColor(sf::Color::Red);

    // Update all entities
    for(auto& entity : entities) {
        entity->update(dt);
        // Keep entities within bounds
        if(!QuadTreeNode::contains({ 0U, 0U, 800U, 600U }, entity->getGlobalBounds()))
            entity->setVelocity({entity->getVelocity().y, -entity->getVelocity().x});
    }

    // Update the quad tree
    // First, clear the tree
    qt.clear();
    // Then reinsert the entities and rebuild the tree
    qt.insert(entities);

    handleCollisions();
}

void render()
{
    // Clear the window
    window.clear(sf::Color::Black);

    // Draw the balls
    for(auto const& entity : entities)
        window.draw(*entity);

    // Recursively draw the rectangles that represent the borders
    // of the quad tree's nodes
    qt.drawBorders(window);

    // Flip the display buffer
    window.display();
}

int main()
{
    init();

    sf::Event e;
    sf::Clock frameClock;
    sf::Time elapsedTime{ sf::Time::Zero };
    sf::Time TimePerFrame{ sf::seconds(1.f / 60.f) };

    // Game loop
    while(window.isOpen()) {
        elapsedTime += frameClock.restart();

        while(window.pollEvent(e))
            handleEvents(e);

        while(elapsedTime > TimePerFrame)
        {
            elapsedTime -= TimePerFrame;
            update(TimePerFrame);
        }

        render();
    }

    return EXIT_SUCCESS;
}
