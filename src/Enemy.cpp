#include "Enemy.hpp"

Enemy::Enemy(sf::Texture* texture, sf::Vector2f position, float range){
    sprite.setTexture(*texture);

    // orijini görselin tam ortası yapıyorum
    sf::FloatRect localBounds = sprite.getLocalBounds();
    sprite.setOrigin(localBounds.width / 2.0f, localBounds.height);

    sprite.setPosition(position);

    this->startX = position.x;
    this->range = range;
    this->speed = 100.0f;
    this->direction = 1;

    sprite.setScale(2.5f, 2.5f); // başlangıçta sağa bakacak
}

void Enemy::update(float deltaTime){
    sprite.move(speed * direction * deltaTime, 0);

    // Sınır kontrollerine genişliğin yarısını (offset) ekleyip çıkarmak milimetrik hassasiyet sağlayacak
   float yarimGenislik = (sprite.getGlobalBounds().width / 2.0f);

    if(sprite.getPosition().x + yarimGenislik > startX + range){
        direction = -1;
        sprite.setScale(-2.5f, 2.5f); // Sola döndürme
    }
    else if(sprite.getPosition().x - yarimGenislik < startX){
        direction = 1;
        sprite.setScale(2.5f, 2.5f);  // Sağa döndürme
    }
}

void Enemy::draw(sf::RenderWindow& window){
        window.draw(sprite);
    }

sf::FloatRect Enemy::getBounds() const{
        return sprite.getGlobalBounds();
    }
