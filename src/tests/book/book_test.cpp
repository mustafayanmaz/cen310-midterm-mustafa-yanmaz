// book_test.cpp

#include "book.h"
#include <gtest/gtest.h>
#include <sstream>
#include <cstdio>    // for remove()
#include <ctime>

// Helper function: Format date/time using strftime (avoiding std::put_time)
std::string formatDateTime(std::time_t t) {
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}

// Helper function: Remove test files
void RemoveTestFiles() {
    std::remove("books.dat");
    std::remove("users.dat");
    std::remove("requests.dat");
    std::remove("transactions.dat");
}

// ==========================
// 1. Encryption Tests
// ==========================
TEST(EncryptionTest, BasicEncryption) {
    std::string original = "Hello, World!";
    std::string encrypted = encryptString(original);
    std::string decrypted = decryptString(encrypted);
    EXPECT_EQ(decrypted, original);
}

// ==========================
// 2. Serialization/Deserialization Tests
// ==========================

// --- Book ---
TEST(BookTest, SerializationDeserialization) {
    Book b1(1, "Test Book", "Test Author", "Test Genre", "TestUser");
    std::stringstream ss;
    b1.serialize(ss);
    Book b2;
    b2.deserialize(ss);
    EXPECT_EQ(b1.id, b2.id);
    EXPECT_EQ(b1.title, b2.title);
    EXPECT_EQ(b1.author, b2.author);
    EXPECT_EQ(b1.genre, b2.genre);
    EXPECT_EQ(b1.owner, b2.owner);
}

// --- User ---
TEST(UserTest, SerializationDeserialization) {
    User u1("TestUser", "TestPass");
    u1.rating = 4.5f;
    u1.ratingCount = 2;
    std::stringstream ss;
    u1.serialize(ss);
    User u2;
    u2.deserialize(ss);
    EXPECT_EQ(u1.username, u2.username);
    EXPECT_EQ(u1.password, u2.password);
    EXPECT_FLOAT_EQ(u1.rating, u2.rating);
    EXPECT_EQ(u1.ratingCount, u2.ratingCount);
}

// --- ExchangeRequest ---
TEST(ExchangeRequestTest, SerializationDeserialization) {
    ExchangeRequest req1(1, "UserA", "UserB", 0);
    std::stringstream ss;
    req1.serialize(ss);
    ExchangeRequest req2;
    req2.deserialize(ss);
    EXPECT_EQ(req1.bookId, req2.bookId);
    EXPECT_EQ(req1.fromUser, req2.fromUser);
    EXPECT_EQ(req1.toUser, req2.toUser);
    EXPECT_EQ(req1.status, req2.status);
}

// --- Transaction ---
TEST(TransactionTest, SerializationDeserialization) {
    Transaction t1(1, "UserA", "UserB", "2025-03-28 12:34:56");
    std::stringstream ss;
    t1.serialize(ss);
    Transaction t2;
    t2.deserialize(ss);
    EXPECT_EQ(t1.bookId, t2.bookId);
    EXPECT_EQ(t1.fromUser, t2.fromUser);
    EXPECT_EQ(t1.toUser, t2.toUser);
    EXPECT_EQ(t1.date, t2.date);
}

// ==========================
// 3. File Operation Tests
// ==========================
TEST(FileOpsTest, SaveLoadBooks) {
    RemoveTestFiles();
    std::vector<Book> books;
    books.push_back(Book(1, "Test Book", "Test Author", "Test Genre", "TestUser"));
    saveBooks(books);
    auto loaded = loadBooks();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].id, 1);
    EXPECT_EQ(loaded[0].title, "Test Book");
    RemoveTestFiles();
}

TEST(FileOpsTest, RegisterAndLoginUser) {
    RemoveTestFiles();
    User u("TestUser", "TestPass");
    EXPECT_TRUE(registerUser(u));          // First registration should succeed
    EXPECT_FALSE(registerUser(u));         // Duplicate registration should fail
    EXPECT_TRUE(loginUser("TestUser", "TestPass"));
    EXPECT_FALSE(loginUser("TestUser", "WrongPass"));
    RemoveTestFiles();
}

