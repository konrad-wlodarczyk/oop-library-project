#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <map>
#include <chrono>
#include <ctime>

using namespace std;

const string PASSWORD = "library123";  

// Base class for person
class Person {
protected:
    string m_name;  
    int m_age;      

public:
    // Constructor to initialize name and age
    Person(const string& name, int age) : m_name(name), m_age(age) {}

    // Pure virtual function to display person details
    virtual void display() const = 0;

    // Virtual destructor to ensure proper cleanup
    virtual ~Person() = default;
};

// Student class derived from Person class
class Student : public Person {
private:
    int m_student_ID;           
    bool m_has_Issued_Book;     

public:
    // Constructor to initialize student details
    Student(const string& name, int age, int student_ID)
        : Person(name, age), m_student_ID(student_ID), m_has_Issued_Book(false) {}

    // Getters for student details
    const int& getStudentID() const { return m_student_ID; }
    bool canIssueBook() const { return !m_has_Issued_Book; }
    void issueBook() { m_has_Issued_Book = true; }
    void returnBook() { m_has_Issued_Book = false; }

    // Display student details (Overriding the virtual function from person)
    void display() const override {
        cout << "\nStudent ID: " << m_student_ID
             << ", Name: " << m_name
             << ", Age: " << m_age
             << ", Has Issued Book: " << (m_has_Issued_Book ? "Yes" : "No") << endl;
    }

    // Serialize student data into a string for saving
    string serialize() const {
        return m_name + "|" + to_string(m_age) + "|" + to_string(m_student_ID) + "|" + (m_has_Issued_Book ? "1" : "0");
    }

    // Deserialize a string to create a Student object
    static shared_ptr<Student> deserialize(const string& data) {
        stringstream ss(data);
        string name, ageStr, idStr, issuedStr;

        getline(ss, name, '|');
        getline(ss, ageStr, '|');
        getline(ss, idStr, '|');
        getline(ss, issuedStr, '|');

        // Create a new Student object with the extracted data
        auto student = make_shared<Student>(name, stoi(ageStr), stoi(idStr));
        if (issuedStr == "1")
            student->issueBook(); 
        return student;
    }
};

// Book class
class Book {
private:
    string m_title;        
    string m_author;        
    int m_page_count;      
    long long m_ISBN;       
    bool m_is_issued;       
    int m_reserved_by;      

public:
    // Constructor to initialize book details
    Book(const string& title, const string& author, int page_count, long long ISBN, bool is_issued = false)
    : m_title(title), m_author(author), m_page_count(page_count), m_ISBN(ISBN), m_is_issued(is_issued), m_reserved_by(-1) {}

    // Getters for book details
    const long long& getISBN() const { return m_ISBN; }
    const string& getTitle() const { return m_title; }
    const string& getAuthor() const { return m_author; }
    bool getIssuedStatus() const { return m_is_issued; }
    int getReservedBy() const { return m_reserved_by; }

    // Mark the book as issued
    void issue() {
        if (m_reserved_by > 0) throw runtime_error("Book is already reserved by another student.");
        if (m_is_issued) throw runtime_error("Book is already issued by another student.");
        m_is_issued = true;
    }

    // Mark the book as returned
    void returnBook() {
        m_is_issued = false;
        m_reserved_by = -1;
    }

    // Reserve the book for a specific student
    void reserve(int student_ID) { m_reserved_by = student_ID; }

    // Display book details
    void display() const {
        cout << "\nTitle: " << m_title
             << ", Author: " << m_author
             << ", Pages: " << m_page_count
             << ", ISBN: " << m_ISBN
             << ", Issued: " << (m_is_issued ? "Yes" : "No")
             << ", Reserved By Student With ID: " << (m_reserved_by == -1 ? "No one" : to_string(m_reserved_by)) << endl;
    }

    // Serialize book data into a string for saving
    string serialize() const {
        return m_title + "|" + m_author + "|" + to_string(m_page_count) + "|" + to_string(m_ISBN) + "|" + (m_is_issued ? "1" : "0") + "|" + to_string(m_reserved_by);
    }

    // Deserialize a string to create a Book object
    static shared_ptr<Book> deserialize(const string& data) {
        stringstream ss(data);
        string title, author, pagesStr, isbnStr, issuedStr, reservedStr;

        getline(ss, title, '|');
        getline(ss, author, '|');
        getline(ss, pagesStr, '|');
        getline(ss, isbnStr, '|');  
        getline(ss, issuedStr, '|');
        getline(ss, reservedStr, '|');

        // Create a new Book object with the extracted data
        auto book = make_shared<Book>(title, author, stoi(pagesStr), stoll(isbnStr), issuedStr == "1");
        book->m_reserved_by = stoi(reservedStr);
        return book;
    }
};

