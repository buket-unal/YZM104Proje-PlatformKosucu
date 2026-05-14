#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <vector>
#include <SFML/Graphics.hpp>
#include "Platform.hpp"

class Player {
    public:
    Player();

    void update(float deltaTime); 
    void checkCollision(std::vector<Platform>& platforms);
    void draw(sf::RenderWindow& window); // Ekrana çizer
    sf::Vector2f getPosition() const;
    void resetPosition();
    void takeDamage();
    sf::FloatRect getBounds();

    private:
    sf::RectangleShape shape;
    sf::Texture texture; // resmi ekran kartı hafızasına yükleyecek
    sf::Sprite sprite; // resmi ekranda hareket ettiren obje
    sf::Vector2f velocity;
    float gravity;
    float moveSpeed;
    float jumpSpeed;

    bool isGrounded;
    
    sf::Clock animationClock; // Zamanı tutacak
    int currentFrame; // Hangi karede olduğumuzu tutacak
    bool isMoving; // Karakter hareket ediyor mu

    int health;
    bool isInvincible; // hasar aldığında true dönecek
    sf::Clock damageTimer;
};

#endif