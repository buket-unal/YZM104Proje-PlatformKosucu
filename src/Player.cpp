#include "Player.hpp"

Player::Player() {

    shape.setSize(sf::Vector2f(30.0f, 40.0f));
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(100.0f, 100.0f);

    // ---- GÖRSEL VE BOYUT AYARLARI ----
    isGrounded = false;

    if (!this->textureIdle.loadFromFile("assets/player.png") || 
        !this->textureWalk.loadFromFile("assets/player_walk.png")) {
        // Hata kontrolü
    }
    
    // Sprite'a bu resmi kullanmasını söylüyoruz
    sprite.setTexture(textureIdle);
    // resim çok küçükse (pixel art olduğu için) 2 kat büyütelim
    sprite.setScale(2.0f, 2.0f);


    // ---- FİZİK VE HAREKET ----
    velocity = sf::Vector2f(0.0f, 0.0f);
    gravity = 0.4f;
    moveSpeed = 5.0f;
    jumpSpeed = -15.0f;

    // ---- ANİMASYON DURUMLARI ----
    currentFrame = 0; // ilk resimle başlayacak
    isMoving = false; // başlangıçta karakter dursun

    // ---- OYUN İSTATİSTİKLERİ ----
    health = 6;
    isInvincible = false;    
}

void Player::update(float deltaTime) {

    // ---- DURUM GÜNCELLEMELRİ ----
    if(isInvincible){
        if(damageTimer.getElapsedTime().asSeconds() > 1.5f){
            isInvincible = false;
        }
    }

    // KRİTİK DÜZELTME: Hasar alındıktan sonraki ilk 0.2 saniye boyunca
    // klavye girdilerini tamamen engelliyorum (Knockback Kilitlenmesi)

    if(isInvincible && damageTimer.getElapsedTime().asSeconds() < 0.2f){
        // Yer çekimini fırlama hızına uygulamaya devam etmek için hareketi işletiyorum
        velocity.y += gravity * 60.0f; 
        shape.move(velocity * deltaTime); 
        sprite.setPosition(shape.getPosition().x - 10, shape.getPosition().y - 8.0f); // -8.0f zemine sıfır 

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
    }

    velocity.y += gravity * 60.0f; // yer çekimi
    shape.move(velocity * deltaTime); // hareket uygulama
    // görselin, görünmez çarpışma kutusunu (shape) takip etmesini sağlayacak
    sprite.setPosition(shape.getPosition().x - 10, shape.getPosition().y - 8.0f);

    // ---- ANİMASYON VE GÖRSEL MANTIK ----
    if(!isGrounded){
        // karakter havadaysa,zıplıyorsa; yürümeyi değil zıplama resmini gösterecek
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
        // duruyorsa sabit durma resmi
        sprite.setTexture(textureIdle);
    }
    
    // yön ayarları
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
        //damageTimer'ı milisnaiyeye çevirip 150'ye bölüyorum
        // %2 olduğunda sonuç sürekli 0 veya 1 çıkacak(hızlıca yanıp sönme hissi verecek)
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
    // karakterin kendi sınıflarını (hayalet kutusunu) alıyoruz
    sf::FloatRect playerBounds = shape.getGlobalBounds();

    // varsayılan olarak havada olduğunu kabul edilecek
    // aşağıda döngüde bir yere değiyorsa true yapılacak
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
                if(playerBounds.left + playerBounds.width / 2.0f < platformBounds.left + platformBounds.width / 2.0f){
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

void Player::takeDamage(){
    if(!isInvincible){
        health--;
        isInvincible = true;
        damageTimer.restart(); // şu andan itibaren kronometre başlatacak,bu karakteri hasar aldığı 0. saniye

        // karakteri hafif geri ve yukarı fırlatmak için
        velocity.x = -400.0f;
        velocity.y = -300.0f;

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