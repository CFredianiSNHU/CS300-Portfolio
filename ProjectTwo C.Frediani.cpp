//============================================================================
// ProjectTwo.cpp
// Christina Frediani
// CS 300 Project Two - Course Planner
// This program reads course data from a file, stores it in a binary search
// tree, and lets the user view and search courses.
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


// Course structure


// This struct stores course information
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};

// Removes extra spaces from the beginning and end of a string
static string trim(const string& value) {
    const string whitespace = " \t\n\r\f\v";
    size_t start = value.find_first_not_of(whitespace);
    if (start == string::npos) return "";
    size_t end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

// Makes sure course numbers are clean (no spaces, all uppercase)
static string normalizeCourseNumber(string value) {
    value = trim(value);
    value.erase(remove_if(value.begin(), value.end(),
        [](unsigned char ch) { return isspace(ch); }),
        value.end());

    transform(value.begin(), value.end(), value.begin(),
        [](unsigned char ch) { return toupper(ch); });

    return value;
}

// Splits a line of text into parts using commas
static vector<string> split(const string& line, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// Prints a simple course line (for list)
static void displayCourseSummary(const Course& course) {
    cout << course.courseNumber << ", " << course.courseName << endl;
}

// Prints full course details including prerequisites
static void displayCourseDetails(const Course& course) {
    cout << course.courseNumber << ", " << course.courseName << endl;
    cout << "Prerequisites: ";

    if (course.prerequisites.empty()) {
        cout << "None" << endl;
        return;
    }

    for (size_t i = 0; i < course.prerequisites.size(); ++i) {
        cout << course.prerequisites[i];
        if (i + 1 < course.prerequisites.size()) {
            cout << ", ";
        }
    }
    cout << endl;
}

// Binary Search Tree

// This class stores courses in a binary search tree
class BinarySearchTree {
private:
    struct Node {
        Course course;
        Node* left;
        Node* right;

        Node(const Course& c) : course(c), left(nullptr), right(nullptr) {}
    };

    Node* root;

    // Helper function to insert a course into the tree
    void addNode(Node* node, const Course& course) {
        if (course.courseNumber < node->course.courseNumber) {
            if (node->left == nullptr)
                node->left = new Node(course);
            else
                addNode(node->left, course);
        }
        else {
            if (node->right == nullptr)
                node->right = new Node(course);
            else
                addNode(node->right, course);
        }
    }

    // In-order traversal (prints courses in sorted order)
    void inOrder(Node* node) const {
        if (node != nullptr) {
            inOrder(node->left);
            displayCourseSummary(node->course);
            inOrder(node->right);
        }
    }

    // Searches the tree for a course
    Node* searchNode(Node* node, const string& courseNumber) const {
        while (node != nullptr) {
            if (node->course.courseNumber == courseNumber)
                return node;

            if (courseNumber < node->course.courseNumber)
                node = node->left;
            else
                node = node->right;
        }
        return nullptr;
    }

    // Deletes all nodes in the tree
    void clear(Node* node) {
        if (node != nullptr) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() : root(nullptr) {}

    ~BinarySearchTree() {
        clear(root);
    }

    // Clears the tree so we can reload data
    void Clear() {
        clear(root);
        root = nullptr;
    }

    // Inserts a course into the BST
    void Insert(const Course& course) {
        if (root == nullptr)
            root = new Node(course);
        else
            addNode(root, course);
    }

    // Searches for a course
    Course Search(const string& courseNumber) const {
        Node* found = searchNode(root, normalizeCourseNumber(courseNumber));
        return (found != nullptr) ? found->course : Course{};
    }

    // Prints all courses sorted
    void InOrder() const {
        inOrder(root);
    }
};

// File loading


// Reads the CSV file and loads courses into the tree
static void loadCourses(const string& csvPath, BinarySearchTree& bst) {
    cout << "Loading CSV file " << csvPath << endl;

    ifstream file(csvPath);
    if (!file.is_open()) {
        cout << "Failed to open " << csvPath << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        vector<string> tokens = split(line, ',');
        if (tokens.size() < 2) continue;

        Course course;
        course.courseNumber = normalizeCourseNumber(tokens[0]);
        course.courseName = trim(tokens[1]);

        // Add prerequisites if they exist
        for (size_t i = 2; i < tokens.size(); ++i) {
            string prereq = normalizeCourseNumber(tokens[i]);
            if (!prereq.empty()) {
                course.prerequisites.push_back(prereq);
            }
        }

        bst.Insert(course);
    }

    file.close();
}


// Main Program


int main() {
    BinarySearchTree bst;
    string csvPath;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Enter file name: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, csvPath);

            bst.Clear();
            loadCourses(csvPath, bst);
            break;

        case 2:
            cout << "Here is a sample schedule:" << endl;
            bst.InOrder();
            break;

        case 3: {
            cout << "What course do you want to know about? ";
            string courseNumber;
            cin >> courseNumber;

            Course course = bst.Search(courseNumber);
            if (!course.courseNumber.empty())
                displayCourseDetails(course);
            else
                cout << "Course not found." << endl;
            break;
        }

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
        }

        cout << endl;
    }

    return 0;
}