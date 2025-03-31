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