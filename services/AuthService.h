#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "../models/User.h"
#include <openssl/sha.h>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>

class AuthService {
private:
    std::vector<User> users;
    std::string generateUserId() {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = now_ms.time_since_epoch().count();
        return "USER_" + std::to_string(value);
    }

    std::string generateOTP() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100000, 999999);
        return std::to_string(dis(gen));
    }

    std::string hashPassword(const std::string& password) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        password = ss.str();
        return password;
    }

public:
    bool registerUser(User& user) {
        if (!user.validateUsername() || !user.validatePassword() ||
            !user.validateFullName() || !user.validatePhone() ||
            !user.validateEmail() || !user.validateBirthday()) {
            return false;
        }

        for (const auto& existingUser : users) {
            if (existingUser.username == user.username) {
                return false;
            }
        }

        user.userId = generateUserId();
        user.password = hashPassword(user.password);
        
        users.push_back(user);
        return true;
    }

    User* login(const std::string& username, const std::string& password) {
        for (auto& user : users) {
            if (user.username == username && user.password == hashPassword(password)) {
                if (user.isFirstLogin) {
                    std::cout << "Đây là lần đăng nhập đầu tiên. Vui lòng đổi mật khẩu!\n";
                }
                return &user;
            }
        }
        return nullptr;
    }

    bool changePassword(User* user, const std::string& oldPassword, const std::string& newPassword) {
        if (user->password != hashPassword(oldPassword)) {
            return false;
        }

        if (newPassword.length() < 6 || newPassword.length() > 64) {
            return false;
        }

        user->password = hashPassword(newPassword);
        user->isFirstLogin = false;
        return true;
    }

    std::string generateAndSendOTP(const std::string& email) {
        std::string otp = generateOTP();
        std::cout << "Mã OTP đã được gửi đến email: " << email << "\n";
        std::cout << "Mã OTP: " << otp << "\n"; 
        return otp;
    }

    bool verifyOTP(const std::string& inputOTP, const std::string& correctOTP) {
        return inputOTP == correctOTP;
    }
};

#endif 