// Main library working class
class Library {
private:
    vector<shared_ptr<Book>> books; 
    vector<shared_ptr<Student>> students;  
    map<long long, chrono::system_clock::time_point> issued_books; 
    const int max_Issue_Days = 15;  
    const double fine = 0.50;  

    const string books_file = "books.txt";  // File to store book data
    const string students_file = "students.txt";  // File to store student data

public:
    // Constructor to load books and students from files
    Library() {
        loadBooks();
        loadStudents();
    }

    // Destructor to save books and students to files
    ~Library() {
        saveBooks();
        saveStudents();
    }

    // Add a new book to the library
    void addBook(const string& title, const string& author, int page_count, long long ISBN) {
        books.push_back(make_shared<Book>(title, author, page_count, ISBN));
    }

    // Add a new student to the library
    void addStudent(const string& name, int age, int student_ID) {
        students.push_back(make_shared<Student>(name, age, student_ID));
    }

    // Issue a book to a student
    void issueBook(int student_ID, long long ISBN) {
        auto student = findStudent(student_ID);
        auto book = findBook(ISBN);

        if (!student->canIssueBook())
            throw runtime_error("Student already has a book issued.");

        book->issue();
        student->issueBook();
        issued_books[ISBN] = chrono::system_clock::now(); 
        cout << "Book with ISBN " << ISBN << " issued to student with ID " << student_ID << "." << endl; 
    }

    // Return a book to the library
    void returnBook(long long ISBN) {
        auto book = findBook(ISBN);
        auto it = issued_books.find(ISBN);

        cout << "Attempting to return book with ISBN: " << ISBN << endl;

        if (it == issued_books.end()) {
            throw runtime_error("Book is not issued.");
        }

        book->returnBook();
        for (auto& student : students) {
            if (!student->canIssueBook()) {
                student->returnBook();
                break;
            }
        }
        issued_books.erase(it);  // Remove the book from the issued books map
        cout << "Book with ISBN " << ISBN << " returned." << endl;  // Debug statement
    }

    // Reserve a book for a student
    void reserve(int student_ID, long long ISBN) {
        auto student = findStudent(student_ID);
        auto book = findBook(ISBN);

        if (book->getReservedBy() != -1)
            throw runtime_error("Book is already reserved by another student.");

        book->reserve(student_ID);
    }

    // Display all overdue books with their fines
    void displayOverdueBooks() const {
        auto now = std::chrono::system_clock::now();
        for (const auto& [ISBN, issue_date] : issued_books) {
            auto duration = std::chrono::duration_cast<std::chrono::hours>(now - issue_date).count()/24;

            if (duration > max_Issue_Days) {
                double fine = static_cast<double>(duration - max_Issue_Days) * fine;
                auto book = findBook(ISBN);
                cout << "Overdue Book: " << book->getTitle() << ", ISBN: " << book->getISBN() << ", Fine: " << fine << " zł." << endl;
            }
        }
    }

    // Display all books in the library
    void displayAllBooks() const {
        for (const auto& book : books)
            book->display();
    }

    // Display all students in the library
    void displayAllStudents() const {
        for (const auto& student : students)
            student->display();
    }

    // User interface to add a book
    void addBookUI() {
        int page_count;
        long long ISBN;
        string title, author;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Title (or /back to return to main menu): ";
        getline(cin, title);

        if (title == "/back") {
            cout << "Returning to main menu...\n";
            return;
        }

        cout << "Enter Author: ";
        getline(cin, author);

        cout << "Enter Page Count: ";
        cin >> page_count;

        cout << "Enter ISBN: ";
        cin >> ISBN;

        if(to_string(ISBN).length() != 13 || to_string(ISBN).empty()) {
            throw runtime_error("ISBN must be a 13-digit number.");
        }

        addBook(title, author, page_count, ISBN);
        cout << "The book has been added to the database.\n";
    }

    // User interface to add a student
    void addStudentUI() {
        int studentID, age;
        string name;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Name (or /back to return to main menu): ";
        getline(cin, name);

        if (name == "/back") {
            cout << "Returning to main menu...\n";
            return;
        }

        cout << "Enter Age: ";
        cin >> age;

        cout << "Enter Student ID: ";
        cin >> studentID;

        addStudent(name, age, studentID);
        cout << "The student has been added to the database.\n";
    }

    // User interface to issue a book
    void issueBookUI() {
        int student_ID;
        long long ISBN;
        string input;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Student ID (or /back to return to main menu): ";
        getline(cin, input);

        if (input == "/back") {
            cout << "Returning to main menu...\n";
            return;
        }

        student_ID = stoi(input);

        cout << "Enter book ISBN: ";
        cin >> ISBN;

        if(to_string(ISBN).length() != 13 || to_string(ISBN).empty()) {
            throw runtime_error("ISBN must be a 13-digit number.");
        }

        issueBook(student_ID, ISBN);
        cout << "The book with ISBN: " << ISBN << " has been issued to student with ID: " << student_ID << "." << endl;
    }

