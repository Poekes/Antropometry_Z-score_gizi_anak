#include "json_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <stdexcept>

// Tokenisasi string JSON menjadi token-token kecil
std::vector<JsonParser::Token> JsonParser::tokenize(const std::string& source) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < source.size()) {
        char c = source[i];
        if (std::isspace(static_cast<unsigned char>(c))) {
            i++;
            continue;
        }

        if (c == '{') {
            tokens.push_back({TokenType::LBRACE, "{"});
            i++;
        } else if (c == '}') {
            tokens.push_back({TokenType::RBRACE, "}"});
            i++;
        } else if (c == '[') {
            tokens.push_back({TokenType::LBRACKET, "["});
            i++;
        } else if (c == ']') {
            tokens.push_back({TokenType::RBRACKET, "]"});
            i++;
        } else if (c == ':') {
            tokens.push_back({TokenType::COLON, ":"});
            i++;
        } else if (c == ',') {
            tokens.push_back({TokenType::COMMA, ","});
            i++;
        } else if (c == '"') {
            std::string s;
            i++; // Lewati tanda kutip pembuka
            while (i < source.size() && source[i] != '"') {
                if (source[i] == '\\' && i + 1 < source.size()) {
                    i++; // Lewati karakter escape
                }
                s += source[i];
                i++;
            }
            if (i < source.size()) i++; // Lewati tanda kutip penutup
            tokens.push_back({TokenType::STRING, s});
        } else if (std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '+') {
            std::string num;
            while (i < source.size() && (std::isdigit(static_cast<unsigned char>(source[i])) || 
                   source[i] == '.' || source[i] == '-' || source[i] == '+' || 
                   source[i] == 'e' || source[i] == 'E')) {
                num += source[i];
                i++;
            }
            tokens.push_back({TokenType::NUMBER, num});
        } else if (std::isalpha(static_cast<unsigned char>(c))) {
            std::string word;
            while (i < source.size() && std::isalpha(static_cast<unsigned char>(source[i]))) {
                word += source[i];
                i++;
            }
            if (word == "true") tokens.push_back({TokenType::TRUE, "true"});
            else if (word == "false") tokens.push_back({TokenType::FALSE, "false"});
            else if (word == "null") tokens.push_back({TokenType::NULL_T, "null"});
            else tokens.push_back({TokenType::STRING, word});
        } else {
            // Abaikan karakter yang tidak dikenal
            i++;
        }
    }
    tokens.push_back({TokenType::END, ""});
    return tokens;
}

// Rekursif mengurai token menjadi struktur data JsonValue
JsonValue JsonParser::parseValue(const std::vector<Token>& tokens, size_t& index) {
    if (index >= tokens.size()) {
        JsonValue val;
        val.type = JsonValue::Type::NULL_TYPE;
        return val;
    }
    const Token& t = tokens[index];

    if (t.type == TokenType::LBRACE) {
        index++; // Lewati '{'
        JsonValue val;
        val.type = JsonValue::Type::OBJECT;
        while (index < tokens.size() && tokens[index].type != TokenType::RBRACE) {
            if (tokens[index].type != TokenType::STRING) {
                index++;
                continue;
            }
            std::string key = tokens[index].text;
            index++; // Lewati key string
            
            if (index < tokens.size() && tokens[index].type == TokenType::COLON) {
                index++; // Lewati ':'
            }
            
            val.obj_val[key] = parseValue(tokens, index);
            
            if (index < tokens.size() && tokens[index].type == TokenType::COMMA) {
                index++; // Lewati ','
            }
        }
        if (index < tokens.size()) index++; // Lewati '}'
        return val;
    } 
    else if (t.type == TokenType::LBRACKET) {
        index++; // Lewati '['
        JsonValue val;
        val.type = JsonValue::Type::ARRAY;
        while (index < tokens.size() && tokens[index].type != TokenType::RBRACKET) {
            val.arr_val.push_back(parseValue(tokens, index));
            if (index < tokens.size() && tokens[index].type == TokenType::COMMA) {
                index++; // Lewati ','
            }
        }
        if (index < tokens.size()) index++; // Lewati ']'
        return val;
    } 
    else if (t.type == TokenType::STRING) {
        JsonValue val;
        val.type = JsonValue::Type::STRING;
        val.str_val = t.text;
        index++;
        return val;
    } 
    else if (t.type == TokenType::NUMBER) {
        JsonValue val;
        val.type = JsonValue::Type::NUMBER;
        try {
            val.num_val = std::stod(t.text);
        } catch (...) {
            val.num_val = 0.0;
        }
        index++;
        return val;
    } 
    else if (t.type == TokenType::TRUE) {
        JsonValue val;
        val.type = JsonValue::Type::BOOLEAN;
        val.bool_val = true;
        index++;
        return val;
    } 
    else if (t.type == TokenType::FALSE) {
        JsonValue val;
        val.type = JsonValue::Type::BOOLEAN;
        val.bool_val = false;
        index++;
        return val;
    } 
    else if (t.type == TokenType::NULL_T) {
        index++;
        JsonValue val;
        val.type = JsonValue::Type::NULL_TYPE;
        return val;
    }

    index++;
    JsonValue val;
    val.type = JsonValue::Type::NULL_TYPE;
    return val;
}