// Test for registerUserMenu() - Successful Registration
TEST(InteractiveTest, RegisterUserMenu_Success) {
    RemoveTestFiles();
    // Simulate input: new username, new password, and extra newline for pauseScreen.
    std::istringstream input("NewUser\nNewPass\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    registerUserMenu();

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    // Expect that the output contains a successful registration message.
    EXPECT_NE(outStr.find("Registration successful"), std::string::npos);
    RemoveTestFiles();
}

// Test for registerUserMenu() - Duplicate Registration (Failure)
TEST(InteractiveTest, RegisterUserMenu_Failure_Duplicate) {
    RemoveTestFiles();
    // First registration should succeed.
    {
        std::istringstream input("DupUser\nPass1\n\n");
        std::ostringstream output;
        auto oldCin = std::cin.rdbuf(input.rdbuf());
        auto oldCout = std::cout.rdbuf(output.rdbuf());

        registerUserMenu();

        std::cin.rdbuf(oldCin);
        std::cout.rdbuf(oldCout);
    }
    // Second registration with same username should fail.
    {
        std::istringstream input("DupUser\nPass1\n\n");
        std::ostringstream output;
        auto oldCin = std::cin.rdbuf(input.rdbuf());
        auto oldCout = std::cout.rdbuf(output.rdbuf());

        registerUserMenu();

        std::cin.rdbuf(oldCin);
        std::cout.rdbuf(oldCout);
        std::string outStr = output.str();
        EXPECT_NE(outStr.find("Registration failed"), std::string::npos);
    }
    RemoveTestFiles();
}

// Test for loginUserMenu() - Successful Login
TEST(InteractiveTest, LoginUserMenu_Success) {
    RemoveTestFiles();
    // Pre-register a user.
    User u("LoginUser", "LoginPass");
    registerUser(u);

    // Simulate input: correct username, correct password, and extra newline for pauseScreen.
    std::istringstream input("LoginUser\nLoginPass\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    std::string loggedIn;
    bool result = loginUserMenu(loggedIn);

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    EXPECT_TRUE(result);
    EXPECT_EQ(loggedIn, "LoginUser");
    RemoveTestFiles();
}

// Test for loginUserMenu() - Failed Login (Wrong Password)
TEST(InteractiveTest, LoginUserMenu_Failure) {
    RemoveTestFiles();
    // Pre-register a user.
    User u("LoginUser", "LoginPass");
    registerUser(u);

    // Simulate input: correct username, wrong password, and extra newline for pauseScreen.
    std::istringstream input("LoginUser\nWrongPass\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    std::string loggedIn;
    bool result = loginUserMenu(loggedIn);

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    EXPECT_FALSE(result);
    RemoveTestFiles();
}
TEST(FileOpsTest, SaveLoadExchangeRequests) {
    RemoveTestFiles();
    std::vector<ExchangeRequest> reqs;
    reqs.push_back(ExchangeRequest(1, "UserA", "UserB", 0));
    saveExchangeRequests(reqs);
    auto loaded = loadExchangeRequests();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].bookId, 1);
    EXPECT_EQ(loaded[0].fromUser, "UserA");
    RemoveTestFiles();
}

TEST(FileOpsTest, SaveLoadTransactions) {
    RemoveTestFiles();
    std::vector<Transaction> trans;
    trans.push_back(Transaction(1, "UserA", "UserB", "2025-03-28 12:34:56"));
    saveTransactions(trans);
    auto loaded = loadTransactions();
    ASSERT_EQ(loaded.size(), 1);
    EXPECT_EQ(loaded[0].date, "2025-03-28 12:34:56");
    RemoveTestFiles();
}


// ==========================
// 4. Auto Add Functions Tests
// ==========================
TEST(AutoAddTest, ThreadPerBook) {
    RemoveTestFiles();
    EXPECT_EQ(loadBooks().size(), 0);
    autoAddBooksThreadPerBook(5);
    EXPECT_EQ(loadBooks().size(), 5);
    RemoveTestFiles();
}

TEST(AutoAddTest, ParallelForImproved) {
    RemoveTestFiles();
    autoAddBooksParallelForImproved(10, 4);
    EXPECT_EQ(loadBooks().size(), 10);
    RemoveTestFiles();
}

// ==========================
// 5. Exchange Request & Transaction Tests
// ==========================
TEST(ExchangeTest, SendAndAcceptRequest) {
    RemoveTestFiles();
    // Send an exchange request: from UserA to UserB for BookID 1
    ExchangeRequest req(1, "UserA", "UserB", 0);
    sendExchangeRequest(req);
    auto reqs = loadExchangeRequests();
    ASSERT_EQ(reqs.size(), 1);
    EXPECT_EQ(reqs[0].status, 0);

    // Simulate accepting the request:
    reqs[0].status = 1;
    std::time_t t = std::time(nullptr);
    std::string dateStr = formatDateTime(t);
    Transaction trans(reqs[0].bookId, reqs[0].fromUser, reqs[0].toUser, dateStr);
    addTransaction(trans);
    saveExchangeRequests(reqs);
    auto transList = loadTransactions();
    EXPECT_EQ(transList.size(), 1);
    RemoveTestFiles();
}

// ==========================
// 6. Rate User Test
// ==========================
TEST(RateUserTest, RateUserCalculation) {
    RemoveTestFiles();
    User u("UserA", "password");
    std::vector<User> usersVec = { u };
    saveUsers(usersVec);
    auto loadedUsers = loadUsers();
    ASSERT_EQ(loadedUsers.size(), 1);
    int oldCount = loadedUsers[0].ratingCount;
    // Simulate rating: add 4.0 rating
    loadedUsers[0].rating = (loadedUsers[0].rating * loadedUsers[0].ratingCount + 4.0f) / (loadedUsers[0].ratingCount + 1);
    loadedUsers[0].ratingCount++;
    saveUsers(loadedUsers);
    auto newUsers = loadUsers();
    EXPECT_NEAR(newUsers[0].rating, 4.0f, 0.001);
    EXPECT_EQ(newUsers[0].ratingCount, oldCount + 1);
    RemoveTestFiles();
}

// ==========================
// 7. UI Output Tests (Non-interactive parts)
// ==========================
TEST(UIOutputTest, ShowMainMenuContainsPlatform) {
    std::streambuf* orig_buf = std::cout.rdbuf();
    std::ostringstream oss;
    std::cout.rdbuf(oss.rdbuf());
    showMainMenu();
    std::string output = oss.str();
    std::cout.rdbuf(orig_buf);
    EXPECT_NE(output.find("Book Exchange Platform"), std::string::npos);
}

TEST(UIOutputTest, ShowUserMenuContainsWelcome) {
    std::streambuf* orig_buf = std::cout.rdbuf();
    std::ostringstream oss;
    std::cout.rdbuf(oss.rdbuf());
    showUserMenu("TestUser");
    std::string output = oss.str();
    std::cout.rdbuf(orig_buf);
    EXPECT_NE(output.find("Welcome, TestUser"), std::string::npos);
}

// ==========================
// 8. Miscellaneous Tests: clearScreen & pauseScreen (Basic call test)
// ==========================
TEST(MiscTest, ClearAndPauseDoNotThrow) {
    EXPECT_NO_THROW(clearScreen());
    // pauseScreen() waits for user input, so it is not called here.
}


// This test spawns the bookapp executable, provides input "3\n" to exit,
// and verifies that the output contains "Exiting application.".
TEST(BookAppMainTest, ExitsOnChoice3) {
    RemoveTestFiles(); // Clean any prior test files

    // Create a temporary input file with the simulated user input.
    // In the main menu, when currentUser is empty, option 3 causes exit.
    const std::string tempInputFile = "temp_input.txt";
    {
        std::ofstream ofs(tempInputFile);
        ofs << "3\n"; // Choose option 3 to exit immediately.
    }

    // Build the command to run the bookapp executable with input redirection.
    // Adjust the executable name if needed (for Windows use "bookapp.exe").
#ifdef _WIN32
    std::string command = "bookapp.exe < " + tempInputFile;
#else
    std::string command = "./bookapp < " + tempInputFile;
#endif

    // Open a pipe to run the command.
#if defined(_WIN32)
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    ASSERT_NE(pipe, nullptr) << "Failed to open pipe to run the executable.";

    // Read all output from the executable.
    char buffer[128];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;}

    // Close the pipe and get exit status.
#if defined(_WIN32)
    int exitCode = _pclose(pipe);
#else
    int exitCode = pclose(pipe);
#endif

    // Check that the output contains "Exiting application."
    EXPECT_NE(output.find("Exiting application."), std::string::npos)
        << "Output did not contain expected exit message.";
    // Also, expect that exit code is 0.
    EXPECT_EQ(exitCode, 0);

    // Clean up the temporary input file.
    std::remove(tempInputFile.c_str());
    RemoveTestFiles();
}

// ==========================
// main() for tests
// ==========================
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    RemoveTestFiles(); // Clean start
    int ret = RUN_ALL_TESTS();
    RemoveTestFiles(); // Clean up after tests
    return ret;
}