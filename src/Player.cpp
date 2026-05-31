#include "Player.hpp"

Player::Player() {
    shape.setSize(sf::Vector2f(30.0f, 40.0f));
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(100.0f, 100.0f);

    // ---- GÖRSEL VE BOYUT AYARLARI ----
    isGrounded = false;

    if (!this->textureIdle.loadFromFile("assets/player.png") || 
        !this->textureWalk.loadFromFile("assets/player_walk.png")) {
        // hata kontrolü
    }

    sprite.setTexture(textureIdle);
    sprite.setScale(2.0f, 2.0f);


    // ---- FİZİK VE HAREKET ----
    velocity = sf::Vector2f(0.0f, 0.0f);
    gravity = 0.4f;
    moveSpeed = 5.0f;
    jumpSpeed = -15.0f;

    // ---- ANİMASYON DURUMLARI ----
    currentFrame = 0; // ilk resimle başlayacak
    isMoving = false; 

    // ---- OYUN İSTATİSTİKLERİ ----
    health = 6;
    isInvincible = false;    
}

void Player::update(float deltaTime, sf::Sound& jumpSound) {

    // ---- DURUM GÜNCELLEMELRİ ----
    if(isInvincible){
        if(damageTimer.getElapsedTime().asSeconds() > 1.5f){
            isInvincible = false;
        }
    }

    // Hasar alındıktan sonraki ilk 0.2 saniye boyunca
    // klavye girdilerini tamamen engelliyorum (Knockback Kilitlenmesi)

    if(isInvincible && damageTimer.getElapsedTime().asSeconds() < 0.2f){
        velocity.y += gravity * 60.0f; 
        shape.move(velocity * deltaTime); 
        sprite.setPosition(shape.getPosition().x - 10, shape.getPosition().y - 8.0f);  
        return;
    }
    else{
        velocity.x = 0;
        isMoving = false;
    }

    // ---- HAREKET VE GİRDİLER ----
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = moveSpeed * 60.0f;
        isMoving = true;
    }
       else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -moveSpeed * 60.0f;
        isMoving = true;
    }

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && isGrounded){
        velocity.y = jumpSpeed * 60.0f;
        isGrounded = false;
        jumpSound.play();
    }

    velocity.y += gravity * 60.0f; // yer çekimi
    shape.move(velocity * deltaTime); // hareket uygulama
    // görselin, görünmez çarpışma kutusunu (shape) takip etmesini sağlayacak
    sprite.setPosition(shape.getPosition().x - 10, shape.getPosition().y - 8.0f);

    // ---- ANİMASYON VE GÖRSEL MANTIK ----
    if(!isGrounded){
        sprite.setTexture(textureWalk);
    }
    else if(isMoving){
        if(animationClock.getElapsedTime().asSeconds() > 0.1f){
            currentFrame = (currentFrame == 0) ? 1 : 0;
            
            if(currentFrame == 0) sprite.setTexture(textureIdle);
            else sprite.setTexture(textureWalk);

            animationClock.restart();
        }
    }
    else{
        sprite.setTexture(textureIdle);
    }
    
    // Yön Ayarları
    if(velocity.x > 0){ // sağa gidiyorsa
        // orijinsl resim sola baktığı için sağa giderken ayna efekti yapıyorum
        sprite.setScale(-2.0f, 2.0f);
        // genişliği o anki resimden alıyorum
        sprite.setOrigin(sprite.getLocalBounds().width, 0.f);
    }
    else if(velocity.x < 0){ // sola gidiyorsa
        sprite.setScale(2.0f, 2.0f);
        sprite.setOrigin(0.f, 0.f);   
    }

    // ÖLÜMSÜZLÜK efektini renkle belli etmek için
    if(isInvincible){
        int timeMs = static_cast<int>(damageTimer.getElapsedTime().asMilliseconds());
        if((timeMs / 150) % 2 == 0){
            sprite.setColor(sf::Color(255, 0, 0, 70)); // şeffaf hali
        } else{
            sprite.setColor(sf::Color(255, 255, 255, 255)); // tam görünür hali
        }
    }
    else{
        sprite.setColor(sf::Color::White); // hasar bitince her zaman normal halinde olmsı için
    }
    
}

