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

//=================================================================
// 5. Interactive Function Tests (Simulated Input/Output)
//=================================================================

// For interactive functions, we redirect std::cin and std::cout.
// To avoid hangs, we provide sufficient newline characters for each pauseScreen call.

// Test registerUserMenu by simulating input.
TEST(InteractiveTest, RegisterUserMenu) {
    RemoveTestFiles();
    // Input: username, password, and extra newline for pauseScreen.
    std::istringstream input("NewUser\nNewPass\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    registerUserMenu();
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    EXPECT_NE(outStr.find("Registration successful"), std::string::npos);
}

// Test loginUserMenu by simulating input.
TEST(InteractiveTest, LoginUserMenu) {
    RemoveTestFiles();
    // Pre-register a user.
    User u("NewUser", "NewPass");
    registerUser(u);
    // Input: username, password, and extra newline for pauseScreen.
    std::istringstream input("NewUser\nNewPass\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    std::string loggedIn;
    bool res = loginUserMenu(loggedIn);
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    EXPECT_TRUE(res);
    EXPECT_EQ(loggedIn, "NewUser");
}

// Test exchangeRequestsMenu by simulating immediate exit (input "4" for exit).
TEST(InteractiveTest, ExchangeRequestsMenu) {
    // Input "4\n" to exit immediately.
    std::istringstream input("4\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    exchangeRequestsMenu("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    EXPECT_NE(outStr.find("Exchange Requests Menu"), std::string::npos);
}

// ExchangeRequestsMenu tests for all branches

// Test branch 0: "Send Exchange Request"
TEST(InteractiveTest, ExchangeRequestsMenu_Branch0_Send) {
    RemoveTestFiles();
    // Pre-add a book that is owned by someone else so it is found by search.
    addBook(Book(1, "TestBook", "TestAuthor", "TestGenre", "OtherUser"));
    // Simulate input sequence:
    //  - First, choose option "0" to send an exchange request.
    //  - Then, in sendExchangeRequestMenu, enter keyword "Test" to match the book,
    //    choose index "0" (the first matching book),
    //    and supply enough newlines for pauseScreen.
    //  - Finally, choose "4" to exit the loop.
    std::istringstream input("0\nTest\n0\n\n4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    exchangeRequestsMenu("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    // Check that the send branch was executed and output indicates that an exchange request was sent.
    EXPECT_NE(outStr.find("Exchange request sent"), std::string::npos);
    RemoveTestFiles();
}

// Test branch 1: "View Received (Pending) Requests"
TEST(InteractiveTest, ExchangeRequestsMenu_Branch1_ViewReceived) {
    RemoveTestFiles();
    // Pre-add a pending exchange request for current user "TestUser".
    ExchangeRequest req(2, "UserX", "TestUser", 0);
    sendExchangeRequest(req);
    // Simulate input:
    //  - Choose option "1" to view received requests.
    //  - Provide newline for pauseScreen.
    //  - Then choose "4" to exit.
    std::istringstream input("1\n\n4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    exchangeRequestsMenu("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    // Check that the output contains details of the pending request.
    EXPECT_NE(outStr.find("BookID: 2"), std::string::npos);
    EXPECT_NE(outStr.find("From: UserX"), std::string::npos);
    RemoveTestFiles();
}

// Test branch 2: "Accept a Pending Request"
TEST(InteractiveTest, ExchangeRequestsMenu_Branch2_Accept) {
    RemoveTestFiles();
    // Pre-add a pending exchange request for current user.
    ExchangeRequest req(3, "UserY", "TestUser", 0);
    sendExchangeRequest(req);
    // Simulate input:
    //  - Choose option "2" for accept/decline.
    //  - Then, input "0" to select the first (and only) pending request.
    //  - Then input "1" to accept it.
    //  - Provide newline for pauseScreen.
    //  - Finally, input "4" to exit.
    std::istringstream input("2\n0\n1\n\n4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    exchangeRequestsMenu("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    // Check that the output indicates acceptance.
    EXPECT_NE(outStr.find("Request accepted"), std::string::npos);
    RemoveTestFiles();
}

// Test branch 2: "Decline a Pending Request"
TEST(InteractiveTest, ExchangeRequestsMenu_Branch2_Decline) {
    RemoveTestFiles();
    // Pre-add a pending exchange request.
    ExchangeRequest req(4, "UserZ", "TestUser", 0);
    sendExchangeRequest(req);
    // Simulate input:
    //  - Choose option "2" for accept/decline.
    //  - Then, input "0" to select the first pending request.
    //  - Then input "2" to decline it.
    //  - Provide newline for pauseScreen.
    //  - Finally, input "4" to exit.
    std::istringstream input("2\n0\n2\n\n4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    exchangeRequestsMenu("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    // Check that the output indicates decline.
    EXPECT_NE(outStr.find("Request declined"), std::string::npos);
    RemoveTestFiles();
}

// Test branch 3: "View Sent Requests"
TEST(InteractiveTest, ExchangeRequestsMenu_Branch3_ViewSent) {
    RemoveTestFiles();
    // Pre-add an exchange request sent by TestUser.
    ExchangeRequest req(5, "TestUser", "OtherUser", 0);
    sendExchangeRequest(req);
    // Simulate input:
    //  - Choose option "3" to view sent requests.
    //  - Provide newline for pauseScreen.
    //  - Then choose "4" to exit.
    std::istringstream input("3\n\n4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    exchangeRequestsMenu("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    // Check that the sent request details appear.
    EXPECT_NE(outStr.find("BookID: 5"), std::string::npos);
    EXPECT_NE(outStr.find("To: OtherUser"), std::string::npos);
    RemoveTestFiles();
}

// Test branch 4: "Return"
TEST(InteractiveTest, ExchangeRequestsMenu_Branch4_Return) {
    RemoveTestFiles();
    // Simulate input: choose option "4" to exit immediately.
    std::istringstream input("4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    exchangeRequestsMenu("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    // Check that the menu was printed and then returned.
    EXPECT_NE(outStr.find("Exchange Requests Menu"), std::string::npos);
    RemoveTestFiles();
}

TEST(InteractiveTest, ExchangeRequestsMenu_Branch2_InvalidIndex) {
    RemoveTestFiles();
    // Pre-add a pending exchange request for current user "TestUser".
    ExchangeRequest req(6, "UserW", "TestUser", 0);
    sendExchangeRequest(req);

    // Now, simulate:
    // Option "2" for Accept/Decline,
    // then input "5" as the index (invalid if only one request exists),
    // then extra newline for pauseScreen,
    // then "4" to exit the menu.
    std::istringstream input("2\n5\n\n4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    exchangeRequestsMenu("TestUser");

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    // Expect that the output contains "Invalid selection." message.
    EXPECT_NE(outStr.find("Invalid selection"), std::string::npos);
    RemoveTestFiles();
}

// Test the default branch: When an invalid top-level choice is entered.
// For example, if the user enters a number not equal to 0, 1, 2, 3, or 4.
TEST(InteractiveTest, ExchangeRequestsMenu_InvalidChoiceTop) {
    RemoveTestFiles();
    // Simulate input:
    // Enter an invalid option "7" (valid options are 0-4),
    // then extra newline for pauseScreen,
    // then "4" to exit.
    std::istringstream input("7\n\n4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    exchangeRequestsMenu("TestUser");

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    // Expect that "Invalid choice." is printed.
    EXPECT_NE(outStr.find("Invalid choice"), std::string::npos);
    RemoveTestFiles();
}

// Test sendExchangeRequestMenu by simulating search and selection.
TEST(InteractiveTest, SendExchangeRequestMenu) {
    RemoveTestFiles();
    // Pre-add a book owned by another user.
    addBook(Book(1, "SampleBook", "SampleAuthor", "SampleGenre", "OtherUser"));
    // Input: keyword "Sample", then select index 0, then extra newline.
    std::istringstream input("Sample\n0\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    sendExchangeRequestMenu("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    EXPECT_NE(outStr.find("Exchange request sent"), std::string::npos);
    RemoveTestFiles();
}

// Test for sendExchangeRequestMenu: When no matching books are found (empty branch)
TEST(InteractiveTest, SendExchangeRequestMenu_Empty) {
    RemoveTestFiles();
    // Add a book owned by the current user ("TestUser") so that it is filtered out.
    addBook(Book(1, "TestBook", "Author", "Genre", "TestUser"));

    // Simulate input:
    // - Enter keyword "Test" (which matches the book title but the book is owned by currentUser)
    // - Then provide extra newlines for pauseScreen.
    std::istringstream input("Test\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    sendExchangeRequestMenu("TestUser");

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    // Expect that the output indicates no matching books found.
    EXPECT_NE(outStr.find("No matching books found or you already own them."), std::string::npos);
    RemoveTestFiles();
}

// Test for sendExchangeRequestMenu: When an invalid selection is made
TEST(InteractiveTest, SendExchangeRequestMenu_InvalidSelection) {
    RemoveTestFiles();
    // Add a book owned by someone else so that it will be included.
    addBook(Book(1, "TestBook", "Author", "Genre", "OtherUser"));

    // Simulate input:
    // - Enter keyword "Test" to match the book.
    // - Then, when prompted for selection, enter an invalid index "1" (only index 0 is valid).
    // - Then extra newline for pauseScreen.
    std::istringstream input("Test\n1\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    sendExchangeRequestMenu("TestUser");

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    // Expect that the output contains "Invalid selection."
    EXPECT_NE(outStr.find("Invalid selection"), std::string::npos);
    RemoveTestFiles();
}
// Test addBookManually by simulating input.
TEST(InteractiveTest, AddBookManually) {
    RemoveTestFiles();
    // Input: book title, author, genre, then extra newline.
    std::istringstream input("BookTitle\nBookAuthor\nBookGenre\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    addBookManually("TestUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    auto books = loadBooks();
    EXPECT_EQ(books.size(), 1);
    EXPECT_EQ(books[0].title, "BookTitle");
    RemoveTestFiles();
}

TEST(InteractiveTest, ListAllBooks) {
    RemoveTestFiles();
    // Pre-add a book.
    addBook(Book(1, "Book1", "Author1", "Genre1", "TestUser"));
    // Provide enough newlines for pauseScreen (which calls cin.ignore() and cin.get())
    std::istringstream input("\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    listAllBooks();  // This function prints the book list and then calls pauseScreen()

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    EXPECT_NE(outStr.find("Book List:"), std::string::npos);
    RemoveTestFiles();
}

// Test searchBooksMenu by simulating input (search by title).
TEST(InteractiveTest, SearchBooksMenu) {
    RemoveTestFiles();
    // Pre-add books.
    addBook(Book(1, "UniqueTitle", "Author1", "Genre1", "TestUser"));
    addBook(Book(2, "OtherBook", "Author2", "Genre2", "TestUser"));
    // Input: choice 1 (title), keyword "Unique", then extra newline.
    std::istringstream input("1\nUnique\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    searchBooksMenu();
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    EXPECT_NE(outStr.find("UniqueTitle"), std::string::npos);
    RemoveTestFiles();
}


TEST(InteractiveTest, TransactionHistoryMenu) {
    RemoveTestFiles();
    // Pre-add a transaction.
    Transaction t(1, "UserA", "UserB", "2025-03-28 12:34:56");
    addTransaction(t);
    // Provide enough newlines for pauseScreen
    std::istringstream input("\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    transactionHistoryMenu();  // This prints the transaction history and calls pauseScreen()

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    EXPECT_NE(outStr.find("Transaction History:"), std::string::npos);
    RemoveTestFiles();
}
// Test deleteBooksMenu by simulating input (choose exit option "4").
TEST(InteractiveTest, DeleteBooksMenu) {
    RemoveTestFiles();
    // Pre-add a book.
    addBook(Book(1, "BookToDelete", "Author", "Genre", "TestUser"));
    // Input: option "4" to exit.
    std::istringstream input("4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    deleteBooksMenu();
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    EXPECT_EQ(loadBooks().size(), 1); // Book remains.
    RemoveTestFiles();
}

// DeleteBooksMenu tests

// Test for branch 1: Delete Single Book
TEST(InteractiveTest, DeleteBooksMenu_SingleDeletion) {
    RemoveTestFiles();
    // Pre-add two books: one with ID=1 and one with ID=2.
    addBook(Book(1, "Book1", "Author1", "Genre1", "TestUser"));
    addBook(Book(2, "Book2", "Author2", "Genre2", "TestUser"));

    // Simulate input for branch 1:
    // "1" -> choose "Delete Single Book"
    // Then the book list is printed and prompt "Enter the ID of the book to delete:" appears.
    // We input "1" to delete book with ID=1.
    // Then an extra newline for pauseScreen.
    // Finally, the function ends.
    std::istringstream input("1\n1\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    deleteBooksMenu();

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    // Check that output indicates deletion of the book.
    EXPECT_NE(outStr.find("Book with ID 1 deleted"), std::string::npos);
    // Verify that only one book remains (ID=2).
    auto books = loadBooks();
    ASSERT_EQ(books.size(), 1);
    EXPECT_EQ(books[0].id, 2);
    RemoveTestFiles();
}

// Test for branch 2: Delete Multiple Books
TEST(InteractiveTest, DeleteBooksMenu_MultipleDeletion) {
    RemoveTestFiles();
    // Pre-add five books with IDs 1 to 5.
    for (int i = 1; i <= 5; i++) {
        addBook(Book(i, "Book" + std::to_string(i), "Author" + std::to_string(i),
            "Genre" + std::to_string(i), "TestUser"));
    }

    // Simulate input for branch 2:
    // "2" -> choose "Delete Multiple Books"
    // Then prompt "Enter the lower bound ID:" -> input "2"
    // Then prompt "Enter the upper bound ID:" -> input "4"
    // Then extra newline for pauseScreen.
    std::istringstream input("2\n2\n4\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    deleteBooksMenu();

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    // Expected: 3 books deleted in range [2, 4].
    EXPECT_NE(outStr.find("3 books deleted in the range [2, 4]"), std::string::npos);
    // Verify that only books with IDs 1 and 5 remain.
    auto books = loadBooks();
    ASSERT_EQ(books.size(), 2);
    EXPECT_EQ(books[0].id, 1);
    EXPECT_EQ(books[1].id, 5);
    RemoveTestFiles();
}

// Test for branch 3: Invalid Choice
TEST(InteractiveTest, DeleteBooksMenu_InvalidChoice) {
    RemoveTestFiles();
    // No pre-added books needed.
    // Simulate input:
    // "9" -> invalid choice
    // Then extra newline for pauseScreen.
    std::istringstream input("9\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());

    deleteBooksMenu();

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    std::string outStr = output.str();
    // Expect output to contain "Invalid choice."
    EXPECT_NE(outStr.find("Invalid choice"), std::string::npos);
    RemoveTestFiles();
}


// Test heavyLoadListingTest to ensure it runs.
TEST(InteractiveTest, HeavyLoadListingTest) {
    RemoveTestFiles();
    // Pre-add some books.
    for (int i = 1; i <= 5; i++) {
        addBook(Book(i, "Book" + std::to_string(i), "Author", "Genre", "TestUser"));
    }
    // Input: provide a newline for pauseScreen.
    std::istringstream input("\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    heavyLoadListingTest();
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    EXPECT_NE(outStr.find("Heavy Load Book Listing Performance:"), std::string::npos);
    RemoveTestFiles();
}

// Test autoAddPerformanceTest to ensure it runs.
TEST(InteractiveTest, AutoAddPerformanceTest) {
    RemoveTestFiles();
    // Use small test count for faster testing.
    std::istringstream input("\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    autoAddPerformanceTest();
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    EXPECT_NE(outStr.find("Auto add performance results saved"), std::string::npos);
    RemoveTestFiles();
}

// Test searchPerformanceTest to ensure it runs.
TEST(InteractiveTest, SearchPerformanceTest) {
    RemoveTestFiles();
    // Pre-add some books with "Auto" in the title.
    for (int i = 1; i <= 5; i++) {
        addBook(Book(i, "AutoBook" + std::to_string(i), "Author", "Genre", "TestUser"));
    }
    std::istringstream input("\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    searchPerformanceTest();
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    std::string outStr = output.str();
    EXPECT_NE(outStr.find("Search Performance Test with keyword:"), std::string::npos);
    RemoveTestFiles();
}

// Test rateUserMenu by simulating input.
TEST(InteractiveTest, RateUserMenu) {
    RemoveTestFiles();
    // Pre-register a user to be rated.
    User u("RateTarget", "pass");
    registerUser(u);
    // Input: username to rate, rating value, and extra newline.
    std::istringstream input("RateTarget\n4.0\n\n");
    std::ostringstream output;
    auto oldCin = std::cin.rdbuf(input.rdbuf());
    auto oldCout = std::cout.rdbuf(output.rdbuf());
    rateUserMenu("AnyUser");
    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    auto usersVec = loadUsers();
    ASSERT_FALSE(usersVec.empty());
    EXPECT_NEAR(usersVec[0].rating, 4.0f, 0.001);
    EXPECT_EQ(usersVec[0].ratingCount, 1);
    RemoveTestFiles();
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