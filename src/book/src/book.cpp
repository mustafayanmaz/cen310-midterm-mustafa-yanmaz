/**
 * @brief Includes the header file for the book exchange platform declarations.
 */
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
/**
 * @brief Mutex for synchronizing access to the books collection.
 */
std::mutex booksMutex;

/**
 * @brief Mutex for synchronizing access to the users collection.
 */
std::mutex usersMutex;

/**
 * @brief Mutex for synchronizing access to the exchange requests.
 */
std::mutex requestsMutex;

/**
 * @brief Mutex for synchronizing access to the transactions.
 */
std::mutex transactionsMutex;

// ---------------------------
// Helper Functions for Encrypted I/O
// ---------------------------
/**
 * @brief Writes an encrypted version of the given string to the output stream.
 * @param os Output stream where the encrypted string is written.
 * @param str The string to encrypt and write.
 */
void writeEncryptedString(std::ostream& os, const std::string& str) {
    size_t len = str.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    std::string enc = encryptString(str);
    os.write(enc.c_str(), len);
}

/**
 * @brief Reads an encrypted string from the input stream and returns the decrypted string.
 * @param is Input stream from which the encrypted string is read.
 * @return The decrypted string.
 */
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
/**
 * @brief Encrypts a string using XOR encryption.
 * @param input The input string to encrypt.
 * @return The encrypted string.
 */
std::string encryptString(const std::string& input) {
    std::string output = input;
    for (char& c : output)
        c ^= ENCRYPTION_KEY;
    return output;
}

/**
 * @brief Decrypts a string that was encrypted using XOR encryption.
 * @param input The encrypted string to decrypt.
 * @return The decrypted string.
 */
std::string decryptString(const std::string& input) {
    return encryptString(input); // XOR symmetric
}

// ---------------------------
// Book Class Implementations
// ---------------------------
/**
 * @brief Default constructor for the Book class.
 */
Book::Book() : id(0), title(""), author(""), genre(""), owner("") {}

/**
 * @brief Parameterized constructor for the Book class.
 * @param id The book identifier.
 * @param title The title of the book.
 * @param author The author of the book.
 * @param genre The genre of the book.
 * @param owner The owner of the book.
 */
Book::Book(int id, const std::string& title, const std::string& author, const std::string& genre, const std::string& owner)
    : id(id), title(title), author(author), genre(genre), owner(owner) {}

/**
 * @brief Clears the console screen.
 */
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * @brief Pauses the screen until user input.
 */
void pauseScreen() {
    std::cout << "Press enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

/**
 * @brief Displays the main menu.
 */
void showMainMenu() {
    std::cout << "=============================" << std::endl;
    std::cout << "   Book Exchange Platform    " << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "1. Register" << std::endl;
    std::cout << "2. Login" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

/**
 * @brief Displays the user menu.
 * @param username The name of the logged in user.
 */
void showUserMenu(const std::string& username) {
    std::cout << "=============================" << std::endl;
    std::cout << "Welcome, " << username << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "1. Add Book Manually" << std::endl;
    std::cout << "2. Auto Add Books (Thread per Book)" << std::endl;
    std::cout << "3. Auto Add Books (Improved Single Lock)" << std::endl;
    std::cout << "4. List All Books" << std::endl;
    std::cout << "5. Search Books" << std::endl;
    std::cout << "6. Exchange Requests" << std::endl;
    std::cout << "7. Rate User" << std::endl;
    std::cout << "8. Transaction History" << std::endl;
    std::cout << "9. Delete Books" << std::endl;
    std::cout << "10. Heavy Load Listing Performance Test" << std::endl;
    std::cout << "11. Auto Add Performance Test" << std::endl;
    std::cout << "12. Search Performance Test" << std::endl;
    std::cout << "13. Logout" << std::endl;
    std::cout << "Enter your choice: ";
}

/**
 * @brief Displays the user registration menu and handles user registration.
 */
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

/**
 * @brief Displays the login menu and handles user login.
 * @param loggedInUser Reference to a string to store the logged in user's name.
 * @return True if login was successful, false otherwise.
 */
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

/**
 * @brief Serializes the Book object to a binary output stream.
 * @param os Output stream where the book data is written.
 */
void Book::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(&id), sizeof(id));
    writeEncryptedString(os, title);
    writeEncryptedString(os, author);
    writeEncryptedString(os, genre);
    writeEncryptedString(os, owner);
}

