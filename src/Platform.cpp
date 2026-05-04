#include "Platform.hpp"
#include <iostream>

sf::Texture Platform::texture;

using namespace std;

Platform::Platform(sf::Vector2f size, sf::Vector2f position) {
    shape.setSize(size);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(position);

    if(!texture.loadFromFile("../assets/ground.png")){
        // Hata kontrolü
        cout << "Zemin resmi yuklenemedi!" << endl;
    }

    sprite.setTexture(texture);

    sprite.setPosition(shape.getPosition());

    sprite.setScale(4.0f, 4.0f);
}

void Platform::draw(sf::RenderWindow& window) {
    //window.draw(shape);
    window.draw(sprite);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}