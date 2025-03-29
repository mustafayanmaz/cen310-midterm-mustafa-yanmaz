#include "../header/book.h"
#include <stdexcept>

using namespace Coruh::Book;

double Book::add(double a, double b) {
    return a + b;
}

double Book::subtract(double a, double b) {
    return a - b;
}

double Book::multiply(double a, double b) {
    return a * b;
}

double Book::divide(double a, double b) {
    if (b == 0) {
        throw std::invalid_argument("Division by zero is not allowed.");
    }
    return a / b;
}