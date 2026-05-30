#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <SFML/Graphics.hpp>

class Enemy{
    public:
    // boyut, başlangıç pozisyonu ve ne kadar uzağa gideceği
    Enemy(sf::Texture* texture, sf::Texture* smallTexture, sf::Font* font, sf::Vector2f position, float range);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const; // çarpışma kontrolü için
    void makeSmall();
    bool isDead() const;

    bool getIsSmall() const { return isSmall; }

    private: 
    sf::Sprite sprite;
    float speed;
    float startX; // başladığı yer (sınır kontrolü için)
    float range; // ne kadar uzağa gidip dönecek
    int direction; // 1 sağ, -1 sol

    bool isSmall = false;
    sf::Texture* smallTexture; // küçük görselin adresini tutacak pointer
    sf::Font* gameFont; // main'den gelecek fontun adresini saklamak için ekledim
    sf::Clock smallClock; // Ezildikten sonra geçen 0.2 saniyeyi sayacak kronometre

};  

#endif