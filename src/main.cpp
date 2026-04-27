#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Bir pencere oluşturma (800x600 boyutunda)
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platform Kosucusu Oyunu");
    window.setFramerateLimit(60); // Saniyede 60 kare (FPS) sınırı

    // karakter değişkenleri
    sf::RectangleShape player(sf::Vector2f(50.0f, 50.0f));
    player.setFillColor(sf::Color::Cyan);
    player.setPosition(100.0f, 100.0f);

    // Fizik Değişkenleri
    sf::Vector2f velocity(0, 0);
    float gravity = 0.5f;
    float moveSpeed =5.0f;
    float jumpSpeed = -12.0f;

    while(window.isOpen()){
        sf::Event event; // event:bilgisayarda yaptığın fiziksel etkileşimler(fare oynatma,tuşa basma...)
        while(window.pollEvent(event)){ // olayları(esc basma, pencereyi kapatma.. tek tek alıp event'e koyar)
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        // FİZİK VE HAREKET
        velocity.x = 0;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) velocity.x = -moveSpeed;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) velocity.x = moveSpeed;

        velocity.y += gravity; // yer çekimi karakteri hep aşağı çekecek
        player.move(velocity); // karakteri hesaplanan hız kadar yürütecek

        // GÖRÜNÜM
        window.clear();
        window.draw(player); // karakteri sahneye çizecek
        window.display();
    }

    return 0;
} 