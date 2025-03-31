#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// Encryption key constant
const char ENCRYPTION_KEY = 0xAA;

// ---------------------------
// Class Definitions
// ---------------------------
class Book {
public:
    int id;
    std::string title;
    std::string author;
    std::string genre;
    std::string owner;

    Book();
    Book(int id, const std::string& title, const std::string& author, const std::string& genre, const std::string& owner);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};

class User {
public:
    std::string username;
    std::string password;
    float rating;
    int ratingCount;

    User();
    User(const std::string& username, const std::string& password);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};

class ExchangeRequest {
public:
    int bookId;
    std::string fromUser;
    std::string toUser;
    int status; // 0: pending, 1: accepted, -1: declined

    ExchangeRequest();
    ExchangeRequest(int bookId, const std::string& fromUser, const std::string& toUser, int status);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};

class Transaction {
public:
    int bookId;
    std::string fromUser;
    std::string toUser;
    std::string date;

    Transaction();
    Transaction(int bookId, const std::string& fromUser, const std::string& toUser, const std::string& date);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};

// ---------------------------
// Function Prototypes
// ---------------------------

// Encryption
std::string encryptString(const std::string& input);
std::string decryptString(const std::string& input);

// File operations
void addBook(const Book& book);
std::vector<Book> loadBooks();
void saveBooks(const std::vector<Book>& books);

// Auto add functions
void autoAddBooksThreadPerBook(int count);
void autoAddBooksParallelForImproved(int count, int numThreads = 4);

// User management
bool registerUser(const User& user);
bool loginUser(const std::string& username, const std::string& password);
std::vector<User> loadUsers();
void saveUsers(const std::vector<User>& users);

// Exchange request management
void sendExchangeRequest(const ExchangeRequest& req);
std::vector<ExchangeRequest> loadExchangeRequests();
void saveExchangeRequests(const std::vector<ExchangeRequest>& requests);

// Transaction management
void addTransaction(const Transaction& trans);
std::vector<Transaction> loadTransactions();
void saveTransactions(const std::vector<Transaction>& transactions);


void clearScreen();
void pauseScreen();
void showMainMenu();
void showUserMenu(const std::string& username);
void registerUserMenu();
bool loginUserMenu(std::string& loggedInUser);
void addBookManually(const std::string& currentUser);
void listAllBooks();
void searchBooksMenu();
void transactionHistoryMenu();
void deleteBooksMenu();
void heavyLoadListingTest();
void autoAddPerformanceTest();
void searchPerformanceTest();

void exchangeRequestsMenu(const std::string& currentUser);
void rateUserMenu(const std::string& currentUser);
void sendExchangeRequestMenu(const std::string& currentUser);

#endif // BOOK_H
