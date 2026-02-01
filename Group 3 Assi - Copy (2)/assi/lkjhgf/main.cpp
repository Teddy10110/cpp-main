#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include <conio.h>
#include <iomanip>

using namespace std;

/* ======== SCREEN FUNCTIONS ======== */
void setColor(const string &code) { cout << "\033[" << code << "m"; }
void resetColor() { cout << "\033[0m"; }
void fullScreenBackground(const string &colorCode) {
    setColor(colorCode);
    cout << "\033[2J\033[H";
}

/* ================= STRUCT ================= */
struct User {
    string name;
    string accountNumber;
    int pin;
    int balance;
    string phone;
};

/* ================= CONSTANTS ================= */
const int DAILY_WITHDRAWAL_LIMIT = 20000;
const int MAX_USERS = 100;
const string ATM_CASH_FILE = "atmCash.txt";
const string ADMIN_FILE = "admin.txt";

/* ================= HELPER FUNCTIONS ================= */
int findUserByPin(User users[], int size, int pin) {
    for (int i = 0; i < size; i++)
        if (users[i].pin == pin) return i;
    return -1;
}

int findUserByAccountNumber(User users[], int size, const string &accountNumber) {
    for (int i = 0; i < size; i++)
        if (users[i].accountNumber == accountNumber) return i;
    return -1;
}

/* ================= MASKED PIN INPUT ================= */
string getMaskedInput(int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "Enter PIN: " : "PIN ያስገቡ: ");

    string input = "";
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' && !input.empty()) {
            input.pop_back();
            cout << "\b \b";
        }
        else if (isdigit(ch)) {
            input.push_back(ch);
            cout << "*";
        }
    }
    cout << endl;
    return input;
}

/* ================= FILE HANDLING ================= */
int readATMCash() {
    ifstream file(ATM_CASH_FILE);
    int cash = 20000;
    if (file.is_open()) { file >> cash; file.close(); }
    return cash;
}

void writeATMCash(int cash) {
    ofstream file(ATM_CASH_FILE);
    if (file.is_open()) { file << cash; file.close(); }
}

void loadUsersFromFiles(User users[], int &userCount) {
    userCount = 0;  // Reset counter before loading
    for (int i = 0; i < MAX_USERS; i++) {
        string filename = "account" + to_string(100001 + i) + ".txt";
        ifstream file(filename);
        if (file.is_open()) {
            User u;
            getline(file, u.name);
            getline(file, u.accountNumber);
            file >> u.pin >> u.balance;
            file.ignore();
            getline(file, u.phone);
            users[userCount++] = u;
            file.close();
        }
    }
}

void saveUserToFile(const User &user) {
    ofstream file("account" + user.accountNumber + ".txt");
    if (file.is_open()) {
        file << user.name << endl << user.accountNumber << endl;
        file << user.pin << endl << user.balance << endl << user.phone << endl;
        file.close();
    }
}

void logTransaction(const string &accountNumber, const string &transaction, int amount, int newBalance) {
    ofstream file("transactions" + accountNumber + ".txt", ios::app);
    if (file.is_open()) {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        file << (1900 + ltm->tm_year) << "-" << setw(2) << setfill('0')
             << (1 + ltm->tm_mon) << "-" << setw(2) << ltm->tm_mday << " "
             << setw(2) << ltm->tm_hour << ":" << setw(2) << ltm->tm_min << " - "
             << transaction << " - " << amount << " ETB - Balance " << newBalance << endl;
        file.close();
    }
}

/* ================= RECEIPT FUNCTIONS ================= */
void printReceipt(const User &user, const string &type, int amount, int balance, int lang) {
    if (lang == 1) {
        cout << "\n----- RECEIPT -----\nName: " << user.name << "\nAccount: "
             << user.accountNumber << "\nTransaction: " << type << "\nAmount: "
             << amount << " ETB\nRemaining Balance: " << balance << " ETB\n-------------------\n";
    } else {
        cout << "\n----- ደረሰኝ -----\nስም: " << user.name << "\nመለያ: "
             << user.accountNumber << "\nግብይት: " << type << "\nመጠን: "
             << amount << " ብር\nቀሪ ሂሳብ: " << balance << " ብር\n-------------------\n";
    }
}

