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

    
    int margin = 5;

    sprite.setTextureRect(sf::IntRect(margin, margin, texture.getSize().x-(margin*2), texture.getSize().y-(margin*2)));

    // kırpılmış görselin yeni boyutunu alacak
    sf::FloatRect spriteSize = sprite.getLocalBounds();

    // fiziksel kutuyu görsele eşitlemek lazım, resimden 5 piksel kırptığım için fiziksel
    // kutunun üst sınırını da görsel olarak o kadar aşağı çekmiş oldum
    // çarpışma kutusunun (shape) yüksekliğini ve konumunu buna göre ayarlıyorum

    // görseldeki çimlerin gerçek konumuna gelmesi için shape'i biraz küçültüyorum
    shape.setSize(sf::Vector2f(size.x, size.y));

    shape.setPosition(position.x, position.y + margin);

    // sprite'ı (görseli) ise orijinal pozisyonuna koyuyorum
    sprite.setPosition(position.x, position.y);
    

    // 0'a bölmeyi engellemek için güvenlik kontrolü yapıyorum
    if(spriteSize.width > 0 && spriteSize.height > 0){
        // platformun istediğimiz boyutun (getSize()) göre ölçeklendiriyorum
        sprite.setScale(
            shape.getSize().x / spriteSize.width,
            shape.getSize().y / spriteSize.height
        );
    }

}

void Platform::draw(sf::RenderWindow& window) {
    //window.draw(shape);
    window.draw(sprite);
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}