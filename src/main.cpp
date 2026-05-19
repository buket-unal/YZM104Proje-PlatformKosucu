#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include <vector>
#include "Platform.hpp"
#include <ctime> // rastgele sayıları zamana göre değiştirmek için
#include "Enemy.hpp"
#include <iostream>
#include "Coin.hpp"

using namespace std;

void spawnCoins(float newX, float newY, std::vector<Coin>& coins, sf::Texture* coinTexture) {
    float spacing = 45.0f; 
    float startX = newX + (150.0f - spacing - 24.0f) / 2.0f; 
    
    // coinTexture zaten bir pointer (adres) olduğu için başına tekrar & koymuyoruz
    coins.push_back(Coin(coinTexture, sf::Vector2f(startX, newY - 35.0f)));
    coins.push_back(Coin(coinTexture, sf::Vector2f(startX + spacing, newY - 35.0f)));
}

int main() {
    //srand(static_cast<unsigned int>(time(0)));
    srand(19);

    // ---- PENCERE VE GÖRÜNÜM AYARLARI ----
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platform Kosucu");
    window.setFramerateLimit(60);
    sf::View view(sf::FloatRect(0, 0, 800, 600)); 

    // ---- VARLIKLARIN (ASSETS) YÜKLENMESİ ----
    sf::Texture backgroundTexture, enemyTexture, coinTexture;
    if(!backgroundTexture.loadFromFile("assets/background.png") || 
       !enemyTexture.loadFromFile("assets/enemy.png") ||
        !coinTexture.loadFromFile("assets/coin.png")){
        cout << "Gorseller yuklenemedi!" << endl;
    }
    
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
    platforms.push_back(Platform(sf::Vector2f(9000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), "assets/ground_dirt.png"));
    // ALT TOPRAK DOLGU
    platforms.push_back(Platform(sf::Vector2f(9000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), "assets/ground_dirt.png"));
    // HAVADA ASILI SABİT PLATFORMLAR
    platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), "assets/ground_dirt.png"));
    platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), "assets/ground_dirt.png"));

    float lastX = 700.0f; // En son eklediğim platformun yaklaşık konumu 
    
    // DÜŞMAN 1
    for(int i=0; i<=10; i++){
        // düşmanı her 1200 pikselde bir koymayı deniyorum
        float testX = 800.00f + (i * 1200.0f);
        float testY = 490.0f; // ana zemindeki durma yüksekliği

        bool platformVarMi = false;

        // mevcut platform listesini taramak için
        for(auto& platform : platforms){
            sf::FloatRect pBounds = platform.getBounds();
            // eğer havada asılı duran bir platformun X aralığı, benim düşman koymak istediğim X ile çakışıyorsa
        // ve bu platform ana zemin değilse (Y koordinatı 550'den küçükse, yani havadaysa)
            if (testX >= pBounds.left && testX <= (pBounds.left + pBounds.width) && pBounds.top < 550.0f){
                platformVarMi = true;
                break;
            }
        }
        if(!platformVarMi){
            enemies.push_back(Enemy(&enemyTexture, sf::Vector2f(testX, testY), 150.0f));
        }
    }

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
        }

        // ** GÜNCELLEME **
        float deltaTime = clock.restart().asSeconds(); // zamanı başlattım

        // * Nesne Güncellemeleri *
        player.update(deltaTime);
        player.checkCollision(platforms); // Artık sayı değil, liste gönderiyoruz!
        

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
    

            // * SONSUZ PLATFORM Üretimi *
        // Oyuncu son platforma yaklaştıysa VE henüz yeni platform eklenmediyse
        if(player.getPosition().x + 800.0f > lastX){
             float newX = lastX + (rand() % 200 + 200); 
             float newY = (rand() % 120 + 300);
             
             // lastX'i hemen güncelliyorum ki döngü bir sonraki milisaniyede buraya tekrar sızamasın!
             lastX = newX;

             platforms.push_back(Platform(sf::Vector2f(150.0f, 60.0f), sf::Vector2f(newX, newY), "assets/ground_dirt.png"));
             
             // %50 şansla tam bu yeni platformun üzerine 2 adet coin koymak için
             if (rand() % 100 < 50) {
                 spawnCoins(newX, newY, coins, &coinTexture);
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

        // * Ölüm Kontrolü (Aşağı Düşme)
        // eğer karakter çok aşağı düştüyse onu resetleyecek, başa gönderecek
        if(player.getPosition().y > 1000.0f){
            player.resetPosition();
        }

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

        player.draw(window); // karakteri en son çizdiriyorum ki her şeyin önünde görünsün

        // CAN Görseli Çizimi
        window.setView(window.getDefaultView()); //kamerayı ekrana sabitledim

        for(int i=0; i<player.getHealth(); i++){ // can sayısı kadar kalbi yan yana dizmesi için
            heartSprite.setPosition(20.0f + (i * 45.0f), 20.0f); // 45 pixel aralıkla dizmesi için
            window.draw(heartSprite);
        }

        window.display();
    }

    return 0;
}