/**
 * @brief Deserializes the Book object from a binary input stream.
 * @param is Input stream from which the book data is read.
 */
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
/**
 * @brief Default constructor for the User class.
 */
User::User() : username(""), password(""), rating(0.0f), ratingCount(0) {}

/**
 * @brief Parameterized constructor for the User class.
 * @param username The username for the user.
 * @param password The password for the user.
 */
User::User(const std::string& username, const std::string& password)
    : username(username), password(password), rating(0.0f), ratingCount(0) {}

/**
 * @brief Serializes the User object to a binary output stream.
 * @param os Output stream where the user data is written.
 */
void User::serialize(std::ostream& os) const {
    writeEncryptedString(os, username);
    writeEncryptedString(os, password);
    os.write(reinterpret_cast<const char*>(&rating), sizeof(rating));
    os.write(reinterpret_cast<const char*>(&ratingCount), sizeof(ratingCount));
}

/**
 * @brief Deserializes the User object from a binary input stream.
 * @param is Input stream from which the user data is read.
 */
void User::deserialize(std::istream& is) {
    username = readEncryptedString(is);
    password = readEncryptedString(is);
    is.read(reinterpret_cast<char*>(&rating), sizeof(rating));
    is.read(reinterpret_cast<char*>(&ratingCount), sizeof(ratingCount));
}

// ---------------------------
// ExchangeRequest Class Implementations
// ---------------------------
/**
 * @brief Default constructor for the ExchangeRequest class.
 */
ExchangeRequest::ExchangeRequest() : bookId(0), fromUser(""), toUser(""), status(0) {}

/**
 * @brief Parameterized constructor for the ExchangeRequest class.
 * @param bookId The ID of the book involved in the exchange.
 * @param fromUser The username of the user initiating the exchange.
 * @param toUser The username of the recipient user.
 * @param status The status of the exchange request.
 */
ExchangeRequest::ExchangeRequest(int bookId, const std::string& fromUser, const std::string& toUser, int status)
    : bookId(bookId), fromUser(fromUser), toUser(toUser), status(status) {}

/**
 * @brief Serializes the ExchangeRequest object to a binary output stream.
 * @param os Output stream where the exchange request data is written.
 */
void ExchangeRequest::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(&bookId), sizeof(bookId));
    writeEncryptedString(os, fromUser);
    writeEncryptedString(os, toUser);
    os.write(reinterpret_cast<const char*>(&status), sizeof(status));
}

/**
 * @brief Deserializes the ExchangeRequest object from a binary input stream.
 * @param is Input stream from which the exchange request data is read.
 */
void ExchangeRequest::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(&bookId), sizeof(bookId));
    fromUser = readEncryptedString(is);
    toUser = readEncryptedString(is);
    is.read(reinterpret_cast<char*>(&status), sizeof(status));
}

/**
 * @brief Displays and handles the exchange requests menu.
 * @param currentUser The username of the current user.
 */
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
                std::cout << "No pending requests to process.\n";}
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
            break;}
        else {
            std::cout << "Invalid choice.\n";
            pauseScreen();
        }
    }
}

/**
 * @brief Displays the send exchange request menu and processes the request.
 * @param currentUser The username of the current user.
 */
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
/**
 * @brief Default constructor for the Transaction class.
 */
Transaction::Transaction() : bookId(0), fromUser(""), toUser(""), date("") {}

/**
 * @brief Parameterized constructor for the Transaction class.
 * @param bookId The ID of the book involved in the transaction.
 * @param fromUser The username of the user sending the book.
 * @param toUser The username of the user receiving the book.
 * @param date The date of the transaction.
 */
Transaction::Transaction(int bookId, const std::string& fromUser, const std::string& toUser, const std::string& date)
    : bookId(bookId), fromUser(fromUser), toUser(toUser), date(date) {}

/**
 * @brief Serializes the Transaction object to a binary output stream.
 * @param os Output stream where the transaction data is written.
 */
