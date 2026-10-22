#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream> // For File Handling
#include <conio.h> // For PIN Masking

using namespace std;

/* ================= STRUCT ================= */

struct User
{
    int pin;
    int balance;
    string phone;
};
const string LOG_FILE = "transactions.txt";
const string DB_FILE = "atm_data.txt";
const int MAX_USERS = 100;

/* ================= NEW FUNCTIONALITIES ================= */

/* Saves user data so it isn't lost when the program closes*/
void saveToFile(User users[], int size)
{
    ofstream outFile("atm_data.txt");
    for (int i = 0; i < size; i++)
    {
        outFile << users[i].pin << " " << users[i].balance << " " << users[i].phone << endl;
    }
    outFile.close();
}

/* Loads user data from the file at startup*/
void loadFromFile(User users[], int size)
{
    ifstream inFile("atm_data.txt");
    if (inFile)
    {
        for (int i = 0; i < size; i++)
        {
            inFile >> users[i].pin >> users[i].balance >> users[i].phone;
        }
        inFile.close();
    }
}
void logTransaction(string phone, string action, int amount)
{
    ofstream log(LOG_FILE, ios::app);
    time_t now = time(0);
    char *dt = ctime(&now);
    string ts(dt);
    ts.pop_back(); /*Remove newline*/ 
    log << "[" << ts << "] User: " << phone << " | " << action << ": " << amount << " ETB" << endl;
    log.close();
}
int loadFromFile(User users[])
{
    ifstream inFile(DB_FILE);
    if (!inFile)
        return 0;
    int count = 0;
    while (count < MAX_USERS && inFile >> users[count].phone >> users[count].pin >> users[count].balance)
    {
        count++;
    }
    inFile.close();
    return count;
}

/* function to tell time*/

string getCurrentTime()
{
    time_t now = time(0);   /* Get current system time*/
    char *dt = ctime(&now); /*Convert to string format*/ 
    string timeStr(dt);

    /*Remove the newline character that ctime automatically adds*/ 
    if (!timeStr.empty() && timeStr.back() == '\n')
    {
        timeStr.pop_back();
    }
    return timeStr;
}

/*Security: Masks the PIN entry with asterisks (****)*/ 
int getMaskedPin()
{
    string pinStr = "";
    char ch;
    while (true)
    {
        ch = _getch();
        if (ch == 13)
            break; /*Enter key*/ 
        if (ch == 8)
        { /*Backspace*/ 
            if (pinStr.length() > 0)
            {
                pinStr.pop_back();
                cout << "\b \b";
            }
        }
        else if (isdigit(ch))
        {
            pinStr += ch;
            cout << "*";
        }
    }
    cout << endl;
    return (pinStr.empty()) ? 0 : stoi(pinStr);
}

/* ================= YOUR ORIGINAL UI FUNCTIONS ================= */

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

/* ================= YOUR ORIGINAL HELPERS ================= */

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

/* ================= YOUR ORIGINAL RECEIPT HANDLING ================= */

void printReceipt(int amount, int balance, int lang)
{
    if (lang == 1)
    {
        cout << "\n----- RECEIPT -----\n";
        cout << "Withdrawn Amount : " << amount << " ETB\n";
        cout << "Remaining Balance: " << balance << " ETB\n";
        cout << "-------------------\n";
    }
    else
    {
        cout << "\n----- ደረሰኝ -----\n";
        cout << "የተወጣ መጠን : " << amount << " ብር\n";
        cout << "ቀሪ ሂሳብ : " << balance << " ብር\n";
        cout << "-------------------\n";
    }
}

void askAndPrintReceipt(int amount, int balance, int lang)
{
    int receiptChoice;
    if (lang == 1)
    {
        cout << "\nDo you want a receipt?\n1. Yes\n2. No\nChoice: ";
    }
    else
    {
        cout << "\nደረሰኝ ይፈልጋሉ?\n1. እፈልጋለሁ\n2. አልፈልግም\nምርጫ: ";
    }
    cin >> receiptChoice;
    if (receiptChoice == 1)
        printReceipt(amount, balance, lang);
}

/* ================= UPDATED TRANSACTION FUNCTIONS ================= */

