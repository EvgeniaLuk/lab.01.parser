// Copyright 2018 Your Name <your_email>

#include "header.hpp"

int Json::parse_number(const std::string &number, size_t &pos) const {
    std::string res;
    int result;
    while (number[pos] != ',') {
        if (number[pos] == ']') break;
        if (number[pos] == '}') break;
        if (!isspace(number[pos])) res.push_back(number[pos]);
        pos++;
    }
    pos--;
    std::stringstream ss;
    ss << res;
    ss >> result;
    return result;
}

std::string Json::parse_string(const std::string str, size_t &pos) const {
    std::string res;
    pos++;
    while (str[pos] != '"') {
        res.push_back(str[pos]);
        pos++;
    }
    return res;
}

bool Json::parse_bool(const std::string b, size_t &pos) const {
    std::string res;
    while (b[pos] != 'e') {
        res.push_back(b[pos]);
        pos++;
    }
    res.push_back('e');
    if (res == "false") {
        return false;
    } else if (res == "true") {
        return true;
    }else {
        throw std::bad_any_cast();
    }
}

void Json::skip(const std::string str, size_t &pos) {
    while (isspace(str[pos])) pos++;
}

std::vector<std::any>
Json::parse_array(const std::string &str, size_t &pos) {
    std::vector<std::any> res;
    auto state = Act::find_value;
    for (size_t i = pos; i < str.length(); i++) {
        skip(str, i);
        if (str[i] == '[') {
            if (state == Act::find_value) {
                i++;
                res.emplace_back(Json(parse_array(str, i)));
                state = Act::find_comma_or_end;
            }
        } else if (str[i] == ']') {
            if (state == Act::find_comma_or_end) {
                pos = i;
                return res;
            }
            if (state == Act::find_value) {
                pos = i;
                return res;
            }
        } else if (str[i] == '{') {
            if (state == Act::find_value) {
                i++;
                res.emplace_back(Json(parse_object(str, i)));
                state = Act::find_comma_or_end;
            }
        } else if (str[i] == '"') {
            if (state == Act::find_value) {
                res.emplace_back(parse_string(str, i));
                state = Act::find_comma_or_end;
            }
        } else if (str[i] == ',') {
            if (state == Act::find_comma_or_end) {
                state = Act::find_value;
            }
        } else if (isdigit(str[i]) || isalpha(str[i])) {
            if ((str[i] == '+' && isdigit(str[i + 1]) != 0)
                || (str[i] == '-' && isdigit(str[i + 1])) != 0) i++;
            if (str[i] >= '0' && str[i] <= '9') {
                if (state == Act::find_value) {
                    res.emplace_back(parse_number(str, i));
                    state = Act::find_comma_or_end;
                }
            } else if (str[i] == 'f' || str[i] == 't') {
                if (state == Act::find_value) {
                    res.emplace_back(parse_bool(str, i));
                    state = Act::find_comma_or_end;
                }
            }
        }
    }
    res.clear();
    return res;
}

std::map<std::string, std::any>
Json::parse_object(const std::string &str, size_t &pos) {
    std::map<std::string, std::any> res;
    auto state = Act::find_key_or_end;
    std::string key;
    for (size_t i = pos; i < str.length(); i++) {
        skip(str, i);
        if (str[i] == '"') {
            if (state == Act::find_key_or_end) {
                key = parse_string(str, i);
                state = Act::find_colon;
            } else if (state == Act::find_value) {
                res[key] = parse_string(str, i);
                state = Act::find_comma_or_end;
            }
        } else if (str[i] == ',') {
            if (state == Act::find_comma_or_end) {
                state = Act::find_key_or_end;
            }
        } else if (str[i] == '[') {
            if (state == Act::find_value) {
                i++;
                res[key] = Json(parse_array(str, i));
                state = Act::find_comma_or_end;
            }
        } else if (str[i] == '}') {
            if (state == Act::find_comma_or_end) {
                pos = i;
                return res;
            }
            if (state == Act::find_key_or_end) {
                pos = i;
                return res;
            }
            if (state == Act::find_value) {
                pos = i;
                return res;
            }
        } else if (str[i] == '{') {
            if (state == Act::find_value) {
                i++;
                res[key] = Json(parse_object(str, i));
                state = Act::find_comma_or_end;
            }
        } else if (str[i] == ':') {
            if (state == Act::find_colon) {
                state = Act::find_value;
            }
        } else if (isdigit(str[i]) || isalpha(str[i])) {
            if ((str[i] == '+'|| str[i] == '-')
                  && isdigit(str[i + 1])) i++;
            if (str[i] >= '0' && str[i] <= '9') {
                if (state == Act::find_value) {
                    res[key] = parse_number(str, i);
                    state = Act::find_comma_or_end;
                }
            } else if (str[i] == 'f' || str[i] == 't') {
                if (state == Act::find_value) {
                    res[key] = parse_bool(str, i);
                    state = Act::find_comma_or_end;
                }
            }
        }
    }
    throw std::bad_any_cast();
}
//------------------------------------------------------------------------------------------------------------------

Json::Json(const std::string &s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == '{') {
            i++;
            _data =
                    std::any_cast<std::map<std::string, std::any>>
                    (parse_object(s, i));
        } else if (s[i] == '[') {
            i++;
            _data =
                    std::any_cast<std::vector<std::any>>(parse_array(s, i));
        } else if (!isspace(s[i])) {
            throw std::bad_any_cast();
        }
    }
}

bool Json::is_array() const {
    if (this->_data.type() !=
        typeid(std::vector<std::any>)) return false;
    return true;
}

bool Json::is_object() const {
    if (this->_data.type() !=
        typeid(std::map<std::string, std::any>)) return false;
    return true;
}