void Transaction::serialize(std::ostream& os) const {
    os.write(reinterpret_cast<const char*>(&bookId), sizeof(bookId));
    writeEncryptedString(os, fromUser);
    writeEncryptedString(os, toUser);
    writeEncryptedString(os, date);
}

/**
 * @brief Deserializes the Transaction object from a binary input stream.
 * @param is Input stream from which the transaction data is read.
 */
void Transaction::deserialize(std::istream& is) {
    is.read(reinterpret_cast<char*>(&bookId), sizeof(bookId));
    fromUser = readEncryptedString(is);
    toUser = readEncryptedString(is);
    date = readEncryptedString(is);
}

// ---------------------------
// File Paths
// ---------------------------
/**
 * @brief File path for the books data file.
 */
const std::string BOOKS_FILE = "books.dat";

/**
 * @brief File path for the users data file.
 */
const std::string USERS_FILE = "users.dat";

/**
 * @brief File path for the exchange requests data file.
 */
const std::string REQUESTS_FILE = "requests.dat";

/**
 * @brief File path for the transactions data file.
 */
const std::string TRANSACTIONS_FILE = "transactions.dat";

// ---------------------------
// File Operation Functions
// ---------------------------
/**
 * @brief Adds a new book to the books file.
 * @param book The Book object to add.
 */
void addBook(const Book& book) {
    std::lock_guard<std::mutex> lock(booksMutex);
    std::vector<Book> books = loadBooks();
    books.push_back(book);
    saveBooks(books);
}

/**
 * @brief Loads all books from the books file.
 * @return A vector containing all the Book objects.
 */
std::vector<Book> loadBooks() {
    std::vector<Book> books;
    std::ifstream ifs(BOOKS_FILE, std::ios::binary);
    if (!ifs) return books;
    while (ifs.peek() != EOF) {
        Book b;
        b.deserialize(ifs);
        if (ifs.fail()) break;
        books.push_back(b);
    }
    ifs.close();
    return books;
}

/**
 * @brief Saves the list of books to the books file.
 * @param books The vector of Book objects to save.
 */
void saveBooks(const std::vector<Book>& books) {
    std::ofstream ofs(BOOKS_FILE, std::ios::binary | std::ios::trunc);
    for (const Book& b : books)
        b.serialize(ofs);
    ofs.close();
}

// ---------------------------
// Auto Add Functions
// ---------------------------
/**
 * @brief Automatically adds books using a thread per book.
 * @param count The number of books to add.
 */
void autoAddBooksThreadPerBook(int count) {
    std::vector<Book> oldBooks = loadBooks();
    int nextId = oldBooks.empty() ? 1 : oldBooks.back().id + 1;
    auto addTask = [nextId](int index) {
        int bookId = nextId + index;
        std::stringstream ss;
        ss << "Auto Book " << bookId;
        Book b(bookId, ss.str(), "Author " + std::to_string(bookId),
            "Genre " + std::to_string(bookId % 5), "AutoUser");
        addBook(b);
        };
    std::vector<std::thread> threads;
    threads.reserve(count);
    for (int i = 0; i < count; ++i)
        threads.emplace_back(addTask, i);
    for (auto& t : threads)
        t.join();
    std::cout << "Auto addition of " << count << " books (thread per book) completed." << std::endl;
}

/**
 * @brief Automatically adds books using an improved parallel for loop.
 * @param count The number of books to add.
 * @param numThreads The number of threads to use for parallel addition.
 */
void autoAddBooksParallelForImproved(int count, int numThreads) {
    std::vector<Book> oldBooks = loadBooks();
    int nextId = oldBooks.empty() ? 1 : oldBooks.back().id + 1;
    std::vector<Book> newBooks(count);
#ifdef _OPENMP
#pragma omp parallel for num_threads(numThreads)
    for (int i = 0; i < count; i++) {
        int bookId = nextId + i;
        std::stringstream ss;
        ss << "Auto Book " << bookId;
        newBooks[i] = Book(bookId, ss.str(), "Author " + std::to_string(bookId),
            "Genre " + std::to_string(bookId % 5), "AutoUser");
    }
#else
    for (int i = 0; i < count; i++) {
        int bookId = nextId + i;
        newBooks[i] = Book(bookId, "Auto Book " + std::to_string(bookId),
            "Author " + std::to_string(bookId),
            "Genre " + std::to_string(bookId % 5), "AutoUser");
    }
#endif
    {
        std::lock_guard<std::mutex> lock(booksMutex);
        oldBooks.insert(oldBooks.end(), newBooks.begin(), newBooks.end());
        saveBooks(oldBooks);
    }
    std::cout << "Auto addition of " << count << " books (improved parallel for, "
        << numThreads << " threads) completed." << std::endl;
}

