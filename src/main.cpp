#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include <vector>
#include "Platform.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platform Kosucusu");
    window.setFramerateLimit(60);

    std::vector<Platform> platforms;
    // Platformları listeye ekliyoruz
    platforms.push_back(Platform(sf::Vector2f(200.0f, 20.0f), sf::Vector2f(400.0f, 400.0f)));
    platforms.push_back(Platform(sf::Vector2f(150.0f, 20.0f), sf::Vector2f(100.0f, 300.0f)));

    Player player;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // --- GÜNCELLEME ---
        player.update();
        player.checkCollision(platforms); // Artık sayı değil, liste gönderiyoruz!
 
        // --- ÇİZİM ---
        window.clear();
        
        // 1. Önce platformları çizdiriyoruz (Döngü ile listedeki her şeyi ekrana basar)
        for (auto& platform : platforms) {
            platform.draw(window);
        }

        // 2. Sonra karakteri çizdiriyoruz
        player.draw(window);
        
        window.display();
    }

    return 0;
}