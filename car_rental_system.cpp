#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct User {
    string username;
    string password;
    double balance;
    bool hasBorrowedCar;
};

struct Car {
    string brand;
    int available;
};

vector<User> users;
vector<Car> cars;
string adminUsername, adminPassword;

vector<string> split(string line, char delimiter) {
    vector<string> parts;
    stringstream ss(line);
    string item;
    while (getline(ss, item, delimiter)) {
        parts.push_back(item);
    }
    return parts;
}

void loadAdmin() {
    ifstream file("admin.txt");
    if (file.is_open()) {
        getline(file, adminUsername);
        getline(file, adminPassword);
    } else {
        adminUsername = "admin";
        adminPassword = "admin123";
        ofstream out("admin.txt");
        out << adminUsername << endl << adminPassword << endl;
    }
}

void loadUsers() {
    users.clear();
    ifstream file("usersDB.txt");
    string line;
    while (getline(file, line)) {
        vector<string> data = split(line, ',');
        if (data.size() >= 4) {
            User u;
            u.username = data[0];
            u.password = data[1];
            u.balance = stod(data[2]);
            u.hasBorrowedCar = (data[3] == "1");
            users.push_back(u);
        }
    }
}

void loadCars() {
    cars.clear();
    ifstream file("carsDB.txt");
    string line;
    while (getline(file, line)) {
        vector<string> data = split(line, ',');
        if (data.size() >= 2) {
            Car c;
            c.brand = data[0];
            c.available = stoi(data[1]);
            cars.push_back(c);
        }
    }
}

void saveUsers() {
    ofstream file("usersDB.txt");
    for (auto u : users) {
        file << u.username << "," << u.password << "," << u.balance << "," << (u.hasBorrowedCar ? "1" : "0") << endl;
    }
}

void saveCars() {
    ofstream file("carsDB.txt");
    for (auto c : cars) {
        file << c.brand << "," << c.available << endl;
    }
}

bool adminLogin() {
    string uname, pass;
    cout << "Enter admin username: "<<endl;
    cin >> uname;
    cout << "Enter admin password: "<<endl;
    cin >> pass;
    return (uname == adminUsername && pass == adminPassword);
}

int userLogin() {
    string uname, pass;
    cout << "Enter username: ";
    cin >> uname;
    cout << "Enter password: ";
    cin >> pass;
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].username == uname && users[i].password == pass) {
            return i;
        }
    }
    return -1;
}

void registerUser() {
    User u;
    cout << "Enter new username: ";
    cin >> u.username;
    cout << "Enter new password: ";
    cin >> u.password;
    u.balance = 10500;
    u.hasBorrowedCar = false;

    for (auto user : users) {
        if (user.username == u.username) {
            cout << "Username already exists.\n";
            return;
        }
    }

    users.push_back(u);
    saveUsers();
    cout << "User registered successfully.\n";
}

void approveCarRequest() {
    string username, brand;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter car brand: ";
    cin >> brand;

    int userIndex = -1, carIndex = -1;
    for (int i = 0; i < users.size(); ++i)
        if (users[i].username == username) userIndex = i;

    for (int i = 0; i < cars.size(); ++i)
        if (cars[i].brand == brand) carIndex = i;

    if (userIndex == -1 || carIndex == -1 || cars[carIndex].available <= 0 || users[userIndex].balance < 5000) {
        cout << "Error: Invalid data or conditions not met.\n";
        return;
    }

    users[userIndex].hasBorrowedCar = true;
    users[userIndex].balance -= 5000;
    cars[carIndex].available--;

    saveUsers();
    saveCars();
    cout << "Car approved for user.\n";
}

void acceptReturnedCar() {
    string username, brand;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter car brand: ";
    cin >> brand;

    int userIndex = -1, carIndex = -1;
    for (int i = 0; i < users.size(); ++i)
        if (users[i].username == username) userIndex = i;

    for (int i = 0; i < cars.size(); ++i)
        if (cars[i].brand == brand) carIndex = i;

    if (userIndex == -1 || carIndex == -1 || !users[userIndex].hasBorrowedCar) {
        cout << "Error: Invalid return.\n";
        return;
    }

    users[userIndex].hasBorrowedCar = false;
    cars[carIndex].available++;

    saveUsers();
    saveCars();
    cout << "Car returned successfully.\n";
}

