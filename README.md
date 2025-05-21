# 📚 Book Project Library - Parallel Programming Implementation

## 🎓 CEN310 Parallel Programming Course - Midterm Project (2024-2025)

<div align="center">
  <img src="https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B" alt="C++"/>
  <img src="https://img.shields.io/badge/CMake-3.12+-064F8C.svg?style=flat&logo=cmake" alt="CMake"/>
  <img src="https://img.shields.io/badge/OpenMP-Enabled-28B463.svg?style=flat" alt="OpenMP"/>
</div>

---

## 👥 Contributors

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/mustafayanmaz">
        <img src="https://avatars.githubusercontent.com/u/114070977?v=4" width="100px;" alt="Mustafa Yanmaz"/><br />
        <sub><b>Mustafa Yanmaz</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/Hayrunnisa4">
        <img src="https://avatars.githubusercontent.com/u/114340067?v=4" width="100px;" alt="Hayrunnisa Kasımay"/><br />
        <sub><b>Hayrunnisa Kasımay</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/AliTopcuu">
        <img src="https://avatars.githubusercontent.com/u/114070829?v=4" width="100px;" alt="Ali Ufuktan Topçu"/><br />
        <sub><b>Ali Ufuktan Topçu</b></sub>
      </a>
    </td>
  </tr>
</table>

## 📋 Project Description

This project demonstrates a Book Management System with parallel programming concepts. It showcases:

- ⚡ High-performance parallel book operations
- 🧪 Test-driven development with CMake & CTest
- 📝 Comprehensive Doxygen documentation
- 📊 Detailed test coverage reports

## 🛠️ Technical Requirements

- ⚙️ CMake >= 3.12
- 🔄 C++ Standard >= 11
- 🧪 GoogleTest (for testing modules)
- 💻 Visual Studio Community Edition (for Windows)
- 🐱‍👤 Ninja (for WSL/Linux)

## 📊 Status

### 🚀 Releases

[Latest Release](.github/workflows/cpp.yml)

### 📈 Coverage Reports

<div align="center">
  <img src="assets/codecoveragelibwin/badge_linecoverage.svg" alt="Test Coverage"/>
  <img src="assets/doccoveragelibwin/badge_linecoverage.svg" alt="Doxygen Coverage"/>
</div>

## 💻 Installation

```bash
# Clone the repository
git clone https://github.com/mustafayanmaz/cen310-midterm-mustafa-yanmaz.git
cd cen310-midterm-mustafa-yanmaz

# Run setup scripts (3rd, 4th, and 7th batch or shell files)
# For Windows:
.\scripts\3_setup_environment.bat
.\scripts\4_configure_project.bat
.\scripts\7_run_tests.bat

# For Linux/WSL:
./scripts/3_setup_environment.sh
./scripts/4_configure_project.sh
./scripts/7_run_tests.sh
```

## ✨ Features

### 👤 User Management

- 🔐 User registration and login with secure authentication
- ⭐ Rating system for evaluating user interactions

### 📚 Book Management

- ✏️ **Manual Book Addition:** Add books with details (title, author, genre, owner)
- 🚀 **Parallel Book Addition:**
  - 🧵 Thread-Per-Book: Each book added by a separate thread
  - ⚡ OpenMP Implementation: Controlled thread count for concurrent book addition
- 🗑️ Book deletion options (single or multiple by ID range)

### 🔍 Search and Listing

- 📋 Comprehensive book listing functionality
- 🔎 Search by title, author, or genre
- 📊 Performance testing for heavy load operations using parallel techniques

### 🔄 Exchange and Transaction System

- 🤝 Book trading through exchange requests
- 📬 Options to send, view, and process exchange requests
- 📝 Automatic transaction recording

### 🔒 Data Security

- 🛡️ Encrypted I/O operations for secure data storage

## ⚡ Parallel Programming Implementation Details

This project demonstrates various parallel programming concepts:

- 🧵 Multi-threading for independent book operations
- 🔄 OpenMP for controlled concurrent operations
- 📈 Performance improvements demonstrated through heavy load testing
- 🧮 Concurrent mathematical computations to simulate real-world performance conditions

## 🧪 Testing

Use the provided environment scripts to run tests:

```bash
# Run all tests
./scripts/7_run_tests.bat  # Windows
./scripts/7_run_tests.sh   # Linux/WSL

# Generate test coverage reports
./scripts/8_generate_coverage.bat  # Windows
./scripts/8_generate_coverage.sh   # Linux/WSL
```

## 🧑‍🏫 Support & Supervision

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/ugurcoruh">
        <img src="https://avatars.githubusercontent.com/u/7415667?v=4" width="100px;" alt="Uğur Coruh"/><br />
        <sub><b>Uğur Coruh</b></sub>
      </a>
    </td>
  </tr>
</table>

---

<div align="center">
  <sub>Built with ❤️ by the Book Project Team</sub>
</div>
