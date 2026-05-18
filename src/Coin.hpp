#ifndef COIN_HPP
#define COIN_HPP

#include <SFML/Graphics.hpp>

class Coin{
    public:

    // coinin nerede duracağını ve hangi resmi kullanacağını belirleyen yapıcı fonksiyonum
    Coin(sf::Texture* texture, sf::Vector2f position);

    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    bool isCollected() const{
        return collected;
    }
    void collect(){
        collected = true;
    }

    private:
    sf::Sprite sprite;
    bool collected = false;
};


#endif