// ---------------------------
// User Management Functions
// ---------------------------
/**
 * @brief Registers a new user.
 * @param user The User object to register.
 * @return True if registration is successful, false if the username already exists.
 */
bool registerUser(const User& user) {
    std::lock_guard<std::mutex> lock(usersMutex);
    std::vector<User> users = loadUsers();
    for (const User& u : users)
        if (u.username == user.username)
            return false;
    users.push_back(user);
    saveUsers(users);
    return true;
}

/**
 * @brief Authenticates a user.
 * @param username The username of the user.
 * @param password The password of the user.
 * @return True if the login is successful, false otherwise.
 */
bool loginUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(usersMutex);
    std::vector<User> users = loadUsers();
    for (const User& u : users)
        if (u.username == username && u.password == password)
            return true;
    return false;
}

/**
 * @brief Loads all users from the users file.
 * @return A vector containing all the User objects.
 */
std::vector<User> loadUsers() {
    std::vector<User> users;
    std::ifstream ifs(USERS_FILE, std::ios::binary);
    if (!ifs) {
        std::ofstream ofs(USERS_FILE, std::ios::binary);
        ofs.close();
        return users;
    }
    while (ifs.peek() != EOF) {
        User u;
        u.deserialize(ifs);
        if (ifs.fail()) break;
        users.push_back(u);
    }
    ifs.close();
    return users;
}

/**
 * @brief Saves the list of users to the users file.
 * @param users The vector of User objects to save.
 */
void saveUsers(const std::vector<User>& users) {
    std::ofstream ofs(USERS_FILE, std::ios::binary | std::ios::trunc);
    for (const User& u : users)
        u.serialize(ofs);
    ofs.close();
}

// ---------------------------
// Exchange Request Functions
// ---------------------------
/**
 * @brief Sends an exchange request.
 * @param req The ExchangeRequest object to send.
 */
void sendExchangeRequest(const ExchangeRequest& req) {
    std::lock_guard<std::mutex> lock(requestsMutex);
    std::vector<ExchangeRequest> reqs = loadExchangeRequests();
    reqs.push_back(req);
    saveExchangeRequests(reqs);
}

/**
 * @brief Loads all exchange requests from the requests file.
 * @return A vector containing all the ExchangeRequest objects.
 */
std::vector<ExchangeRequest> loadExchangeRequests() {
    std::vector<ExchangeRequest> reqs;
    std::ifstream ifs(REQUESTS_FILE, std::ios::binary);
    if (!ifs) return reqs;
    while (ifs.peek() != EOF) {
        ExchangeRequest r;
        r.deserialize(ifs);
        if (ifs.fail()) break;
        reqs.push_back(r);
    }
    ifs.close();
    return reqs;
}

/**
 * @brief Saves the list of exchange requests to the requests file.
 * @param reqs The vector of ExchangeRequest objects to save.
 */
void saveExchangeRequests(const std::vector<ExchangeRequest>& reqs) {
    std::ofstream ofs(REQUESTS_FILE, std::ios::binary | std::ios::trunc);
    for (const ExchangeRequest& r : reqs)
        r.serialize(ofs);
    ofs.close();
}

// ---------------------------
// Transaction Functions
// ---------------------------
/**
 * @brief Adds a new transaction to the transactions file.
 * @param trans The Transaction object to add.
 */
void addTransaction(const Transaction& trans) {
    std::lock_guard<std::mutex> lock(transactionsMutex);
    std::vector<Transaction> transList = loadTransactions();
    transList.push_back(trans);
    saveTransactions(transList);
}

/**
 * @brief Loads all transactions from the transactions file.
 * @return A vector containing all the Transaction objects.
 */