    // User interface to return a book
   void returnBookUI() {
        string input;
        cout << "Enter Book ISBN to Return (or /back to return to main menu): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, input);

        if (input == "/back") {
            cout << "Returning to main menu...\n";
            return;
        }

        if (input.empty() || input.length() != 13) {
            throw runtime_error("ISBN must be a 13-digit number.");
            return;
        }

        long long ISBN = stoll(input); 

        returnBook(ISBN);
        cout << "The book with ISBN: " << ISBN << " has been returned by a student." << endl;
    }

    // User interface to reserve a book
    void reserveBookUI() {
        int student_ID;
        long long ISBN;
        string input;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Student ID (or /back to return to main menu): ";
        getline(cin, input);

        if (input == "/back") {
            cout << "Returning to main menu...\n";
            return;
        }

        student_ID = stoi(input);

        cout << "Enter ISBN of the book to reserve: ";
        cin >> ISBN;

        if(to_string(ISBN).length() != 13 || to_string(ISBN).empty()) {
            throw runtime_error("ISBN must be a 13-digit number.");
        }

        reserve(student_ID, ISBN);
        cout << "The book with ISBN: " << ISBN << " has been reserved by student with ID: " << student_ID << "." << endl;
    }

private:
    // Save all books to the books file
    void saveBooks() const {
        ofstream ofs(books_file);
        for (const auto& book : books) {
            ofs << book->serialize() << "\n";
        }
    }

    // Save all students to the students file
    void saveStudents() const {
        ofstream ofs(students_file);
        for (const auto& student : students) {
            ofs << student->serialize() << "\n";
        }
    }

    // Load all books from the books file
    void loadBooks() {
        ifstream ifs(books_file);
        string line;
        while (getline(ifs, line)) {
            books.push_back(Book::deserialize(line));
        }
    }

    // Load all students from the students file
    void loadStudents() {
        ifstream ifs(students_file);
        string line;
        while (getline(ifs, line)) {
            students.push_back(Student::deserialize(line));
        }
    }

    // Find a book by ISBN
    shared_ptr<Book> findBook(long long ISBN) const {
        for (const auto& book : books) {
            if (book->getISBN() == ISBN) return book;
        }
        throw runtime_error("Book not found.");
    }

    // Find a student by ID
    shared_ptr<Student> findStudent(int student_ID) const {
        for (const auto& student : students) {
            if (student->getStudentID() == student_ID) return student;
        }
        throw runtime_error("Student not found.");
    }
};

// Function to check if the entered password is correct
bool checkPassword() {
    string input_password;
    cout << "Enter password: ";
    cin >> input_password;
    return input_password == PASSWORD;
}

// Main function to run the library management system
int main() {
    Library library;
    int choice;

    while (true) {
        cout << "\n***********************\n";
        cout << "Online Library\n";
        cout << "\n***********************\n";
        cout << "\n1. Add Book\n";
        cout << "2. Add Student\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. Reserve Book\n";
        cout << "6. Display All Books\n";
        cout << "7. Display All Students\n";
        cout << "8. Display Overdue Books\n";
        cout << "9. Exit";
        cout << "\n***********************\n";
        cout << "Enter Your Choice: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        try {
            switch (choice) {
                case 1: {
                    if (!checkPassword()) {
                        cout << "Incorrect password.\n";
                        break;
                    }
                    library.addBookUI();
                    break;
                }
                case 2: {
                    if (!checkPassword()) {
                        cout << "Incorrect password.\n";
                        break;
                    }
                    library.addStudentUI();
                    break;
                }
                case 3: {
                    library.issueBookUI();
                    break;
                }
                case 4: {
                    library.returnBookUI();
                    break;
                }
                case 5: {
                    library.reserveBookUI();
                    break;
                }
                case 6: {
                    cout << "All Books available in the database: \n";
                    library.displayAllBooks();
                    break;
                }
                case 7: {
                    if (!checkPassword()) {
                        cout << "Incorrect password.\n";
                        break;
                    }
                    cout << "All Students added to the database: \n";
                    library.displayAllStudents();
                    break;
                }
                case 8: {
                    cout << "All overdue books: \n";
                    library.displayOverdueBooks();
                    break;
                }
                case 9: {
                    return 0;
                }
                default:
                    cout << "Invalid choice, Try Again.\n";
            }
        } catch (const std::exception& ex) {
            cout << "Error: " << ex.what() << "\n";
        }
    }

    return 0;
}