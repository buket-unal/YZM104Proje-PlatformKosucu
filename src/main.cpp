#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // 1. Bir pencere oluşturuyoruz (800x600 boyutunda)
    sf::RenderWindow window(sf::VideoMode(800, 600), "Buket'in Ilk SFML Projesi");

    // 2. Yeşil dairemizi hazırlıyoruz
    sf::CircleShape shape(100.f); // 100 yarıçapında
    shape.setFillColor(sf::Color::Green); // Rengi yeşil
    shape.setPosition(300.f, 200.f); // Penceredeki yeri

    std::cout << "Pencere aciliyor, heyecan dorukta!" << std::endl;

    // 3. Oyun döngüsü (Pencere açık kaldığı sürece çalışır)
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();      // Ekranı temizle
        window.draw(shape);   // Daireyi çiz
        window.display();    // Ekranda göster
    }

    return 0;
}