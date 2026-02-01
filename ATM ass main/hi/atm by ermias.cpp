#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include <conio.h> // For getch() and masked input
#include <iomanip> // For setw, setfill

using namespace std;

/* ======== FULL-SCREEN BACKGROUND & SCREEN CLEAR ======== */
void setColor(const string &code)
{
    cout << "\033[" << code << "m";
}

void resetColor()
{
    cout << "\033[0m";
}

void fullScreenBackground(const string &colorCode)
{
    setColor(colorCode);
    cout << "\033[2J\033[H";
}

/* ================= STRUCT ================= */
struct User
{
    string name;
    string accountNumber;
    int pin; // PIN or secret code
    int balance;
    string phone;
};

/* ================= CONSTANTS ================= */
const int DAILY_WITHDRAWAL_LIMIT = 20000; // Hard-coded as requested
const int MAX_USERS = 100;

/* ================= FILE PATHS ================= */
const string ATM_CASH_FILE = "atmCash.txt";
const string ADMIN_FILE = "admin.txt";

/* ================= BASIC HELPER FUNCTIONS ================= */
int findUserByPin(User users[], int size, int pin)
{
    for (int i = 0; i < size; i++)
        if (users[i].pin == pin)
            return i;
    return -1;
}

int findUserByPhone(User users[], int size, const string &phone)
{
    for (int i = 0; i < size; i++)
        if (users[i].phone == phone)
            return i;
    return -1;
}

int findUserByAccountNumber(User users[], int size, const string &accountNumber)
{
    for (int i = 0; i < size; i++)
        if (users[i].accountNumber == accountNumber)
            return i;
    return -1;
}

/* ================= MASKED PIN INPUT ================= */
string getMaskedInput(int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    if (lang == 1)
        cout << "Enter PIN: ";
    else
        cout << "PIN ያስገቡ: ";

    string input = "";
    char ch;

    while ((ch = _getch()) != '\r')
    { // Enter key
        if (ch == '\b' && !input.empty())
        { // Backspace
            input.pop_back();
            cout << "\b \b";
        }
        else if (isdigit(ch))
        {
            input.push_back(ch);
            cout << "*";
        }
    }
    cout << endl;
    return input;
}

/* ================= FILE HANDLING FUNCTIONS ================= */
int readATMCash()
{
    ifstream file(ATM_CASH_FILE);
    int cash = 20000; // Default if file doesn't exist
    if (file.is_open())
    {
        file >> cash;
        file.close();
    }
    return cash;
}

void writeATMCash(int cash)
{
    ofstream file(ATM_CASH_FILE);
    if (file.is_open())
    {
        file << cash;
        file.close();
    }
}

void loadUsersFromFiles(User users[], int &userCount)
{
    // Look for account files
    for (int i = 0; i < MAX_USERS; i++)
    {
        string filename = "account" + to_string(100001 + i) + ".txt";
        ifstream file(filename);

        if (file.is_open())
        {
            User u;
            getline(file, u.name);
            getline(file, u.accountNumber);
            file >> u.pin;
            file >> u.balance;
            file.ignore();
            getline(file, u.phone);

            users[userCount] = u;
            userCount++;
            file.close();
        }
    }
}

void saveUserToFile(const User &user)
{
    string filename = "account" + user.accountNumber + ".txt";
    ofstream file(filename);
    if (file.is_open())
    {
        file << user.name << endl;
        file << user.accountNumber << endl;
        file << user.pin << endl;
        file << user.balance << endl;
        file << user.phone << endl;
        file.close();
    }
}

void logTransaction(const string &accountNumber, const string &transaction, int amount, int newBalance)
{
    string filename = "transactions" + accountNumber + ".txt";
    ofstream file(filename, ios::app); // Append mode

    if (file.is_open())
    {
        // Get current date and time
        time_t now = time(0);
        tm *ltm = localtime(&now);

        file << (1900 + ltm->tm_year) << "-"
             << setw(2) << setfill('0') << (1 + ltm->tm_mon) << "-"
             << setw(2) << setfill('0') << ltm->tm_mday << " "
             << setw(2) << setfill('0') << ltm->tm_hour << ":"
             << setw(2) << setfill('0') << ltm->tm_min << " - "
             << transaction << " - "
             << amount << " ETB - Balance "
             << newBalance << endl;
        file.close();
    }
}

