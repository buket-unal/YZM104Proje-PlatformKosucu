#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

class Player {
    public:
    Player();

    void update(); // her karede hareket hesaplayacak
    void checkCollision(float groundLevel); // yer çekimi ve zemin kontrolü
    void draw(sf::RenderWindow& window); // Ekrana çizer

    private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    float gravity;
    float moveSpeed;
    float jumpSpeed;
    
};

#endif