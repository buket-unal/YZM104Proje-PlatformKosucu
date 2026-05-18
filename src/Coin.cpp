#include "Coin.hpp"

Coin::Coin(sf::Texture* texture, sf::Vector2f position){
    sprite.setTexture(*texture);
    sprite.setPosition(position);
    sprite.setScale(2.0f, 2.0f);
    this->collected = false;
}

// ---- ÇİZİM ----

void Coin::draw(sf::RenderWindow& window){
    // eğer toplanmadıysa ekrana çizsin
    if(!collected){
        window.draw(sprite);
    }
}

// ---- ÇARPIŞMA ----

sf::FloatRect Coin::getBounds() const{
    return sprite.getGlobalBounds();
}