/* ================= RECEIPT HANDLING ================= */
void printReceipt(const User &user, const string &transactionType, int amount, int balance, int lang)
{
    if (lang == 1)
    {
        cout << "\n----- RECEIPT -----\n";
        cout << "Name: " << user.name << endl;
        cout << "Account: " << user.accountNumber << endl;
        cout << "Transaction: " << transactionType << endl;
        cout << "Amount: " << amount << " ETB\n";
        cout << "Remaining Balance: " << balance << " ETB\n";
        cout << "-------------------\n";
    }
    else
    {
        cout << "\n----- ደረሰኝ -----\n";
        cout << "ስም: " << user.name << endl;
        cout << "መለያ: " << user.accountNumber << endl;
        cout << "ግብይት: " << transactionType << endl;
        cout << "መጠን: " << amount << " ብር\n";
        cout << "ቀሪ ሂሳብ: " << balance << " ብር\n";
        cout << "-------------------\n";
    }
}

void askAndPrintReceipt(const User &user, const string &transactionType, int amount, int balance, int lang)
{
    int receiptChoice;

    if (lang == 1)
    {
        cout << "\nDo you want a receipt?\n";
        cout << "1. Yes\n";
        cout << "2. No\n";
        cout << "Choice: ";
    }
    else
    {
        cout << "\nደረሰኝ ይፈልጋሉ?\n";
        cout << "1. እፈልጋለሁ\n";
        cout << "2. አልፈልግም\n";
        cout << "ምርጫ: ";
    }

    cin >> receiptChoice;

    if (receiptChoice == 1)
        printReceipt(user, transactionType, amount, balance, lang);
}