std::vector<Transaction> loadTransactions() {
    std::vector<Transaction> transList;
    std::ifstream ifs(TRANSACTIONS_FILE, std::ios::binary);
    if (!ifs) return transList;
    while (ifs.peek() != EOF) {
        Transaction t;
        t.deserialize(ifs);
        if (ifs.fail()) break;
        transList.push_back(t);
    }
    ifs.close();
    return transList;
}

/**
 * @brief Saves the list of transactions to the transactions file.
 * @param transList The vector of Transaction objects to save.
 */
void saveTransactions(const std::vector<Transaction>& transList) {
    std::ofstream ofs(TRANSACTIONS_FILE, std::ios::binary | std::ios::trunc);
    for (const Transaction& t : transList)
        t.serialize(ofs);
    ofs.close();
}

/**
 * @brief Adds a book manually by prompting the user for details.
 * @param currentUser The username of the current user.
 */
void addBookManually(const std::string& currentUser) {
    std::string title, author, genre;
    std::cout << "Enter book title: ";
    std::getline(std::cin, title);
    std::cout << "Enter book author: ";
    std::getline(std::cin, author);
    std::cout << "Enter book genre: ";
    std::getline(std::cin, genre);
    std::vector<Book> books = loadBooks();
    int nextId = books.empty() ? 1 : books.back().id + 1;
    Book b(nextId, title, author, genre, currentUser);
    addBook(b);
    std::cout << "Book added successfully." << std::endl;
    pauseScreen();
}

/**
 * @brief Lists all books available in the system.
 */
void listAllBooks() {
    std::vector<Book> books = loadBooks();
    if (books.empty())
        std::cout << "No books found." << std::endl;
    else {
        std::cout << "Book List:" << std::endl;
        for (const Book& b : books) {
            std::cout << "ID: " << b.id
                << " | Title: " << b.title
                << " | Author: " << b.author
                << " | Genre: " << b.genre
                << " | Owner: " << b.owner << std::endl;
        }
    }
    pauseScreen();
}

/**
 * @brief Displays the search menu and performs book search.
 */
void searchBooksMenu() {
    std::cout << "Search by:" << std::endl;
    std::cout << "1. Title" << std::endl;
    std::cout << "2. Author" << std::endl;
    std::cout << "3. Genre" << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;
    std::cout << "Enter search keyword: ";
    std::string keyword;
    std::cin.ignore();
    std::getline(std::cin, keyword);
    std::vector<Book> books = loadBooks();
    bool found = false;
    for (const Book& b : books) {
        bool match = false;
        if (choice == 1 && b.title.find(keyword) != std::string::npos)
            match = true;
        else if (choice == 2 && b.author.find(keyword) != std::string::npos)
            match = true;
        else if (choice == 3 && b.genre.find(keyword) != std::string::npos)
            match = true;
        if (match) {
            std::cout << "ID: " << b.id
                << " | Title: " << b.title
                << " | Author: " << b.author
                << " | Genre: " << b.genre
                << " | Owner: " << b.owner << std::endl;
            found = true;
        }
    }
    if (!found)
        std::cout << "No matching books found." << std::endl;
    pauseScreen();
}

/**
 * @brief Displays the transaction history.
 */
void transactionHistoryMenu() {
    std::vector<Transaction> transactions = loadTransactions();
    if (transactions.empty())
        std::cout << "No transactions found." << std::endl;
    else {
        std::cout << "Transaction History:" << std::endl;
        for (const Transaction& t : transactions) {
            std::cout << "Book ID: " << t.bookId
                << " | From: " << t.fromUser
                << " | To: " << t.toUser
                << " | Date: " << t.date << std::endl;
        }
    }
    pauseScreen();
}

/**
 * @brief Displays the delete books menu and processes book deletion.
 */