void withdrawCash(User &u, int &atmCash, int lang, const string &bgColor, User users[], int size)
{
    fullScreenBackground(bgColor);
    int amount;
    cout << (lang == 1 ? "ENTER AMOUNT (ETB): " : "መጠን ያስገቡ (ብር): ");

    /*Added Input Validation to prevent crashes*/ 
    if (!(cin >> amount))
    {
        cout << (lang == 1 ? "Invalid input!\n" : "የተሳሳተ ግቤት!\n");
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    if (amount > u.balance || amount <= 0 || amount > atmCash)
    {
        cout << (lang == 1 ? "Transaction not allowed.\n" : "ግብይቱ አይፈቀድም።\n");
        return;
    }

    u.balance -= amount;
    atmCash -= amount;
    logTransaction(u.phone, "WITHDRAW", amount);
    saveToFile(users, size); /*NEW: Saves change to file*/ 

    cout << (lang == 1 ? "Please take your cash.\n" : "ገንዘብዎን ይውሰዱ።\n");
    askAndPrintReceipt(amount, u.balance, lang);
}

void showBalance(const User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    if (lang == 1)
        cout << "Your balance is: " << u.balance << " ETB\n";
    else
        cout << "ቀሪ ሂሳብዎ: " << u.balance << " ብር\n";
}

void changePIN(User &u, int lang, const string &bgColor, User users[], int size)
{
    fullScreenBackground(bgColor);
    int oldPin;
    cout << (lang == 1 ? "Enter old PIN: " : "የቀድሞ PIN ያስገቡ: ");
    oldPin = getMaskedPin(); /*NEW: Masked input*/ 

    if (oldPin != u.pin)
    {
        cout << (lang == 1 ? "Wrong PIN.\n" : "የተሳሳተ PIN።\n");
        return;
    }

    cout << (lang == 1 ? "Enter new PIN: " : "አዲስ PIN ያስገቡ: ");
    u.pin = getMaskedPin();  /*NEW: Masked input*/ 
    saveToFile(users, size); /*NEW: Saves change to file*/ 
    cout << (lang == 1 ? "PIN changed successfully.\n" : "PIN ተቀይሯል።\n");
}

/* ================= YOUR ORIGINAL MENU HANDLERS ================= */

void runCardSession(User users[], int size, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "PLEASE ENTER YOUR PIN: " : "PIN ያስገቡ: ");
    int enteredCode = getMaskedPin(); /*NEW: Masked input*/ 

    int userIndex = findUserByPin(users, size, enteredCode);
    if (userIndex == -1)
    {
        cout << (lang == 1 ? "Wrong PIN.\n" : "የተሳሳተ PIN።\n");
        return;
    }

    int choice;
    do
    {
        fullScreenBackground(bgColor);
        if (lang == 1)
            cout << "\n1. Balance\n2. Withdraw\n3. Change PIN\n4. Exit\nChoice: ";
        else
            cout << "\n1. ቀሪ ሂሳብ\n2. ገንዘብ ማውጣት\n3. PIN መቀየር\n4. መውጣት\nምርጫ: ";
        cin >> choice;

        if (choice == 1)
            showBalance(users[userIndex], lang, bgColor);
        else if (choice == 2)
            withdrawCash(users[userIndex], atmCash, lang, bgColor, users, size);
        else if (choice == 3)
            changePIN(users[userIndex], lang, bgColor, users, size);
        else
            break;

        cout << (lang == 1 ? "\nAnother transaction? (1=Yes, 0=No): " : "\nሌላ ግብይት? (1=አዎ, 0=አይ): ");
        cin >> choice;
    } while (choice == 1);
}

void runPhoneBasedSession(User users[], int size, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    string enteredPhone;
    cout << (lang == 1 ? "Enter phone number: " : "የስልክ ቁጥር ያስገቡ: ");
    cin >> enteredPhone;

    int userIndex = findUserByPhone(users, size, enteredPhone);
    if (userIndex == -1)
    {
        cout << (lang == 1 ? "Not recognized.\n" : "አልተለየም።\n");
        return;
    }

    cout << (lang == 1 ? "Enter secret code: " : "ሚስጥር ቁጥር ያስገቡ: ");
    int enteredCode = getMaskedPin(); /*NEW: Masked input*/ 

    if (enteredCode == users[userIndex].pin)
    {
        withdrawCash(users[userIndex], atmCash, lang, bgColor, users, size);
    }
    else
    {
        cout << (lang == 1 ? "Wrong code.\n" : "የተሳሳተ ቁጥር።\n");
    }
}

/* ================= MAIN ================= */

int main()
{
    string bgColor = "0;37;44";
    User users[5] = {
        {1111, 5000, "0911111111"},
        {2222, 7000, "0922222222"},
        {3333, 4000, "0933333333"},
        {4444, 9000, "0944444444"},
        {5555, 3000, "0955555555"}};
    int userCount = loadFromFile(users);
    int atmCash = 20000;

    loadFromFile(users, 5); /*NEW: Load data from disk*/ 

    fullScreenBackground(bgColor);
    cout << "WELCOME TO ETHIOPIA ATM\n\n1. English\n2. አማርኛ\nChoice: ";
    int lang;
    cin >> lang;
    if (lang != 1)
        lang = 2;

    fullScreenBackground(bgColor);
    if (lang == 1)
        cout << "1. Card\n2. Cardless\n3. Tell Time\nChoice: ";
    else
        cout << "1. ካርድ\n2. ያለ ካርድ\nምርጫ: ";

    int service;
    cin >> service;
    if (service == 1)
        runCardSession(users, 5, atmCash, lang, bgColor);
    else if (service == 2)
        runPhoneBasedSession(users, 5, atmCash, lang, bgColor);
    else if (service == 3)
    {
        string timestump = getCurrentTime();
        cout << "\nThank you for using our bunk.\n\nThe time is: " << timestump << endl << "\n";
    }
    else if (service == 4)
    { /*Admin Creation*/ 
        int id, p;
        cout << "ADMIN ID: ";
        cin >> id;
        cout << "ADMIN PIN: ";
        cin >> p;
        if (id == 999 && p == 0000)
        {
            cout << "New Phone: ";
            cin >> users[userCount].phone;
            cout << "New PIN: ";
            cin >> users[userCount].pin;
            cout << "Deposit: ";
            cin >> users[userCount].balance;
            userCount++;
            saveToFile(users, userCount);
            logTransaction("ADMIN", "CREATED_ACC", 0);
            cout << "Account Created.\n";
        }
        cout << "\nPress Enter...";
        string s;
        cin.ignore();
        getline(cin, s);
    }

     resetColor();
    return 0;
}
