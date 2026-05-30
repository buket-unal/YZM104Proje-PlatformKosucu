#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Platform.hpp"

class Player {
    
    public:
    Player();

    void update(float deltaTime, sf::Sound& jumpSpeed); 
    void checkCollision(std::vector<Platform>& platforms);
    void draw(sf::RenderWindow& window); // Ekrana çizer
    sf::Vector2f getPosition() const;
    void resetPosition();
    void takeDamage(float enemyX);
    sf::FloatRect getBounds();
    int getHealth() const {
        return health; //oyuncunun kaç canı kaldığını bilmesi için 
    }
    void setHealth(int h){
        health = h;
    }
    float getVelocityY() const;
    void bounce();

    private:
    sf::RectangleShape shape;
    sf::Texture textureIdle; // Sabit durma resmi (player.png)
    sf::Texture textureWalk; // Yürüme/Zıplama resmi (player_walk.png)
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