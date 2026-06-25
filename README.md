# Kalkulator Antropometri Z-Score Anak (C++)

Aplikasi berbasis *console* (CLI) interaktif yang dirancang khusus untuk mempermudah kader Posyandu, bidan, maupun tenaga kesehatan dalam mengukur dan mengklasifikasikan status gizi balita secara cepat dan akurat.

Sistem ini melakukan perhitungan berdasarkan **[Standar Antropometri Anak Kemenkes RI / WHO 2020](https://fs.stunting.go.id/index.php/s/bfZAQbcqDZo4ySo?_gl=1*1p290nc*_ga*OTgwMzIyNjY3LjE3NzkxMDczMzM.*_ga_TV21Y9HW17*czE3NzkxMTIwNzgkbzIkZzAkdDE3NzkxMTIwNzgkajYwJGwwJGgw#pdfviewer)**.


---

## 🚀 Fitur Utama

* **Input Sekali Jalan (*One-Time Input*)**: Cukup masukkan Nama, Jenis Kelamin, Umur, Berat Badan, dan Panjang/Tinggi Badan sekali saja.
* **Deteksi 3 Indikator Sekaligus**:
  1. **BB/U (Berat Badan menurut Umur)**: Mendeteksi indikasi *Underweight* (Gizi Kurang/Buruk).
  2. **PB/U atau TB/U (Panjang/Tinggi Badan menurut Umur)**: Mendeteksi indikasi *Stunting* (Pendek/Kerdil).
  3. **BB/PB atau BB/TB (Berat Badan menurut Panjang/Tinggi)**: Mendeteksi status *Wasting* (Kurus) hingga Obesitas.
* **Penyesuaian Otomatis Berdasarkan Usia**: 
  * Balita usia **0-24 bulan** diukur menggunakan Panjang Badan (posisi terlentang).
  * Balita usia **25-60 bulan** diukur menggunakan Tinggi Badan (posisi berdiri).
* **Validasi Input Aman**: Mencegah input nilai negatif atau tidak logis saat pendataan.

---

## 🛠️ Persyaratan & Cara Menjalankan

Aplikasi ini dibuat menggunakan bahasa pemrograman C++. Anda memerlukan compiler C++ (seperti `g++`) untuk mengompilasi kode program ini.

### Kompilasi (Compile)
Jalankan perintah berikut di terminal/command prompt Anda:
```bash
g++ -std=c++11 -o antropometri_gizi_z_score src/main.cpp src/json_parser.cpp src/calculator.cpp src/controllers/*.cpp src/models/*.cpp src/database/*.cpp src/views/*.cpp
```

### Menjalankan Aplikasi (Run)
Setelah berhasil dikompilasi, jalankan program:
* **Linux/macOS**:
  ```bash
  ./antropometri_gizi_z_score
  ```
* **Windows**:
  ```cmd
  antropometri_gizi_z_score.exe
  ```

---

## 📊 Kriteria Klasifikasi (Standar Kemenkes RI 2020)

Hasil klasifikasi gizi yang dikeluarkan sistem meliputi:
* **BB/U**: Sangat Kurang, Kurang, Normal, Risiko Berat Badan Lebih.
* **PB/U atau TB/U**: Sangat Pendek, Pendek, Normal, Tinggi.
* **BB/PB atau BB/TB**: Gizi Buruk, Gizi Kurang, Gizi Baik (Normal), Berisiko Gizi Lebih, Gizi Lebih, Obesitas.
