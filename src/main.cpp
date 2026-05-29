#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include <vector>
#include "Platform.hpp"
#include <ctime> // rastgele sayıları zamana göre değiştirmek için
#include "Enemy.hpp"
#include <iostream>
#include "Coin.hpp"
#include <algorithm>
#include "FlyingEnemy.hpp"

using namespace std;

void spawnCoins(float newX, float newY, std::vector<Coin>& coins, sf::Texture* coinTexture) {
    float spacing = 45.0f; 
    float startX = newX + (150.0f - spacing - 24.0f) / 2.0f; 
    
    // coinTexture zaten bir pointer (adres) olduğu için başına tekrar & koymuyoruz
    coins.push_back(Coin(coinTexture, sf::Vector2f(startX, newY - 35.0f)));
    coins.push_back(Coin(coinTexture, sf::Vector2f(startX + spacing, newY - 35.0f)));
}

int main() {
    srand(19);

    // ---- PENCERE VE GÖRÜNÜM AYARLARI ----
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platform Kosucu");
    window.setFramerateLimit(60);
    sf::View view(sf::FloatRect(0, 0, 800, 600)); 

    // ---- VARLIKLARIN (ASSETS) YÜKLENMESİ ----
    sf::Texture backgroundTexture, enemyTexture, coinTexture, platformTexture, portalTexture;
    if(!backgroundTexture.loadFromFile("assets/background.png") || 
        !enemyTexture.loadFromFile("assets/enemy.png") ||
        !coinTexture.loadFromFile("assets/coin.png") || 
        !platformTexture.loadFromFile("assets/ground_dirt.png") || 
        !portalTexture.loadFromFile("assets/portal.png")){
        cout << "Gorseller yuklenemedi!" << endl;
    }

    // ---- LEVEL 2 GECE ARKA PLANI YÜKLEME ----
    sf::Texture backgroundNightTexture;
    if(!backgroundNightTexture.loadFromFile("assets/level2/background_night.png")){
        std::cout << "Gorsel yuklenemedi!" << std::endl;
    }
    backgroundNightTexture.setRepeated(true);

    // ---- LEVEL 2 FLYINGENEMY GÖRSELLERİ ---- 
    sf::Texture flyingEnemyOpenTexture;
    sf::Texture flyingEnemyClosedTexture;

    if(!flyingEnemyOpenTexture.loadFromFile("assets/level2/flying_enemy_open.png") ||
        !flyingEnemyClosedTexture.loadFromFile("assets/level2/flying_enemy_closed.png")){
            std::cout << "Gorseller yuklenemedi!" << std::endl;
        }
    
    // ---- LEVEL 3 MAĞARA VE ASİT GÖRSELLERİ YÜKLEME ----
    sf::Texture backgroundCaveTexture;
    if(!backgroundCaveTexture.loadFromFile("assets/level3/background_cave.png")){
        std::cout << "Gorseller yuklenemedi!" << std::endl;
    }   
    backgroundCaveTexture.setRepeated(true);
    
    sf::Texture acidTexture;
    if(!acidTexture.loadFromFile("assets/level3/hazard_liquid.png")){
        std::cout << "Gorseller yuklenemedi!" << std::endl;
    }
    acidTexture.setRepeated(true);

    sf::RectangleShape portalKutusu(sf::Vector2f(250.0f, 300.0f));
    portalKutusu.setTexture(&portalTexture);

    // Sadece çarpışmayı algılayacak görünmez dar hitbox (Genişlik: 40, Yükseklik: 200)
    sf::RectangleShape portalHitbox(sf::Vector2f(40.0f, 200.0f));
    portalHitbox.setFillColor(sf::Color::Transparent); // Tamamen görünmez yapıyorum

    backgroundTexture.setRepeated(true);
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    //üstteki 2 satır yerine direkt sf::Sprite backgroundSprite(backgroundTexture); yazabilirim
    // arka planı çok geniş bir dikdörtgen olarak ayarladım
    backgroundSprite.setTextureRect(sf::IntRect(0, 0, 1600, 600));

    int score = 0; // toplanan altın sayınını tutacak
    int currentLevel = 3;
    bool isPortalSpawned = false;
    
    // ------------ OYUN NESNELERİ ------------
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Platform> platforms;
    std::vector<Coin> coins;
    std::vector<FlyingEnemy> flyingEnemies;
    std::vector<sf::RectangleShape> acidHazards;
    std::vector<sf::RectangleShape> acidFills;

    // ÜST ÇİMLİ ZEMİN
    platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), &platformTexture, currentLevel));
    // ALT TOPRAK DOLGU
    platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), &platformTexture, currentLevel));
    // HAVADA ASILI SABİT PLATFORMLAR
    platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), &platformTexture, currentLevel));
    platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), &platformTexture, currentLevel));
    // LEVEL BİTİŞ ZEMİNİ (oyuncunun portala rahatça yürümesi için)
    //platforms.push_back(Platform(sf::Vector2f(1000.0f, 64.0f), sf::Vector2f(9000.0f, 550.0f), &platformTexture));


    float lastX = 700.0f; // En son eklediğim platformun yaklaşık konumu 

    // ---- SABİT PLATFORMLARIN ÜZERİNE ALTIN YERLEŞTİRME ----
    for(auto& platform : platforms){
        sf::FloatRect pBounds = platform.getBounds();

        // Eğer platform ana zemin değilse (Y koordinatı 550'den küçükse, yani havadaysa)
        if(pBounds.top < 550.0f){
            // coini platformun tam ortasına (X) ve birazcık üstüne (Y) yerleştiriyorum
            float coinX = pBounds.left + (pBounds.width / 2.0f) - 16.0f;
            float coinY = pBounds.top - 40.0f;

            coins.push_back(Coin(&coinTexture, sf::Vector2f(coinX, coinY)));
        }
    }


    // ------------------------- FONT VE YAZI AYARLARI -------------------------
    sf::Font gameFont;
    if(!gameFont.loadFromFile("assets/arial.ttf")){
        std::cout << "Font yuklenemedi! assets/arial.ttf dosyasini kontrol edin. " << std::endl;
    }

    // SKOR Yazısı Ayarları
    sf::Text scoreText;
    scoreText.setFont(gameFont);
    scoreText.setCharacterSize(22);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setStyle(sf::Text::Bold);

    // LEVEL Yazısı Ayarları
    sf::Text levelText;
    levelText.setFont(gameFont);
    levelText.setCharacterSize(22);
    levelText.setFillColor(sf::Color::Yellow);
    levelText.setStyle(sf::Text::Bold);

    // -------- LEVEL GEÇİŞ EKRANI DEĞİŞKENLERİ --------
    bool isLevelCompleteScreen = false; // geçiş ekranı açık mı kontrolü

    // Geçiş ekranının arkasındaki hafif şeffaf siyah panel
    sf::RectangleShape overplayPanel(sf::Vector2f(800.0f, 600.0f));
    overplayPanel.setFillColor(sf::Color(0, 0, 0, 130));

    // Level 2'ye geçiş butonu (kutu olarak)
    sf::RectangleShape nextLevelButton(sf::Vector2f(200.0f, 60.0f));
    nextLevelButton.setFillColor(sf::Color(50, 205, 50)); // lime yeşili renk
    nextLevelButton.setOrigin(100.0f, 30.0f); // butonun merkezini tam ortası yapıyorum
    nextLevelButton.setPosition(400.0f, 400.0f); // ekranın ortasının biraz aşağısına koydum

    // Butonun üzerindeki yazı
    sf::Text buttonText;
    buttonText.setFont(gameFont);
    buttonText.setString("Next Level");
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setStyle(sf::Text::Bold);

    // Yazıyı butonun tam merkezine ortalamak için matematiksel hesap
    sf::FloatRect btnTextBounds = buttonText.getGlobalBounds();
    buttonText.setOrigin(btnTextBounds.left + btnTextBounds.width / 2.0f, btnTextBounds.top + btnTextBounds.height / 2.0f);
    buttonText.setPosition(400.0f, 400.0f);

    // -------- GAME OVER EKRANI DEĞİŞKENLERİ --------
    bool isGameOverScreen = false; // Game over ekranı açık mı kontrolü

    // Game Over ekranının arkasındaki hafif şeffaf kırmızı/siyah panel
    sf::RectangleShape gameOverOverlay(sf::Vector2f(800.0f, 600.0f));
    gameOverOverlay.setFillColor(sf::Color(40, 0, 0, 180)); // Koyu şeffaf kırmızı bir ton

    // Try Again butonu (kutu olarak)
    sf::RectangleShape tryAgainButton(sf::Vector2f(200.0f, 60.0f));
    tryAgainButton.setFillColor(sf::Color(220, 20, 60)); // Crimson kırmızısı renk
    tryAgainButton.setOrigin(100.0f, 30.0f); // Butonun merkezini tam ortası yapıyorum
    tryAgainButton.setPosition(400.0f, 400.0f); // Ekranın ortasının biraz aşağısına koydum

    // Butonun üzerindeki yazı
    sf::Text tryAgainText;
    tryAgainText.setFont(gameFont);
    tryAgainText.setString("Try Again");
    tryAgainText.setCharacterSize(20);
    tryAgainText.setFillColor(sf::Color::White);
    tryAgainText.setStyle(sf::Text::Bold);

    // Yazıyı butonun tam merkezine ortalamak için matematiksel hesap
    sf::FloatRect tryAgainTextBounds = tryAgainText.getGlobalBounds();
    tryAgainText.setOrigin(tryAgainTextBounds.left + tryAgainTextBounds.width / 2.0f, tryAgainTextBounds.top + tryAgainTextBounds.height / 2.0f);
    tryAgainText.setPosition(400.0f, 400.0f);

    // ------ GAME COMPLETE EKRANI DEĞİŞKENLERİ ------
    bool isGameFinishedScreen = false;
    // ekranın arkasındaki hafif şeffaf yeşil/mavi panel ekliyorum kutlama hissi vermesi için
    sf::RectangleShape gameCompleteOverlay(sf::Vector2f(800.0f, 600.0f));
    gameCompleteOverlay.setFillColor(sf::Color(0, 40, 40, 200)); // koyu şeffaf turkuaz bir ton

    // Play Again butonu (kutu olarak)
    sf::RectangleShape playAgainButton(sf::Vector2f(200.0f, 60.0f));
    playAgainButton.setFillColor(sf::Color(20, 144, 255)); // canlı mavi
    playAgainButton.setOrigin(100.0f, 30.0f);
    playAgainButton.setPosition(400.0f, 400.0f);

    // Buton üzerindeki PLAY AGAIN yazısı
    sf::Text playAgainText;
    playAgainText.setFont(gameFont);
    playAgainText.setString("PLAY AGAIN");
    playAgainText.setCharacterSize(20);
    playAgainText.setFillColor(sf::Color::White);
    playAgainText.setStyle(sf::Text::Bold);

    // Yazıyı butonun tam merkezine ortalamak için matematiksel hesap
    sf::FloatRect playAgainTextBounds = playAgainText.getGlobalBounds();
    playAgainText.setOrigin(playAgainTextBounds.left + playAgainTextBounds.width / 2.0f, playAgainTextBounds.top + playAgainTextBounds.height / 2.0f);
    playAgainText.setPosition(400.0f, 400.0f);

