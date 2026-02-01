#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <conio.h> // for getch()
using namespace std;

/* ================= FULL-SCREEN BACKGROUND & SCREEN CLEAR ================= */

void setColor(const string &code) 
{ cout << "\033[" << code << "m"; }
void resetColor() { cout << "\033[0m"; }
void fullScreenBackground(const string &colorCode)
{
    setColor(colorCode);
    cout << "\033[2J\033[H"; // clear screen & move cursor to top-left
}

/* ================= STRUCT ================= */
struct User
{
    string name;
    string accountNumber;
    int pin;
    int balance;
    string phone;
};
/*=================const==================*/
const int MAX_USERS = 100;

/* ================= FILE HELPERS ================= */
int findUserByPin(User users[], int size, int pin)
{
    for (int i = 0; i < size; i++)
        if (users[i].pin == pin)
            return i;
    return -1;
}
/* ================= FILE HELPERS ================= */
int loadATMCash()
{
    ifstream f("atmCash.txt");
    int cash = 0;
    if (f.is_open())
    {
        f >> cash;
        f.close();
    }
    return cash;
}
void saveATMCash(int cash)
{
    ofstream f("atmCash.txt");
    f << cash;
    f.close();
}
void amountAtmCash(char amount, int lang)
{
    ifstream f("atmCash.txt");
    f >> amount;
    f.close();
}
bool loadUser(const string &accountNumber, User &u)
{
    ifstream f("account" + accountNumber + ".txt");
    if (!f.is_open())
        return false;
    getline(f, u.name);
    getline(f, u.accountNumber);
    f >> u.pin >> u.balance;
    f.ignore();
    getline(f, u.phone);
    f.close();
    return true;
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

void saveUser(const User &u)
{
    ofstream f("account" + u.accountNumber + ".txt");
    f << u.name << "\n"
      << u.accountNumber << "\n"
      << u.pin << "\n"
      << u.balance << "\n"
      << u.phone << "\n";
    f.close();
}
void logTransaction(const string &accountNumber, const string &action, int amount, int balance, const string &otherAcc = "")
{
    ofstream f("transactions" + accountNumber + ".txt", ios::app);
    time_t now = time(0);
    char *dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0';
    f << dt << " - " << action;
    if (!otherAcc.empty())
        f << " " << otherAcc;
    f << " - " << amount << " ETB - Balance " << balance << "\n";
    f.close();
}

/* ================= HIDDEN PIN INPUT ================= */
int inputHiddenPIN()
{
    int pin = 0;
    char ch;
    while (true)
    {
        ch = getch();
        if (ch == '\r' || )
            break; // Enter
            cout << "\b \b";
        if (ch >= '0' && ch <= '9')
        {
            pin = pin * 10 + (ch - '0');
            cout << "*";
        }
    }
    cout << endl;
    return pin;
}

/* ================= RECEIPT ================= */
void printReceipt(const User &u, const string &action, int amount, int balance, int lang, const string &otherAcc = "")
{
    if (lang == 1)
    {
        cout << "\n----- RECEIPT -----\n"
             << "User: " << u.name << "\nAccount: " << u.accountNumber
             << "\nTransaction: " << action;
        if (!otherAcc.empty())
            cout << " " << otherAcc;
        cout << "\nAmount: " << amount << " ETB\nBalance: " << balance << " ETB\n-------------------\n";
    }
    else
    {
        cout << "\n----- ደረሰኝ -----\n"
             << "ተጠቃሚ: " << u.name << "\nመለያ ቁጥር: " << u.accountNumber
             << "\nግብይት: " << action;
        if (!otherAcc.empty())
            cout << " " << otherAcc;
        cout << "\nመጠን: " << amount << " ብር\nቀሪ ሂሳብ: " << balance << " ብር\n-------------------\n";
    }
}
void askAndPrintReceipt(const User &u, const string &action, int amount, int balance, int lang, const string &otherAcc = "")
{
    int choice;
    if (lang == 1)
    {
        cout << "\nDo you want receipt? \n1.Yes \n2.No: ";
    }
    else
    {
        cout << "\nደረሰኝ ይፈልጋሉ? \n1.አዎ \n2.አይ: ";
    }
    cin >> choice;
    if (choice == 1)
        printReceipt(u, action, amount, balance, lang, otherAcc);
}

/* ================= USER FUNCTIONS ================= */
void showBalance(const User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    if (lang == 1)
        cout << "\nUser: " << u.name << " | Account: " << u.accountNumber << "\nBalance: " << u.balance << " ETB\n";
    else
        cout << "\nተጠቃሚ: " << u.name << " | መለያ ቁጥር: " << u.accountNumber << "\nቀሪ ሂሳብ: " << u.balance << " ብር\n";
    cout << (lang == 1 ? "Enter any key to go back\n" : "ማንኛውንም ፊደል ይንኩ ለመመለስ።\n");
    getch();
}

void withdrawCash(User &u, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    int amount;
    if (lang == 1)
        cout << "Enter withdrawal amount: ";
    else
        cout << "መጠን ያስገቡ (ብር): ";
    cin >> amount;
    if (amount > u.balance)
    {
        cout << (lang == 1 ? "Insufficient balance\n" : "በቂ ቀሪ ሂሳብ የለም\n");
        return;
    }
    if (amount > atmCash)
    {
        cout << (lang == 1 ? "ATM cannot dispense this amount\n" : "ATM ከአገልግሎት ውጭ ነው\n");
        return;
    }
    u.balance -= amount;
    atmCash -= amount;
    saveUser(u);
    saveATMCash(atmCash);
    logTransaction(u.accountNumber, "Withdraw", amount, u.balance);
    askAndPrintReceipt(u, "Withdraw", amount, u.balance, lang);
    cout << (lang == 1 ? "Enter any key to go back\n" : "ማንኛውንም ፊደል ይንኩ ለመመለስ።\n");
    getch();
}

void depositCash(User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    int amount;
    if (lang == 1)
        cout << "Enter deposit amount: ";
    else
        cout << "መጠን ያስገቡ (ብር): ";
    cin >> amount;
    if (amount <= 0)
    {
        cout << "Invalid amount\n";
        return;
    }
    u.balance += amount;
    saveUser(u);
    logTransaction(u.accountNumber, "Deposit", amount, u.balance);
    askAndPrintReceipt(u, "Deposit", amount, u.balance, lang);
    cout << (lang == 1 ? "Enter any key to go back\n" : "ማንኛውንም ፊደል ይንኩ ለመመለስ።\n");
    getch();
}

void transferMoney(User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    string receiverAcc;
    cout << (lang == 1 ? "Enter receiver account number: " : "የተቀበለውን መለያ ቁጥር ያስገቡ: ");
    cin >> receiverAcc;
    User receiver;
    if (!loadUser(receiverAcc, receiver))
    {
        cout << (lang == 1 ? "Receiver not found\n" : "ተቀበለው ተጠቃሚ አልተገኘም\n");
        return;
    }
    int amount;
    cout << (lang == 1 ? "Enter amount to transfer: " : "መጠን ያስገቡ: ");
    cin >> amount;
    if (amount > u.balance)
    {
        cout << (lang == 1 ? "Insufficient balance\n" : "በቂ ቀሪ ሂሳብ የለም\n");
        return;
    }
    u.balance -= amount;
    receiver.balance += amount;
    saveUser(u);
    saveUser(receiver);
    logTransaction(u.accountNumber, "TransferTo", amount, u.balance, receiverAcc);
    logTransaction(receiver.accountNumber, "ReceivedFrom", amount, receiver.balance, u.accountNumber);
    askAndPrintReceipt(u, "Transfer", amount, u.balance, lang, receiverAcc);
    cout << (lang == 1 ? "Enter any key to go back\n" : "ማንኛውንም ፊደል ይንኩ ለመመለስ።\n");
    getch();
}

void showTransactions(const User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    ifstream f("transactions" + u.accountNumber + ".txt");
    string line;
    if (lang == 1)
        cout << "\nTransaction History:\n";
    else
        cout << "\nየግብይት ታሪክ:\n";
    while (getline(f, line))
        cout << line << "\n";
    f.close();
    cout << (lang == 1 ? "Enter any key to go back\n" : "ማንኛውንም ፊደል ይንኩ ለመመለስ።\n");
    getch();
}

void changePIN(User &u, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    int oldPin, newPin, confermPin;
    cout << (lang == 1 ? "Enter old PIN: " : "የቀድሞ PIN ያስገቡ: ");
    oldPin = inputHiddenPIN();
    if (oldPin != u.pin)
    {
        cout << (lang == 1 ? "Incorrect PIN\n" : "የተሳሳተ PIN\n");
        return;
    }
    int loop = 1;
    while (loop == 1)
    {
        cout << (lang == 1 ? "Enter new PIN: " : "አዲስ PIN ያስገቡ: ");
        newPin = inputHiddenPIN();
        cout << (lang == 1 ? "Conferm your new PIN: " : "አዲስ PIN ያስገቡ: ");
        confermPin = inputHiddenPIN();
        if (!(confermPin == newPin))
            cout << "You have entered diffent pins please try again." << endl;
        else
            loop = 2;
    }

    u.pin = newPin;
    saveUser(u);
    cout << (lang == 1 ? "PIN changed successfully, Enter any key to go back\n" : "PIN ተቀይሯል ማንኛውንም ፊደል ይንኩ ለመመለስ።\n");
    getch();
}

/* ================= cardSession ================= */
void runCardSession(User users[], int size, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    cout << (lang == 1 ? "Please insert your card...\n"
                       : "እባክዎን ካርድዎን ያስገቡ...\n");
                       
        int loop = 1;
        int count = 0;
        int pin;
        int userIndex = findUserByPin(users, size, pin);
        while (count == 0 || count == 1 || count == 2)
        {
            count++;
            cout << (lang == 1 ? "Enter PIN(You only have 3 chance): " : "PIN ያስገቡ(3 እድል ኣለዎት): ");
            int pin = inputHiddenPIN();
            if (pin != users[userIndex].pin)
            {
                cout << (lang == 1 ? "Incorrect PIN please try again\n" : "የተሳሳተ PIN\n") << endl;
            }
            else
                count = 4;
        }
        if (count == 3)
        {
            return ;
        }
        

    

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
            transferMoney(users[userIndex], lang, bgColor);
            break;
        case 5:
            showTransactions(users[userIndex], lang, bgColor);
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

/* ================= ADMIN FUNCTIONS ================= */
bool adminLogin(int lang)
{
    string id;
    cout << (lang == 1 ? "Enter Admin ID: " : "የአስተዳደር መለያ ያስገቡ: ");
    cin >> id;
    cout << (lang == 1 ? "Enter Admin PIN: " : "የአስተዳደር PIN ያስገቡ: ");
    int pinInt = inputHiddenPIN();
    string pin = to_string(pinInt);
    ifstream f("admin.txt");
    string fileId, filePin;
    if (f.is_open())
    {
        f >> fileId >> filePin;
        f.close();
    }
    else
    {
        // Create default admin if file doesn't exist
        ofstream newFile("admin.txt");
        newFile << "Admin@123\n2222\n";
        newFile.close();
        fileId = "Admin@123";
        filePin = "2222";
    }

    if (id == fileId && pin == filePin)
        return true;
    cout << (lang == 1 ? "Invalid admin credentials\n" : "የአስተዳደር መረጃ ትክክል አይደለም\n");
    return false;
}
void registerUser(int lang)
{
    User u;
    cin.ignore();
    cout << (lang == 1 ? "Enter name: " : "ስም ያስገቡ: ");
    getline(cin, u.name);
    cout << (lang == 1 ? "Enter phone: " : "ስልክ ቁጥር ያስገቡ: ");
    getline(cin, u.phone);
    cout << (lang == 1 ? "Enter initial PIN: " : "መነሻ PIN ያስገቡ: ");
    cin >> u.pin;
    cout << (lang == 1 ? "Enter initial balance: " : "መነሻ ቀሪ ሂሳብ ያስገቡ: ");
    cin >> u.balance;
    int accNum = rand() % 900000 + 100000;
    u.accountNumber = to_string(accNum);
    saveUser(u);
    ofstream f("transactions" + u.accountNumber + ".txt");
    f.close();
    cout << (lang == 1 ? "User registered. Account number: " : "ተጠቃሚ ተመዝግቧል መለያ ቁጥር: ") << u.accountNumber << "\n";
}
void resetUserPIN(int lang)
{
    string acc;
    cout << (lang == 1 ? "Enter user account number: " : "የተጠቃሚ መለያ ቁጥር ያስገቡ: ");
    cin >> acc;
    User u;
    if (!loadUser(acc, u))
    {
        cout << (lang == 1 ? "User not found\n" : "ተጠቃሚ አልተገኘም\n");
        return;
    }
    int newPin;
    cout << (lang == 1 ? "Enter new PIN: " : "አዲስ PIN ያስገቡ: ");
    cin >> newPin;
    u.pin = newPin;
    saveUser(u);
    cout << (lang == 1 ? "PIN reset successfully\n" : "PIN ተቀይሯል\n");
}
void loadATMCashAdmin(int lang)
{
    int cash;
    cout << (lang == 1 ? "Enter amount to add to ATM: " : "ወደ ATM መጨመር የሚፈልጉትን መጠን ያስገቡ: ");
    cin >> cash;
    int atmCash = loadATMCash();
    atmCash += cash;
    saveATMCash(atmCash);
    cout << (lang == 1 ? "ATM cash updated\n" : "ATM ገንዘብ ተማረከ\n");
}

/* ================= USER SESSION ================= */
void userSessionPhone(User &u, int &atmCash, int lang, const string &bgColor)
{
    int choice;
    char again;
    do
    {
        cout << "\n"
             << (lang == 1 ? "1.Balance \n2.Withdraw \n3.Exit\n" : "1.ቀሪ ሂሳብ \n2.ገንዘብ ማውጣት \n3.መውጣት\n")
             << (lang == 1 ? "Choice: " : "ምርጫ: ");
        cin >> choice;
        switch (choice)
        {
        case 1:
            showBalance(u, lang, bgColor);
            break;
        case 2:
            withdrawCash(u, atmCash, lang, bgColor);
            break;
        }
    } while (choice != 3);
}
void userSession(User &u, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);
    int choice;
    char again;
    do
    {
        cout << "\n"
             << (lang == 1 ? "1.Balance \n2.Withdraw \n3.Deposit \n4.Transfer \n5.Transactions \n6.Change PIN \n7.Exit\n" : "1.ቀሪ ሂሳብ \n2.ገንዘብ ማውጣት \n3.ክፍያ \n4.ማስተላለፊያ \n5.ግብይት ታሪክ \n6.PIN መቀየር \n7.መውጣት\n")
             << (lang == 1 ? "Choice: " : "ምርጫ: ");
        cin >> choice;
        switch (choice)
        {
        case 1:
            showBalance(u, lang, bgColor);
            break;
        case 2:
            withdrawCash(u, atmCash, lang, bgColor);
            break;
        case 3:
            depositCash(u, lang, bgColor);
            break;
        case 4:
            transferMoney(u, lang, bgColor);
            break;
        case 5:
            showTransactions(u, lang, bgColor);
            break;
        case 6:
            changePIN(u, lang, bgColor);
            break;
        }
    } while (choice != 7);
}

/* ================= MAIN ================= */
int main()
{
    srand(time(0));
    // ANSI SGR codes: format -> "<attr>;<fg>;<bg>"
    // Foreground (fg): 30=black,31=red,32=green,33=yellow,34=blue,35=magenta,36=cyan,37=white
    // Background (bg): 40=black,41=red,42=green,43=yellow,44=blue,45=magenta,46=cyan,47=white
    // Attribute (attr): 0=reset,1=bold/bright
    // Example: "1;32;40" -> bright green text on black background
    string bgColor = "0;36;47";
    fullScreenBackground(bgColor);

    int lang;
    cout << "WELCOME TO ETHIOPIA ATM MACHINE\n1. English\n2. አማርኛ\nChoose Language: ";
    cin >> lang;
    fullScreenBackground(bgColor);
    if (lang != 1)
        lang = 2;

    int serviceType;
   if (lang == 1)
    {
        
        cout << "\nSelect Service\n";
        cout << "1. Card\n2. Cardless\n3. Show Time\nChoice: ";
    }
    else
    {
        cout << "\nአገልግሎት ይምረጡ\n";
        cout << "1. ካርድ\n2. ያለ ካርድ\n3. ሰአት አሳይ\nምርጫ: ";
    }
    cin >> serviceType;

    int atmCash = loadATMCash();
    int size;
    User users[MAX_USERS];
    if (serviceType == 1)
    {
        fullScreenBackground(bgColor);
        runCardSession(users, size, atmCash, lang, bgColor);
    }

    if (serviceType == 2)
    {
        string acc;
        cout << (lang == 1 ? "Enter account number: " : "መለያ ቁጥር ያስገቡ: ");
        cin >> acc;
        User u;
        if (!loadUser(acc, u))
        {
            cout << (lang == 1 ? "User not found\n" : "ተጠቃሚ አልተገኘም\n");
            return 0;
        }
        int loop = 1;

        int count = 0;
        while (count == 0 || count == 1 || count == 2)
        {
            count++;
            cout << (lang == 1 ? "Enter PIN(You only have 3 chance): " : "PIN ያስገቡ(3 እድል ኣለዎት): ");
            int pin = inputHiddenPIN();
            if (pin != u.pin)
            {
                cout << (lang == 1 ? "Incorrect PIN please try again\n" : "የተሳሳተ PIN\n") << endl;
            }
            else
                count = 4;
        }
        if (count == 3)
        {
            return 0;
        }
        userSession(u, atmCash, lang, bgColor);
    }
    else if (serviceType == 3)
    {
        string timestump = getCurrentTime();
        cout << (lang == 1 ? "Thank You for Using Our Bank" : "የኛን ባንክ ስለተተቀሙ እናመሰግናለን። ");
    }

    else if (serviceType == 10)
    {
        if (!adminLogin(lang))
            return 0;
        int choice;
        do
        {
            cout << "\n"
                 << (lang == 1 ? "1.Register User \n2.Reset User PIN \n3.Amount of ATM \n4.Load ATM Cash \n5.Exit\n" : "1.አዲስ ተጠቃሚ መመዝገብ \n2.PIN መቀየር \3.የ ATM ብርር መተን። \n4.ATM ገንዘብ መጨመር \n5.መውጣት\n") << (lang == 1 ? "Choice: " : "ምርጫ: ");
            cin >> choice;
            switch (choice)
            {
            case 1:
                registerUser(lang);
                break;
            case 2:
                (lang);
                break;
            case 3:
                int numcash;
                amountAtmCash(numcash, lang);
                cout << numcash;
                cout << (lang == 1 ? "Enter any key to go back\n" : "ማንኛውንም ፊደል ይንኩ ለመመለስ።\n");
                getch();
                break;
            case 4:
                loadATMCashAdmin(lang);
                break;
            }
        } while (choice != 5);
    }
    else
        cout << (lang == 1 ? "Invalid selection\n" : "ትክክለኛ ምርጫ አልተመረጠም\n");

    resetColor();
    return 0;
}
