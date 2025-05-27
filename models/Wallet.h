#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include <ctime>

struct Transaction {
    std::string transactionId;
    std::string fromWalletId;
    std::string toWalletId;
    int amount;
    std::string status;
    std::string timestamp;
    std::string otp;
};

class Wallet {
public:
    std::string walletId;
    std::string userId;
    int balance;
    std::vector<Transaction> transactions;
    bool isMainWallet;

    Wallet() {
        balance = 0;
        isMainWallet = false;
    }

    bool addTransaction(const Transaction& transaction) {
        if (transaction.amount <= 0) return false;
        if (transaction.amount > balance && !isMainWallet) return false;
        
        transactions.push_back(transaction);
        if (transaction.status == "completed") {
            balance -= transaction.amount;
        }
        return true;
    }

    bool receiveTransaction(const Transaction& transaction) {
        if (transaction.amount <= 0) return false;
        
        transactions.push_back(transaction);
        if (transaction.status == "completed") {
            balance += transaction.amount;
        }
        return true;
    }
};

#endif 