// -----------------------------------------------------------------------
    sf::Clock clock;
    // ---- CAN ARAYÜZÜ ----
    sf::Texture heartFulltex;
    if(!heartFulltex.loadFromFile("assets/heart_full.png")){
        // hata mesajı
    }
    sf::Sprite heartSprite;
    heartSprite.setTexture(heartFulltex);
    heartSprite.setScale(2.0f, 2.0f);

    // ---------------------------------- OYUN DÖNGÜSÜ ----------------------------------   
    while (window.isOpen()) {
        // ** ETKİNLİK KONTROLÜ (EVENTS) **
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // FARE TIKLAMA KONTROLÜ (TEBRİKLER EKRANI)
            // eğer tebrikler ekranı açıksa ve sol fare tıkı yapıldıysa
            if(isLevelCompleteScreen && event.type == sf::Event::MouseButtonPressed){
                if(event.mouseButton.button == sf::Mouse::Left){
                    // farenin pencere içindeki piksel koordinatını alıyoruz
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    // eğer tıklanan yer Next Level butonunun sınırları içindeyse gerçek level geçişini yapması için
                    if(nextLevelButton.getGlobalBounds().contains(mousePosF)){
                        currentLevel++;
                        std::cout << "Butona tiklandi! Level" << currentLevel << " yukleniyor..." << std::endl;

                        player.resetPosition();
                        player.setHealth(5);

                        platforms.clear();
                        enemies.clear();
                        coins.clear();
                        flyingEnemies.clear();
                        acidHazards.clear();

                        isPortalSpawned = false;
                        isLevelCompleteScreen = false; // tebrikler ekranını kapatıp oyunu devam ettirmesi için

                        srand(19 + currentLevel * 15);
          
                        // yeni level haritası
                        platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), &platformTexture, currentLevel));
                        platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), &platformTexture, currentLevel));
                        platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), &platformTexture, currentLevel));
                        platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), &platformTexture, currentLevel));

                        for(auto& platform : platforms){
                            sf::FloatRect pBounds = platform.getBounds();
                            if(pBounds.top < 550.0f){
                                float coinX = pBounds.left + (pBounds.width / 2.0f) - 16.0f;
                                float coinY = pBounds.top - 40.0f;
                                coins.push_back(Coin(&coinTexture, sf::Vector2f(coinX, coinY)));
                            }
                        }
                        lastX = 700.0f;
                        score = 0; 
                    }   
                }
            }
            // FARE TIKLAMA KONTROLÜ (GAME OVER EKRANI)
            if(isGameOverScreen && event.type == sf::Event::MouseButtonPressed){
                if(event.mouseButton.button == sf::Mouse::Left){
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    // Eğer Try Again butonuna tıklandıysa her şeyi sıfırla
                    if(tryAgainButton.getGlobalBounds().contains(mousePosF)){
                        std::cout << "Try Again butonuna tiklandi! Oyun bastan basliyor..." << std::endl;

                        // 1. Oyuncuyu koordinat olarak başa döndür ve canını yenile
                        player.resetPosition();
                        player.setHealth(5);

                        // 2. Hafızayı temizle
                        platforms.clear();
                        enemies.clear();
                        coins.clear();
                        flyingEnemies.clear();
                        acidHazards.clear();

                        currentLevel = 1;

                        // 3. Harita motorunu (seed) ilk günkü haline getir
                        srand(19);

                        // 4. İlk bölüm zeminlerini yeniden oluştur
                        platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), &platformTexture, currentLevel));
                        platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), &platformTexture, currentLevel));
                        platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), &platformTexture, currentLevel));
                        platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), &platformTexture, currentLevel));

                        for(auto& platform : platforms){
                            sf::FloatRect pBounds = platform.getBounds();
                            if(pBounds.top < 550.0f){
                                float coinX = pBounds.left + (pBounds.width / 2.0f) - 16.0f;
                                float coinY = pBounds.top - 40.0f;
                                coins.push_back(Coin(&coinTexture, sf::Vector2f(coinX, coinY)));
                            }
                        }

                        // 5. Değişkenleri sıfırla ve ekranı kapat
                        lastX = 700.0f; 
                        score = 0; 
                        isPortalSpawned = false;
                        isGameOverScreen = false; // Ekranı kapatıp oyunu başlat
                    }
                }
            }
        }
        // FARE TIKLAMA KONTROLÜ (GAME COMPLETE EKRANI)
        if (isGameFinishedScreen && event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                // Eğer Play Again butonuna tıklandıysa her şeyi manuel sıfırla
                if (playAgainButton.getGlobalBounds().contains(mousePosF)) {
                    std::cout << "Play Again butonuna tiklandi! Oyun bastan basliyor..." << std::endl;

                    // 1. Oyuncuyu koordinat olarak başa döndür ve canını yenile
                    player.resetPosition();
                    player.setHealth(5);

                    // 2. Hafızayı temizle
                    platforms.clear();
                    enemies.clear();
                    coins.clear();
                    flyingEnemies.clear();
                    acidHazards.clear();
                    acidFills.clear();

                    currentLevel = 1;

                    backgroundSprite.setTexture(backgroundTexture);

                    // 3. Harita motorunu (seed) ilk günkü haline getir
                    srand(19);

                    // 4. İlk bölüm zeminlerini yeniden oluştur
                    platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), &platformTexture, 1));
                    platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), &platformTexture, 1));
                    platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), &platformTexture, 1));
                    platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), &platformTexture, 1));

                    for(auto& platform : platforms){
                        sf::FloatRect pBounds = platform.getBounds();
                        if(pBounds.top < 550.0f){
                            float coinX = pBounds.left + (pBounds.width / 2.0f) - 16.0f;
                            float coinY = pBounds.top - 40.0f;
                            coins.push_back(Coin(&coinTexture, sf::Vector2f(coinX, coinY)));
                        }
                    }

                    // 5. Değişkenleri sıfırla ve ekranları kapat
                    lastX = 700.0f; 
                    score = 0; 
                    isPortalSpawned = false;

                    view.setCenter(player.getPosition().x, 300);
                    window.setView(view);

                    isGameOverScreen = false; 
                    isGameFinishedScreen = false; // Oyun bitti ekranını kapatıp oyunu başlat
                }
            }
        }
        
     

        // ** GÜNCELLEME **
        float deltaTime = clock.restart().asSeconds(); // zamanı başlattım

        if(!isLevelCompleteScreen && !isGameOverScreen){

            // Oyuncunun canı bittiyse Game Over ekranını tetiklemesi için
            if(player.getHealth() <= 0){
                isGameOverScreen = true;
                std::cout << "Oyuncunun cani bitti! GAME OVER." << std::endl;
            }

        // * Nesne Güncellemeleri *
        player.update(deltaTime);
        player.checkCollision(platforms); // Artık sayı değil, liste gönderiyor
        

        // * ÇARPIŞMA Güncellemeri (Düşmanlar) *
        for(auto& enemy : enemies){
            enemy.update(deltaTime);
            //eğer oyuncu ölümsüz değilse ve düşman çarptıysa
            if(player.getBounds().intersects(enemy.getBounds())){
                player.takeDamage(); //kendi içinde zaten isInvincible kontrolü yapıyor
            }
        }
        for(auto& fEnemy : flyingEnemies){
            fEnemy.update(deltaTime); // yarasanın kendi içindeki sinüs dalgasını ve kanat çırpışını oynatması için
            // eğer oyuncu yarasaya çarparsa hasar alması için
            if(player.getBounds().intersects(fEnemy.getBounds())){
                player.takeDamage();
            }
        }

        // * ÇARPIŞMA GÜNCELLEMELERİ (Coin Toplama) *
        for(auto& coin : coins){
            // eğer altın daha önce toplanmadıysa ve oyuncu altına değdiyse
            if(!coin.isCollected() && player.getBounds().intersects(coin.getBounds())){
                coin.collect(); // altını toplandı olarak işaretleyecek
                score++;
                std::cout << "Coin Toplandı! Mevcut Skor: " << score << std::endl;
            }
        }
        
        // * ÇARPIŞMA GÜNCELLEMELERİ (Level 3 Asit Hasarı) *
        if(currentLevel >= 3){
            // asitlerin sinüs dalgasıyla sallanması
            static float acidTime = 0.0f;
            acidTime += 0.05f;

            for(auto& acid : acidHazards){
                sf::Vector2f pos = acid.getPosition();
                pos.y = 545.0f + std::sin(acidTime) * 2.0f;
                acid.setPosition(pos);
            }
            for(auto& fill : acidFills){
                sf::Vector2f pos = fill.getPosition();
                pos.y = 575.0f + std::sin(acidTime) * 2.0f;
                fill.setPosition(pos);
            }

            for(auto& acid : acidHazards){
                if(player.getBounds().intersects(acid.getGlobalBounds())){
                    player.takeDamage();
                    std::cout << "⚠️ Oyuncu aside basti! Cani azaliyor." << std::endl;
                }
            }
        }

        // * ÇARPIŞMA GÜNCELLEMELERİ (Portal / Level Geçişi) *
        // Eğer tebrikler ekranı halihazırda açık değilse, portal doğduysa ve oyuncu değdiyse ekranı tetikle
        if (!isLevelCompleteScreen && !isGameFinishedScreen && isPortalSpawned && player.getBounds().intersects(portalHitbox.getGlobalBounds())) {

            if(currentLevel == 3){
                // Eğer 3. leveldeyse oyun bitmiştir
                isGameFinishedScreen = true;
                std::cout << "Portal tetiklendi! Oyun tamamen bitti, Tebrikler ekrani aciliyor." << std::endl;
            }
            else{
                // 1. veya 2. leveldeyse normal sonraki level ekranı açılsın
                isLevelCompleteScreen = true; 
                std::cout << "Portal tetiklendi! Tebrikler ekrani aciliyor." << std::endl;
            }
            
        }
    
        // * SONSUZ PLATFORM Üretimi *
        if(lastX < 9000.0f){  // platformları hangi zemine kadar üretmeye devam edeceğini belirkemek için
            if(player.getPosition().x + 800.0f > lastX){
                float newX = lastX + (rand() % 200 + 200); 
                float newY = (rand() % 120 + 300);
             
                lastX = newX;

                // ---- LEVEL 3 ÇUKUR VE ASİT MEKANİĞİ ----
                // eğer level 3 ise %35 şans ile zemin üretmeyip çukur açması için
                if(currentLevel >= 3 && (rand() % 100 < 35)){

                    platforms.push_back(Platform(sf::Vector2f(150.0f, 60.0f), sf::Vector2f(newX, newY), &platformTexture, currentLevel));

                    // ASİT YÜZEYİ
                    sf::RectangleShape acidBlock(sf::Vector2f(150.0f, 45.0f));
                    acidBlock.setPosition(newX, 565.0f);
                    acidBlock.setTexture(&acidTexture);
                    acidBlock.setFillColor(sf::Color(50, 255, 50)); //kodla yeşile boyadım
                    acidHazards.push_back(acidBlock);

                    // ASİT DOLGUSU
                    sf::RectangleShape acidFill(sf::Vector2f(150.0f, 200.f));
                    acidFill.setPosition(newX, 575.0f);
                    acidFill.setFillColor(sf::Color(50, 255, 50));
                    acidFills.push_back(acidFill);

                }
                else{
                    platforms.push_back(Platform(sf::Vector2f(150.0f, 60.0f), sf::Vector2f(newX, newY), &platformTexture, currentLevel));
                    platforms.push_back(Platform(sf::Vector2f(400.0f, 64.0f), sf::Vector2f(newX, 550.0f), &platformTexture, currentLevel));
                    platforms.push_back(Platform(sf::Vector2f(400.0f, 400.0f), sf::Vector2f(newX, 614.0f), &platformTexture, currentLevel));

                }
                
                // Rastgele bir şans sayısı seçiyoruz (0-99 arası)
                int sans = rand() % 100;
                if (sans < 50) {
                 // %50 şansla platformun üzerine 2 adet altın koyması için
                 spawnCoins(newX, newY, coins, &coinTexture);
                }
                else if (sans >= 50 && sans < 80 && currentLevel < 3) {
                 // %30 şansla platformun üzerine bir adet devriye gezen düşman oturtmak için
                 // newY - 40.0f yapıyorum ki düşman platformun içine gömülmesin, üzerinde dursun
                 // range değerini 100.0f yapıyorum ki platform genişliği olan 150 içinde sağa sola dönebilsin
                 float groundEnemyY = 550.0f;
                 enemies.push_back(Enemy(&enemyTexture, sf::Vector2f(newX + 50.0f, groundEnemyY), 250.0f));
             }
             // Geri kalan %20 şansla da platform boş kalır, oyuncu rahatça zıplar

             if(currentLevel >= 2 && (rand() % 100) < 30){
                flyingEnemies.push_back(FlyingEnemy(&flyingEnemyOpenTexture, &flyingEnemyClosedTexture, sf::Vector2f(newX + 75.0f, newY - 120.0f), 40.0f));
             }
            }            
        }

        else{
            if(!isPortalSpawned){
                portalKutusu.setPosition(10400.0f, 298.0f);
                portalHitbox.setPosition(10400.0f + 105.0f, 350.0f);
                isPortalSpawned = true;
            }
        }
        

        // * KAMERA ve ARKA PLAN Pozisyonu *
        // karakterlerin konumunu alıp kamerayı oraya odaklıyoruz
        view.setCenter(player.getPosition().x, 300);
        window.setView(view); // pencereye. bu kamerayı kullanmasını söyledik
        
        sf::Vector2f cameraPos = view.getCenter() - (view.getSize() / 2.0f); // kameranın sol üst köşesini hesapla
        backgroundSprite.setPosition(cameraPos.x, cameraPos.y); // arka planı kamera hızından biraz daha yavaş hareket ettiriyorum
        
        int offsetX = static_cast<int>(cameraPos.x * 0.1f);
        backgroundSprite.setTextureRect(sf::IntRect(offsetX, 0, 800, 600));

        
        // * Ölüm Kontrolü (Aşağı Düşme ve Tam Reset) *
        if(player.getPosition().y > 1000.0f){
            // 1. Oyuncuyu koordinat olarak başa döndürmek için
            player.resetPosition();

            // 2. Hafızadaki tüm eski, yarım yamalak silinmiş nesneleri tamamen temizlemek için
            platforms.clear();
            enemies.clear();
            coins.clear();
            flyingEnemies.clear();
            acidHazards.clear();
            acidFills.clear();

            currentLevel = 1;

            // 3. Rastgelelik motorunu (seed) sıfırlıyorum ki platformlar yine AYNI yerlerde doğsun
            srand(19);

            // 4. Oyunun en başındaki o ana zeminleri ve sabit platformları yeniden oluştur
            platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), &platformTexture, currentLevel));
            platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), &platformTexture, currentLevel));

            platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), &platformTexture, currentLevel));
            platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), &platformTexture, currentLevel));

            //platforms.push_back(Platform(sf::Vector2f(2000.0f, 64.0f), sf::Vector2f(9000.0f, 550.0f), &platformTexture));
            
            // Sabit platformların üzerine altınları yeniden yerleştirmek için
            for(auto& platform : platforms){
                sf::FloatRect pBounds = platform.getBounds();
                if(pBounds.top < 550.0f){
                    float coinX = pBounds.left + (pBounds.width / 2.0f) - 16.0f;
                    float coinY = pBounds.top - 40.0f;
                    coins.push_back(Coin(&coinTexture, sf::Vector2f(coinX, coinY)));
                }
            }

            // 5. Üretim motorunun kaldığı yer işaretçisini de ilk günkü haline getirmek için
            lastX = 700.0f; 
            score = 0; 

            isPortalSpawned = false;
            isLevelCompleteScreen = false;
            
            std::cout << "Oyuncu öldü, harita ve nesneler ilk konumlarına başarıyla sıfırlandı!" << std::endl;
        }
    
        // ---- HAFIZA TEMİZLİĞİ ----
        float silecekSinirX = cameraPos.x - 1000.0f;
        //arkada kalan platformları silmek için
        platforms.erase(
            std::remove_if(platforms.begin(), platforms.end(), [silecekSinirX](const Platform& p){
                return (p.getBounds().left + p.getBounds().width) < silecekSinirX;
            }),
            platforms.end()
        );

        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(), [silecekSinirX](const Enemy& e) {
                return (e.getBounds().left + e.getBounds().width) < silecekSinirX;
            }),
            enemies.end()
        );

        coins.erase(
            std::remove_if(coins.begin(), coins.end(), [silecekSinirX] (const Coin& c){
                return c.isCollected() || (c.getBounds().left + c.getBounds().width) < silecekSinirX;
            }),
            coins.end()
        );

        flyingEnemies.erase(
            std::remove_if(flyingEnemies.begin(), flyingEnemies.end(), [silecekSinirX] (const FlyingEnemy fEnemy){
                return (fEnemy.getBounds().left + fEnemy.getBounds().width) < silecekSinirX;
            }),
            flyingEnemies.end()
        );

        acidHazards.erase(
            std::remove_if(acidHazards.begin(), acidHazards.end(), [silecekSinirX](const sf::RectangleShape& a){
                return (a.getPosition().x + a.getSize().x) < silecekSinirX;
            }),
            acidHazards.end()
        );

        acidFills.erase(
            std::remove_if(acidFills.begin(), acidFills.end(), [silecekSinirX](const sf::RectangleShape& f){
            return (f.getPosition().x + f.getSize().x) < silecekSinirX;
            }),
            acidFills.end()
        );
    }  

        view.setCenter(player.getPosition().x, 300);
        window.setView(view);

        sf::Vector2f cameraPos = view.getCenter() - (view.getSize() / 2.0f); 
        backgroundSprite.setPosition(cameraPos.x, cameraPos.y); 
        
        int offsetX = static_cast<int>(cameraPos.x * 0.1f);
        backgroundSprite.setTextureRect(sf::IntRect(offsetX, 0, 800, 600));
        

        // ---------------------------- ÇİZİM ----------------------------
        window.clear();     

        // ---- ARKA PLAN GÖRSEL YÖNETİMİ ----
        if(currentLevel == 2){
            backgroundSprite.setTexture(backgroundNightTexture);
            backgroundSprite.setColor(sf::Color::White);
        }
        else if(currentLevel >= 3){
            backgroundSprite.setTexture(backgroundCaveTexture);
            backgroundSprite.setColor(sf::Color::White);
        }
        else{
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setColor(sf::Color::White);
        }
        window.draw(backgroundSprite); // gökyüzünü çizdirdim

        // platformları çizdiriyorum (Döngü ile listedeki her şeyi ekrana basacak)
        for (auto& platform : platforms) {
            platform.draw(window);
        }

        // ---- LEVEL 3: ASİT BLOKLARINI EKRANA ÇİZME ----
        if (currentLevel >= 3) {
            for(const auto& fill : acidFills){
                window.draw(fill);
            }
            for (const auto& acid : acidHazards) {
                window.draw(acid);
            }
        }

        // listedeki her bir düşmanı ekranan tek tek çizdireceğim
        for(auto& enemy : enemies){
            enemy.draw(window);
        }
        for(auto& fEnemy : flyingEnemies){
            fEnemy.draw(window);
        }
        // coinleri çizdirme (sadece toplanmayanları çizdiriyorum)
        for(auto& coin : coins){
            if(!coin.isCollected()){
                coin.draw(window);
            }
        }
        
        if(isPortalSpawned){
            window.draw(portalKutusu);
        }
        player.draw(window); // karakteri en son çizdiriyorum ki her şeyin önünde görünsün

        // CAN Görseli Çizimi ve Sabir Ekran Ayarı
        window.setView(window.getDefaultView());

        // YAZILARI ÇİZDİRME
        // Skor yazısını dinamik skor değişkenine bağlayıp kalplerin sağ tarafına (280, 20) koyuyorum
        scoreText.setString("Skor: " + std::to_string(score));
        scoreText.setPosition(280.0f, 20.0f);
        window.draw(scoreText);

        // Level yazısını dinamik level değişkenine bağlayıp skorun hemen yanına (420, 20) koyuyorum
        levelText.setString("Level: " + std::to_string(currentLevel));
        levelText.setPosition(420.0f, 20.0f);
        window.draw(levelText);

        for(int i=0; i<player.getHealth(); i++){ 
            heartSprite.setPosition(20.0f + (i * 45.0f), 20.0f); 
            window.draw(heartSprite);
        }

        if (isLevelCompleteScreen) {
            window.setView(window.getDefaultView()); 

            //overplayPanel.setPosition(0.0f, 0.0f);

            window.draw(overplayPanel);   
            window.draw(nextLevelButton); 
            window.draw(buttonText);      
            
            sf::Text congratsText;
            congratsText.setFont(gameFont);
            congratsText.setString("CONGRATS!\nYou have completed level " + to_string(currentLevel) + "\nTotal Score: " + to_string(score));
            congratsText.setCharacterSize(30);
            congratsText.setFillColor(sf::Color::Yellow);
            congratsText.setStyle(sf::Text::Bold);
            congratsText.setPosition(250.0f, 200.0f);
            window.draw(congratsText);
        }

        // GAME OVER EKRANI ÇİZİMİ
        if (isGameOverScreen) {
            window.setView(window.getDefaultView()); // Arayüzü ekrana sabitlemek için

            window.draw(gameOverOverlay);   // Koyu kırmızı arka plan paneli
            window.draw(tryAgainButton);    // Yeniden dene butonu
            window.draw(tryAgainText);      // Buton yazısı
            
            sf::Text gameOverText;
            gameOverText.setFont(gameFont);
            gameOverText.setString("GAME OVER\nYour journey ended at Level " + to_string(currentLevel));
            gameOverText.setCharacterSize(35);
            gameOverText.setFillColor(sf::Color::White);
            gameOverText.setStyle(sf::Text::Bold);
            
            // Yazıyı ekrana ortalamak için
            gameOverText.setPosition(220.0f, 200.0f);
            window.draw(gameOverText);
        }

        // GAME COMPLETE EKRANI ÇİZİMİ
        if(isGameFinishedScreen){
            window.setView(window.getDefaultView());
            
            window.draw(gameCompleteOverlay);
            window.draw(playAgainButton);
            window.draw(playAgainText);

            sf::Text winText;
            winText.setFont(gameFont);
            winText.setString("CONGRATULATIONS!\nYou Have Finished The Game!");
            winText.setCharacterSize(35);
            winText.setFillColor(sf::Color::Yellow);
            winText.setStyle(sf::Text::Bold);

            // Yazıyı ekrana ortalamak için pozisyon ayarı
            winText.setPosition(160.0f, 200.0f);
            window.draw(winText);
        }

        window.display();
    }

    return 0;
}
