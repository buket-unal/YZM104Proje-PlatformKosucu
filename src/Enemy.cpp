#include "Enemy.hpp"

Enemy::Enemy(sf::Texture* texture, sf::Texture* smallTexture, sf::Font* font,  sf::Vector2f position, float range){
    sprite.setTexture(*texture);
    // orijini görselin tam ortası yapıyorum
    sf::FloatRect localBounds = sprite.getLocalBounds();
    sprite.setOrigin(localBounds.width / 2.0f, localBounds.height);
    
    sprite.setPosition(position);
    this->smallTexture = smallTexture;
    this->gameFont = font; 
    this->startX = position.x;
    this->range = range;
    this->speed = 100.0f;
    this->direction = 1;
    this->isSmall = false; 
    sprite.setScale(2.5f, 2.5f); 
}

void Enemy::update(float deltaTime){
    // küçülen düşman yerde sabit dursun
    if(this->isSmall){
        return;
    }
    
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

        // eğer düşmanı küçültüp yok ettiysek +2 yazsın
        if(this->isSmall && this->gameFont){
            
                sf::Text plusTwoText;
                plusTwoText.setFont(*this->gameFont); // diskten okumayacak, ramdeki hazır fontu kullanacak
                plusTwoText.setString("+2");
                plusTwoText.setCharacterSize(18);
                plusTwoText.setFillColor(sf::Color::Yellow);
                plusTwoText.setStyle(sf::Text::Bold);

                // Yazıyı düşmanın tam kafasının biraz üstünde konumlandırıyorum
                // smallClock ilerledikçe Y koordinatını azaltarak yazının yukarı doğru "uçmasını" sağlıyorum
                float elapsed = this->smallClock.getElapsedTime().asSeconds();
                float animationOffset = elapsed * 80.0f; // Yukarı doğru uçma hızı

                plusTwoText.setPosition(sprite.getPosition().x - 10.0f, sprite.getPosition().y - 60.0f - animationOffset);
                window.draw(plusTwoText);
            }

        }
    

sf::FloatRect Enemy::getBounds() const{
        return sprite.getGlobalBounds();
    }

void Enemy::makeSmall() {
    this->isSmall = true;
    this->sprite.setTexture(*this->smallTexture); 
    
    sf::FloatRect localBounds = sprite.getLocalBounds();
    sprite.setOrigin(localBounds.width / 2.0f, localBounds.height);

    // Yönü ne olursa olsun (sağ/sol) küçülünce düz dursun diye scale değerini 2.5f yapıyorum
    sprite.setScale(2.5f, 2.5f); 
    this->smallClock.restart(); // 0.2 saniyelik yok olma kronometresi 
}

bool Enemy::isDead() const {
    // Hem küçülmüş olmalı hem de üzerinden 0.2 saniye geçmiş olmalı
    return this->isSmall && this->smallClock.getElapsedTime().asSeconds() > 0.2f;
}