#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>   // For File Handling
#include <conio.h>   // For PIN Masking (_getch)

using namespace std;

/* ================= STRUCT ================= */
struct User {
    int pin;
    int balance;
    string phone;
};

/* ================= NEW: FILE HANDLING ================= */
// Saves current user data to a text file
void saveToFile(User users[], int size) {
    ofstream outFile("atm_data.txt");
    for (int i = 0; i < size; i++) {
        outFile << users[i].pin << " " << users[i].balance << " " << users[i].phone << endl;
    }
    outFile.close();
}

// Loads user data from the text file at startup
void loadFromFile(User users[], int size) {
    ifstream inFile("atm_data.txt");
    if (inFile) {
        for (int i = 0; i < size; i++) {
            inFile >> users[i].pin >> users[i].balance >> users[i].phone;
        }
        inFile.close();
    }
}

/* ================= NEW: PIN MASKING ================= */
// Replaces cin >> pin to show **** instead of numbers
int getMaskedPin() {
    string pinStr = "";
    char ch;
    while (true) {
        ch = _getch(); // Read character without echoing
        if (ch == 13) break; // Enter key
        if (ch == 8) { // Backspace logic
            if (pinStr.length() > 0) {
                pinStr.pop_back();
                cout << "\b \b";
            }
        } else {
            pinStr += ch;
            cout << "*";
        }
    }
    cout << endl;
    return stoi(pinStr); // Convert string to int
}

/* ================= UPDATED HELPERS ================= */

void setColor(const string &code) { cout << "\033[" << code << "m"; }
void resetColor() { cout << "\033[0m"; }
void fullScreenBackground(const string &colorCode) {
    setColor(colorCode);
    cout << "\033[2J\033[H";
}

int findUserByPin(User users[], int size, int pin) {
    for (int i = 0; i < size; i++)
        if (users[i].pin == pin) return i;
    return -1;
}

int findUserByPhone(User users[], int size, const string &phone) {
    for (int i = 0; i < size; i++)
        if (users[i].phone == phone) return i;
    return -1;
}

/* ================= TRANSACTION FUNCTIONS ================= */

void withdrawCash(User &u, int &atmCash, int lang, const string &bgColor, User users[], int size) {
    fullScreenBackground(bgColor);
    int amount;
    cout << (lang == 1 ? "ENTER AMOUNT (ETB): " : "መጠን ያስገቡ (ብር): ");

    // Improved Input Validation
    if (!(cin >> amount)) {
        cout << (lang == 1 ? "Invalid input!\n" : "የተሳሳተ ግቤት!\n");
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    if (amount > u.balance || amount <= 0 || amount > atmCash) {
        cout << (lang == 1 ? "Transaction denied.\n" : "ግብይቱ አልተፈቀደም።\n");
        return;
    }

    u.balance -= amount;
    atmCash -= amount;
    saveToFile(users, size); // Persistent save
    cout << (lang == 1 ? "Success! Please take your cash.\n" : "ተሳክቷል! ገንዘብዎን ይውሰዱ።\n");
}

void showBalance(const User &u, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "Balance: " : "ቀሪ ሂሳብ: ") << u.balance << " ETB\n";
}

void changePIN(User &u, int lang, const string &bgColor, User users[], int size) {
    fullScreenBackground(bgColor);
    int oldPin, newPin;
    cout << (lang == 1 ? "Old PIN: " : "የቀድሞ PIN: ");
    oldPin = getMaskedPin(); 

    if (oldPin != u.pin) {
        cout << (lang == 1 ? "Wrong PIN.\n" : "የተሳሳተ PIN።\n");
        return;
    }

    cout << (lang == 1 ? "New PIN: " : "አዲስ PIN: ");
    u.pin = getMaskedPin();
    saveToFile(users, size); // Persistent save
    cout << (lang == 1 ? "Changed!\n" : "ተቀይሯል!\n");
}

/* ================= MAIN FLOW ================= */

void runCardSession(User users[], int size, int &atmCash, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "ENTER PIN: " : "PIN ያስገቡ: ");
    int enteredCode = getMaskedPin();

    int userIndex = findUserByPin(users, size, enteredCode);
    if (userIndex == -1) {
        cout << (lang == 1 ? "Access Denied.\n" : "መግባት አልተፈቀደም።\n");
        return;
    }

    int choice;
    do {
        fullScreenBackground(bgColor);
        cout << (lang == 1 ? "\n1. Balance\n2. Withdraw\n3. Change PIN\n4. Exit\nChoice: " 
                            : "\n1. ቀሪ ሂሳብ\n2. ገንዘብ ማውጣት\n3. PIN መቀየር\n4. መውጣት\nምርጫ: ");
        cin >> choice;

        if (choice == 1) showBalance(users[userIndex], lang, bgColor);
        else if (choice == 2) withdrawCash(users[userIndex], atmCash, lang, bgColor, users, size);
        else if (choice == 3) changePIN(users[userIndex], lang, bgColor, users, size);
        else break;

        cout << (lang == 1 ? "\nAnother? (1 for Yes): " : "\nሌላ? (1 ለ አዎ): ");
        cin >> choice;
    } while (choice == 1);
}

int main() {
    string bgColor = "0;37;44";
    User users[5] = {
        {1111, 5000, "0911111111"},
        {2222, 7000, "0922222222"},
        {3333, 4000, "0933333333"},
        {4444, 9000, "0944444444"},
        {5555, 3000, "0955555555"}
    };
    int atmCash = 20000;

    loadFromFile(users, 5); // Load saved data if file exists

    fullScreenBackground(bgColor);
    cout << "1. English\n2. አማርኛ\nChoice: ";
    int lang; cin >> lang;
    if (lang != 1) lang = 2;

    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "1. Card\n2. Cardless\nChoice: " : "1. ካርድ\n2. ያለ ካርድ\nምርጫ: ");
    int service; cin >> service;

    if (service == 1) runCardSession(users, 5, atmCash, lang, bgColor);
    // Add logic for cardless as needed...

    resetColor();
    return 0;
}
