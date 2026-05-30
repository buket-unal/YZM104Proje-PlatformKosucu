#include "Platform.hpp"
#include <iostream>


using namespace std;

Platform::Platform(sf::Vector2f size, sf::Vector2f position, const sf::Texture& platformTexture, int level) 
    : texture(platformTexture)
{
    this->currentLevel = level;
    shape.setSize(size);
    shape.setFillColor(sf::Color::Transparent);
    shape.setPosition(position);

    sprite.setTexture(texture.get());
    sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    sprite.setPosition(position);
    
    //  ---- RENK FİLTRESİ ----
    if(currentLevel == 1){
        sprite.setColor(sf::Color::White);
    }
    else if(currentLevel == 2){
        sprite.setColor(sf::Color(130, 150, 180));
    }
    else if(currentLevel >= 3){
        sprite.setColor(sf::Color(160, 90, 80));
    }
}


void Platform::draw(sf::RenderWindow& window) {
    //window.draw(shape);
    window.draw(sprite);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}