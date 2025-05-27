#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "../models/User.h"
#include "../models/Wallet.h"
#include <string>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class FileManager {
private:
    std::string dataDir;
    std::string backupDir;

    std::string getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_time_t), "%Y%m%d");
        return ss.str();
    }

    void createDirectoryIfNotExists(const std::string& path) {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    }

public:
    FileManager() {
        dataDir = "data";
        backupDir = "backup";
        createDirectoryIfNotExists(dataDir);
        createDirectoryIfNotExists(backupDir);
    }

    bool saveUser(const User& user) {
        std::string userDir = dataDir + "/" + user.userId;
        createDirectoryIfNotExists(userDir);

        json userJson;
        userJson["userId"] = user.userId;
        userJson["username"] = user.username;
        userJson["password"] = user.password;
        userJson["fullName"] = user.fullName;
        userJson["gender"] = user.gender;
        userJson["address"] = user.address;
        userJson["phone"] = user.phone;
        userJson["email"] = user.email;
        userJson["birthday"] = user.birthday;
        userJson["points"] = user.points;
        userJson["role"] = user.role;
        userJson["isFirstLogin"] = user.isFirstLogin;
        userJson["lastBackupDate"] = user.lastBackupDate;

        std::ofstream file(userDir + "/user.json");
        file << userJson.dump(4);
        return true;
    }

    bool saveWallet(const Wallet& wallet) {
        std::string walletDir = dataDir + "/" + wallet.userId + "/wallet";
        createDirectoryIfNotExists(walletDir);

        json walletJson;
        walletJson["walletId"] = wallet.walletId;
        walletJson["userId"] = wallet.userId;
        walletJson["balance"] = wallet.balance;
        walletJson["isMainWallet"] = wallet.isMainWallet;

        json transactionsJson = json::array();
        for (const auto& transaction : wallet.transactions) {
            json transactionJson;
            transactionJson["transactionId"] = transaction.transactionId;
            transactionJson["fromWalletId"] = transaction.fromWalletId;
            transactionJson["toWalletId"] = transaction.toWalletId;
            transactionJson["amount"] = transaction.amount;
            transactionJson["status"] = transaction.status;
            transactionJson["timestamp"] = transaction.timestamp;
            transactionJson["otp"] = transaction.otp;
            transactionsJson.push_back(transactionJson);
        }
        walletJson["transactions"] = transactionsJson;

        std::ofstream file(walletDir + "/wallet.json");
        file << walletJson.dump(4);
        return true;
    }

    bool backupData() {
        std::string backupDate = getCurrentDate();
        std::string backupPath = backupDir + "/" + backupDate;
        createDirectoryIfNotExists(backupPath);

        // Copy all data to backup directory
        try {
            std::filesystem::copy(dataDir, backupPath, 
                                std::filesystem::copy_options::recursive);
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Backup failed: " << e.what() << std::endl;
            return false;
        }
    }

    bool shouldBackup(const std::string& lastBackupDate) {
        if (lastBackupDate.empty()) return true;

        auto lastBackup = std::chrono::system_clock::from_time_t(
            std::stoll(lastBackupDate));
        auto now = std::chrono::system_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::hours>(now - lastBackup);
        
        return diff.count() >= 24 * 7; // 7 days
    }
};

#endif 