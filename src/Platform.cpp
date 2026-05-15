#include "Platform.hpp"
#include <iostream>

sf::Texture Platform::texture;

using namespace std;

Platform::Platform(sf::Vector2f size, sf::Vector2f position, std::string texturePath) {
    shape.setSize(size);
    shape.setFillColor(sf::Color::Transparent);
    shape.setPosition(position);

    if(!texture.loadFromFile(texturePath)){
        // Hata kontrolü
        cout << texturePath << " yuklenemedi! " << endl;
    }
    
    texture.setRepeated(true); // resmin esnememesi, yan yana dizilmesi için
    sprite.setTexture(texture);
    // kutunun boyutu kadar doku alanı ayarlamak için
    sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    sprite.setPosition(position); // görseli fiziksel kutu ile aynı yere koydum

    
}

void Platform::draw(sf::RenderWindow& window) {
    //window.draw(shape);
    window.draw(sprite);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}