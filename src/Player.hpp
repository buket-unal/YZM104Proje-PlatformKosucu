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
    void resetPosition();

    private:
    sf::RectangleShape shape;
    sf::Texture texture; // resmi ekran kartı hafızasına yükleyecek
    sf::Sprite sprite; // resmi ekranda hareket ettiren obje
    sf::Vector2f velocity;
    float gravity;
    float moveSpeed;
    float jumpSpeed;
    
};

#endif