#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "zscore_data.h"
#include <string>
#include <vector>
#include <map>

// Representasi pohon JSON sederhana untuk memproses data referensi
struct JsonValue {
    enum class Type { OBJECT, ARRAY, STRING, NUMBER, BOOLEAN, NULL_TYPE };
    Type type = Type::NULL_TYPE;
    std::map<std::string, JsonValue> obj_val;
    std::vector<JsonValue> arr_val;
    std::string str_val;
    double num_val = 0.0;
    bool bool_val = false;
};

// Parser JSON utama
class JsonParser {
public:
    // Membaca file JSON dan mengekstrak data referensi Z-score
    static bool loadReferenceData(const std::string& filepath, ReferenceData& refData);

private:
    enum class TokenType {
        LBRACE, RBRACE, LBRACKET, RBRACKET, COLON, COMMA,
        STRING, NUMBER, TRUE, FALSE, NULL_T, END
    };

    struct Token {
        TokenType type;
        std::string text;
    };

    // Fungsi bantu tokenisasi dan parsing internal
    static std::vector<Token> tokenize(const std::string& source);
    static JsonValue parseValue(const std::vector<Token>& tokens, size_t& index);
    static void populateGenderData(const JsonValue& genderJson, GenderData& genderData);
};

#endif // JSON_PARSER_H
