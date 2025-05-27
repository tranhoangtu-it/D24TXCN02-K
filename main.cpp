#include <iostream>
#include <string>
#include "models/User.h"
#include "models/Wallet.h"
#include "services/AuthService.h"
#include "services/WalletService.h"
#include "utils/FileManager.h"

using namespace std;

void showMainMenu() {
    cout << "\n=== HỆ THỐNG QUẢN LÝ TÀI KHOẢN VÀ VÍ ===\n";
    cout << "1. Đăng nhập\n";
    cout << "2. Đăng ký tài khoản mới\n";
    cout << "3. Thoát\n";
    cout << "Chọn chức năng: ";
}

void showUserMenu() {
    cout << "\n=== MENU NGƯỜI DÙNG ===\n";
    cout << "1. Xem thông tin cá nhân\n";
    cout << "2. Cập nhật thông tin cá nhân\n";
    cout << "3. Đổi mật khẩu\n";
    cout << "4. Xem số dư ví\n";
    cout << "5. Xem lịch sử giao dịch\n";
    cout << "6. Chuyển điểm\n";
    cout << "7. Đăng xuất\n";
    cout << "Chọn chức năng: ";
}

void showAdminMenu() {
    cout << "\n=== MENU QUẢN LÝ ===\n";
    cout << "1. Xem danh sách người dùng\n";
    cout << "2. Tạo tài khoản mới\n";
    cout << "3. Cập nhật thông tin người dùng\n";
    cout << "4. Backup dữ liệu\n";
    cout << "5. Xem thống kê giao dịch\n";
    cout << "6. Đăng xuất\n";
    cout << "Chọn chức năng: ";
}

void handleUserMenu(User* user, AuthService& authService, WalletService& walletService, FileManager& fileManager) {
    int choice;
    while (true) {
        showUserMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: { 
                cout << "\nThông tin cá nhân:\n";
                cout << "Username: " << user->username << endl;
                cout << "Họ tên: " << user->fullName << endl;
                cout << "Giới tính: " << user->gender << endl;
                cout << "Địa chỉ: " << user->address << endl;
                cout << "Số điện thoại: " << user->phone << endl;
                cout << "Email: " << user->email << endl;
                cout << "Ngày sinh: " << user->birthday << endl;
                cout << "Số điểm: " << user->points << endl;
                break;
            }
            case 2: { 
                string otp = authService.generateAndSendOTP(user->email);
                string inputOTP;
                cout << "Nhập mã OTP đã gửi đến email của bạn: ";
                cin >> inputOTP;
                
                if (authService.verifyOTP(inputOTP, otp)) {
                    cout << "Nhập họ tên mới: ";
                    cin.ignore();
                    getline(cin, user->fullName);
                    cout << "Nhập địa chỉ mới: ";
                    getline(cin, user->address);
                    cout << "Nhập số điện thoại mới: ";
                    cin >> user->phone;
                    cout << "Nhập email mới: ";
                    cin >> user->email;
                    
                    if (user->validateFullName() && user->validatePhone() && user->validateEmail()) {
                        fileManager.saveUser(*user);
                        cout << "Cập nhật thông tin thành công!\n";
                    } else {
                        cout << "Thông tin không hợp lệ!\n";
                    }
                } else {
                    cout << "Mã OTP không đúng!\n";
                }
                break;
            }
            case 3: {
                string oldPassword, newPassword;
                cout << "Nhập mật khẩu cũ: ";
                cin >> oldPassword;
                cout << "Nhập mật khẩu mới: ";
                cin >> newPassword;
                
                if (authService.changePassword(user, oldPassword, newPassword)) {
                    fileManager.saveUser(*user);
                    cout << "Đổi mật khẩu thành công!\n";
                } else {
                    cout << "Đổi mật khẩu thất bại!\n";
                }
                break;
            }
            case 4: {
                string walletId = "WALLET_" + user->userId;
                int balance = walletService.getBalance(walletId);
                cout << "Số dư ví: " << balance << " điểm\n";
                break;
            }
            case 5: {
                string walletId = "WALLET_" + user->userId;
                auto transactions = walletService.getTransactionHistory(walletId);
                cout << "\nLịch sử giao dịch:\n";
                for (const auto& trans : transactions) {
                    cout << "ID: " << trans.transactionId << endl;
                    cout << "Từ: " << trans.fromWalletId << endl;
                    cout << "Đến: " << trans.toWalletId << endl;
                    cout << "Số điểm: " << trans.amount << endl;
                    cout << "Trạng thái: " << trans.status << endl;
                    cout << "Thời gian: " << trans.timestamp << endl;
                    cout << "------------------------\n";
                }
                break;
            }
            case 6: {
                string toWalletId;
                int amount;
                cout << "Nhập ID ví nhận: ";
                cin >> toWalletId;
                cout << "Nhập số điểm muốn chuyển: ";
                cin >> amount;
                
                string otp = authService.generateAndSendOTP(user->email);
                string inputOTP;
                cout << "Nhập mã OTP đã gửi đến email của bạn: ";
                cin >> inputOTP;
                
                if (authService.verifyOTP(inputOTP, otp)) {
                    string fromWalletId = "WALLET_" + user->userId;
                    if (walletService.transferPoints(fromWalletId, toWalletId, amount, otp)) {
                        cout << "Chuyển điểm thành công!\n";
                    } else {
                        cout << "Chuyển điểm thất bại!\n";
                    }
                } else {
                    cout << "Mã OTP không đúng!\n";
                }
                break;
            }
            case 7:
                return;
            default:
                cout << "Lựa chọn không hợp lệ!\n";
        }
    }
}

