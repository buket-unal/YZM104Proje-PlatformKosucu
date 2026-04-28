#include <SFML/Graphics.hpp>
#include "Player.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platform Kosucusu");
    window.setFramerateLimit(60);

    Player player; // Karakter objemiz

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // --- GÜNCELLEME ---
        player.update();
        player.checkCollision(500.0f); // 500. pikseli zemin yaptık

        // --- ÇİZİM ---
        window.clear();
        player.draw(window);
        window.display();
    }

    return 0;
}