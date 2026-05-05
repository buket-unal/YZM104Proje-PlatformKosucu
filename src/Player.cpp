#include "Player.hpp"

Player::Player() {
    shape.setSize(sf::Vector2f(50.0f, 50.0f));
    shape.setFillColor(sf::Color::Cyan);
    shape.setPosition(100.0f, 100.0f);

    velocity = sf::Vector2f(0.0f, 0.0f);
    gravity = 0.4f;
    moveSpeed = 5.0f;
    jumpSpeed = -15.0f;

    isGrounded = false;

    if(!texture.loadFromFile("../assets/player.png")){
        // resim yüklenmezse terminalde hata mesajı verecek
    }

    // Sprite'a bu resmi kullanmasını söylüyoruz
    sprite.setTexture(texture);

    // resim çok küçükse (pixel art olduğu için) 2 kat büyütelim
    sprite.setScale(2.0f, 2.0f);
}

void Player::update() {
    // Yatay Hareket
    velocity.x = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = moveSpeed;
    }

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && isGrounded){
        velocity.y = jumpSpeed;
        isGrounded = false;
    }

    // Yer Çekimi
    velocity.y += gravity;

    // Hareket Uygula
    shape.move(velocity);

    // görselin, görünmez çarpışma kutusunu (shape) takip etmesini sağlayacak
    sprite.setPosition(shape.getPosition());
}

void Player::checkCollision(std::vector<Platform>& platforms){
    // karakterin kendi sınıflarını (hayalet kutusunu) alıyoruz
    sf::FloatRect playerBounds = shape.getGlobalBounds();

    // varsayılan olarak havada olduğunu kabul edilecek
    // aşağıda döngüde bir yere değiyorsa true yapılacak

    bool touchAnyPlatform = false;

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
                isGrounded = true;
                touchAnyPlatform = true;
                
            }
        }
    }
    // eğer hiçbir platforma değmiyorsa isGrounded false kalacak
    if(!touchAnyPlatform){
        isGrounded = false;
    }
}

void Player::draw(sf::RenderWindow& window) {
    // window.draw(shape);
    window.draw(sprite);
}

sf::Vector2f Player::getPosition() const {
    return shape.getPosition();
}

void Player::resetPosition(){
    shape.setPosition(100.0f, 100.0f); // başlangıç noktasına gönderecek
    velocity = sf::Vector2f(0.0f, 0.0f); // düşme hızını sıfırlayacak
}