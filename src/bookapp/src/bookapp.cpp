#include <iostream>
#include <mpi.h>
#include <cstring>  // strncpy
#include "book.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::string currentUser = "";
    int choice;
    char userBuffer[100] = { 0 };

    while (true) {
        if (rank == 0) {
            clearScreen();
            if (currentUser.empty()) {
                showMainMenu();
            }
            else {
                showUserMenu(currentUser);
            }
            std::cin >> choice;
        }

        MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            std::strncpy(userBuffer, currentUser.c_str(), sizeof(userBuffer) - 1);
        }
        MPI_Bcast(userBuffer, sizeof(userBuffer), MPI_CHAR, 0, MPI_COMM_WORLD);
        currentUser = std::string(userBuffer);

        if (currentUser.empty()) {
            switch (choice) {
            case 1:
                if (rank == 0) registerUserMenu();
                break;
            case 2:
                if (rank == 0) loginUserMenu(currentUser);
                break;
            case 3:
                if (rank == 0) std::cout << "Exiting application." << std::endl;
                MPI_Finalize();
                return 0;
            default:
                if (rank == 0) {
                    std::cout << "Invalid choice." << std::endl;
                    pauseScreen();
                }
                break;
            }
        }

        else if (choice >= 13 && choice <= 18) {
            std::vector<Book> allBooks = loadBooks();

            switch (choice) {
            case 13:
                bookSimilarityMatrixTestMPI(allBooks);
                break;
            case 14:
                bookExchangeShortestPathTestMPI();
                break;
            case 15:
                bookTrigramSimilarityTestMPI(allBooks);
                break;
            case 16: {
                int N;
                if (rank == 0) {
                    std::cout << "Enter matrix dimension N: ";
                    std::cin >> N;
                }
                MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
                matrixMultiplicationTestMPI(N);
                break;
            }
            
           
            }

            if (rank == 0) pauseScreen();
        }

        else {
            switch (choice) {
            case 1: if (rank == 0) addBookManually(currentUser); break;
            case 2:
                if (rank == 0) {
                    int count;
                    std::cout << "Enter number of books to add (thread per book): ";
                    std::cin >> count;
                    autoAddBooksThreadPerBook(count);
                    pauseScreen();
                }
                break;
            case 3:
                if (rank == 0) {
                    int count, threads;
                    std::cout << "Enter number of books to add: ";
                    std::cin >> count;
                    std::cout << "Enter number of threads: ";
                    std::cin >> threads;
                    autoAddBooksParallelForImproved(count, threads);
                    pauseScreen();
                }
                break;
            case 4: if (rank == 0) listAllBooks(); break;
            case 5: if (rank == 0) searchBooksMenu(); break;
            case 6: if (rank == 0) exchangeRequestsMenu(currentUser); break;
            case 7: if (rank == 0) rateUserMenu(currentUser); break;
            case 8: if (rank == 0) transactionHistoryMenu(); break;
            case 9: if (rank == 0) deleteBooksMenu(); break;
            case 10: if (rank == 0) heavyLoadListingTest(); break;
            case 11: if (rank == 0) autoAddPerformanceTest(); break;
            case 12: if (rank == 0) searchPerformanceTest(); break;
            case 0:
                if (rank == 0) {
                    currentUser = "";
                    std::cout << "Logged out successfully.\n";
                    pauseScreen();
                }
                break;
            default:
                if (rank == 0) {
                    std::cout << "Invalid choice." << std::endl;
                    pauseScreen();
                }
                break;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
