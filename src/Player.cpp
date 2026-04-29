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

void Player::checkCollision(float groundLevel) {
    // Zemin Çarpışma Kontrolü
    if (shape.getPosition().y + shape.getSize().y >= groundLevel) {
        shape.setPosition(shape.getPosition().x, groundLevel - shape.getSize().y);
        velocity.y = 0;

        // Zıplama (Sadece yerdeyken)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            velocity.y = jumpSpeed;
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}