void Player::checkCollision(std::vector<Platform>& platforms){
    // karakterin kendi sınıflarını (hayalet kutusunu) alıyorum
    sf::FloatRect playerBounds = shape.getGlobalBounds();
    bool touchAnyPlatform = false;

    for(auto& platform : platforms){
        sf::FloatRect platformBounds = platform.getBounds();

        // eğer kesişme varsa
        if(playerBounds.intersects(platformBounds)){
        // karakterin ve platformun merkez noktalarını bulup hangi eksende daha çok iç içe geçtiklerini hesaplamak için
        float overlapX = std::min(playerBounds.left + playerBounds.width, platformBounds.left + platformBounds.width) - std::max(playerBounds.left, platformBounds.left);
        float overlapY = std::min(playerBounds.top + playerBounds.height, platformBounds.top + platformBounds.height) - std::max(playerBounds.top, platformBounds.top);

        // eğer yataydaki iç içe geçme daha küçükse bu bir yandan çarpmadır
        if(overlapX < overlapY){
            // oyuncu platformun solundaysa (sağa doğru çarpıyorsa)
            // karakterin merkez noktası, platformun merkez noktasından daha soldaysa, karakter platformun sol duvarına çarpmıştır
                if(playerBounds.left + playerBounds.width / 2.0f < platformBounds.left + platformBounds.width / 2.0f){
// karakteri platformun içine girmekten kurtarmak için onu geriye doğru iter. karakterin yeni x koordinatını, 
// platformun başladığı yerin (platformBounds.left) karakterin kendi genişliği kadar gerisine (- playerBounds.width) sabitler
                shape.setPosition(platformBounds.left - playerBounds.width, playerBounds.top);
                }
            // oyuncu platformun sağındaysa (sola doğru çarpıyorsa)
                else{
                    shape.setPosition(platformBounds.left + platformBounds.width, playerBounds.top);
                }
                velocity.x = 0; // yatay hızı 0 olsun ki duvar hissi versin
            }
        // eğer dikeydeki iç içe geçme daha küçükse bu üstten veya alttan çarpmadır
            else{
            // üstten çarpma
                if(velocity.y > 0 && playerBounds.top + playerBounds.height / 2.0f < platformBounds.top + platformBounds.height / 2.0f){
                velocity.y = 0;
                shape.setPosition(playerBounds.left, platformBounds.top - playerBounds.height);
                isGrounded = true;
                touchAnyPlatform = true;
                }
                else if(velocity.y < 0){
                velocity.y = 0;
                shape.setPosition(playerBounds.left, platformBounds.top + platformBounds.height);
                }
            }      
        }
    }
    if(!touchAnyPlatform){
        isGrounded = false;
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::Vector2f Player::getPosition() const {
    return shape.getPosition();
}

void Player::resetPosition(){
    shape.setPosition(100.0f, 100.0f); // başlangıç noktasına gönderecek
    velocity = sf::Vector2f(0.0f, 0.0f); // düşme hızını sıfırlayacak
}

void Player::takeDamage(float enemyX){
    if(!isInvincible){
        health--;
        isInvincible = true;
        damageTimer.restart(); // hasar aldığı o ilk saliseyi 0.0 olarak kabul etmek için ölümsüzlük kronometresini sıfırlayıp baştan başlatacak

        if(shape.getPosition().x < enemyX){
            velocity.x = -400.0f; 
        }
        else{
            velocity.x = 400.0f;
        }
        velocity.y = -300.0f; // yukarı fırlaması için
        isGrounded = false;
    }
}

sf::FloatRect Player::getBounds(){
    return shape.getGlobalBounds();
}

float Player::getVelocityY() const{
    return velocity.y;
}

void Player::bounce(){
    velocity.y = -350; // oyuncuyu eksi yönde yani yukarı doğru fırlatıyorum
}