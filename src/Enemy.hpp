#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <SFML/Graphics.hpp>

class Enemy{
    public:
    // boyut, başlangıç pozisyonu ve ne kadar uzağa gideceği
    Enemy(sf::Texture* texture, sf::Vector2f position, float range);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const; // çarpışma kontrolü için



    private: 
    sf::Sprite sprite;
    float speed;
    float startX; // başladığı yer (sınır kontrolü için)
    float range; // ne kadar uzağa gidip dönecek
    int direction; // 1 sağ, -1 sol

};  

#endif