#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // 1. Bir pencere oluşturuyoruz (800x600 boyutunda)
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platform Kosucusu Oyunu");
    window.setFramerateLimit(60); // Saniyede 60 kare (FPS) sınırı

    while(window.isOpen()){
        sf::Event event; // event:bilgisayarda yaptığın fiziksel etkileşimler(fare oynatma,tuşa basma...)
        while(window.pollEvent(event)){ // olayları(esc basma, pencereyi kapatma.. tek tek alıp event'e koyar)
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }
    }
    
    window.clear();
    window.display();

    return 0;
}