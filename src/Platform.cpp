#include "Platform.hpp"
#include <iostream>


using namespace std;

Platform::Platform(sf::Vector2f size, sf::Vector2f position, sf::Texture* platformTexture, int level) {
    shape.setSize(size);
    shape.setFillColor(sf::Color::Transparent);
    shape.setPosition(position);

    this->texture = platformTexture;
    this->texture->setRepeated(true);
    sprite.setTexture(*this->texture);
    sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    sprite.setPosition(position);
    
    //  ---- GECE/GÜNDÜZ RENK FİLTRESİ ----
    if(level >= 2){
        sprite.setColor(sf::Color(70, 70, 100));
    }
    else{
        sprite.setColor(sf::Color::White);
    }
}


void Platform::draw(sf::RenderWindow& window) {
    //window.draw(shape);
    window.draw(sprite);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}