#include "GameFunctions.hpp"
#include <cmath> // std::sin için

void spawnCoins(float newX, float newY, std::vector<Coin>& coins, sf::Texture* coinTexture) {
    float spacing = 45.0f; 
    float startX = newX + (150.0f - spacing - 24.0f) / 2.0f; 
    
    coins.push_back(Coin(coinTexture, sf::Vector2f(startX, newY - 35.0f)));
    coins.push_back(Coin(coinTexture, sf::Vector2f(startX + spacing, newY - 35.0f)));
}

void temizleAltinlar(std::vector<Coin>& coins, float silecekSinirX) {
    coins.erase(
        std::remove_if(coins.begin(), coins.end(), [silecekSinirX](const Coin& c) {
            return c.isCollected() || (c.getBounds().left + c.getBounds().width) < silecekSinirX;
        }),
        coins.end()
    );
}

void initializeStartingMap(std::vector<Platform>& platforms, std::vector<Coin>& coins, sf::Texture* platformTexture, sf::Texture* coinTexture, int currentLevel) {
    platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), platformTexture, currentLevel));
    platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), platformTexture, currentLevel));
    platforms.push_back(Platform(sf::Vector2f(150.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), platformTexture, currentLevel));
    platforms.push_back(Platform(sf::Vector2f(150.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), platformTexture, currentLevel));

    for(auto& platform : platforms){
        sf::FloatRect pBounds = platform.getBounds();
        if(pBounds.top < 550.0f){
            spawnCoins(pBounds.left, pBounds.top, coins, coinTexture);
        }
    }
}

void loadLevelAssets(int level, 
                    sf::Texture& backgroundTexture, 
                    sf::Texture& enemyTexture, 
                    sf::Texture& enemySmallTexture, 
                    sf::Texture& flyingEnemyOpenTexture, 
                    sf::Texture& flyingEnemyClosedTexture, 
                    sf::Texture& acidTexture) {
    if (level == 1) {
        if (!backgroundTexture.loadFromFile("assets/level1/background.png") || 
            !enemyTexture.loadFromFile("assets/level1/enemy.png") ||
            !enemySmallTexture.loadFromFile("assets/enemy_small.png")) {
            std::cout << "Level 1 gorselleri yuklenemedi!" << std::endl;
        }
    } 
    else if (level == 2) {
        if (!backgroundTexture.loadFromFile("assets/level2/background_night.png") ||
            !flyingEnemyOpenTexture.loadFromFile("assets/level2/flying_enemy_open.png") ||
            !flyingEnemyClosedTexture.loadFromFile("assets/level2/flying_enemy_closed.png")) {
            std::cout << "Level 2 gorselleri yuklenemedi!" << std::endl;
        }
        backgroundTexture.setRepeated(true);
    } 
    else if (level == 3) {
        if (!backgroundTexture.loadFromFile("assets/level3/background_cave.png") ||
            !acidTexture.loadFromFile("assets/level3/hazard_liquid.png")) {
            std::cout << "Level 3 gorselleri yuklenemedi!" << std::endl;
        }
        backgroundTexture.setRepeated(true);
        acidTexture.setRepeated(true);
    }
}

void drawHUD(sf::RenderWindow& window, Player& player, int score, int currentLevel, 
             sf::Text& scoreText, sf::Text& levelText, sf::Sprite& heartSprite, 
             sf::Texture& heartFulltex, sf::Texture& heartHalfTex, sf::Texture& heartEmptyTex) {
             
    window.setView(window.getDefaultView());

    scoreText.setString("Skor: " + std::to_string(score));
    scoreText.setPosition(180.0f, 20.0f);
    window.draw(scoreText);

    levelText.setString("Level: " + std::to_string(currentLevel));
    levelText.setPosition(300.0f, 20.0f);
    window.draw(levelText);

    int currentHealth = player.getHealth(); 
    for (int i = 0; i < 3; i++) {
        float heartX = 20.0f + (i * 45.0f);
        heartSprite.setPosition(heartX, 20.0f);

        if (currentHealth >= (i * 2) + 2) {
            heartSprite.setTexture(heartFulltex);
        }
        else if (currentHealth == (i * 2) + 1) {
            heartSprite.setTexture(heartHalfTex);
        }
        else{
            heartSprite.setTexture(heartEmptyTex);
        }
        window.draw(heartSprite);
    }
}

void drawGameScreens(sf::RenderWindow& window, sf::Font& gameFont, int currentLevel, int score,
                     bool isLevelCompleteScreen, bool isGameOverScreen, bool isGameFinishedScreen,
                     sf::RectangleShape& overlayPanel, sf::RectangleShape& nextLevelButton, sf::Text& buttonText,
                     sf::RectangleShape& gameOverOverlay, sf::RectangleShape& tryAgainButton, sf::Text& tryAgainText,
                     sf::RectangleShape& gameCompleteOverlay, sf::RectangleShape& playAgainButton, sf::Text& playAgainText) {
                     
    window.setView(window.getDefaultView());

    if (isLevelCompleteScreen) {
        window.draw(overlayPanel);   
        window.draw(nextLevelButton); 
        window.draw(buttonText);      
        
        sf::Text congratsText;
        congratsText.setFont(gameFont);
        congratsText.setString("CONGRATS!\nYou have completed level " + std::to_string(currentLevel) + "\nTotal Score: " + std::to_string(score));
        congratsText.setCharacterSize(30);
        congratsText.setFillColor(sf::Color::Yellow);
        congratsText.setStyle(sf::Text::Bold);
        congratsText.setPosition(250.0f, 200.0f);
        window.draw(congratsText);
    }

    if (isGameOverScreen) {
        window.draw(gameOverOverlay);   
        window.draw(tryAgainButton);    
        window.draw(tryAgainText);      
        
        sf::Text gameOverText;
        gameOverText.setFont(gameFont);
        gameOverText.setString("GAME OVER\nYour journey ended at Level " + std::to_string(currentLevel));
        gameOverText.setCharacterSize(35);
        gameOverText.setFillColor(sf::Color::White);
        gameOverText.setStyle(sf::Text::Bold);
        gameOverText.setPosition(220.0f, 200.0f);
        window.draw(gameOverText);
    }

    if(isGameFinishedScreen){
        window.draw(gameCompleteOverlay);
        window.draw(playAgainButton);
        window.draw(playAgainText);

        sf::Text winText;
        winText.setFont(gameFont);
        winText.setString("CONGRATULATIONS!\nYou Have Finished The Game!");
        winText.setCharacterSize(35);
        winText.setFillColor(sf::Color::Yellow);
        winText.setStyle(sf::Text::Bold);
        winText.setPosition(160.0f, 200.0f);
        window.draw(winText);
    }
}