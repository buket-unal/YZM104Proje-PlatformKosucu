#ifndef FLYING_ENEMY_HPP
#define FLYING_ENEMY_HPP

#include <SFML/Graphics.hpp>
#include <cmath> // yarasanın havada süzülme mekaniği için

class FlyingEnemy{
    public:
    // Yapıcı fonksiyon: Kanat açık/kapalı görselleri, başlangıç konumu ve süzülme mesafesi
    FlyingEnemy(sf::Texture* texOpen, sf::Texture* texClosed, sf::Vector2f position, float amplitude);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;

    private:
    sf::Sprite sprite;
    sf::Texture* textureOpen;
    sf::Texture* textureClosed;

    float speed;
    float startY;  // yarasanın havada asılı kalacağı merkez yükseklik
    float amplitude; // aşağı-yukarı ne kadar geniş bir mesafede süzüleceği
    float totalTime; // süzülme matematiği için zaman tutucu

    // animasyon değişkenleri
    float animationTimer;
    float animationSwitchTime; // kaç saniyede bir kanat çırpacak
    bool isWingOpen;
};

#endif