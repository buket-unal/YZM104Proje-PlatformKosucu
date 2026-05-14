#include "Enemy.hpp"

Enemy::Enemy(sf::Texture* texture, sf::Vector2f position, float range){
    sprite.setTexture(*texture);
    sprite.setPosition(position);

    this->startX = position.x;
    this->range = range;
    this->speed = 100.0f;
    this->direction = 1;

    sprite.setScale(2.5f, 2.5f);
}

void Enemy::update(float deltaTime){
    sprite.move(speed * direction * deltaTime, 0);

    if(sprite.getPosition().x > startX + range){
        direction = -1;
        sprite.setScale(-2.5f, 2.5f);
        sprite.setOrigin(sprite.getLocalBounds().width, 0);
    }
    else if(sprite.getPosition().x < startX){
        direction = 1;
        sprite.setScale(2.5f, 2.5f);
        sprite.setOrigin(0, 0);
    }
}

void Enemy::draw(sf::RenderWindow& window){
        window.draw(sprite);
    }

sf::FloatRect Enemy::getBounds() const{
        return sprite.getGlobalBounds();
    }
