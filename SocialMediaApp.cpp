#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

// Exception class for handling errors in DynamicList
class DynamicListException : public exception {
private:
    string message;

public:
    DynamicListException(const string& msg) : message(msg) {}
    virtual const char* what() const throw() {
        return message.c_str();
    }
};

// DynamicList class
template<typename T>
class DynamicList {
private:
    T* data;
    size_t capacity;
    size_t size;

    void resize(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < size; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }

public:
    DynamicList() : data(nullptr), capacity(0), size(0) {}

    DynamicList(const DynamicList& other) : data(nullptr), capacity(0), size(0) {
        *this = other;
    }

    ~DynamicList() {
        delete[] data;
    }

    DynamicList& operator=(const DynamicList& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            size = other.size;
            data = new T[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    void push_back(const T& value) {
        if (size >= capacity) {
            resize(capacity == 0 ? 1 : 2 * capacity);
        }
        data[size++] = value;
    }

    void pop_back() {
        if (size > 0) {
            --size;
        }
    }

    T& operator[](size_t index) {
        if (index >= size) {
            throw DynamicListException("Index out of bounds");
        }
        return data[index];
    }

    const T& operator[](size_t index) const {
        if (index >= size) {
            throw DynamicListException("Index out of bounds");
        }
        return data[index];
    }

    size_t getSize() const {
        return size;
    }
};

// User structure for social media
struct User {
    string name;
    string username;
    string password;
    DynamicList<string> notifications; // Moved notifications member here

    User() {}

    User(const string& n, const string& u, const string& p) : name(n), username(u), password(p) {}
};

// Social Media Application class
class SocialMediaApp {
private:
    DynamicList<User> users;
    string currentUserFile; // File to save/load current user's data
    User* currentUser = nullptr;

    void loadInitialUsers(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Failed to open initial users file");
        }

        string line;
while (getline(file, line)) {
    stringstream ss(line);
    string name, username, password;
    getline(ss, name, ',');
    getline(ss, username, ',');
    getline(ss, password, ',');

    User newUser(name, username, password); // Create a new User object
    newUser.notifications = DynamicList<string>(); // Initialize notifications list
    users.push_back(newUser);
}
file.close();

    }

    void saveCurrentUser() {
        if (currentUser == nullptr) {
            throw runtime_error("No user logged in");
        }

        ofstream file(currentUserFile);
        if (!file.is_open()) {
            throw runtime_error("Failed to open current user file for saving");
        }

        file << currentUser->name << "," << currentUser->username << "," << currentUser->password << endl;
        file << "Notifications:" << endl;
        for (size_t i = 0; i < currentUser->notifications.getSize(); ++i) {
            file << currentUser->notifications[i] << endl;
        }
        file.close();
    }

    void loadCurrentUser(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Failed to open current user file");
        }

        string line;
        while (getline(file, line)) {
            currentUser->notifications.push_back(line);
        }
        file.close();
    }

public:
    SocialMediaApp(const string& initialUsersFile, const string& currentUserFile)
        : currentUserFile(currentUserFile) {
        loadInitialUsers(initialUsersFile);
    }

    void signUp(const string& name, const string& username, const string& password) {
        for (size_t i = 0; i < users.getSize(); ++i) {
            if (users[i].username == username) {
                throw runtime_error("Username already exists");
            }
        }
        User newUser(name, username, password); // Create a new User object
        newUser.notifications = DynamicList<string>(); // Initialize notifications list
        users.push_back(newUser);
        ofstream file("initial_users.txt", ios::app);
        if (!file.is_open()) {
            throw runtime_error("Failed to open initial users file for appending");
        }
        file << name << "," << username << "," << password << endl;
        file.close();
    }

    void logIn(const string& username, const string& password) {
        // Check if the user is among the existing users
        for (size_t i = 0; i < users.getSize(); ++i) {
            if (users[i].username == username && users[i].password == password) {
                currentUser = &users[i];
                if (ifstream(currentUserFile)) {
                    loadCurrentUser(currentUserFile);
                }
                cout << "Logged in successfully as " << currentUser->username << endl;
                return;
            }
        }
        throw runtime_error("Invalid username or password");
    }

    void logOut() {
        if (currentUser == nullptr) {
            throw runtime_error("No user logged in");
        }
        saveCurrentUser();
        currentUser = nullptr;
        cout << "Logged out successfully" << endl;
    }

    void createPost(const string& postText) {
        if (currentUser == nullptr) {
            throw runtime_error("No user logged in");
        }
        currentUser->notifications.push_back("You created a post: " + postText);
        saveCurrentUser();
        cout << "Post created successfully" << endl;
    }

    void sendFollowRequest(const string& username) {
        if (currentUser == nullptr) {
            throw runtime_error("No user logged in");
        }
        for (size_t i = 0; i < users.getSize(); ++i) {
            if (users[i].username == username) {
                users[i].notifications.push_back(currentUser->username + " followed you");
                cout << "Follow request sent to " << username << endl;
                return;
            }
        }
        throw runtime_error("User not found");
    }

    void viewNotifications() {
        if (currentUser == nullptr) {
            throw runtime_error("No user logged in");
        }
        cout << "Notifications:" << endl;
        for (size_t i = 0; i < currentUser->notifications.getSize(); ++i) {
            cout << currentUser->notifications[i] << endl;
        }
    }
};

int main() {
    SocialMediaApp app("initial_users.txt", "current_user.txt");

    int choice;
    string username, password;

    while (true) {
        cout << "Select one of the following options:" << endl;
        cout << "1- Sign Up" << endl;
        cout << "2- Log In" << endl;
        cout << "3- Log Out" << endl;
        cout << "4- Create a Post" << endl;
        cout << "5- Send Follow Request" << endl;
        cout << "6- View Notifications" << endl;
        cout << "7- Exit" << endl;

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: // Sign Up
                {
                    string name;
                    cout << "Enter your name: ";
                    cin >> name;
                    cout << "Enter your username: ";
                    cin >> username;
                    cout << "Enter your password: ";
                    cin >> password;
                    try {
                        app.signUp(name, username, password);
                        cout << "Sign up successful!" << endl;
                    } catch (const exception& e) {
                        cerr << "Error: " << e.what() << endl;
                    }
                }
                break;

            case 2: // Log In
                {
                    cout << "Enter your username: ";
                    cin >> username;
                    cout << "Enter your password: ";
                    cin >> password;
                    try {
                        app.logIn(username, password);
                    } catch (const exception& e) {
                        cerr << "Error: " << e.what() << endl;
                    }
                }
                break;

            case 3: // Log Out
                try {
                    app.logOut();
                } catch (const exception& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;

            case 4: // Create a Post
                {
                    string postText;
                    cout << "Enter your post text: ";
                    cin.ignore(); // Ignore newline character left in the buffer
                    getline(cin, postText);
                    try {
                        app.createPost(postText);
                    } catch (const exception& e) {
                        cerr << "Error: " << e.what() << endl;
                    }
                }
                break;

            case 5: // Send Follow Request
                {
                    string followUsername;
                    cout << "Enter username to follow: ";
                    cin >> followUsername;
                    try {
                        app.sendFollowRequest(followUsername);
                    } catch (const exception& e) {
                        cerr << "Error: " << e.what() << endl;
                    }
                }
                break;

            case 6: // View Notifications
                try {
                    app.viewNotifications();
                } catch (const exception& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;

            case 7: // Exit
                return 0;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