void changeAdminPassword() {
    string oldPass, newPass;
    cout << "Enter current password: ";
    cin >> oldPass;

    if (oldPass != adminPassword) {
        cout << "Incorrect password.\n";
        return;
    }

    cout << "Enter new password: ";
    cin >> newPass;
    adminPassword = newPass;

    ofstream file("admin.txt");
    file << adminUsername << endl << adminPassword;
    cout << "Password changed.\n";
}

void adminMenu() {
    int choice;
    do {
        cout << "\n--- ADMIN MENU ---\n";
        cout << "1. Register User\n2. Approve Car Request\n3. Accept Returned Car\n4. Change Password\n5. Logout\nChoose: ";
        cin >> choice;
        switch (choice) {
            case 1: registerUser(); break;
            case 2: approveCarRequest(); break;
            case 3: acceptReturnedCar(); break;
            case 4: changeAdminPassword(); break;
            case 5: cout << "Logging out...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 5);
}

void viewProfile(int index) {
    cout << "\n--- PROFILE ---\n";
    cout << "Username: " << users[index].username << endl;
    cout << "Balance: " << users[index].balance << " shillings\n";
    cout << "Has borrowed car: " << (users[index].hasBorrowedCar ? "Yes" : "No") << endl;
}

void requestCar(int index) {
    if (users[index].hasBorrowedCar) {
        cout << "You already have a car borrowed.\n";
        return;
    }

    string brand;
    cout << "Enter car brand to rent: ";
    cin >> brand;

    int carIndex = -1;
    for (int i = 0; i < cars.size(); ++i)
        if (cars[i].brand == brand) carIndex = i;

    if (carIndex == -1 || cars[carIndex].available <= 0 || users[index].balance < 5000) {
        cout << "Cannot rent this car.\n";
        return;
    }

    users[index].hasBorrowedCar = true;
    users[index].balance -= 5000;
    cars[carIndex].available--;

    saveUsers();
    saveCars();
    cout << "Car request successful.\n";
}

void returnCar(int index) {
    if (!users[index].hasBorrowedCar) {
        cout << "You haven't borrowed a car.\n";
        return;
    }

    string brand;
    cout << "Enter brand to return: ";
    cin >> brand;

    int carIndex = -1;
    for (int i = 0; i < cars.size(); ++i)
        if (cars[i].brand == brand) carIndex = i;

    if (carIndex == -1) {
        cout << "Car brand not found.\n";
        return;
    }

    users[index].hasBorrowedCar = false;
    cars[carIndex].available++;

    saveUsers();
    saveCars();
    cout << "Car returned.\n";
}

void changeUserPassword(int index) {
    string oldPass, newPass;
    cout << "Enter old password: ";
    cin >> oldPass;

    if (oldPass != users[index].password) {
        cout << "Incorrect password.\n";
        return;
    }

    cout << "Enter new password: ";
    cin >> newPass;
    users[index].password = newPass;
    saveUsers();
    cout << "Password changed.\n";
}

void userMenu(int index) {
    int choice;
    do {
        cout << "\n--- USER MENU ---\n";
        cout << "1. View Profile\n2. Request Car\n3. Return Car\n4. Change Password\n5. Logout\nChoose: ";
        cin >> choice;
        switch (choice) {
            case 1: viewProfile(index); break;
            case 2: requestCar(index); break;
            case 3: returnCar(index); break;
            case 4: changeUserPassword(index); break;
            case 5: cout << "Logging out...\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 5);
}

int main() {
    loadAdmin();
    loadUsers();
    loadCars();

    int choice;
    cout << "====== Car Rental System ======\n";
    cout << "1. Admin Login\n2. User Login\nChoose: ";
    cin >> choice;

    if (choice == 1) {
        if (adminLogin()) {
            cout << "Welcome, Admin!\n";
            adminMenu();
        } else {
            cout << "Invalid admin credentials.\n";
        }
    } else if (choice == 2) {
        int index = userLogin();
        if (index != -1) {
            cout << "Welcome, " << users[index].username << "!\n";
            userMenu(index);
        } else {
            cout << "Invalid username or password.\n";
        }
    } else {
        cout << "Invalid choice.\n";
    }

    return 0;
}
