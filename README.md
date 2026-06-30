# Simulasi Kader Posyandu & Pemantauan Gizi Anak (C++)

Aplikasi berbasis *console* (CLI) interaktif yang dirancang sebagai simulasi sistem informasi Posyandu. Aplikasi ini mempermudah pencatatan data balita oleh Kader dan memberikan akses bagi Ibu Balita untuk memantau status gizi anak secara berkala.

Sistem ini mengintegrasikan perhitungan antropometri gizi berdasarkan **[Standar Antropometri Anak Kemenkes RI / WHO 2020](https://fs.stunting.go.id/index.php/s/bfZAQbcqDZo4ySo?_gl=1*1p290nc*_ga*OTgwMzIyNjY3LjE3NzkxMDczMzM.*_ga_TV21Y9HW17*czE3NzkxMTIwNzgkbzIkZzAkdDE3NzkxMTIwNzgkajYwJGwwJGgw#pdfviewer)**.

---

## 🚀 Fitur Utama

* **Sistem Akun & Multi-Role**: Mendukung login dengan 2 peran utama:
  * **Kader Posyandu**: Dapat mendaftarkan Ibu Balita, mencatat data balita, dan melakukan pengukuran (input berat badan, tinggi badan, dll).
  * **Ibu Balita**: Dapat login untuk memantau rekam jejak pertumbuhan dan hasil klasifikasi gizi (Z-Score) anak-anaknya secara mandiri.
* **Manajemen Data Relasional**: Setiap data balita terikat langsung dengan akun Ibu yang terdaftar, memastikan integritas data.
* **Perhitungan Gizi Otomatis (Z-Score)**:
  1. **BB/U (Berat Badan menurut Umur)**: Mendeteksi indikasi *Underweight* (Gizi Kurang/Buruk).
  2. **PB/U atau TB/U (Panjang/Tinggi Badan menurut Umur)**: Mendeteksi indikasi *Stunting* (Pendek/Kerdil).
  3. **BB/PB atau BB/TB (Berat Badan menurut Panjang/Tinggi)**: Mendeteksi status *Wasting* (Kurus) hingga Obesitas.
* **Validasi Input Aman**: Mencegah input nilai negatif atau tidak logis saat pendataan balita maupun pengukuran.

---

## 🛠️ Persyaratan & Cara Menjalankan

Aplikasi ini dibuat menggunakan bahasa pemrograman C++. Anda memerlukan compiler C++ (seperti `g++`) untuk mengompilasi kode program ini.

### Kompilasi (Compile) & Instalasi
Jalankan script instalasi berikut di terminal Anda untuk mengompilasi program:
```bash
./install.sh
```

### Menjalankan Aplikasi (Run)
Setelah berhasil dikompilasi, jalankan program:
```bash
./antropometri_gizi_z_score
```

---

## 📊 Kriteria Klasifikasi (Standar Kemenkes RI 2020)

Hasil klasifikasi gizi yang dikeluarkan sistem meliputi:
* **BB/U**: Sangat Kurang, Kurang, Normal, Risiko Berat Badan Lebih.
* **PB/U atau TB/U**: Sangat Pendek, Pendek, Normal, Tinggi.
* **BB/PB atau BB/TB**: Gizi Buruk, Gizi Kurang, Gizi Baik (Normal), Berisiko Gizi Lebih, Gizi Lebih, Obesitas.