// Membaca file dan memetakan ke ReferenceData
bool JsonParser::loadReferenceData(const std::string& filepath, ReferenceData& refData) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Gagal membuka file referensi: " << filepath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::vector<Token> tokens = tokenize(buffer.str());
    size_t index = 0;
    JsonValue root = parseValue(tokens, index);

    if (root.type != JsonValue::Type::OBJECT) {
        std::cerr << "[ERROR] Format JSON tidak valid. Root harus berupa objek." << std::endl;
        return false;
    }

    if (root.obj_val.find("data") == root.obj_val.end()) {
        std::cerr << "[ERROR] Kunci 'data' tidak ditemukan dalam JSON." << std::endl;
        return false;
    }

    const JsonValue& dataNode = root.obj_val.at("data");
    
    // Parse Laki-laki
    if (dataNode.obj_val.find("Laki-laki") != dataNode.obj_val.end()) {
        populateGenderData(dataNode.obj_val.at("Laki-laki"), refData.laki_laki);
    } else {
        std::cerr << "[WARNING] Data 'Laki-laki' tidak ditemukan." << std::endl;
    }

    // Parse Perempuan
    if (dataNode.obj_val.find("Perempuan") != dataNode.obj_val.end()) {
        populateGenderData(dataNode.obj_val.at("Perempuan"), refData.perempuan);
    } else {
        std::cerr << "[WARNING] Data 'Perempuan' tidak ditemukan." << std::endl;
    }

    return true;
}

// Mengekstrak array indikator gizi ke dalam GenderData
void JsonParser::populateGenderData(const JsonValue& genderJson, GenderData& genderData) {
    auto parseRecordList = [](const JsonValue& arrayNode, std::vector<ZScoreRecord>& targetList, bool isHeightBased) {
        if (arrayNode.type != JsonValue::Type::ARRAY) return;
        
        for (const auto& item : arrayNode.arr_val) {
            if (item.type != JsonValue::Type::OBJECT) continue;
            
            ZScoreRecord rec;
            rec.key_val = 0.0;
            
            if (isHeightBased) {
                if (item.obj_val.count("height_cm")) {
                    rec.key_val = item.obj_val.at("height_cm").num_val;
                }
            } else {
                if (item.obj_val.count("age_months")) {
                    rec.key_val = item.obj_val.at("age_months").num_val;
                }
            }
            
            // Mengambil standard deviasi dengan aman
            rec.sd_minus_3 = item.obj_val.count("sd_minus_3") ? item.obj_val.at("sd_minus_3").num_val : 0.0;
            rec.sd_minus_2 = item.obj_val.count("sd_minus_2") ? item.obj_val.at("sd_minus_2").num_val : 0.0;
            rec.sd_minus_1 = item.obj_val.count("sd_minus_1") ? item.obj_val.at("sd_minus_1").num_val : 0.0;
            rec.median     = item.obj_val.count("median")     ? item.obj_val.at("median").num_val     : 0.0;
            rec.sd_plus_1  = item.obj_val.count("sd_plus_1")  ? item.obj_val.at("sd_plus_1").num_val  : 0.0;
            rec.sd_plus_2  = item.obj_val.count("sd_plus_2")  ? item.obj_val.at("sd_plus_2").num_val  : 0.0;
            rec.sd_plus_3  = item.obj_val.count("sd_plus_3")  ? item.obj_val.at("sd_plus_3").num_val  : 0.0;
            
            targetList.push_back(rec);
        }
    };

    if (genderJson.obj_val.count("bbu")) {
        parseRecordList(genderJson.obj_val.at("bbu"), genderData.bbu, false);
    }
    if (genderJson.obj_val.count("pbu")) {
        parseRecordList(genderJson.obj_val.at("pbu"), genderData.pbu, false);
    }
    if (genderJson.obj_val.count("tbu")) {
        parseRecordList(genderJson.obj_val.at("tbu"), genderData.tbu, false);
    }
    if (genderJson.obj_val.count("bbpb")) {
        parseRecordList(genderJson.obj_val.at("bbpb"), genderData.bbpb, true);
    }
    if (genderJson.obj_val.count("bbtb")) {
        parseRecordList(genderJson.obj_val.at("bbtb"), genderData.bbtb, true);
    }
}
