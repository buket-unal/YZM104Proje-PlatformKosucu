#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include <vector>
#include "Platform.hpp"
#include <ctime> // rastgele sayıları zamana göre değiştirmek için
#include "Enemy.hpp"
#include <iostream>

using namespace std;

int main() {
    srand(static_cast<unsigned int>(time(0)));

    // ---- PENCERE VE GÖRÜNÜM AYARLARI ----
    sf::RenderWindow window(sf::VideoMode(800, 600), "Platform Kosucu");
    window.setFramerateLimit(60);
    sf::View view(sf::FloatRect(0, 0, 800, 600)); 

    // ---- VARLIKLARIN (ASSETS) YÜKLENMESİ ----
    sf::Texture backgroundTexture, enemyTexture;
    if(!backgroundTexture.loadFromFile("assets/background.png") || !enemyTexture.loadFromFile("assets/enemy.png")){
        cout << "Arka plan veya dusman resmi yuklenemedi!" << endl;
    }
    
    backgroundTexture.setRepeated(true);
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    //üstteki 2 satır yerine direkt sf::Sprite backgroundSprite(backgroundTexture); yazabilirim
    // arka planı çok geniş bir dikdörtgen olarak ayarladım
    backgroundSprite.setTextureRect(sf::IntRect(0, 0, 1600, 600));

    // ---- OYUN NESNELERİ ----
    Player player;
    Enemy enemy(&enemyTexture, sf::Vector2f(600.0f, 400.0f), 200.0f);

    std::vector<Platform> platforms;

    // ÜST ÇİMLİ ZEMİN
    platforms.push_back(Platform(sf::Vector2f(9000.0f, 64.0f), sf::Vector2f(-1000.0f, 550.0f), "assets/ground_dirt.png"));
    // ALT TOPRAK DOLGU
    platforms.push_back(Platform(sf::Vector2f(9000.0f, 400.0f), sf::Vector2f(-1000.0f, 614.0f), "assets/ground_dirt.png"));
    // HAVADA ASILI SABİT PLATFORMLAR
    platforms.push_back(Platform(sf::Vector2f(200.0f, 64.0f), sf::Vector2f(400.0f, 400.0f), "assets/ground_dirt.png"));
    platforms.push_back(Platform(sf::Vector2f(200.0f, 60.0f), sf::Vector2f(700.0f, 380.0f), "assets/ground_dirt.png"));

    float lastX = 700.0f; // En son eklediğim platformun yaklaşık konumu 
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
        enemy.update(deltaTime);
        player.checkCollision(platforms); // Artık sayı değil, liste gönderiyoruz!
        
        // * ÇARPIŞMA Güncellemeri (Düşman) *
        if(player.getBounds().intersects(enemy.getBounds())){
            player.takeDamage();
        }

        // * SONSUZ PLATFORM Üretimi *
        // oyuncunun konumu + 800 (ekran genişliği), son platformun x'ini geçti mi
        
        if(player.getPosition().x + 800.0f > lastX){
             // yeni platformun koordinatlarını belirleme
             float newX = lastX + (rand() % 200 + 200); // lastX'in üzerine 200 ile 400 arasında rastgele bir mesafe ekliyorum (zıplayabilsin diye)
             float newY = (rand() % 120 + 300);
             platforms.push_back(Platform(sf::Vector2f(150.0f, 60.0f), sf::Vector2f(newX, newY), "assets/ground_dirt.png"));
             // lastX'i güncelliyorum ki bir sonraki platform bunun ilerisine kurulsun
             lastX = newX;
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
        enemy.draw(window);
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