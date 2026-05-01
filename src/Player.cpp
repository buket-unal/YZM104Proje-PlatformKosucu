#include "Player.hpp"

Player::Player() {
    shape.setSize(sf::Vector2f(50.0f, 50.0f));
    shape.setFillColor(sf::Color::Cyan);
    shape.setPosition(100.0f, 100.0f);

    velocity = sf::Vector2f(0.0f, 0.0f);
    gravity = 0.4f;
    moveSpeed = 5.0f;
    jumpSpeed = -15.0f;
}

void Player::update() {
    // Yatay Hareket
    velocity.x = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x = -moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x = moveSpeed;
    }

    // Yer Çekimi
    velocity.y += gravity;

    // Hareket Uygula
    shape.move(velocity);
}

void Player::checkCollision(std::vector<Platform>& platforms){
    // karakterin kendi sınıflarını (hayalet kutusunu) alıyoruz
    sf::FloatRect playerBounds = shape.getGlobalBounds();

    for(auto& platform : platforms){
        sf::FloatRect platformBounds = platform.getBounds();

        // eğer karakter ve platform kutuları kesişiyorsa (çarpışma varsa)
        if(playerBounds.intersects(platformBounds)){
            // Üstten çarpma (Platformun üstünde durma) mantığı
            // Karakter aşağı düşerken çarpıyorsa:
            if(velocity.y > 0 && playerBounds.top < platformBounds.top){
                velocity.y = 0; // düşmeyi durdur
                shape.setPosition(playerBounds.left, platformBounds.top - playerBounds.height);
                // buradan zıplayabilir
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
                    velocity.y = jumpSpeed;
                }
            }
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}