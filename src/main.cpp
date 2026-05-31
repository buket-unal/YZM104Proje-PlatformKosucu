#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.hpp"
#include <vector>
#include "Platform.hpp"
#include <ctime> 
#include "Enemy.hpp"
#include <iostream>
#include "Coin.hpp"
#include <algorithm>
#include "FlyingEnemy.hpp"
#include "GameFunctions.hpp"

using namespace std;

int main() {
    srand(19);

    // ---- PENCERE VE GÖRÜNÜM AYARLARI ----
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platform Kosucu");
    window.setFramerateLimit(60);
    sf::View view(sf::FloatRect(0, 0, 800, 600)); 

    // ------------ OYUN İSTATİSTİKLERİ VE SEVİYE DURUMU ------------
    int score = 0;              // Toplanan altın sayısını tutacak kalıcı kasa
    int currentLevel = 1;       // Oyunun başladığı ilk seviye
    bool isPortalSpawned = false; // Bölüm sonu portalı doğdu mu kontrolü


    // ---- VARLIKLARIN TANIMLANMASI ----
    sf::Texture backgroundTexture, enemyTexture, coinTexture, platformTexture, portalTexture, enemySmallTexture;
    sf::Texture flyingEnemyOpenTexture, flyingEnemyClosedTexture, acidTexture;

    // Her seviyede kesinlikle değişmeyen ortak objeleri yüklüyorum
    if (!coinTexture.loadFromFile("assets/coin.png") || 
        !platformTexture.loadFromFile("assets/ground_dirt2.png") || 
        !portalTexture.loadFromFile("assets/portal.png")) {
        std::cout << "Ortak gorseller yuklenemedi!" << std::endl;
    }

    // ---- Sadece Level 1 Resimlerini RAM'e Yüklüyorum ----
    loadLevelAssets(currentLevel, backgroundTexture, enemyTexture, enemySmallTexture, 
                    flyingEnemyOpenTexture, flyingEnemyClosedTexture, acidTexture);

    // ------------ GEOMETRİK PORTAL VE HİTBOX AYARLARI ------------
    sf::RectangleShape portalKutusu(sf::Vector2f(250.0f, 300.0f));
    portalKutusu.setTexture(&portalTexture); // Ekranda görünecek süslü portal resmi

    // Sadece çarpışmayı algılayacak görünmez dar hitbox (Genişlik: 40, Yükseklik: 200)
    sf::RectangleShape portalHitbox(sf::Vector2f(40.0f, 200.0f));
    portalHitbox.setFillColor(sf::Color::Transparent); // Tamamen görünmez yapıyorum

    // ---- ARKA PLAN (SPRITE) AYARI ----
    // backgroundTexture.setRepeated(true) ayarını loadLevelAssets içinde de yaptığımız için burada güvenle sprite'a bağlıyorum
    sf::Sprite backgroundSprite(backgroundTexture); 
    backgroundSprite.setTextureRect(sf::IntRect(0, 0, 1600, 600));

    // ------------ OYUN NESNELERİ (VEKTÖRLER) ------------
    Player player;
    player.setHealth(6); 
    
    std::vector<Enemy> enemies;
    std::vector<Platform> platforms;
    std::vector<Coin> coins;
    std::vector<FlyingEnemy> flyingEnemies;
    std::vector<sf::RectangleShape> acidHazards;
    std::vector<sf::RectangleShape> acidFills;

    // İlk bölümün harita zeminlerini ve altınlarını harita motoruyla otomatik döşüyorum
    initializeStartingMap(platforms, coins, &platformTexture, &coinTexture, currentLevel);

    // ---- SES DOSYALARININ YÜKLENMESİ ----
    sf::SoundBuffer coinBuffer, jumpBuffer, damageBuffer;

    if(!coinBuffer.loadFromFile("assets/coinSound.ogg") ||
        !jumpBuffer.loadFromFile("assets/jumpSound.ogg") ||
        !damageBuffer.loadFromFile("assets/damageSound.ogg")){
            std::cout << "Ses dosyalari yuklenemedi! Isimleri kontrol edin." << std::endl;
    }

    sf::Sound coinSound(coinBuffer);
    sf::Sound jumpSound(jumpBuffer);
    sf::Sound damageSound(damageBuffer);

    // Ses seviyesi optimizasyonları
    coinSound.setVolume(40.0f);
    jumpSound.setVolume(50.0f);
    damageSound.setVolume(50.0f);

    // -------------------------------------------------------

    float lastX = 700.0f; // En son eklediğim platformun yaklaşık konumu 

    // ---- SABİT PLATFORMLARIN ÜZERİNE ALTIN YERLEŞTİRME ----
    for(auto& platform : platforms){
        sf::FloatRect pBounds = platform.getBounds();
        // Eğer platform ana zemin değilse (Y koordinatı 550'den küçükse, yani havadaysa)
        if(pBounds.top < 550.0f){
            // tek tek elle eklemek yerine döngüdeki spawnCoins motorunu çsğırdım
            spawnCoins(pBounds.left, pBounds.top, coins, &coinTexture);
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
    sf::RectangleShape overlayPanel(sf::Vector2f(800.0f, 600.0f));
    overlayPanel.setFillColor(sf::Color(0, 0, 0, 130));

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

    // -------- CAN ARAYÜZÜ --------
    sf::Texture heartFulltex, heartHalfTex, heartEmptyTex;
    if(!heartFulltex.loadFromFile("assets/heart_full.png") || 
        !heartHalfTex.loadFromFile("assets/heart_half.png") || 
        !heartEmptyTex.loadFromFile("assets/heart_empty.png")){
        std::cout << "Kalp gorselleri yuklenemedi!" << std::endl;
    }
    sf::Sprite heartSprite(heartFulltex);
    heartSprite.setScale(2.0f, 2.0f);

    // Oyun döngüsünden hemen önce tanımlanan RESETGAME FONKSİYONU
auto resetGame = [&]() {
    // 1. Oyuncuyu koordinat olarak başa döndürmesi ve canını yenilemesi için
    player.resetPosition();
    player.setHealth(6);

    // 2. Hafızayı tamamen temizlesi için
    platforms.clear();
    enemies.clear();
    coins.clear();
    flyingEnemies.clear();
    acidHazards.clear();
    acidFills.clear();

    // 3. Bölüm ve skor değişkenlerini sıfırlaması için
    currentLevel = 1;
    score = 0;
    lastX = 700.0f;

    // 4. Ekran durumlarını kapatması için
    isPortalSpawned = false;
    isGameOverScreen = false;
    isGameFinishedScreen = false;
    isLevelCompleteScreen = false;

    // 5. Harita motorunu ilk günkü haline getirmesi için
    srand(19);

    loadLevelAssets(currentLevel, backgroundTexture, enemyTexture, enemySmallTexture, 
                        flyingEnemyOpenTexture, flyingEnemyClosedTexture, acidTexture);
        backgroundSprite.setTexture(backgroundTexture);

    // 6. İlk bölüm zeminlerini ve altınlarını yeniden oluşturması için
    initializeStartingMap(platforms, coins, &platformTexture, &coinTexture, currentLevel);
    std::cout << "Oyun basariyla tek bir merkezden sifirlandi!" << std::endl;
};

    // ---------------------------------- OYUN DÖNGÜSÜ ----------------------------------   
    while (window.isOpen()) {
        // ** ETKİNLİK KONTROLÜ (EVENTS) **
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // FARE TIKLAMA KONTROLÜ (TEBRİKLER EKRANI) (NEXT LEVEL)
            // eğer tebrikler ekranı açıksa ve sol fare tıkı yapıldıysa
            if(isLevelCompleteScreen && event.type == sf::Event::MouseButtonPressed){
                if(event.mouseButton.button == sf::Mouse::Left){
                    // farenin pencere içindeki piksel koordinatını alıyoruz
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                    // eğer tıklanan yer Next Level butonunun sınırları içindeyse gerçek level geçişini yapması için
                    if(nextLevelButton.getGlobalBounds().contains(mousePosF)){
                        currentLevel++;

                        loadLevelAssets(currentLevel, backgroundTexture, enemyTexture, enemySmallTexture, 
                                        flyingEnemyOpenTexture, flyingEnemyClosedTexture, acidTexture);
                               
                        if(currentLevel == 2) backgroundSprite.setTexture(backgroundTexture); // night texture
                        else if(currentLevel == 3) backgroundSprite.setTexture(backgroundTexture); // cave texture

                        std::cout << "Butona tiklandi! Level" << currentLevel << " yukleniyor..." << std::endl;

                        player.resetPosition();
                        player.setHealth(6);

                        platforms.clear();
                        enemies.clear();
                        coins.clear();
                        flyingEnemies.clear();
                        acidHazards.clear();

                        isPortalSpawned = false;
                        isLevelCompleteScreen = false; // tebrikler ekranını kapatıp oyunu devam ettirmesi için

                        srand(19 + currentLevel * 15);
          
                        // yeni level haritası
                        initializeStartingMap(platforms, coins, &platformTexture, &coinTexture, currentLevel);
                        
                        lastX = 700.0f;
                        score = 0; 
                    }   
                }
            }
            // FARE TIKLAMA KONTROLÜ (GAME OVER EKRANI)
            if(isGameOverScreen && event.type == sf::Event::MouseButtonPressed){
                if(event.mouseButton.button == sf::Mouse::Left){
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    //sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                    sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);

                    // Eğer Try Again butonuna tıklandıysa her şeyi sıfırla
                    if(tryAgainButton.getGlobalBounds().contains(mousePosF)){
                        std::cout << "Try Again butonuna tiklandi! Oyun bastan basliyor..." << std::endl;

                        resetGame();
                    }
                }
            }
        
        // FARE TIKLAMA KONTROLÜ (GAME COMPLETE EKRANI)
        if (isGameFinishedScreen && event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                //sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                sf::Vector2f mousePosF = window.mapPixelToCoords(mousePos);
                
                // Eğer Play Again butonuna tıklandıysa her şeyi manuel sıfırla
                if (playAgainButton.getGlobalBounds().contains(mousePosF)) {
                    std::cout << "Play Again butonuna tiklandi! Oyun bastan basliyor..." << std::endl;

                    resetGame();

                    backgroundSprite.setTexture(backgroundTexture);
                    view.setCenter(player.getPosition().x, 300);
                    window.setView(view);
                }
            }
        }
    }   
     
        // ------- GÜNCELLEME -------
        float deltaTime = clock.restart().asSeconds(); // zamanı başlattım

        if(!isLevelCompleteScreen && !isGameOverScreen && !isGameFinishedScreen){

            // Oyuncunun canı bittiyse Game Over ekranını tetiklemesi için
            if(player.getHealth() <= 0){
                isGameOverScreen = true;
                std::cout << "Oyuncunun cani bitti! GAME OVER." << std::endl;
            }

        // * Nesne Güncellemeleri *
        player.update(deltaTime, jumpSound);
        player.checkCollision(platforms); // Artık sayı değil, liste gönderiyor
        

        // SAVAŞMA VE EZME MOTORU:
        for (auto it = enemies.begin(); it != enemies.end(); ) {
            // eğer düşman zaten küçülmüşse ve ekranda kalma süresi dolmuşsa listeden çıkarması için
            if(it->isDead()){
                it = enemies.erase(it);
                continue;
            }
            it->update(deltaTime);

            if (!it->getIsSmall() && player.getBounds().intersects(it->getBounds())) {
                // Oyuncunun düşmanın kafasına basıp basmadığını kontrol ediyorum:
                // 1. Oyuncu aşağı doğru düşüyor olmalı (hızı pozitif olmalı, SFML'de aşağısı pozitif)
                // 2. Oyuncunun ayakları, düşmanın merkezinden/kafasından yukarıda olmalı
                if (player.getVelocityY() > 0.0f && (player.getBounds().top + player.getBounds().height) < (it->getBounds().top + 20.0f)) {
                    
                    std::cout << "Düsmanin kafasina basildi! Düsman yok edildi." << std::endl;   
                    // Oyuncuyu Mario gibi yukarı doğru hafifçe zıplatıyorum (yaylanma efekti)
                    player.bounce();
                    it->makeSmall();
                    score += 2; // Düşmanı yenince oyuncuya ödül olarak +2 skor veriyorum
                    coinSound.play();
                    continue; 
                } 
                else {
                    // Eğer kafasına basmadıysa, yanından çarptıysa oyuncu hasar alır
                    player.takeDamage(it->getBounds().left); 
                    damageSound.play();
                }
            }
            it++; // Eğer çarpışma yoksa sıradaki düşmana geçmesi için
        }   

        for(auto& fEnemy : flyingEnemies){
            fEnemy.update(deltaTime); // yarasanın kendi içindeki sinüs dalgasını ve kanat çırpışını oynatması için
            // eğer oyuncu yarasaya çarparsa hasar alması için
            if(player.getBounds().intersects(fEnemy.getBounds())){
                player.takeDamage(fEnemy.getBounds().left);
                damageSound.play();
            }
        }

        // * ÇARPIŞMA GÜNCELLEMELERİ (Coin Toplama) *
        for(auto& coin : coins){
            // eğer altın daha önce toplanmadıysa ve oyuncu altına değdiyse
            if(!coin.isCollected() && player.getBounds().intersects(coin.getBounds())){
                coin.collect(); // altını toplandı olarak işaretleyecek
                score++;
                coinSound.play();
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
                    player.takeDamage(player.getPosition().x);
                    damageSound.play();
                    std::cout << "Oyuncu aside basti! Cani azaliyor." << std::endl;
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
                
                int sans = rand() % 100;
                if (sans < 50) {
                 // %50 şansla platformun üzerine 2 adet altın koyması için
                 spawnCoins(newX, newY, coins, &coinTexture);
                }
                else if (sans >= 50 && sans < 80 && currentLevel < 3) {
                 float groundEnemyY = 550.0f;
                 enemies.push_back(Enemy(&enemyTexture, &enemySmallTexture, &gameFont, sf::Vector2f(newX + 50.0f, groundEnemyY), 250.0f));
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
        
        int offsetX = static_cast<int>(cameraPos.x * 0.1f); // derinlik etkisi için
        backgroundSprite.setTextureRect(sf::IntRect(offsetX, 0, 800, 600));

        
        // * Ölüm Kontrolü (Aşağı Düşme ve Tam Reset) *
        if(player.getPosition().y > 1000.0f){   
           resetGame();
           std::cout << "Oyuncu öldü, harita ve nesneler ilk konumlarına başarıyla sıfırlandı!" << std::endl;
        }
    
        // ---- HAFIZA TEMİZLİĞİ ----
        float silecekSinirX = cameraPos.x - 1000.0f;

        temizleEkranDisiNesneler(platforms, silecekSinirX);
        temizleEkranDisiNesneler(enemies, silecekSinirX);
        temizleAltinlar(coins, silecekSinirX); // Altınların kendi özel fonksiyonu
        temizleEkranDisiNesneler(flyingEnemies, silecekSinirX);
        
        // asit kutuları direkt sf::RectangleShape olduğu için getBounds() yerine getGlobalBounds()  

        acidHazards.erase(std::remove_if(acidHazards.begin(), acidHazards.end(), [silecekSinirX](const sf::RectangleShape& a){ return (a.getPosition().x + a.getSize().x) < silecekSinirX; }), acidHazards.end());
        acidFills.erase(std::remove_if(acidFills.begin(), acidFills.end(), [silecekSinirX](const sf::RectangleShape& f){ return (f.getPosition().x + f.getSize().x) < silecekSinirX; }), acidFills.end());

    }  
        
       // silindiğinde arayüz ekranlarına zarar veriyor
        view.setCenter(player.getPosition().x, 300);
        window.setView(view);

        sf::Vector2f cameraPos = view.getCenter() - (view.getSize() / 2.0f); 
        backgroundSprite.setPosition(cameraPos.x, cameraPos.y); 
        
        int offsetX = static_cast<int>(cameraPos.x * 0.1f);
        backgroundSprite.setTextureRect(sf::IntRect(offsetX, 0, 800, 600));
       

// ---------------------------- ÇİZİM ----------------------------
        window.clear();     

        // Arka plan ve Dünya Nesnelerini Çizdir (Burası kamerayı takip eden dünya)
        window.draw(backgroundSprite); 
        for (auto& platform : platforms) platform.draw(window);

        if (currentLevel >= 3) {
            for(const auto& fill : acidFills) window.draw(fill);
            for (const auto& acid : acidHazards) window.draw(acid);
        }

        for(auto& enemy : enemies) enemy.draw(window);
        for(auto& fEnemy : flyingEnemies) fEnemy.draw(window);
        for(auto& coin : coins) { if(!coin.isCollected()) coin.draw(window); }
        if(isPortalSpawned) window.draw(portalKutusu);
        player.draw(window); 

        // Sabit HUD elemanlarını (Can, Skor, Seviye) çizdirme
        drawHUD(window, player, score, currentLevel, scoreText, levelText, heartSprite, heartFulltex, heartHalfTex, heartEmptyTex);

        // Durum ekranlarını (Giriş/Çıkış panellerini) çizdirme
        drawGameScreens(window, gameFont, currentLevel, score,
                        isLevelCompleteScreen, isGameOverScreen, isGameFinishedScreen,
                        overlayPanel, nextLevelButton, buttonText,
                        gameOverOverlay, tryAgainButton, tryAgainText,
                        gameCompleteOverlay, playAgainButton, playAgainText);

        window.display();
    }

    return 0;
}