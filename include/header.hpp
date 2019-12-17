// Copyright 2018 Your Name <your_email>
#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <iostream>
#include <string>
#include <any>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>

class Json {
private:
    enum class Act {
        find_colon,
        find_value,
        find_key_or_end,
        find_comma_or_end
    };

    int parse_number(const std::string &number, size_t &pos) const;

    bool parse_bool(const std::string b, size_t &pos) const;

    std::string parse_string(const std::string str, size_t &pos) const;

    void skip(const std::string str, size_t &pos);

    std::vector<std::any> parse_array(const std::string &str, size_t &pos);

    std::map<std::string, std::any>
    parse_object(const std::string &str, size_t &pos);

public:
    explicit Json(const std::string &s);

    Json() {}

    explicit Json(const std::vector<std::any> &array) : _data(array) {}

    explicit Json(const std::map<std::string, std::any> &object)
            : _data(object) {}


    bool is_array() const;


    bool is_object() const;


    std::any &operator[](const std::string &key) {
        if (this->is_object()) {
            auto &data =
            std::any_cast<std::map<std::string, std::any> &>(_data);
            return data[key];
        } else{
            throw std::bad_any_cast();
        }
    }


    std::any &operator[](int index) {
        if (this->is_array()) {
            auto &data =
                    std::any_cast<std::vector<std::any> &>(_data);
            return data[index];
        } else {
            throw std::bad_any_cast();
        }
    }

    static Json parse(const std::string &s) {
        try {
            return Json(s);
        } catch (std::bad_any_cast()) {
            std::cout << "This is not a Json-object "
                         "or Json-array!" << std::endl;
            return Json();
        }
    }


    static Json parseFile(const std::string &path_to_file) {
        std::ifstream f(path_to_file);
        std::stringstream ss;
        ss << f.rdbuf();
        std::cout << ss.str();
        return Json(ss.str());
    }

private:
    std::any _data;
};

#endif // INCLUDE_HEADER_HPP_
