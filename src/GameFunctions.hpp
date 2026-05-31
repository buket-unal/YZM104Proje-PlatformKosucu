#ifndef GAME_FUNCTIONS_HPP
#define GAME_FUNCTIONS_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include "Coin.hpp"
#include "Player.hpp"
#include "Platform.hpp"
#include "Enemy.hpp"
#include "FlyingEnemy.hpp"

// ---- GENEL HAFIZA SİLECEK MOTORU (TEMPLATES) ----
template <typename T>
void temizleEkranDisiNesneler(std::vector<T>& liste, float silecekSinirX) {
    liste.erase(
        std::remove_if(liste.begin(), liste.end(), [silecekSinirX](const T& nesne) {
            return (nesne.getBounds().left + nesne.getBounds().width) < silecekSinirX;
        }),
        liste.end()
    );
}

// ---- DİĞER YARDIMCI FONKSİYONLARIN PROTOTİPLERİ ----
void spawnCoins(float newX, float newY, std::vector<Coin>& coins, sf::Texture* coinTexture);

void temizleAltinlar(std::vector<Coin>& coins, float silecekSinirX);

void initializeStartingMap(std::vector<Platform>& platforms, std::vector<Coin>& coins, sf::Texture* platformTexture, sf::Texture* coinTexture, int currentLevel);

void loadLevelAssets(int level, 
                    sf::Texture& backgroundTexture, 
                    sf::Texture& enemyTexture, 
                    sf::Texture& enemySmallTexture, 
                    sf::Texture& flyingEnemyOpenTexture, 
                    sf::Texture& flyingEnemyClosedTexture, 
                    sf::Texture& acidTexture);

void drawHUD(sf::RenderWindow& window, Player& player, int score, int currentLevel, 
             sf::Text& scoreText, sf::Text& levelText, sf::Sprite& heartSprite, 
             sf::Texture& heartFulltex, sf::Texture& heartHalfTex, sf::Texture& heartEmptyTex);

void drawGameScreens(sf::RenderWindow& window, sf::Font& gameFont, int currentLevel, int score,
                     bool isLevelCompleteScreen, bool isGameOverScreen, bool isGameFinishedScreen,
                     sf::RectangleShape& overlayPanel, sf::RectangleShape& nextLevelButton, sf::Text& buttonText,
                     sf::RectangleShape& gameOverOverlay, sf::RectangleShape& tryAgainButton, sf::Text& tryAgainText,
                     sf::RectangleShape& gameCompleteOverlay, sf::RectangleShape& playAgainButton, sf::Text& playAgainText);

#endif