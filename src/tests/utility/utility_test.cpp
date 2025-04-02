/**
 * @file mathUtility_test.cpp
 * @brief Test file for MathUtility functions in the Coruh::Utility namespace using Google Test.
 */

#include "gtest/gtest.h"
#include "../../utility/header/commonTypes.h"
#include "../../utility/header/mathUtility.h"

#include <fstream>
#include <sys/stat.h>
#include <cstdio>
#ifdef _WIN32
#include <direct.h>
#elif __linux__
#include <unistd.h>
#endif

using namespace Coruh::Utility;

/**
 * @brief Test fixture for MathUtility functions.
 */
class MathUtilityTest : public ::testing::Test {
public:
    int a;
protected:
    /**
     * @brief Setup test data before each test.
     */
    void SetUp() override {
        // Setup test data
    }

    /**
     * @brief Clean up test data after each test.
     */
    void TearDown() override {
        // Clean up test data
    }
};

/**
 * @brief Tests the calculateMean function with a simple dataset.
 */
TEST_F(MathUtilityTest, CalculateMean) {
    int b = this->a;
    // Test data
    const double data[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    const int datalen = sizeof(data) / sizeof(data[0]);
    // Perform the calculation
    double result = MathUtility::calculateMean(data, datalen);
    // Check the result
    EXPECT_DOUBLE_EQ(result, 3.0);
}

/**
 * @brief Tests the calculateMedian function with an odd number of elements.
 */
TEST_F(MathUtilityTest, CalculateMedianOdd) {
    // Test data
    const double data[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    const int datalen = sizeof(data) / sizeof(data[0]);
    // Perform the calculation
    double result = MathUtility::calculateMedian(data, datalen);
    // Check the result
    EXPECT_DOUBLE_EQ(result, 3.0);
}

/**
 * @brief Tests the calculateMedian function with an even number of elements.
 */
TEST_F(MathUtilityTest, CalculateMedianEven) {
    // Test data
    const double data[] = { 1.0, 2.0, 3.0, 4.0 };
    const int datalen = sizeof(data) / sizeof(data[0]);
    // Perform the calculation
    double result = MathUtility::calculateMedian(data, datalen);
    // Check the result
    EXPECT_DOUBLE_EQ(result, 2.5);
}

/**
 * @brief Tests the compareDouble function when the first value is less than the second.
 */
TEST_F(MathUtilityTest, CompareDoubleLessTest) {
    // Test data
    const double val1 = 2.0;
    const double val2 = 4.0;
    // Perform the comparison
    int result = MathUtility::compareDouble(&val1, &val2);
    // Check the result
    EXPECT_EQ(result, -1);
}

/**
 * @brief Tests the compareDouble function when the first value is greater than the second.
 */
TEST_F(MathUtilityTest, CompareDoubleGreaterTest) {
    // Test data
    const double val1 = 4.0;
    const double val2 = 2.0;
    // Perform the comparison
    int result = MathUtility::compareDouble(&val1, &val2);
    // Check the result
    EXPECT_EQ(result, 1);
}

/**
 * @brief Tests the compareDouble function when both values are equal.
 */
TEST_F(MathUtilityTest, CompareDoubleEqualTest) {
    // Test data
    const double val1 = 3.0;
    const double val2 = 3.0;
    // Perform the comparison
    int result = MathUtility::compareDouble(&val1, &val2);
    // Check the result
    EXPECT_EQ(result, 0);
}

/**
 * @brief Tests the calculateMinMax function using a sorted dataset.
 */
TEST_F(MathUtilityTest, CalculateMinMaxTest_1) {
    // Test data
    const double data[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    const int datalen = sizeof(data) / sizeof(data[0]);
    double min, max;
    // Perform the calculation
    MathUtility::calculateMinMax(data, datalen, &min, &max);
    // Check the result
    EXPECT_DOUBLE_EQ(min, 1.0);
    EXPECT_DOUBLE_EQ(max, 5.0);
}

/**
 * @brief Tests the calculateMinMax function using an unsorted dataset.
 */
TEST_F(MathUtilityTest, CalculateMinMaxTest_2) {
    double data[] = { 3.14, 1.0, -2.5, 7.2, -5.0 };
    double min, max;
    MathUtility::calculateMinMax(data, sizeof(data) / sizeof(data[0]), &min, &max);
    EXPECT_DOUBLE_EQ(min, -5.0);
    EXPECT_DOUBLE_EQ(max, 7.2);
}

/**
 * @brief The main function of the test program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return int The exit status of the program.
 */
int main(int argc, char** argv) {
#ifdef ENABLE_UTILITY_TEST
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#else
    return 0;
#endif
}