/* ================= TRANSACTION FUNCTIONS ================= */
void withdrawCash(User &u, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    int amount;

    if (lang == 1)
        cout << "ENTER AMOUNT (ETB): ";
    else
        cout << "መጠን ያስገቡ (ብር): ";

    if (!(cin >> amount))
    {
        cout << (lang == 1 ? "Invalid input! Please enter a number.\n"
                           : "የተሳሳተ ግቤት! ቁጥር ያስገቡ።\n");
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    if (atmCash <= 0)
    {
        cout << (lang == 1 ? "ATM out of service.\n"
                           : "ATM ከአገልግሎት ውጭ ነው።\n");
        return;
    }

    if (amount > u.balance)
    {
        cout << (lang == 1 ? "Insufficient balance.\n"
                           : "በቂ ቀሪ ሂሳብ የለም።\n");
        return;
    }

    if (amount <= 0 || amount > atmCash)
    {
        cout << (lang == 1 ? "Transaction not allowed.\n"
                           : "ግብይቱ አይፈቀድም።\n");
        return;
    }

    if (amount > DAILY_WITHDRAWAL_LIMIT)
    {
        cout << (lang == 1 ? "Amount exceeds daily withdrawal limit.\n"
                           : "መጠኑ ዕለታዊ ከፍተኛ መጠን አልፏል።\n");
        return;
    }

    u.balance -= amount;
    atmCash -= amount;

    // Save to files
    saveUserToFile(u);
    writeATMCash(atmCash);
    logTransaction(u.accountNumber, "Withdraw", amount, u.balance);

    cout << (lang == 1 ? "Please take your cash.\n"
                       : "ገንዘብዎን ይውሰዱ።\n");

    askAndPrintReceipt(u, "Withdraw", amount, u.balance, lang);
}

void depositCash(User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    int amount;

    if (lang == 1)
        cout << "ENTER DEPOSIT AMOUNT (ETB): ";
    else
        cout << "የማስገቢያ መጠን ያስገቡ (ብር): ";

    if (!(cin >> amount))
    {
        cout << (lang == 1 ? "Invalid input! Please enter a number.\n"
                           : "የተሳሳተ ግቤት! ቁጥር ያስገቡ።\n");
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    if (amount <= 0)
    {
        cout << (lang == 1 ? "Invalid amount.\n"
                           : "የተሳሳተ መጠን።\n");
        return;
    }

    u.balance += amount;

    // Save to files
    saveUserToFile(u);
    logTransaction(u.accountNumber, "Deposit", amount, u.balance);

    cout << (lang == 1 ? "Deposit successful.\n"
                       : "ማስገቢያ ተሳክቷል።\n");

    askAndPrintReceipt(u, "Deposit", amount, u.balance, lang);
}

void showBalance(const User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    if (lang == 1)
    {
        cout << "Name: " << u.name << endl;
        cout << "Account Number: " << u.accountNumber << endl;
        cout << "Your balance is: " << u.balance << " ETB\n";
    }
    else
    {
        cout << "ስም: " << u.name << endl;
        cout << "መለያ ቁጥር: " << u.accountNumber << endl;
        cout << "ቀሪ ሂሳብዎ: " << u.balance << " ብር\n";
    }
}

void showTransactionHistory(const User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    string filename = "transactions" + u.accountNumber + ".txt";
    ifstream file(filename);

    if (lang == 1)
        cout << "=== TRANSACTION HISTORY ===\n";
    else
        cout << "=== የግብይት ታሪክ ===\n";

    if (file.is_open())
    {
        string line;
        vector<string> transactions;

        while (getline(file, line))
        {
            transactions.push_back(line);
        }
        file.close();

        // Show last 10 transactions
        int start = max(0, (int)transactions.size() - 10);
        for (int i = start; i < transactions.size(); i++)
        {
            cout << transactions[i] << endl;
        }
    }
    else
    {
        if (lang == 1)
            cout << "No transactions found.\n";
        else
            cout << "ግብይቶች አልተገኙም።\n";
    }
}

void transferFunds(User &sender, User users[], int userCount, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    string receiverAccount;
    int amount;

    if (lang == 1)
    {
        cout << "Enter receiver account number: ";
        cin >> receiverAccount;
        cout << "Enter amount to transfer: ";
    }
    else
    {
        cout << "የተቀባይ መለያ ቁጥር ያስገቡ: ";
        cin >> receiverAccount;
        cout << "ለማስተላለፍ የሚፈልጉትን መጠን ያስገቡ: ";
    }

    if (!(cin >> amount))
    {
        cout << (lang == 1 ? "Invalid input!\n"
                           : "የተሳሳተ ግቤት!\n");
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    if (receiverAccount == sender.accountNumber)
    {
        cout << (lang == 1 ? "Cannot transfer to yourself.\n"
                           : "ራስዎን መላክ አይችሉም።\n");
        return;
    }

    if (amount > sender.balance)
    {
        cout << (lang == 1 ? "Insufficient balance.\n"
                           : "በቂ ቀሪ ሂሳብ የለም።\n");
        return;
    }

    if (amount <= 0)
    {
        cout << (lang == 1 ? "Invalid amount.\n"
                           : "የተሳሳተ መጠን።\n");
        return;
    }

    int receiverIndex = findUserByAccountNumber(users, userCount, receiverAccount);
    if (receiverIndex == -1)
    {
        cout << (lang == 1 ? "Receiver account not found.\n"
                           : "የተቀባይ መለያ አልተገኘም።\n");
        return;
    }

    // Perform transfer
    sender.balance -= amount;
    users[receiverIndex].balance += amount;

    // Save to files
    saveUserToFile(sender);
    saveUserToFile(users[receiverIndex]);

    // Log transactions
    logTransaction(sender.accountNumber, "Transfer to " + receiverAccount, amount, sender.balance);
    logTransaction(receiverAccount, "Transfer from " + sender.accountNumber, amount, users[receiverIndex].balance);

    cout << (lang == 1 ? "Transfer successful.\n"
                       : "ማስተላለፍ ተሳክቷል።\n");

    askAndPrintReceipt(sender, "Transfer to " + receiverAccount, amount, sender.balance, lang);
}

void changePIN(User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    int oldPin, newPin;

    cout << (lang == 1 ? "Enter old PIN: "
                       : "የቀድሞ PIN ያስገቡ: ");
    cin >> oldPin;
    if (oldPin != u.pin)
    {
        cout << (lang == 1 ? "Wrong PIN.\n"
                           : "የተሳሳተ PIN።\n");
        return;
    }

    cout << (lang == 1 ? "Enter new PIN: "
                       : "አዲስ PIN ያስገቡ: ");
    cin >> newPin;

    u.pin = newPin;

    // Save to file
    saveUserToFile(u);
    logTransaction(u.accountNumber, "PIN Change", 0, u.balance);

    cout << (lang == 1 ? "PIN changed successfully.\n"
                       : "PIN ተቀይሯል።\n");
}

/* ================= ADMIN FUNCTIONS ================= */
bool adminLogin(int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    ifstream file(ADMIN_FILE);
    string adminId, adminPin;

    if (file.is_open())
    {
        getline(file, adminId);
        getline(file, adminPin);
        file.close();
    }
    else
    {
        // Create default admin if file doesn't exist
        ofstream newFile(ADMIN_FILE);
        newFile << "admin\n1234\n";
        newFile.close();
        adminId = "admin";
        adminPin = "1234";
    }

    string enteredId, enteredPin;

    if (lang == 1)
        cout << "Admin Login\nEnter ID: ";
    else
        cout << "አስተዳዳሪ መግቢያ\nመታወቂያ ያስገቡ: ";
    cin >> enteredId;

    if (lang == 1)
        cout << "Enter PIN: ";
    else
        cout << "PIN ያስገቡ: ";
    cin >> enteredPin;

    return (enteredId == adminId && enteredPin == adminPin);
}

void registerNewUser(User users[], int &userCount, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    User newUser;

    if (lang == 1)
    {
        cout << "=== NEW USER REGISTRATION ===\n";
        cout << "Enter name: ";
        cin.ignore();
        getline(cin, newUser.name);
        cout << "Enter phone number: ";
        getline(cin, newUser.phone);
        cout << "Enter initial PIN: ";
        cin >> newUser.pin;
        cout << "Enter initial balance: ";
        cin >> newUser.balance;
    }
    else
    {
        cout << "=== አዲስ ተጠቃሚ ምዝገባ ===\n";
        cout << "ስም ያስገቡ: ";
        cin.ignore();
        getline(cin, newUser.name);
        cout << "የስልክ ቁጥር ያስገቡ: ";
        getline(cin, newUser.phone);
        cout << "የመጀመሪያ PIN ያስገቡ: ";
        cin >> newUser.pin;
        cout << "የመጀመሪያ ቀሪ ሂሳብ ያስገቡ: ";
        cin >> newUser.balance;
    }

    // Generate account number (simple sequential)
    newUser.accountNumber = to_string(100001 + userCount);

    // Save to file
    saveUserToFile(newUser);

    // Create empty transaction file
    string transFilename = "transactions" + newUser.accountNumber + ".txt";
    ofstream transFile(transFilename);
    transFile.close();

    // Add to in-memory array
    users[userCount] = newUser;
    userCount++;

    if (lang == 1)
        cout << "User registered successfully!\nAccount Number: " << newUser.accountNumber << endl;
    else
        cout << "ተጠቃሚ ተመዝግቧል!\nመለያ ቁጥር: " << newUser.accountNumber << endl;
}

void resetUserPIN(User users[], int userCount, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    string accountNumber;
    int newPin;

    if (lang == 1)
    {
        cout << "Enter user account number: ";
        cin >> accountNumber;
        cout << "Enter new PIN: ";
        cin >> newPin;
    }
    else
    {
        cout << "የተጠቃሚ መለያ ቁጥር ያስገቡ: ";
        cin >> accountNumber;
        cout << "አዲስ PIN ያስገቡ: ";
        cin >> newPin;
    }

    int userIndex = findUserByAccountNumber(users, userCount, accountNumber);
    if (userIndex == -1)
    {
        cout << (lang == 1 ? "User not found.\n" : "ተጠቃሚ አልተገኘም።\n");
        return;
    }

    users[userIndex].pin = newPin;
    saveUserToFile(users[userIndex]);
    logTransaction(accountNumber, "PIN Reset by Admin", 0, users[userIndex].balance);

    cout << (lang == 1 ? "PIN reset successful.\n" : "PIN እንደገና ተቀድሯል።\n");
}

void loadATMCash(int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    int amount;

    if (lang == 1)
    {
        cout << "Current ATM cash: " << atmCash << " ETB\n";
        cout << "Enter amount to load: ";
    }
    else
    {
        cout << "የአሁኑ ATM ገንዘብ: " << atmCash << " ብር\n";
        cout << "ለመጫን የሚፈልጉትን መጠን ያስገቡ: ";
    }

    cin >> amount;

    if (amount <= 0)
    {
        cout << (lang == 1 ? "Invalid amount.\n" : "የተሳሳተ መጠን።\n");
        return;
    }

    atmCash += amount;
    writeATMCash(atmCash);

    cout << (lang == 1 ? "ATM cash loaded successfully.\n" : "ATM ገንዘብ ተጫኗል።\n");
}

/* ================= ADMIN MENU ================= */
void runAdminSession(User users[], int &userCount, int &atmCash, int lang, const string &bgColor)
{
    char again;
    int choice;

    do
    {
        fullScreenBackground(bgColor);

        if (lang == 1)
        {
            cout << "\n=== ADMIN MENU ===\n";
            cout << "1. Register New User\n";
            cout << "2. Reset User PIN\n";
            cout << "3. Load ATM Cash\n";
            cout << "4. View ATM Cash\n";
            cout << "5. Exit\n";
            cout << "Choice: ";
        }
        else
        {
            cout << "\n=== የአስተዳዳሪ ምናሌ ===\n";
            cout << "1. አዲስ ተጠቃሚ ምዝገባ\n";
            cout << "2. የተጠቃሚ PIN መቀየር\n";
            cout << "3. ATM ገንዘብ መጫን\n";
            cout << "4. ATM ገንዘብ ማየት\n";
            cout << "5. መውጣት\n";
            cout << "ምርጫ: ";
        }

        cin >> choice;

        switch (choice)
        {
        case 1:
            registerNewUser(users, userCount, lang, bgColor);
            break;
        case 2:
            resetUserPIN(users, userCount, lang, bgColor);
            break;
        case 3:
            loadATMCash(atmCash, lang, bgColor);
            break;
        case 4:
            fullScreenBackground(bgColor);
            if (lang == 1)
                cout << "Current ATM Cash: " << atmCash << " ETB\n";
            else
                cout << "የአሁኑ ATM ገንዘብ: " << atmCash << " ብር\n";
            break;
        case 5:
            return;
        default:
            cout << (lang == 1 ? "Invalid choice.\n" : "የተሳሳተ ምርጫ።\n");
        }

        cout << (lang == 1 ? "\nAnother admin operation? (Y/N): "
                           : "\nሌላ የአስተዳዳሪ አሰራር? (Y/N): ");
        cin >> again;

    } while (again == 'Y' || again == 'y');
}

/* ================= MENU HANDLERS ================= */
void runCardSession(User users[], int size, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    cout << (lang == 1 ? "Please insert your card...\n"
                       : "እባክዎን ካርድዎን ያስገቡ...\n");

    string enteredPinStr = getMaskedInput(lang, bgColor);
    int enteredCode = stoi(enteredPinStr);

    int userIndex = findUserByPin(users, size, enteredCode);

    if (userIndex == -1)
    {
        cout << (lang == 1 ? "Wrong PIN.\nPlease take your card.\n"
                           : "የተሳሳተ PIN።\nካርድዎን ይውሰዱ።\n");
        return;
    }

    char again;
    int choice;

    do
    {
        fullScreenBackground(bgColor);

        if (lang == 1)
        {
            cout << "\n=== MAIN MENU ===\n";
            cout << "1. Balance Inquiry\n";
            cout << "2. Withdraw Cash\n";
            cout << "3. Deposit Cash\n";
            cout << "4. Fund Transfer\n";
            cout << "5. Transaction History\n";
            cout << "6. Change PIN\n";
            cout << "7. Exit\n";
            cout << "Choice: ";
        }
        else
        {
            cout << "\n=== ዋና ምናሌ ===\n";
            cout << "1. ቀሪ ሂሳብ ማየት\n";
            cout << "2. ገንዘብ ማውጣት\n";
            cout << "3. ገንዘብ ማስገቢያ\n";
            cout << "4. ገንዘብ ማስተላለፍ\n";
            cout << "5. የግብይት ታሪክ\n";
            cout << "6. PIN መቀየር\n";
            cout << "7. መውጣት\n";
            cout << "ምርጫ: ";
        }

        cin >> choice;

        switch (choice)
        {
        case 1:
            showBalance(users[userIndex], lang, bgColor);
            break;
        case 2:
            withdrawCash(users[userIndex], atmCash, lang, bgColor);
            break;
        case 3:
            depositCash(users[userIndex], lang, bgColor);
            break;
        case 4:
            transferFunds(users[userIndex], users, size, lang, bgColor);
            break;
        case 5:
            showTransactionHistory(users[userIndex], lang, bgColor);
            break;
        case 6:
            changePIN(users[userIndex], lang, bgColor);
            break;
        case 7:
            break;
        default:
            cout << (lang == 1 ? "Invalid choice.\n" : "የተሳሳተ ምርጫ።\n");
        }

        if (choice != 7)
        {
            cout << (lang == 1 ? "\nAnother transaction? (Y/N): "
                               : "\nሌላ ግብይት? (Y/N): ");
            cin >> again;
        }
        else
        {
            again = 'N';
        }

    } while (again == 'Y' || again == 'y');

    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "\nPlease take your card.\nThank you for using Ethiopia ATM.\n"
                       : "\nካርድዎን ይውሰዱ።\nአገልግሎታችንን ስለተጠቀሙ እናመሰግናለን።\n");
}

void runPhoneBasedSession(User users[], int size, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    string enteredPhone;
    string enteredPinStr;

    cout << (lang == 1 ? "Enter phone number: "
                       : "የስልክ ቁጥር ያስገቡ: ");
    cin >> enteredPhone;

    int userIndex = findUserByPhone(users, size, enteredPhone);

    if (userIndex == -1)
    {
        cout << (lang == 1 ? "Phone number not recognized.\n"
                           : "የስልክ ቁጥር አልተለየም።\n");
        return;
    }

    enteredPinStr = getMaskedInput(lang, bgColor);
    int enteredCode = stoi(enteredPinStr);

    if (enteredCode != users[userIndex].pin)
    {
        cout << (lang == 1 ? "Wrong secret code.\n"
                           : "የተሳሳተ ሚስጥር ቁጥር።\n");
        return;
    }

    char again;
    int choice;
    do
    {
        fullScreenBackground(bgColor);

        if (lang == 1)
        {
            cout << "\n=== MAIN MENU ===\n";
            cout << "1. Balance Inquiry\n";
            cout << "2. Withdraw Cash\n";
            cout << "3. Deposit Cash\n";
            cout << "4. Fund Transfer\n";
            cout << "5. Transaction History\n";
            cout << "6. Exit\n";
            cout << "Choice: ";
        }
        else
        {
            cout << "\n=== ዋና ምናሌ ===\n";
            cout << "1. ቀሪ ሂሳብ ማየት\n";
            cout << "2. ገንዘብ ማውጣት\n";
            cout << "3. ገንዘብ ማስገቢያ\n";
            cout << "4. ገንዘብ ማስተላለፍ\n";
            cout << "5. የግብይት ታሪክ\n";
            cout << "6. መውጣት\n";
            cout << "ምርጫ: ";
        }

        cin >> choice;

        switch (choice)
        {
        case 1:
            showBalance(users[userIndex], lang, bgColor);
            break;
        case 2:
            withdrawCash(users[userIndex], atmCash, lang, bgColor);
            break;
        case 3:
            depositCash(users[userIndex], lang, bgColor);
            break;
        case 4:
            transferFunds(users[userIndex], users, size, lang, bgColor);
            break;
        case 5:
            showTransactionHistory(users[userIndex], lang, bgColor);
            break;
        case 6:
            break;
        default:
            cout << (lang == 1 ? "Invalid choice.\n" : "የተሳሳተ ምርጫ።\n");
        }

        if (choice != 6)
        {
            cout << (lang == 1 ? "\nAnother transaction? (Y/N): "
                               : "\nሌላ ግብይት? (Y/N): ");
            cin >> again;
        }
        else
        {
            again = 'N';
        }

    } while (again == 'Y' || again == 'y');

    cout << (lang == 1 ? "Thank you for using Ethiopia ATM.\n"
                       : "አገልግሎታችንን ስለተጠቀሙ እናመሰግናለን።\n");
}

/* ================= HIGH-LEVEL FLOW FUNCTIONS ================= */
int chooseLanguage(const string &bgColor)
{
    int lang;
    fullScreenBackground(bgColor);
    cout << "WELCOME TO ETHIOPIA ATM MACHINE\n\n";
    cout << "1. English\n2. አማርኛ\nChoose Language: ";
    cin >> lang;
    if (lang != 1)
        lang = 2; // default to Amharic if not 1
    return lang;
}

int chooseService(int lang, const string &bgColor)
{
    int service;
    fullScreenBackground(bgColor);

    if (lang == 1)
    {
        cout << "\nSelect Service\n";
        cout << "1. Card\n2. Cardless\n3. Telebirr\n4. Admin Login\nChoice: ";
    }
    else
    {
        cout << "\nአገልግሎት ይምረጡ\n";
        cout << "1. ካርድ\n2. ያለ ካርድ\n3. ቴሌቢር\n4. አስተዳዳሪ መግቢያ\nምርጫ: ";
    }
    cin >> service;
    return service;
}

/* ================= MAIN ================= */
int main()
{
    // ANSI SGR codes: format -> "<attr>;<fg>;<bg>"
    // Foreground (fg): 30=black,31=red,32=green,33=yellow,34=blue,35=magenta,36=cyan,37=white
    // Background (bg): 40=black,41=red,42=green,43=yellow,44=blue,45=magenta,46=cyan,47=white
    // Attribute (attr): 0=reset,1=bold/bright
    // Example: "1;32;40" -> bright green text on black background
    string bgColor = "1;34;47";

    User users[MAX_USERS];
    int userCount = 0;

    // Load existing users from files
    loadUsersFromFiles(users, userCount);

    // Load ATM cash from file
    int atmCash = readATMCash();

    int lang = chooseLanguage(bgColor);
    int service = chooseService(lang, bgColor);

    if (service == 1)
    {
        runCardSession(users, userCount, atmCash, lang, bgColor);
    }
    else if (service == 2 || service == 3)
    {
        runPhoneBasedSession(users, userCount, atmCash, lang, bgColor);
    }
    else if (service == 4)
    {
        if (adminLogin(lang, bgColor))
        {
            runAdminSession(users, userCount, atmCash, lang, bgColor);
        }
        else
        {
            fullScreenBackground(bgColor);
            cout << (lang == 1 ? "Admin login failed.\n" : "የአስተዳዳሪ መግቢያ አልተሳካም።\n");
        }
    }
    else
    {
        fullScreenBackground(bgColor);
        if (lang == 1)
            cout << "Invalid service choice.\n";
        else
            cout << "ትክክለኛ አገልግሎት አልተመረጠም።\n";
    }

    resetColor();
    return 0;
}