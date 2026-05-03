  Platform Koşucu (Side-Scroller) 🏃‍♀️💨

Bu proje, Kocaeli Üniversitesi Yazılım Mühendisliği - Programlama II dersi kapsamında geliştirdiğim, C++ ve SFML kütüphanesi tabanlı bir yan kaydırmalı platform oyunudur.

  Proje Hedefi
Oyuncu karakterinin akıcı bir şekilde hareket edebildiği, fizik kurallarına (yer çekimi, zıplama) dayalı ve tile (döşeme) tabanlı bir dünyada geçen dinamik bir oyun deneyimi sunmak.

  Teknik Özellikler (Mevcut Durum: 1. Hafta Tamamlandı)
Dil: C++
Kütüphane: SFML (Simple and Fast Multimedia Library)
Fizik: Karakter için zıplama ve yer çekimi fiziği uygulandı.
Kontroller: Klavye girdileri ile sağa/sola hareket ve dinamik zıplama mekanizması entegre edildi.
Zemin Algılama:** Temel çarpışma kontrolleri ile karakterin zemin üzerinde durması sağlandı.

   Planlanan Özellikler (Gelecek Haftalar)
 Tile Haritası: Döşemelerin birleşmesiyle oluşan geniş ve detaylı bölümler.
 Kayan Kamera: Karakteri takip eden smooth-camera sistemi.
 Düşman Yapay Zekası: Belirli hat üzerinde devriye gezen (patrol) düşman mekanikleri.
 Can ve Puan Sistemi: Coin toplama ve sağlık yönetimi arayüzü.

   Kurulum ve Çalıştırma
Projeyi kendi yerel ortamınızda denemek için:

1. SFML kütüphanesinin kurulu olduğundan emin olun.
2. Projeyi klonlayın ve klasöre gidin.
3. Derleme komutu:
   ```bash
   g++ main.cpp -o PlatformKosucu -lsfml-graphics -lsfml-window -lsfml-system
