#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include <vector>
#include "Platform.hpp"
#include <ctime> // rastgele sayıları zamana göre değiştirmek için
#include "Enemy.hpp"
#include <iostream>
#include "Coin.hpp"
#include <algorithm>

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

    // ---- OYUN NESNELERİ ----
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Platform> platforms;
    std::vector<Coin> coins;

    // ÜST ÇİMLİ ZEMİN
    platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), &platformTexture));
    // ALT TOPRAK DOLGU
    platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), &platformTexture));
    // HAVADA ASILI SABİT PLATFORMLAR
    platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), &platformTexture));
    platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), &platformTexture));
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

    int score = 0; // toplanan altın sayınını tutacak
    int currentLevel = 1;
    bool isPortalSpawned = false;

    // -------- FONT VE YAZI AYARLARI --------
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

    sf::Clock clock;

    // ---- CAN ARAYÜZÜ ----
    sf::Texture heartFulltex;
    if(!heartFulltex.loadFromFile("assets/heart_full.png")){
        // hata mesajı
    }
    sf::Sprite heartSprite;
    heartSprite.setTexture(heartFulltex);
    heartSprite.setScale(2.0f, 2.0f);

    // ---- OYUN DÖNGÜSÜ ----
    
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

                        isPortalSpawned = false;
                        isLevelCompleteScreen = false; // tebrikler ekranını kapatıp oyunu devam ettirmesi için

                        srand(19 + currentLevel * 15);

                        // yeni level haritası
                        platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), &platformTexture));
                        platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), &platformTexture));
                        platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), &platformTexture));
                        platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), &platformTexture));

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
        }
     

        // ** GÜNCELLEME **
        float deltaTime = clock.restart().asSeconds(); // zamanı başlattım

        if(!isLevelCompleteScreen){

        // * Nesne Güncellemeleri *
        player.update(deltaTime);
        player.checkCollision(platforms); // Artık sayı değil, liste gönderiyor
        

        // * ÇARPIŞMA Güncellemeri (Düşman) *
        for(auto& enemy : enemies){
            enemy.update(deltaTime);
            //eğer oyuncu ölümsüz değilse ve düşman çarptıysa
            if(player.getBounds().intersects(enemy.getBounds())){
                player.takeDamage(); //kendi içinde zaten isInvincible kontrolü yapıyor
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

        // * ÇARPIŞMA GÜNCELLEMELERİ (Portal / Level Geçişi) *
        // Eğer tebrikler ekranı halihazırda açık değilse, portal doğduysa ve oyuncu değdiyse ekranı tetikle
        if (!isLevelCompleteScreen && isPortalSpawned && player.getBounds().intersects(portalHitbox.getGlobalBounds())) {
            isLevelCompleteScreen = true; // Oyunu durdurma sinyalini yak ve ekranı aç!
            std::cout << "Portal tetiklendi! Tebrikler ekrani aciliyor." << std::endl;
        }
    
        // * SONSUZ PLATFORM Üretimi *
        if(lastX < 9000.0f){  // platformları hangi zemine kadar üretmeye devam edeceğini belirkemek için
            if(player.getPosition().x + 800.0f > lastX){
                float newX = lastX + (rand() % 200 + 200); 
                float newY = (rand() % 120 + 300);
             
                lastX = newX;

                platforms.push_back(Platform(sf::Vector2f(150.0f, 60.0f), sf::Vector2f(newX, newY), &platformTexture));
                platforms.push_back(Platform(sf::Vector2f(400.0f, 64.0f), sf::Vector2f(newX, 550.0f), &platformTexture));
                platforms.push_back(Platform(sf::Vector2f(400.0f, 400.0f), sf::Vector2f(newX, 614.0f), &platformTexture));

                // Rastgele bir şans sayısı seçiyoruz (0-99 arası)
                int sans = rand() % 100;
                if (sans < 50) {
                 // %40 şansla platformun üzerine 2 adet altın koyması için
                 spawnCoins(newX, newY, coins, &coinTexture);
                }
                else if (sans >= 50 && sans < 80) {
                 // %30 şansla platformun üzerine bir adet devriye gezen düşman oturtmak için
                 // newY - 40.0f yapıyorum ki düşman platformun içine gömülmesin, üzerinde dursun
                 // range değerini 100.0f yapıyorum ki platform genişliği olan 150 içinde sağa sola dönebilsin
                 float groundEnemyY = 550.0f;
                 enemies.push_back(Enemy(&enemyTexture, sf::Vector2f(newX + 50.0f, groundEnemyY), 250.0f));
             }
             // Geri kalan %20 şansla da platform boş kalır, oyuncu rahatça zıplar
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

            // 3. Rastgelelik motorunu (seed) sıfırlıyorum ki platformlar yine AYNI yerlerde doğsun
            srand(19);

            // 4. Oyunun en başındaki o ana zeminleri ve sabit platformları yeniden oluştur
            platforms.push_back(Platform(sf::Vector2f(12000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), &platformTexture));
            platforms.push_back(Platform(sf::Vector2f(12000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), &platformTexture));

            platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), &platformTexture));
            platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), &platformTexture));

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

            currentLevel = 1;
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
        }   

        view.setCenter(player.getPosition().x, 300);
        window.setView(view);

        sf::Vector2f cameraPos = view.getCenter() - (view.getSize() / 2.0f); 
        backgroundSprite.setPosition(cameraPos.x, cameraPos.y); 
        
        int offsetX = static_cast<int>(cameraPos.x * 0.1f);
        backgroundSprite.setTextureRect(sf::IntRect(offsetX, 0, 800, 600));
        

        // ---- ÇİZİM ----
        window.clear();     
        
        window.draw(backgroundSprite); // gökyüzünü çizdirdik
        // platformları çizdiriyorum (Döngü ile listedeki her şeyi ekrana basacak)
        for (auto& platform : platforms) {
            platform.draw(window);
        }
        // listedeki her bir düşmanı ekranan tek tek çizdireceğim
        for(auto& enemy : enemies){
            enemy.draw(window);
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

        window.display();
    }

    return 0;
}
