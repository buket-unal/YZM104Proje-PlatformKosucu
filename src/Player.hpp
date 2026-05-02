#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <vector>
#include <SFML/Graphics.hpp>
#include "Platform.hpp"

class Player {
    public:
    Player();

    void update(); // her karede hareket hesaplayacak
    void checkCollision(std::vector<Platform>& platforms);
    void draw(sf::RenderWindow& window); // Ekrana çizer
    sf::Vector2f getPosition() const;

    private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float gravity;
    float moveSpeed;
    float jumpSpeed;
    
};

#endif