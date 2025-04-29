# 📚 Library Management System (C++)

![C++](https://img.shields.io/badge/C++-17-blue) 
![OOP](https://img.shields.io/badge/OOP-Yes-green)

A console-based Library Management System developed in C++ that allows the users to borrow, return, and reserve books while managing overdue fines and maintaining records of all books and students.

---

## Features

- Add new books and students (password-protected)
- Issue, return, and reserve books
- Display all books and students
- Calculate and display overdue fines
- Input validation for invalid operations

---

## Technology Stack

- **Language**: C++ (Standard Library only)
- **Concepts**: Object-Oriented Programming, File I/O

---

## Installation

### Prerequisities

- A C++ compiler (e.g., **g++**)
- Standard C++17 support

### Compilation Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/library-management-system.git
   cd library-management-system

2. Compile the project:
   ```bash
   g++ -o online-library -Wall online-library.cpp

3. Run the program:
   ```bash
   ./online-library

---
## File Structure

-  **`online-library.cpp`**  
  Contains all the class definitions (`Person`, `Student`, `Book`, `Library`) and the program's main logic.

- **`books.txt`**  
  Stores information about books (e.g., title, ISBN, issued status).

- **`students.txt`**  
  Stores information about students (e.g., student ID, name, reserved book status).
---
## Project Documentation

For a detailed explanation of the system's design, class description, and more, please refer to the full [Project Documentation](docs/Konrad_Włodarczyk_OOP_project_documentation.docx).

---

## Future Development

- Implement a graphical user interface (GUI).
- Enhance the reservation system with waitlists.
- Integrate a database for better data management.
- Improve authentication and security for library staff.

---

## License

This project is licensed under the **MIT License**.  
See the [LICENSE](LICENSE) file for details.

--- 

## Contact

For questions or suggestions, feel free to reach out!