void deleteBooksMenu() {
    std::cout << "Delete Books Menu:" << std::endl;
    std::cout << "1. Delete Single Book" << std::endl;
    std::cout << "2. Delete Multiple Books" << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;
    if (choice == 1) {
        std::vector<Book> books = loadBooks();
        if (books.empty()) {
            std::cout << "No books to delete." << std::endl;
            pauseScreen();
            return;
        }
        std::cout << "Book List:" << std::endl;
        for (const Book& b : books)
            std::cout << "ID: " << b.id << " | Title: " << b.title << std::endl;
        int id;
        std::cout << "Enter the ID of the book to delete: ";
        std::cin >> id;
        bool found = false;
        std::vector<Book> newBooks;
        for (const Book& b : books) {
            if (b.id == id)
                found = true;
            else
                newBooks.push_back(b);
        }
        if (found)
            std::cout << "Book with ID " << id << " deleted." << std::endl;
        else
            std::cout << "Book with ID " << id << " not found." << std::endl;
        saveBooks(newBooks);
    }
    else if (choice == 2) {
        int low, high;
        std::cout << "Enter the lower bound ID: ";
        std::cin >> low;
        std::cout << "Enter the upper bound ID: ";
        std::cin >> high;
        std::vector<Book> books = loadBooks();
        if (books.empty()) {
            std::cout << "No books to delete." << std::endl;
            pauseScreen();
            return;
        }
        int count = 0;
        std::vector<Book> newBooks;
        for (const Book& b : books) {
            if (b.id >= low && b.id <= high)
                count++;
            else
                newBooks.push_back(b);
        }
        if (count > 0)
            std::cout << count << " books deleted in the range [" << low << ", " << high << "]." << std::endl;
        else
            std::cout << "No books found in the given range." << std::endl;
        saveBooks(newBooks);
    }
    else {
        std::cout << "Invalid choice." << std::endl;
    }
    pauseScreen();
}

/**
 * @brief Performs a heavy load test for listing books with calculations.
 */
void heavyLoadListingTest() {
    std::vector<Book> books = loadBooks();
    int n = books.size();
    if (n == 0) {
        std::cout << "No books found for heavy load test." << std::endl;
        pauseScreen();
        return;
    }

    std::vector<std::string> seqOutputs(n);
    double seqStart = omp_get_wtime();
    for (int i = 0; i < n; i++) {
        double result = 0.0;
        for (int j = 1; j <= 100000; j++) {
            result += std::log(j + 1) * std::sqrt(books[i].id + 1);
        }
        seqOutputs[i] = "ID: " + std::to_string(books[i].id) + " | Title: " +
            books[i].title + " | Calc: " + std::to_string(result);
    }
    double seqEnd = omp_get_wtime();
    double seqTime = (seqEnd - seqStart) * 1000;

    std::vector<std::string> parOutputs(n);
    double parStart = omp_get_wtime();
#ifdef _OPENMP
#pragma omp parallel for num_threads(omp_get_max_threads()) schedule(static)
    for (int i = 0; i < n; i++) {
        double result = 0.0;
        for (int j = 1; j <= 100000; j++) {
            result += std::log(j + 1) * std::sqrt(books[i].id + 1);
        }
        parOutputs[i] = "ID: " + std::to_string(books[i].id) + " | Title: " +
            books[i].title + " | Calc: " + std::to_string(result);
    }
#else
    for (int i = 0; i < n; i++) {
        double result = 0.0;
        for (int j = 1; j <= 100000; j++) {
            result += std::log(j + 1) * std::sqrt(books[i].id + 1);
        }
        parOutputs[i] = "ID: " + std::to_string(books[i].id) + " | Title: " +
            books[i].title + " | Calc: " + std::to_string(result);
    }
#endif
    double parEnd = omp_get_wtime();
    double parTime = (parEnd - parStart) * 1000;

    std::cout << "Heavy Load Book Listing Performance:" << std::endl;
    std::cout << "Books: " << n << std::endl;
    std::cout << "Sequential: " << std::fixed << std::setprecision(4) << seqTime << " ms" << std::endl;
    std::cout << "Parallel:   " << std::fixed << std::setprecision(4) << parTime << " ms" << std::endl;
    double ratio = (parTime == 0.0) ? 1.0 : (seqTime / parTime);
    std::cout << "Performance improvement: " << std::fixed << std::setprecision(2) << ratio << "x" << std::endl;
    pauseScreen();
}

/**
 * @brief Tests the performance of auto-adding books with different thread counts.
 */
