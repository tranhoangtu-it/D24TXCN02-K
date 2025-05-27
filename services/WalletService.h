#ifndef WALLET_SERVICE_H
#define WALLET_SERVICE_H

#include "../models/Wallet.h"
#include "../services/AuthService.h"
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

class WalletService {
private:
    std::vector<Wallet> wallets;
    AuthService* authService;

    std::string generateTransactionId() {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        auto value = now_ms.time_since_epoch().count();
        return "TRANS_" + std::to_string(value);
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

public:
    WalletService(AuthService* auth) : authService(auth) {}

    Wallet* createWallet(const std::string& userId, bool isMainWallet = false) {
        Wallet wallet;
        wallet.walletId = "WALLET_" + userId;
        wallet.userId = userId;
        wallet.isMainWallet = isMainWallet;
        wallets.push_back(wallet);
        return &wallets.back();
    }

    Wallet* getWallet(const std::string& walletId) {
        for (auto& wallet : wallets) {
            if (wallet.walletId == walletId) {
                return &wallet;
            }
        }
        return nullptr;
    }

    bool transferPoints(const std::string& fromWalletId, const std::string& toWalletId,
                       int amount, const std::string& otp) {
        Wallet* fromWallet = getWallet(fromWalletId);
        Wallet* toWallet = getWallet(toWalletId);

        if (!fromWallet || !toWallet) return false;
        if (amount <= 0) return false;
        if (!fromWallet->isMainWallet && fromWallet->balance < amount) return false;

        Transaction transaction;
        transaction.transactionId = generateTransactionId();
        transaction.fromWalletId = fromWalletId;
        transaction.toWalletId = toWalletId;
        transaction.amount = amount;
        transaction.status = "pending";
        transaction.timestamp = getCurrentTimestamp();
        transaction.otp = otp;

        if (!authService->verifyOTP(otp, otp)) {
            transaction.status = "failed";
            fromWallet->addTransaction(transaction);
            toWallet->receiveTransaction(transaction);
            return false;
        }
        if (!fromWallet->addTransaction(transaction)) {
            transaction.status = "failed";
            fromWallet->addTransaction(transaction);
            toWallet->receiveTransaction(transaction);
            return false;
        }
        transaction.status = "completed";
        fromWallet->addTransaction(transaction);
        toWallet->receiveTransaction(transaction);
        return true;
    }

    std::vector<Transaction> getTransactionHistory(const std::string& walletId) {
        Wallet* wallet = getWallet(walletId);
        if (wallet) {
            return wallet->transactions;
        }
        return std::vector<Transaction>();
    }

    int getBalance(const std::string& walletId) {
        Wallet* wallet = getWallet(walletId);
        return wallet ? wallet->balance : 0;
    }
};

#endif 