void handleAdminMenu(User* admin, AuthService& authService, WalletService& walletService, FileManager& fileManager) {
    int choice;
    while (true) {
        showAdminMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                cout << "\nDanh sách người dùng:\n";
                auto users = authService.getAllUsers();
                for (const auto& user : users) {
                    cout << "ID: " << user.userId << ", Username: " << user.username 
                         << ", Họ tên: " << user.fullName << ", Số điểm: " << user.points 
                         << ", Vai trò: " << user.role << endl;
                }
                break;
            }
            case 2: {
                User newUser;
                cout << "Nhập username (4-32 ký tự): ";
                cin >> newUser.username;
                cout << "Nhập password (6-64 ký tự): ";
                cin >> newUser.password;
                cout << "Nhập họ tên (2-50 ký tự): ";
                cin.ignore();
                getline(cin, newUser.fullName);
                cout << "Nhập giới tính (Nam/Nữ): ";
                cin >> newUser.gender;
                cout << "Nhập địa chỉ: ";
                cin.ignore();
                getline(cin, newUser.address);
                cout << "Nhập số điện thoại: ";
                cin >> newUser.phone;
                cout << "Nhập email: ";
                cin >> newUser.email;
                cout << "Nhập ngày sinh (dd/mm/yyyy): ";
                cin >> newUser.birthday;
                
                if (authService.registerUser(newUser)) {
                    fileManager.saveUser(newUser);
                    cout << "Tạo tài khoản thành công!\n";
                } else {
                    cout << "Tạo tài khoản thất bại!\n";
                }
                break;
            }
            case 3: {
                string userId;
                cout << "Nhập ID người dùng cần cập nhật: ";
                cin >> userId;
                User* user = authService.getUserById(userId);
                if (!user) {
                    cout << "Người dùng không tồn tại!\n";
                    break;
                }
                string otp = authService.generateAndSendOTP(user->email);
                string inputOTP;
                cout << "Nhập mã OTP đã gửi đến email của bạn: ";
                cin >> inputOTP;
                break;
            }
            case 4: {
                if (fileManager.backupData()) {
                    cout << "Backup dữ liệu thành công!\n";
                } else {
                    cout << "Backup dữ liệu thất bại!\n";
                }
                break;
            }
            case 5: {
                // Chức năng thống kê giao dịch sẽ được phát triển sau
                cout << "Chức năng đang được phát triển...\n";
                break;
            }
            case 6: // Đăng xuất
                return;
            default:
                cout << "Lựa chọn không hợp lệ!\n";
        }
    }
}

int main() {
    AuthService authService;
    WalletService walletService(&authService);
    FileManager fileManager;
    
    int choice;
    bool isLoggedIn = false;
    User* currentUser = nullptr;
    
    while (true) {
        if (!isLoggedIn) {
            showMainMenu();
            cin >> choice;
            
            switch (choice) {
                case 1: {
                    string username, password;
                    cout << "Nhập username: ";
                    cin >> username;
                    cout << "Nhập password: ";
                    cin >> password;
                    
                    currentUser = authService.login(username, password);
                    if (currentUser) {
                        isLoggedIn = true;
                        cout << "Đăng nhập thành công!\n";
                    } else {
                        cout << "Đăng nhập thất bại!\n";
                    }
                    break;
                }
                case 2: {
                    User newUser;
                    cout << "Nhập username (4-32 ký tự): ";
                    cin >> newUser.username;
                    cout << "Nhập password (6-64 ký tự): ";
                    cin >> newUser.password;
                    cout << "Nhập họ tên (2-50 ký tự): ";
                    cin.ignore();
                    getline(cin, newUser.fullName);
                    cout << "Nhập giới tính (Nam/Nữ): ";
                    cin >> newUser.gender;
                    cout << "Nhập địa chỉ: ";
                    cin.ignore();
                    getline(cin, newUser.address);
                    cout << "Nhập số điện thoại: ";
                    cin >> newUser.phone;
                    cout << "Nhập email: ";
                    cin >> newUser.email;
                    cout << "Nhập ngày sinh (dd/mm/yyyy): ";
                    cin >> newUser.birthday;
                    
                    if (authService.registerUser(newUser)) {
                        fileManager.saveUser(newUser);
                        cout << "Đăng ký thành công!\n";
                    } else {
                        cout << "Đăng ký thất bại!\n";
                    }
                    break;
                }
                case 3:
                    return 0;
                default:
                    cout << "Lựa chọn không hợp lệ!\n";
            }
        } else {
            if (currentUser->role == "Quan_Ly") {
                handleAdminMenu(currentUser, authService, walletService, fileManager);
            } else {
                handleUserMenu(currentUser, authService, walletService, fileManager);
            }
            isLoggedIn = false;
            currentUser = nullptr;
        }
    }
    
    return 0;
} 