void autoAddPerformanceTest() {
    int testCount = 1000;
    std::vector<int> threadCounts = { 2, 4, 8, 16 };
    std::ofstream csv("auto_add_performance.csv");
    csv << "ThreadCount,Time_ms\n";
    for (int threads : threadCounts) {
        auto start = std::chrono::steady_clock::now();
        autoAddBooksParallelForImproved(testCount, threads);
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << "Threads: " << threads << " | Time: " << std::fixed << std::setprecision(4) << elapsed << " ms" << std::endl;
        csv << threads << "," << elapsed << "\n";
    }
    csv.close();
    std::cout << "Auto add performance results saved to auto_add_performance.csv" << std::endl;
    pauseScreen();
}

/**
 * @brief Tests the performance of searching books.
 */
void searchPerformanceTest() {
    std::vector<Book> books = loadBooks();
    std::string keyword = "Auto";

    // Sequential
    std::vector<Book> resultsSeq;
    auto seqStart = std::chrono::steady_clock::now();
    for (const Book& b : books) {
        if (b.title.find(keyword) != std::string::npos ||
            b.author.find(keyword) != std::string::npos ||
            b.genre.find(keyword) != std::string::npos)
        {
            resultsSeq.push_back(b);
        }
    }
    auto seqEnd = std::chrono::steady_clock::now();
    double seqTime = std::chrono::duration<double, std::milli>(seqEnd - seqStart).count();

    // Parallel
    std::vector<Book> resultsPar;
    auto parStart = std::chrono::steady_clock::now();
#ifdef _OPENMP
    int threadCount = omp_get_max_threads();
    std::vector<std::vector<Book>> localResults(threadCount);

#pragma omp parallel for
    for (int i = 0; i < (int)books.size(); i++) {
        int tid = omp_get_thread_num();
        if (books[i].title.find(keyword) != std::string::npos ||
            books[i].author.find(keyword) != std::string::npos ||
            books[i].genre.find(keyword) != std::string::npos)
        {
            localResults[tid].push_back(books[i]);
        }
    }

    for (int i = 0; i < threadCount; ++i) {
        resultsPar.insert(resultsPar.end(), localResults[i].begin(), localResults[i].end());
    }
#else
    resultsPar = resultsSeq; // fallback
#endif

    auto parEnd = std::chrono::steady_clock::now();
    double parTime = std::chrono::duration<double, std::milli>(parEnd - parStart).count();

    // Output
    std::cout << "Search Performance Test with keyword: \"" << keyword << "\"" << std::endl;
    std::cout << "Sequential: " << std::fixed << std::setprecision(4) << seqTime << " ms, Results: " << resultsSeq.size() << std::endl;
    std::cout << "Parallel:   " << std::fixed << std::setprecision(4) << parTime << " ms, Results: " << resultsPar.size() << std::endl;

    std::ofstream csv("search_performance.csv");
    csv << "Mode,Time_ms,ResultsCount\n";
    csv << "Sequential," << seqTime << "," << resultsSeq.size() << "\n";
    csv << "Parallel," << parTime << "," << resultsPar.size() << "\n";
    csv.close();

    std::cout << "Search performance results saved to search_performance.csv" << std::endl;
    pauseScreen();
}

/**
 * @brief Displays the rate user menu and processes user rating.
 * @param currentUser The username of the current user.
 */
void rateUserMenu(const std::string& currentUser) {
    clearScreen();
    std::cout << "===== Rate User Menu =====\n";
    std::cout << "Enter username to rate: ";
    std::string userToRate;
    std::cin >> userToRate;
    std::cout << "Enter rating (0.0 - 5.0): ";
    float ratingVal;
    std::cin >> ratingVal;
    if (ratingVal < 0.0f) ratingVal = 0.0f;
    if (ratingVal > 5.0f) ratingVal = 5.0f;

    std::vector<User> allUsers = loadUsers();
    bool found = false;
    for (auto& u : allUsers) {
        if (u.username == userToRate) {
            // rating = (rating * ratingCount + newRating) / (ratingCount + 1)
            u.rating = (u.rating * u.ratingCount + ratingVal) / (u.ratingCount + 1);
            u.ratingCount++;
            found = true;
            break;}}
    if (found) {
        saveUsers(allUsers);
        std::cout << "User rated successfully.\n";
    }
    else {
        std::cout << "User not found.\n";
    }
    pauseScreen();
}
