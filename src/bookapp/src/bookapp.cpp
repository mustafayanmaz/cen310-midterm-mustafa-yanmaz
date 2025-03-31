#include "book.h"
#include <iostream>

int main() {
    std::string currentUser = "";
    int choice;
    while (true) {
        clearScreen();
        if (currentUser.empty()) {
            showMainMenu();
            std::cin >> choice;
            switch (choice) {
            case 1:
                registerUserMenu();
                break;
            case 2:
                if (loginUserMenu(currentUser)) {
                }
                break;
            case 3:
                std::cout << "Exiting application." << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice." << std::endl;
                pauseScreen();
                break;
            }
        }
        else {
            clearScreen();
            showUserMenu(currentUser);
            std::cin >> choice;
            switch (choice) {
            case 1:
                addBookManually(currentUser);
                break;
            case 2: {
                int count;
                std::cout << "Enter number of books to add (thread per book): ";
                std::cin >> count;
                autoAddBooksThreadPerBook(count);
                pauseScreen();
            }
                  break;
            case 3: {
                int count, numThreads;
                std::cout << "Enter number of books to add (improved parallel for): ";
                std::cin >> count;
                std::cout << "Enter number of threads to use: ";
                std::cin >> numThreads;
                autoAddBooksParallelForImproved(count, numThreads);
                pauseScreen();
            }
                  break;
            case 4:
                listAllBooks();
                break;
            case 5:
                searchBooksMenu();
                break;
            case 6:
                exchangeRequestsMenu(currentUser);
                break;
            case 7:
                rateUserMenu(currentUser);
                break;
            case 8:
                transactionHistoryMenu();
                break;
            case 9:
                deleteBooksMenu();
                break;
            case 10:
                heavyLoadListingTest();
                break;
            case 11:
                autoAddPerformanceTest();
                break;
            case 12:
                searchPerformanceTest();
                break;
            case 13:
                currentUser = "";
                std::cout << "Logged out successfully." << std::endl;
                pauseScreen();
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
                pauseScreen();
                break;
            }
        }
    }
    return 0;
}
