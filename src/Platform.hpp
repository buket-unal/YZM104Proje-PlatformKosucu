#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SFML/Graphics.hpp>

class Platform {
public:
    Platform(sf::Vector2f size, sf::Vector2f position, sf::Texture* platfomTexture);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
    sf::Texture* texture; 
    sf::Sprite sprite; 
};

#endif