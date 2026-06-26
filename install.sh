#!/bin/bash
echo "Mengompilasi aplikasi Kalkulator Antropometri Z-Score Anak..."
g++ -std=c++11 -o antropometri_gizi_z_score src/main.cpp src/json_parser.cpp src/calculator.cpp src/controllers/*.cpp src/models/*.cpp src/database/*.cpp src/views/*.cpp
if [ $? -eq 0 ]; then
    echo "Kompilasi berhasil! Jalankan aplikasi dengan './antropometri_gizi_z_score'"
else
    echo "Kompilasi gagal. Pastikan g++ telah terinstal dan mendukung C++11."
    exit 1
fi
