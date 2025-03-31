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

// ---------------------------
// ExchangeRequest Class Implementations
// ---------------------------
ExchangeRequest::ExchangeRequest() : bookId(0), fromUser(""), toUser(""), status(0) {}

ExchangeRequest::ExchangeRequest(int bookId, const std::string& fromUser, const std::string& toUser, int status)
    : bookId(bookId), fromUser(fromUser), toUser(toUser), status(status) {}

void ExchangeRequest::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(&bookId), sizeof(bookId));
    writeEncryptedString(os, fromUser);
    writeEncryptedString(os, toUser);
    os.write(reinterpret_cast<const char*>(&status), sizeof(status));
}

void ExchangeRequest::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(&bookId), sizeof(bookId));
    fromUser = readEncryptedString(is);
    toUser = readEncryptedString(is);
    is.read(reinterpret_cast<char*>(&status), sizeof(status));
}

void exchangeRequestsMenu(const std::string& currentUser) {
    while (true) {
        clearScreen();
        std::cout << "===== Exchange Requests Menu =====\n";
        std::cout << "0. Send Exchange Request\n";
        std::cout << "1. View Received (Pending) Requests\n";
        std::cout << "2. Accept/Decline Request\n";
        std::cout << "3. View Sent Requests\n";
        std::cout << "4. Return\n";
        std::cout << "Enter choice: ";
        int choice;
        std::cin >> choice;
        if (choice == 0) {
            sendExchangeRequestMenu(currentUser);
        }
        else if (choice == 1) {
            // View received requests
            auto reqs = loadExchangeRequests();
            bool found = false;
            std::cout << "----- Received (Pending) Requests -----\n";
            for (const auto& r : reqs) {
                if (r.toUser == currentUser && r.status == 0) {
                    std::cout << "BookID: " << r.bookId
                        << " | From: " << r.fromUser << std::endl;
                    found = true;
                }
            }
            if (!found) {
                std::cout << "No pending requests.\n";
            }
            pauseScreen();
        }
        else if (choice == 2) {
            // Accept/Decline a pending request
            auto reqs = loadExchangeRequests();
            std::vector<int> indexes;
            int idx = 0;
            std::cout << "----- Pending Requests -----\n";
            for (int i = 0; i < (int)reqs.size(); i++) {
                if (reqs[i].toUser == currentUser && reqs[i].status == 0) {
                    std::cout << "[" << idx << "] "
                        << "BookID: " << reqs[i].bookId
                        << " | From: " << reqs[i].fromUser << std::endl;
                    indexes.push_back(i);
                    idx++;
                }
            }
            if (indexes.empty()) {
                std::cout << "No pending requests to process.\n";
            }
            else {
                std::cout << "Select index to accept/decline: ";
                int sel;
                std::cin >> sel;
                if (sel >= 0 && sel < (int)indexes.size()) {
                    int realIdx = indexes[sel];
                    std::cout << "1. Accept   2. Decline: ";
                    int dec;
                    std::cin >> dec;
                    if (dec == 1) {
                        reqs[realIdx].status = 1; // accepted
                        // Create a transaction record with current date/time
                        auto t = std::time(nullptr);
                        std::stringstream dateStr;
                        dateStr << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
                        Transaction trans(reqs[realIdx].bookId,
                            reqs[realIdx].fromUser,
                            reqs[realIdx].toUser,
                            dateStr.str());
                        addTransaction(trans);
                        std::cout << "Request accepted. Transaction recorded.\n";
                    }
                    else {
                        reqs[realIdx].status = -1; // declined
                        std::cout << "Request declined.\n";
                    }
                    saveExchangeRequests(reqs);
                }
                else {
                    std::cout << "Invalid selection.\n";
                }
            }
            pauseScreen();
        }
        else if (choice == 3) {
            // View sent requests
            auto reqs = loadExchangeRequests();
            bool found = false;
            std::cout << "----- Sent Requests -----\n";
            for (const auto& r : reqs) {
                if (r.fromUser == currentUser) {
                    std::string st;
                    if (r.status == 0) st = "Pending";
                    else if (r.status == 1) st = "Accepted";
                    else if (r.status == -1) st = "Declined";
                    std::cout << "BookID: " << r.bookId
                        << " | To: " << r.toUser
                        << " | Status: " << st << std::endl;
                    found = true;
                }
            }
            if (!found) {
                std::cout << "No sent requests found.\n";
            }
            pauseScreen();
        }
        else if (choice == 4) {
            break; // Return to user menu
        }
        else {
            std::cout << "Invalid choice.\n";
            pauseScreen();
        }
    }
}

void sendExchangeRequestMenu(const std::string& currentUser) {
    clearScreen();
    std::cout << "===== Send Exchange Request =====\n";
    std::cout << "Enter search keyword (e.g., book title): ";
    std::string keyword;
    std::cin >> keyword;

    // Load all books and filter those matching keyword and not owned by currentUser
    auto allBooks = loadBooks();
    std::vector<Book> matchingBooks;
    for (const auto& b : allBooks) {
        if (b.title.find(keyword) != std::string::npos && b.owner != currentUser) {
            matchingBooks.push_back(b);
        }
    }

    if (matchingBooks.empty()) {
        std::cout << "No matching books found or you already own them.\n";
        pauseScreen();
        return;
    }

    std::cout << "Matching Books:\n";
    for (size_t i = 0; i < matchingBooks.size(); i++) {
        std::cout << "[" << i << "] "
            << "ID: " << matchingBooks[i].id
            << " | Title: " << matchingBooks[i].title
            << " | Owner: " << matchingBooks[i].owner << std::endl;
    }
    std::cout << "Select the index of the book to request: ";
    int sel;
    std::cin >> sel;
    if (sel < 0 || sel >= (int)matchingBooks.size()) {
        std::cout << "Invalid selection.\n";
        pauseScreen();
        return;
    }
    // Create exchange request: from currentUser to the book's owner, status = 0 (pending)
    ExchangeRequest req(matchingBooks[sel].id, currentUser, matchingBooks[sel].owner, 0);
    sendExchangeRequest(req);
    std::cout << "Exchange request sent to user " << matchingBooks[sel].owner << " for Book ID " << matchingBooks[sel].id << ".\n";
    pauseScreen();
}

// ---------------------------
// Transaction Class Implementations
// ---------------------------
Transaction::Transaction() : bookId(0), fromUser(""), toUser(""), date("") {}

Transaction::Transaction(int bookId, const std::string& fromUser, const std::string& toUser, const std::string& date)
    : bookId(bookId), fromUser(fromUser), toUser(toUser), date(date) {}

void Transaction::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(&bookId), sizeof(bookId));
    writeEncryptedString(os, fromUser);
    writeEncryptedString(os, toUser);
    writeEncryptedString(os, date);
}

void Transaction::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(&bookId), sizeof(bookId));
    fromUser = readEncryptedString(is);
    toUser = readEncryptedString(is);
    date = readEncryptedString(is);
}
