#ifndef USER_H
#define USER_H

#include <string>
#include <ctime>

class User {
public:
    std::string userId;
    std::string username;
    std::string password;
    std::string fullName;
    std::string gender;
    std::string address;
    std::string phone;
    std::string email;
    std::string birthday;
    int points;
    std::string role;
    bool isFirstLogin;
    std::string lastBackupDate;

    User() {
        points = 0;
        role = "Nguoi_Dung";
        isFirstLogin = true;
    }

    bool validateUsername() const {
        return username.length() >= 4 && username.length() <= 32;
    }

    bool validatePassword() const {
        return password.length() >= 6 && password.length() <= 64;
    }

    bool validateFullName() const {
        if (fullName.length() < 2 || fullName.length() > 50) return false;
        for (char c : fullName) {
            if (!isalpha(c) && c != ' ') return false;
        }
        return true;
    }

    bool validatePhone() const {
        for (char c : phone) {
            if (!isdigit(c)) return false;
        }
        return true;
    }

    bool validateEmail() const {
        size_t atPos = email.find('@');
        if (atPos == std::string::npos) return false;
        
        size_t dotPos = email.find('.', atPos);
        if (dotPos == std::string::npos) return false;
        
        return dotPos > atPos + 1 && dotPos < email.length() - 1;
    }

    bool validateBirthday() const {
        if (birthday.length() != 10) return false;
        if (birthday[2] != '/' || birthday[5] != '/') return false;
        
        int day = std::stoi(birthday.substr(0, 2));
        int month = std::stoi(birthday.substr(3, 2));
        int year = std::stoi(birthday.substr(6, 4));
        
        if (day < 1 || day > 31) return false;
        if (month < 1 || month > 12) return false;
        if (year < 1900 || year > 2024) return false;
        
        return true;
    }
};

#endif 