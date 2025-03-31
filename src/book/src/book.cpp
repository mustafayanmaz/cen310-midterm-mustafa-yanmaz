#include "book.h"
#include <cstring>
#include <mutex>
#include <thread>
#include <chrono>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <omp.h> 
#ifdef _OPENMP
#include <omp.h>
#endif

// ---------------------------
// Global Mutexes
// ---------------------------
std::mutex booksMutex;
std::mutex usersMutex;
std::mutex requestsMutex;
std::mutex transactionsMutex;

// ---------------------------
// Helper Functions for Encrypted I/O
// ---------------------------
void writeEncryptedString(std::ostream& os, const std::string& str) {
    size_t len = str.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    std::string enc = encryptString(str);
    os.write(enc.c_str(), len);
}

std::string readEncryptedString(std::istream& is) {
    size_t len = 0;
    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    char* buffer = new char[len];
    is.read(buffer, len);
    std::string enc(buffer, len);
    delete[] buffer;
    return decryptString(enc);
}

// ---------------------------
// Encryption Functions
// ---------------------------
std::string encryptString(const std::string& input) {
    std::string output = input;
    for (char& c : output)
        c ^= ENCRYPTION_KEY;
    return output;
}

std::string decryptString(const std::string& input) {
    return encryptString(input); // XOR symmetric
}

// ---------------------------
// Book Class Implementations
// ---------------------------
Book::Book() : id(0), title(""), author(""), genre(""), owner("") {}

Book::Book(int id, const std::string& title, const std::string& author, const std::string& genre, const std::string& owner)
    : id(id), title(title), author(author), genre(genre), owner(owner) {}



void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

void showMainMenu() {
    std::cout << "=============================" << std::endl;
    std::cout << "   Book Exchange Platform    " << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "1. Register" << std::endl;
    std::cout << "2. Login" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

void showUserMenu(const std::string& username) {
    std::cout << "=============================" << std::endl;
    std::cout << "Welcome, " << username << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "1. Add Book Manually" << std::endl;
    std::cout << "2. Auto Add Books (Thread per Book)" << std::endl;
    std::cout << "3. Auto Add Books (Improved Single Lock)" << std::endl;
    std::cout << "4. List All Books" << std::endl;
    std::cout << "5. Search Books" << std::endl;
    std::cout << "6. Exchange Requests (Not Implemented)" << std::endl;
    std::cout << "7. Rate User (Not Implemented)" << std::endl;
    std::cout << "8. Transaction History" << std::endl;
    std::cout << "9. Delete Books" << std::endl;
    std::cout << "10. Heavy Load Listing Performance Test" << std::endl;
    std::cout << "11. Auto Add Performance Test" << std::endl;
    std::cout << "12. Search Performance Test" << std::endl;
    std::cout << "13. Logout" << std::endl;
    std::cout << "Enter your choice: ";
}

void registerUserMenu() {
    std::string username, password;
    std::cout << "Enter new username: ";
    std::cin >> username;
    std::cout << "Enter new password: ";
    std::cin >> password;
    User user(username, password);
    if (registerUser(user))
        std::cout << "Registration successful." << std::endl;
    else
        std::cout << "Registration failed. Username may already exist." << std::endl;
    pauseScreen();
}

bool loginUserMenu(std::string& loggedInUser) {
    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;
    if (loginUser(username, password)) {
        std::cout << "Login successful." << std::endl;
        loggedInUser = username;
        pauseScreen();
        return true;
    }
    else {
        std::cout << "Login failed. Please try again." << std::endl;
        pauseScreen();
        return false;
    }
}

void Book::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(&id), sizeof(id));
    writeEncryptedString(os, title);
    writeEncryptedString(os, author);
    writeEncryptedString(os, genre);
    writeEncryptedString(os, owner);
}

void Book::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(&id), sizeof(id));
    title = readEncryptedString(is);
    author = readEncryptedString(is);
    genre = readEncryptedString(is);
    owner = readEncryptedString(is);
}

// ---------------------------
// User Class Implementations
// ---------------------------
User::User() : username(""), password(""), rating(0.0f), ratingCount(0) {}

User::User(const std::string& username, const std::string& password)
    : username(username), password(password), rating(0.0f), ratingCount(0) {}

void User::serialize(std::ostream& os) const {
    writeEncryptedString(os, username);
    writeEncryptedString(os, password);
    os.write(reinterpret_cast<const char*>(&rating), sizeof(rating));
    os.write(reinterpret_cast<const char*>(&ratingCount), sizeof(ratingCount));
}

void User::deserialize(std::istream& is) {
    username = readEncryptedString(is);
    password = readEncryptedString(is);
    is.read(reinterpret_cast<char*>(&rating), sizeof(rating));
    is.read(reinterpret_cast<char*>(&ratingCount), sizeof(ratingCount));
}