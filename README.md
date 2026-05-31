# Platform Koşucu – 2D Side-Scroller Platform Oyunu

![C++17](https://img.shields.io/badge/language-C++17-blue) ![SFML](https://img.shields.io/badge/library-SFML-green) ![Status](https://img.shields.io/badge/status-active-success)

---

## 1. Proje Hakkında

Platform Koşucu, C++ programlama dili ve SFML kütüphanesi kullanılarak geliştirilmiş, dinamik (prosedürel) harita üretim motoruna sahip tek oyunculu bir yan kaydırmalı (side-scroller) platform oyunudur. YZM104 Programlama II dersi dönem projesi kapsamında hazırlanan bu oyunda oyuncu; ileri doğru hareket ettikçe canlı olarak üretilen zeminler üzerinde hayatta kalmaya, altınları toplamaya, devriye atan kara ve hava düşmanlarını atlatarak bölüm sonundaki finale ulaşmaya çalışır.

---

## 2. Özellikler

**Prosedürel Harita ve Dinamik Akış**
* **Dinamik Üretim:** Sabit bir harita dosyası yüklemek yerine, oyuncu ilerledikçe `srand()` mimarisine dayalı rastgele platformlar, altınlar ve düşmanlar üreten yordamsal bir motor tasarlanmıştır.
* **Bölüm Tasarımları (Levels):** Artan zorluk seviyelerine sahip 3 benzersiz seviye bulunur. Level 2'de uçan yarasalar eklenir; Level 3'te ise dinamik üretim 10400. pikseldeki final sınırında durur ve oyunu bitiren final portalı doğar.

**Gelişmiş Fizik ve Çarpışma (AABB)**
* **Hassas Çarpışma:** Karakter için gerçekçi yer çekimi, zıplama ve yaylanma (bounce) fiziği uygulanmıştır. Döşeme tabanlı (Tile-based) platformlar ile dikey/yatay çarpışmalar AABB algoritmasıyla milimetrik çözülür.
* **Knockback & Ölümsüzlük:** Oyuncu hasar aldığında kontroller kısa süreli kilitlenir, geriye doğru fırlatılır ve 1.5 saniye boyunca kırmızı renkte yanıp seneden hasar almazlık (`Invincibility frame`) kazanır.

**Bellek ve Performans Optimizasyonu**
* **Hafıza Silecek Motoru:** Kamera sınırının (ekranın gerisinde) arkasında kalan tüm platform, düşman ve altınlar `std::remove_if` algoritması kullanan şablonlu (Template) bir fonksiyonla RAM'den anlık temizlenir.
* **Pointer Yönetimi:** Çizim döngüsündeki disk okuma maliyetleri (`loadFromFile`) RAM tabanlı pointer yönetimine taşınarak FPS drop sorunları tamamen sıfırlanmıştır.

---

## 3. Teknoloji Yığını

| Kategori | Teknoloji | Açıklama |
| :--- | :--- | :--- |
| **Programlama Dili** | C++ (Standart C++17) | Ana oyun mantığı, fizik hesaplamaları ve şablonlu bellek yönetimi |
| **Grafik & Ses Kütüphanesi** | SFML (v2.6.1) | Pencere oluşturma, görsel çizimi, fontlar ve ses motoru yönetimi |
| **Derleme Sistemi** | CMake (Min. v3.14) | Platformlar arası otomatik bağımlılık yönetimi (`FetchContent`) ve otomatik DLL kopyalama |
| **Geliştirme Ortamı** | VS Code / CLion | Tercih edilen IDE/Editör |

---

## 4. Kontroller

Oyun mantığı klavyeye, arayüz etkileşimi (ekran geçiş butonları) ise fareye dayalı olarak tasarlanmıştır:

| Eylem | Tuş / Kontrol |
| :--- | :--- |
| **Karakteri Hareket Ettirme** | `A` - `D` veya Yön Tuşları (⬅️ ➡️) |
| **Zıplama (Zemindeyken)** | `W` veya Yukarı Yön Tuşu (⬆️) |
| **Arayüz Etkileşimi (Next Level / Try Again)** | Farenin Sol Tuşu |
| **Oyundan Çıkış** | Pencereyi Kapatma İkonu (X) |

---

## 5. Proje Nasıl Çalıştırılır?

**Ön Gereksinimler**
* Sisteminizde **CMake** (v3.14 veya üzeri) yüklü olmalıdır.
* C++17 destekleyen bir derleyici (GCC, Clang, MSVC vb.) gereklidir.
* *Not: SFML kütüphanesini bilgisayarınıza önceden kurmanıza gerek yoktur. Proje bağımlılıkları (SFML ve ses kütüphaneleri), CMake ilk kez derlenirken internetten otomatik olarak indirilecek ve projeye dahil edilecektir.*

**Kurulum ve Derleme Adımları**

1. **Depoyu Yerelinize Kopyalayın:**
   ```bash
   git clone [https://github.com/buket-unal/YZM104Proje-PlatformKosucu.git](https://github.com/buket-unal/YZM104Proje-PlatformKosucu.git)
   cd YZM104Proje-PlatformKosucu
