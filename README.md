# Book Project Library - Parallel Programming Implementation

## CEN310 Parallel Programming Course - Midterm Project (2024-2025)

**Contributors:**
- Mustafa Yanmaz
- Hayrunnisa Kasımay
- Ali Ufuktan Topçu

## Project Description

This project demonstrates a Book Management System with parallel programming concepts. It showcases how CMake can be used with CTest, and includes Doxygen documentation and test coverage reports.

## Technical Requirements

- CMake >= 3.12
- C++ Standard >= 11
- GoogleTest (for testing modules)
- Visual Studio Community Edition (for Windows)
- Ninja (for WSL/Linux)

## Status

### Releases
[Latest Release](.github/workflows/cpp.yml)

### Coverage Reports

**Test Coverage:**
![test](assets/codecoveragelibwin/badge_linecoverage.svg/)

**Doxygen Coverage:**
![doxygen](assets/doccoveragelibwin/badge_linecoverage.svg)

## Installation

Run the 3rd, 4th, and 7th batch or shell files to set up the environment.

## Features

### User Management
- User registration and login with secure authentication
- Rating system for evaluating user interactions

### Book Management
- **Manual Book Addition:** Add books with details (title, author, genre, owner)
- **Parallel Book Addition:**
  - Thread-Per-Book: Each book added by a separate thread
  - OpenMP Implementation: Controlled thread count for concurrent book addition
- Book deletion options (single or multiple by ID range)

### Search and Listing
- Comprehensive book listing functionality
- Search by title, author, or genre
- Performance testing for heavy load operations using parallel techniques

### Exchange and Transaction System
- Book trading through exchange requests
- Options to send, view, and process exchange requests
- Automatic transaction recording

### Data Security
- Encrypted I/O operations for secure data storage

## Parallel Programming Implementation Details

This project demonstrates various parallel programming concepts:
- Multi-threading for independent book operations
- OpenMP for controlled concurrent operations
- Performance improvements demonstrated through heavy load testing
- Concurrent mathematical computations to simulate real-world performance conditions

## Testing

Use the provided environment scripts to run tests.

## Support & Supervision

**Supervisor:**
- Uğur Coruh
  ![ugurcoruh](https://avatars.githubusercontent.com/u/7415667?v=4)

## Contact

- [Mustafa's GitHub](https://github.com/mustafayanmaz)
  ![mustafa yanmaz](https://avatars.githubusercontent.com/u/114070977?v=4)
