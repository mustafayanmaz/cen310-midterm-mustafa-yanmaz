/**
 * @brief Include guard for the book header file.
 */
#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>


 /**
  * @brief Encryption key constant used for XOR encryption.
  */
const char ENCRYPTION_KEY = 0xAA;

// ---------------------------
// Class Definitions
// ---------------------------

/**
 * @brief Represents a book in the exchange platform.
 */
class Book {
public:
    /**
     * @brief The unique identifier of the book.
     */
    int id;

    /**
     * @brief The title of the book.
     */
    std::string title;

    /**
     * @brief The author of the book.
     */
    std::string author;

    /**
     * @brief The genre of the book.
     */
    std::string genre;

    /**
     * @brief The owner of the book.
     */
    std::string owner;

    Book();
    Book(int id, const std::string& title, const std::string& author, const std::string& genre, const std::string& owner);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};


/**
 * @brief Represents a user in the exchange platform.
 */
class User {
public:
    /**
     * @brief The username of the user.
     */
    std::string username;

    /**
     * @brief The password of the user.
     */
    std::string password;

    /**
     * @brief The average rating of the user.
     */
    float rating;

    /**
     * @brief The number of ratings received by the user.
     */
    int ratingCount;

    User();
    User(const std::string& username, const std::string& password);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};

/**
 * @brief Represents an exchange request for a book.
 */
class ExchangeRequest {
public:
    /**
     * @brief The identifier of the book involved in the exchange.
     */
    int bookId;

    /**
     * @brief The username of the user initiating the exchange.
     */
    std::string fromUser;

    /**
     * @brief Username of the user receiving the book.
     */
    std::string toUser;

    /**
     * @brief Date of the transaction.
     */
    int status; // 0: pending, 1: accepted, -1: declined

    ExchangeRequest();
    ExchangeRequest(int bookId, const std::string& fromUser, const std::string& toUser, int status);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);
};

/**
 * @brief Class representing a transaction record for a book exchange.
 */
class Transaction {
public:
    /**
     * @brief Identifier of the book involved in the transaction.
     */
    int bookId;

    /**
     * @brief Username of the user sending the book.
     */
    std::string fromUser;

    /**
     * @brief Username of the user receiving the book.
     */
    std::string toUser;

    /**
     * @brief Date of the transaction.
     */
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

void bookSimilarityMatrixTestMPI(const std::vector<Book>& books);
void matrixMultiplicationTestMPI(int size);
void bookTrigramSimilarityTestMPI(const std::vector<Book>& books);
void bookExchangeShortestPathTestMPI();
void exchangeRequestsMenu(const std::string& currentUser);
void rateUserMenu(const std::string& currentUser);
void sendExchangeRequestMenu(const std::string& currentUser);

#endif // BOOK_H
