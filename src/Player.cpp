#include "Player.hpp"

Player::Player() {
    shape.setSize(sf::Vector2f(30.0f, 40.0f));
    shape.setFillColor(sf::Color::Red);
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

    currentFrame = 0; // ilk resimle başlayacak
    isMoving = false; // başlangıçta karakter dursun
}

void Player::update() {
    // Yatay Hareket
    velocity.x = 0;
    isMoving = false; // her karede başta duruyor kabul ettim

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        velocity.x = moveSpeed;
        isMoving = true;
       
    }

       else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        velocity.x = -moveSpeed;
        isMoving = true;

    }


    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && isGrounded){
        velocity.y = jumpSpeed;
        isGrounded = false;
    }

    if(!isGrounded){
        // karakter havadaysa,zıplıyorsa; yürümeyi değil zıplama resmini gösterecek
        texture.loadFromFile("../assets/player_walk.png");
    }
    else if(isMoving){
        if(animationClock.getElapsedTime().asSeconds() > 0.1f){
            currentFrame = (currentFrame == 0) ? 1 : 0;
            
            if(currentFrame == 0) texture.loadFromFile("../assets/player.png");
            else texture.loadFromFile("../assets/player_walk.png");
            animationClock.restart();
        }
    }
    else{
        // duruyorsa sabit durma resmi
        texture.loadFromFile("../assets/player.png");
    }

    // texture değişmiş olabilir sprite'a buna bakmasını söylüyorum
    sprite.setTexture(texture);

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

    // Yer Çekimi
    velocity.y += gravity;

    // Hareket Uygula
    shape.move(velocity);

    // görselin, görünmez çarpışma kutusunu (shape) takip etmesini sağlayacak
    sprite.setPosition(shape.getPosition().x - 10, shape.getPosition().y -15);
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