#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

struct User
{
    int pin;
    int balance;
    string phone;
    int failedAttempts = 0;
    bool isLocked = false;
};

const string DB_FILE = "database.txt";
const string LOG_FILE = "transactions.txt";
const int MAX_USERS = 100;

/* ================= UTILITY & UI ================= */

void fullScreenBackground(const string &colorCode)
{
    cout << "\033[" << colorCode << "m\033[2J\033[H";
}

// Logs actions to a text file with a timestamp
void logTransaction(string phone, string action, int amount)
{
    ofstream log(LOG_FILE, ios::app);
    time_t now = time(0);
    char *dt = ctime(&now);
    string ts(dt);
    ts.pop_back(); // Remove newline
    log << "[" << ts << "] User: " << phone << " | " << action << ": " << amount << " ETB" << endl;
    log.close();
}

// Simple PIN Masking (Hides input by coloring text same as background or using simple logic)
int getMaskedPIN(int lang)
{
    cout << (lang == 1 ? "ENTER PIN: " : "PIN ያስገቡ: ");
    // To truly hide input in standard C++ without external libs:
    cout << "\033[8m"; // Set text to invisible
    int p;
    cin >> p;
    cout << "\033[0m\033[0;37;44m"; // Reset to white on blue
    return p;
}

/* ================= FILE SYSTEM ================= */

void saveToFile(User users[], int count)
{
    ofstream outFile(DB_FILE);
    for (int i = 0; i < count; i++)
    {
        outFile << users[i].phone << " " << users[i].pin << " " << users[i].balance << " " << users[i].isLocked << endl;
    }
    outFile.close();
}

int loadFromFile(User users[])
{
    ifstream inFile(DB_FILE);
    if (!inFile)
        return 0;
    int count = 0;
    while (count < MAX_USERS && inFile >> users[count].phone >> users[count].pin >> users[count].balance >> users[count].isLocked)
    {
        count++;
    }
    inFile.close();
    return count;
}

/* ================= TRANSACTION LOGIC ================= */

void withdrawCash(User &u, int &atmCash, int lang, User allUsers[], int totalUsers)
{
    if (u.isLocked)
    {
        cout << (lang == 1 ? "ACCOUNT LOCKED. Contact Admin.\n" : "መለያዎ ተቆልፏል። አስተዳዳሪውን ያነጋግሩ።\n");
        return;
    }

    int amount;
    cout << (lang == 1 ? "AMOUNT: " : "መጠን: ");
    cin >> amount;

    if (amount > 0 && amount <= u.balance && amount <= atmCash)
    {
        u.balance -= amount;
        atmCash -= amount;
        logTransaction(u.phone, "WITHDRAW", amount);
        saveToFile(allUsers, totalUsers);
        cout << (lang == 1 ? "Success!\n" : "ተሳክቷል!\n");
    }
    else
    {
        cout << (lang == 1 ? "Transaction Failed.\n" : "ግብይቱ አልተሳካም።\n");
    }
    cout << "Press Enter...";
    string s;
    cin.ignore();
    getline(cin, s);
}

/* ================= MAIN PROGRAM ================= */

int main()
{
    string bgColor = "0;37;44";
    User users[MAX_USERS];
    int userCount = loadFromFile(users);
    int atmCash = 100000;

    while (true)
    {
        fullScreenBackground(bgColor);
        cout << "--- ETHIOPIA ATM SECURITY PLUS ---\n1. English\n2. አማርኛ\n3. Exit\nChoice: ";
        int lang;
        cin >> lang;
        if (lang == 3)
            break;

        fullScreenBackground(bgColor);
        cout << "1. Card/Login\n2. Create Account (Admin)\nChoice: ";
        int service;
        cin >> service;

        if (service == 1)
        {
            string ph;
            cout << "Phone: ";
            cin >> ph;
            int idx = -1;
            for (int i = 0; i < userCount; i++)
                if (users[i].phone == ph)
                    idx = i;

            if (idx == -1)
            {
                cout << "User not found.\n";
            }
            else if (users[idx].isLocked)
            {
                cout << "This account is LOCKED due to failed attempts.\n";
            }
            else
            {
                int inputPin = getMaskedPIN(lang);
                if (inputPin == users[idx].pin)
                {
                    users[idx].failedAttempts = 0;
                    withdrawCash(users[idx], atmCash, lang, users, userCount);
                }
                else
                {
                    users[idx].failedAttempts++;
                    cout << "WRONG PIN. Attempts: " << users[idx].failedAttempts << "/3\n";
                    if (users[idx].failedAttempts >= 3)
                    {
                        users[idx].isLocked = true;
                        saveToFile(users, userCount);
                        cout << "MAX ATTEMPTS REACHED. ACCOUNT LOCKED.\n";
                    }
                }
            }
            cout << "\nPress Enter...";
            string p;
            cin.ignore();
            getline(cin, p);
        }
        else if (service == 2)
        { // Admin Creation
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
                users[userCount].isLocked = false;
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
    }
    return 0;
}