void askAndPrintReceipt(const User &user, const string &type, int amount, int balance, int lang) {
    cout << (lang == 1 ? "\nDo you want a receipt?\n1. Yes\n2. No\nChoice: "
                  : "\nደረሰኝ ይፈልጋሉ?\n1. እፈልጋለሁ\n2. አልፈልግም\nምርጫ: ");
    int choice; cin >> choice;
    if (choice == 1) printReceipt(user, type, amount, balance, lang);
}

/* ================= TRANSACTION FUNCTIONS ================= */
bool withdrawCash(User &u, int &atmCash, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    int amount;
    cout << (lang == 1 ? "ENTER AMOUNT (ETB): " : "መጠን ያስገቡ (ብር): ");

    if (!(cin >> amount)) {
        cout << (lang == 1 ? "Invalid input!\n" : "የተሳሳተ ግቤት!\n");
        cin.clear(); cin.ignore(10000, '\n');
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.get();
        return true;  // Return to menu
    }

    if (atmCash <= 0) {
        cout << (lang == 1 ? "ATM out of service.\n" : "ATM ከአገልግሎት ውጭ ነው።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }
    if (amount > u.balance) {
        cout << (lang == 1 ? "Insufficient balance.\n" : "በቂ ቀሪ ሂሳብ የለም።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }
    if (amount <= 0 || amount > atmCash) {
        cout << (lang == 1 ? "Transaction not allowed.\n" : "ግብይቱ አይፈቀድም።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }
    if (amount > DAILY_WITHDRAWAL_LIMIT) {
        cout << (lang == 1 ? "Exceeds daily limit.\n" : "ዕለታዊ ከፍተኛ መጠን አልፏል።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }

    u.balance -= amount;
    atmCash -= amount;
    saveUserToFile(u);
    writeATMCash(atmCash);
    logTransaction(u.accountNumber, "Withdraw", amount, u.balance);

    cout << (lang == 1 ? "Please take your cash.\n" : "ገንዘብዎን ይውሰዱ።\n");
    askAndPrintReceipt(u, "Withdraw", amount, u.balance, lang);

    cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;
}

bool depositCash(User &u, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    int amount;
    cout << (lang == 1 ? "ENTER DEPOSIT AMOUNT (ETB): " : "የማስገቢያ መጠን ያስገቡ (ብር): ");

    if (!(cin >> amount) || amount <= 0) {
        cout << (lang == 1 ? "Invalid amount.\n" : "የተሳሳተ መጠን።\n");
        cin.clear(); cin.ignore(10000, '\n');
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.get();
        return true;
    }

    u.balance += amount;
    saveUserToFile(u);
    logTransaction(u.accountNumber, "Deposit", amount, u.balance);

    cout << (lang == 1 ? "Deposit successful.\n" : "ማስገቢያ ተሳክቷል።\n");
    askAndPrintReceipt(u, "Deposit", amount, u.balance, lang);

    cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;
}

bool showBalance(const User &u, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    if (lang == 1) {
        cout << "Name: " << u.name << "\nAccount: " << u.accountNumber
             << "\nBalance: " << u.balance << " ETB\n";
    } else {
        cout << "ስም: " << u.name << "\nመለያ: " << u.accountNumber
             << "\nቀሪ ሂሳብ: " << u.balance << " ብር\n";
    }
    cout << (lang == 1 ? "\nPress Enter to continue..." : "\nመቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;
}

bool showTransactionHistory(const User &u, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    ifstream file("transactions" + u.accountNumber + ".txt");

    cout << (lang == 1 ? "=== TRANSACTION HISTORY ===\n" : "=== የግብይት ታሪክ ===\n");

    if (file.is_open()) {
        vector<string> transactions;
        string line;
        while (getline(file, line)) transactions.push_back(line);
        file.close();

        int start = max(0, (int)transactions.size() - 10);
        for (int i = start; i < transactions.size(); i++)
            cout << transactions[i] << endl;
    } else {
        cout << (lang == 1 ? "No transactions found.\n" : "ግብይቶች አልተገኙም።\n");
    }

    cout << (lang == 1 ? "\nPress Enter to continue..." : "\nመቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;
}

bool transferFunds(User &sender, User users[], int userCount, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    string receiverAccount;
    int amount;

    cout << (lang == 1 ? "Enter receiver account number: " : "የተቀባይ መለያ ቁጥር ያስገቡ: ");
    cin >> receiverAccount;
    cout << (lang == 1 ? "Enter amount to transfer: " : "ለማስተላለፍ የሚፈልጉትን መጠን ያስገቡ: ");

    if (!(cin >> amount) || amount <= 0) {
        cout << (lang == 1 ? "Invalid input!\n" : "የተሳሳተ ግቤት!\n");
        cin.clear(); cin.ignore(10000, '\n');
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.get();
        return true;
    }

    if (receiverAccount == sender.accountNumber) {
        cout << (lang == 1 ? "Cannot transfer to yourself.\n" : "ራስዎን መላክ አይችሉም።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }
    if (amount > sender.balance) {
        cout << (lang == 1 ? "Insufficient balance.\n" : "በቂ ቀሪ ሂሳብ የለም።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }

    int receiverIndex = findUserByAccountNumber(users, userCount, receiverAccount);
    if (receiverIndex == -1) {
        cout << (lang == 1 ? "Receiver not found.\n" : "የተቀባይ አልተገኘም።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }

    sender.balance -= amount;
    users[receiverIndex].balance += amount;

    saveUserToFile(sender);
    saveUserToFile(users[receiverIndex]);
    logTransaction(sender.accountNumber, "Transfer to " + receiverAccount, amount, sender.balance);
    logTransaction(receiverAccount, "Transfer from " + sender.accountNumber, amount, users[receiverIndex].balance);

    cout << (lang == 1 ? "Transfer successful.\n" : "ማስተላለፍ ተሳክቷል።\n");
    askAndPrintReceipt(sender, "Transfer to " + receiverAccount, amount, sender.balance, lang);

    cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;
}

bool changePIN(User &u, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    int oldPin, newPin;

    cout << (lang == 1 ? "Enter old PIN: " : "የቀድሞ PIN ያስገቡ: ");
    cin >> oldPin;
    if (oldPin != u.pin) {
        cout << (lang == 1 ? "Wrong PIN.\n" : "የተሳሳተ PIN።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }

    cout << (lang == 1 ? "Enter new PIN: " : "አዲስ PIN ያስገቡ: ");
    cin >> newPin;

    u.pin = newPin;
    saveUserToFile(u);
    logTransaction(u.accountNumber, "PIN Change", 0, u.balance);

    cout << (lang == 1 ? "PIN changed successfully.\n" : "PIN ተቀይሯል።\n");
    cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;
}

/* ================= ADMIN FUNCTIONS ================= */
bool adminLogin(int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    string adminId = "admin", adminPin = "1234";

    ifstream file(ADMIN_FILE);
    if (file.is_open()) {
        getline(file, adminId);
        getline(file, adminPin);
        file.close();
    } else {
        ofstream newFile(ADMIN_FILE);
        newFile << "admin\n1234\n"; newFile.close();
    }

    string enteredId, enteredPin;
    cout << (lang == 1 ? "Admin Login\nEnter ID: " : "አስተዳዳሪ መግቢያ\nመታወቂያ ያስገቡ: ");
    cin >> enteredId;
    cout << (lang == 1 ? "Enter PIN: " : "PIN ያስገቡ: ");
    cin >> enteredPin;

    return (enteredId == adminId && enteredPin == adminPin);
}

bool registerNewUser(User users[], int &userCount, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    User newUser;

    cout << (lang == 1 ? "=== NEW USER REGISTRATION ===\n" : "=== አዲስ ተጠቃሚ ምዝገባ ===\n");
    cout << (lang == 1 ? "Enter name: " : "ስም ያስገቡ: ");
    cin.ignore(); getline(cin, newUser.name);
    cout << (lang == 1 ? "Enter phone number: " : "የስልክ ቁጥር ያስገቡ: ");
    getline(cin, newUser.phone);
    cout << (lang == 1 ? "Enter initial PIN: " : "የመጀመሪያ PIN ያስገቡ: ");
    cin >> newUser.pin;
    cout << (lang == 1 ? "Enter initial balance: " : "የመጀመሪያ ቀሪ ሂሳብ ያስገቡ: ");
    cin >> newUser.balance;

    newUser.accountNumber = to_string(100001 + userCount);
    saveUserToFile(newUser);

    ofstream transFile("transactions" + newUser.accountNumber + ".txt");
    transFile.close();

    users[userCount++] = newUser;

    cout << (lang == 1 ? "User registered!\nAccount: " : "ተጠቃሚ ተመዝግቧል!\nመለያ: ")
         << newUser.accountNumber << endl;
    cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;  // Return to admin menu
}

bool resetUserPIN(User users[], int userCount, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    string accountNumber;
    int newPin;

    cout << (lang == 1 ? "Enter user account number: " : "የተጠቃሚ መለያ ቁጥር ያስገቡ: ");
    cin >> accountNumber;
    cout << (lang == 1 ? "Enter new PIN: " : "አዲስ PIN ያስገቡ: ");
    cin >> newPin;

    int userIndex = findUserByAccountNumber(users, userCount, accountNumber);
    if (userIndex == -1) {
        cout << (lang == 1 ? "User not found.\n" : "ተጠቃሚ አልተገኘም።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }

    users[userIndex].pin = newPin;
    saveUserToFile(users[userIndex]);
    logTransaction(accountNumber, "PIN Reset by Admin", 0, users[userIndex].balance);

    cout << (lang == 1 ? "PIN reset successful.\n" : "PIN እንደገና ተቀድሯል።\n");
    cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;
}

bool loadATMCash(int &atmCash, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    int amount;

    cout << (lang == 1 ? "Current ATM cash: " : "የአሁኑ ATM ገንዘብ: ") << atmCash << endl;
    cout << (lang == 1 ? "Enter amount to load: " : "ለመጫን መጠን ያስገቡ: ");
    cin >> amount;

    if (amount <= 0) {
        cout << (lang == 1 ? "Invalid amount.\n" : "የተሳሳተ መጠን።\n");
        cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return true;
    }

    atmCash += amount;
    writeATMCash(atmCash);
    cout << (lang == 1 ? "ATM cash loaded.\n" : "ATM ገንዘብ ተጫኗል።\n");
    cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
    cin.ignore(); cin.get();
    return true;
}

/* ================= USER SESSIONS ================= */
bool runCardSession(User users[], int &userCount, int &atmCash, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "Please insert your card...\n" : "እባክዎን ካርድዎን ያስገቡ...\n");

    string enteredPin = getMaskedInput(lang, bgColor);
    int userIndex = findUserByPin(users, userCount, stoi(enteredPin));

    if (userIndex == -1) {
        cout << (lang == 1 ? "Wrong PIN.\n" : "የተሳሳተ PIN።\n");
        cout << (lang == 1 ? "Press Enter to return to main menu..." : "ወደ ዋና ምናሌ ለመመለስ አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return false;  // Return to main service selection
    }

    int choice;
    do {
        fullScreenBackground(bgColor);
        if (lang == 1) {
            cout << "\n=== MAIN MENU ===\n1. Balance Inquiry\n2. Withdraw Cash\n"
                 << "3. Deposit Cash\n4. Fund Transfer\n5. Transaction History\n"
                 << "6. Change PIN\n7. Exit to Main Menu\nChoice: ";
        } else {
            cout << "\n=== ዋና ምናሌ ===\n1. ቀሪ ሂሳብ ማየት\n2. ገንዘብ ማውጣት\n"
                 << "3. ገንዘብ ማስገቢያ\n4. ገንዘብ ማስተላለፍ\n5. የግብይት ታሪክ\n"
                 << "6. PIN መቀየር\n7. ወደ ዋና ምናሌ መመለስ\nምርጫ: ";
        }
        cin >> choice;

        bool continueSession = true;
        switch(choice) {
            case 1: continueSession = showBalance(users[userIndex], lang, bgColor); break;
            case 2: continueSession = withdrawCash(users[userIndex], atmCash, lang, bgColor); break;
            case 3: continueSession = depositCash(users[userIndex], lang, bgColor); break;
            case 4: continueSession = transferFunds(users[userIndex], users, userCount, lang, bgColor); break;
            case 5: continueSession = showTransactionHistory(users[userIndex], lang, bgColor); break;
            case 6: continueSession = changePIN(users[userIndex], lang, bgColor); break;
            case 7:
                fullScreenBackground(bgColor);
                cout << (lang == 1 ? "Please take your card.\nThank you!\n"
                                  : "ካርድዎን ይውሰዱ።\nአመሰግናለሁ!\n");
                cout << (lang == 1 ? "Returning to main menu..." : "ወደ ዋና ምናሌ በመመለስ ላይ...");
                cin.ignore(); cin.get();
                return false;  // Exit to main service selection
            default:
                cout << (lang == 1 ? "Invalid choice.\n" : "የተሳሳተ ምርጫ።\n");
                cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
                cin.ignore(); cin.get();
        }

    } while (true);  // Infinite loop until user chooses Exit (7)

    return false;  // Should never reach here
}

bool runCardlessSession(User users[], int &userCount, int &atmCash, int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    string accountNumber;

    cout << (lang == 1 ? "Cardless Login\nEnter Account Number: "
                      : "ያለ ካርድ መግቢያ\nየመለያ ቁጥር ያስገቡ: ");
    cin >> accountNumber;

    int userIndex = findUserByAccountNumber(users, userCount, accountNumber);
    if (userIndex == -1) {
        cout << (lang == 1 ? "Account not found.\n" : "መለያ አልተገኘም።\n");
        cout << (lang == 1 ? "Press Enter to return to main menu..." : "ወደ ዋና ምናሌ ለመመለስ አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return false;  // Return to main service selection
    }

    string enteredPin = getMaskedInput(lang, bgColor);
    if (stoi(enteredPin) != users[userIndex].pin) {
        cout << (lang == 1 ? "Wrong PIN.\n" : "የተሳሳተ PIN።\n");
        cout << (lang == 1 ? "Press Enter to return to main menu..." : "ወደ ዋና ምናሌ ለመመለስ አስገባን ይጫኑ...");
        cin.ignore(); cin.get();
        return false;  // Return to main service selection
    }

    int choice;
    do {
        fullScreenBackground(bgColor);
        if (lang == 1) {
            cout << "\n=== MAIN MENU ===\n1. Balance Inquiry\n2. Withdraw Cash\n"
                 << "3. Deposit Cash\n4. Fund Transfer\n5. Transaction History\n"
                 << "6. Exit to Main Menu\nChoice: ";
        } else {
            cout << "\n=== ዋና ምናሌ ===\n1. ቀሪ ሂሳብ ማየት\n2. ገንዘብ ማውጣት\n"
                 << "3. ገንዘብ ማስገቢያ\n4. ገንዘብ ማስተላለፍ\n5. የግብይት ታሪክ\n"
                 << "6. ወደ ዋና ምናሌ መመለስ\nምርጫ: ";
        }
        cin >> choice;

        bool continueSession = true;
        switch(choice) {
            case 1: continueSession = showBalance(users[userIndex], lang, bgColor); break;
            case 2: continueSession = withdrawCash(users[userIndex], atmCash, lang, bgColor); break;
            case 3: continueSession = depositCash(users[userIndex], lang, bgColor); break;
            case 4: continueSession = transferFunds(users[userIndex], users, userCount, lang, bgColor); break;
            case 5: continueSession = showTransactionHistory(users[userIndex], lang, bgColor); break;
            case 6:
                cout << (lang == 1 ? "Thank you!\n" : "አመሰግናለሁ!\n");
                cout << (lang == 1 ? "Returning to main menu..." : "ወደ ዋና ምናሌ በመመለስ ላይ...");
                cin.ignore(); cin.get();
                return false;  // Exit to main service selection
            default:
                cout << (lang == 1 ? "Invalid choice.\n" : "የተሳሳተ ምርጫ።\n");
                cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
                cin.ignore(); cin.get();
        }

    } while (true);  // Infinite loop until user chooses Exit (6)

    return false;  // Should never reach here
}

bool runAdminSession(User users[], int &userCount, int &atmCash, int lang, const string &bgColor) {
    int choice;
    do {
        fullScreenBackground(bgColor);
        if (lang == 1) {
            cout << "\n=== ADMIN MENU ===\n1. Register New User\n2. Reset User PIN\n"
                 << "3. Load ATM Cash\n4. View ATM Cash\n5. Exit to Main Menu\nChoice: ";
        } else {
            cout << "\n=== የአስተዳዳሪ ምናሌ ===\n1. አዲስ ተጠቃሚ ምዝገባ\n2. የተጠቃሚ PIN መቀየር\n"
                 << "3. ATM ገንዘብ መጫን\n4. ATM ገንዘብ ማየት\n5. ወደ ዋና ምናሌ መመለስ\nምርጫ: ";
        }
        cin >> choice;

        bool continueAdmin = true;
        switch(choice) {
            case 1: continueAdmin = registerNewUser(users, userCount, lang, bgColor); break;
            case 2: continueAdmin = resetUserPIN(users, userCount, lang, bgColor); break;
            case 3: continueAdmin = loadATMCash(atmCash, lang, bgColor); break;
            case 4:
                fullScreenBackground(bgColor);
                cout << (lang == 1 ? "Current ATM Cash: " : "የአሁኑ ATM ገንዘብ: ")
                     << atmCash << endl;
                cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
                cin.ignore(); cin.get();
                break;
            case 5:
                fullScreenBackground(bgColor);
                cout << (lang == 1 ? "Admin session ended.\n" : "የአስተዳዳሪ ክፍለ ጊዜ አብቅቷል።\n");
                cout << (lang == 1 ? "Returning to main menu..." : "ወደ ዋና ምናሌ በመመለስ ላይ...");
                cin.ignore(); cin.get();
                return false;  // Exit to main service selection
            default:
                cout << (lang == 1 ? "Invalid choice.\n" : "የተሳሳተ ምርጫ።\n");
                cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
                cin.ignore(); cin.get();
        }

    } while (true);  // Infinite loop until admin chooses Exit (5)

    return false;  // Should never reach here
}

/* ================= MAIN FLOW ================= */
int chooseLanguage(const string &bgColor) {
    fullScreenBackground(bgColor);
    cout << "WELCOME TO ETHIOPIA ATM MACHINE\n\n1. English\n2. አማርኛ\nChoose Language: ";
    int lang; cin >> lang;
    return (lang == 1) ? 1 : 2;
}

int chooseService(int lang, const string &bgColor) {
    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "\nSelect Service\n1. Card\n2. Cardless\n3. Admin Login\n4. Exit Program\nChoice: "
                      : "\nአገልግሎት ይምረጡ\n1. ካርድ\n2. ያለ ካርድ\n3. አስተዳዳሪ መግቢያ\n4. ፕሮግራም መውጫ\nምርጫ: ");
    int service; cin >> service;
    return service;
}

/* ================= MAIN ================= */
int main() {
    string bgColor = "0;37;44";

    while (true) {  // MAIN LOOP - keeps returning to language selection
        User users[MAX_USERS];
        int userCount = 0;

        // Reload fresh data each time
        loadUsersFromFiles(users, userCount);
        int atmCash = readATMCash();

        int lang = chooseLanguage(bgColor);
        int service;

        do {
            service = chooseService(lang, bgColor);

            if (service == 1) {
                // Card Session - returns false when user exits
                runCardSession(users, userCount, atmCash, lang, bgColor);
            } else if (service == 2) {
                // Cardless Session - returns false when user exits
                runCardlessSession(users, userCount, atmCash, lang, bgColor);
            } else if (service == 3) {
                // Admin Session
                if (adminLogin(lang, bgColor)) {
                    runAdminSession(users, userCount, atmCash, lang, bgColor);
                } else {
                    fullScreenBackground(bgColor);
                    cout << (lang == 1 ? "Admin login failed.\n" : "የአስተዳዳሪ መግቢያ አልተሳካም።\n");
                    cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
                    cin.ignore(); cin.get();
                }
            } else if (service == 4) {
                // Exit Program
                fullScreenBackground(bgColor);
                cout << (lang == 1 ? "Thank you for using Ethiopia ATM.\nGoodbye!\n"
                                  : "አገልግሎታችንን ስለተጠቀሙ እናመሰግናለን።\nቻይ!\n");
                resetColor();
                return 0;  // Exit program completely
            } else {
                fullScreenBackground(bgColor);
                cout << (lang == 1 ? "Invalid choice.\n" : "ትክክለኛ ምርጫ አይደለም።\n");
                cout << (lang == 1 ? "Press Enter to continue..." : "መቀጠል አስገባን ይጫኑ...");
                cin.ignore(); cin.get();
            }

        } while (service != 4);  // Inner loop for service selection

    }  // Outer loop returns to language selection

    return 0;
}
