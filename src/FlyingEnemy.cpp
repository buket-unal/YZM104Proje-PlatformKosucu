#include "FlyingEnemy.hpp"

FlyingEnemy::FlyingEnemy(sf::Texture* texOpen, sf::Texture* texClosed, sf::Vector2f position, float amplitude){
    textureOpen = texOpen;
    textureClosed = texClosed;

    sprite.setTexture(*textureOpen); // oyun başlarken yarasa ilk olarak kanatları açık görselle gelsin
    sprite.setScale(2.0f, 2.0f);

    // görselin merkez noktasını (origin) tam ortası yapıyorum ki aşağı-yukarı pürüzsüz süzülsün
    sf::FloatRect bounds = sprite.getGlobalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    sprite.setPosition(position);

    startY = position.y; // yarasanın havda asılı kalcağı orta hizayı kaydettim
    this->amplitude = amplitude; // Aşağı ve yukarı kaç piksel esneyeceğini belirledim
    speed = 3.0f;
    totalTime = 0.0f;

    // kanat çırpma zamanlayıcıları
    animationTimer = 0.0f;
    animationSwitchTime = 0.2f; // saniyede 5 kez kanat çırpacak
    isWingOpen = true;
}

void FlyingEnemy::update(float deltaTime){
    // havada dalgalanma/süzülme hareketi (sinüs dalgası)
    totalTime += deltaTime * speed;
    float newY = startY + std::sin(totalTime) * amplitude;
    sprite.setPosition(sprite.getPosition().x, newY);

    // kanat çırpma aimasyonu
    animationTimer += deltaTime;
    if(animationTimer >= animationSwitchTime){
        animationTimer = 0.0f;
        isWingOpen = !isWingOpen; // durumu tersine çevirmesi için (açıksa kapatacak, kapalıysa açacak)

        if(isWingOpen){
            sprite.setTexture(*textureOpen);
        }
        else{
            sprite.setTexture(*textureClosed);
        }
    }
}

void FlyingEnemy::draw(sf::RenderWindow& window){
    window.draw(sprite);
}

sf::FloatRect FlyingEnemy::getBounds() const {
    return sprite.getGlobalBounds();
}