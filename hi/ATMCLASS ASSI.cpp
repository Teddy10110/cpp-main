#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

/* ======== FULL-SCREEN BACKGROUND & SCREEN CLEAR ======== */

// colorCode examples:
// "0;37;44" = white text, blue background
// "0;30;46" = black text, cyan background
// "0;30;42" = black text, green background
void setColor(const string &code) {
    cout << "\033[" << code << "m";
}

void resetColor() {
    cout << "\033[0m";
}

// This creates a "full-screen" colored background and moves cursor to top-left
void fullScreenBackground(const string &colorCode) {
    setColor(colorCode);
    // Clear screen and move cursor to home
    cout << "\033[2J\033[H";
}

/* ================= STRUCT ================= */

struct User
{
    int pin;          // PIN or secret code
    int balance;
    string phone;     // unique phone number
};

/* ================= BASIC HELPER FUNCTIONS ================= */

int findUserByPin(User users[], int size, int pin)
{
    for (int i = 0; i < size; i++)
        if (users[i].pin == pin) return i;
    return -1;
}

int findUserByPhone(User users[], int size, const string &phone)
{
    for (int i = 0; i < size; i++)
        if (users[i].phone == phone) return i;
    return -1;
}

/* ================= RECEIPT HANDLING ================= */

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
        printReceipt(amount, balance, lang);
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

    u.balance -= amount;
    atmCash -= amount;

    cout << (lang == 1 ? "Please take your cash.\n"
             : "ገንዘብዎን ይውሰዱ።\n");

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

    cout << (lang == 1 ? "PIN changed successfully.\n"
             : "PIN ተቀይሯል።\n");
}

/* ================= MENU HANDLERS ================= */

void runCardSession(User users[], int size, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    cout << (lang == 1 ? "Please insert your card...\n" 
            : "እባክዎን ካርድዎን ያስገቡ...\n");

    int enteredCode;
    cout << (lang == 1 ? "PLEASE ENTER YOUR PIN: "
             : "PIN / ሚስጥር ቁጥር ያስገቡ: ");
    cin >> enteredCode;

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
            cout << "\n1. Balance\n2. Withdraw\n3. Change PIN\n4. Exit\nChoice: ";
        }
        else
        {
            cout << "\n1. ቀሪ ሂሳብ\n2. ገንዘብ ማውጣት\n3. PIN መቀየር\n4. መውጣት\nምርጫ: ";
        }

        cin >> choice;

        if (choice == 1)
        {
            showBalance(users[userIndex], lang, bgColor);
        }
        else if (choice == 2)
        {
            withdrawCash(users[userIndex], atmCash, lang, bgColor);
        }
        else if (choice == 3)
        {
            changePIN(users[userIndex], lang, bgColor);
        }
        else
        {
            break;
        }

        cout << (lang == 1 ? "\nAnother transaction? (Y/N): "
                 : "\nሌላ ግብይት? (Y/N): ");
        cin >> again;

    } while (again == 'Y' || again == 'y');

    fullScreenBackground(bgColor);
    cout << (lang == 1 ? "\nPlease take your card.\nThank you for using Ethiopia ATM.\n"
             : "\nካርድዎን ይውሰዱ።\nአገልግሎታችንን ስለተጠቀሙ እናመሰግናለን።\n");
}

void runPhoneBasedSession(User users[], int size, int &atmCash, int lang, const string &bgColor)
{
    fullScreenBackground(bgColor);

    string enteredPhone;
    int enteredCode;

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

    cout << (lang == 1 ? "Enter secret code: "
             : "ሚስጥር ቁጥር ያስገቡ: ");
    cin >> enteredCode;

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
            cout << "\n1. Balance\n2. Withdraw\n3. Exit\nChoice: ";
        }
        else
        {
            cout << "\n1. ቀሪ ሂሳብ\n2. ገንዘብ ማውጣት\n3. መውጣት\nምርጫ: ";
        }

        cin >> choice;

        if (choice == 1)
        {
            showBalance(users[userIndex], lang, bgColor);
        }
        else if (choice == 2)
        {
            withdrawCash(users[userIndex], atmCash, lang, bgColor);
        }
        else
        {
            break;
        }

        cout << (lang == 1 ? "\nAnother transaction? (Y/N): "
                 : "\nሌላ ግብይት? (Y/N): ");
        cin >> again;

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
    if (lang != 1) lang = 2; // default to Amharic if not 1
    return lang;
}

int chooseService(int lang, const string &bgColor)
{
    int service;
    fullScreenBackground(bgColor);

    if (lang == 1)
    {
        cout << "\nSelect Service\n";
        cout << "1. Card\n2. Cardless\n3. Telebirr\nChoice: ";
    }
    else
    {
        cout << "\nአገልግሎት ይምረጡ\n";
        cout << "1. ካርድ\n2. ያለ ካርድ\n3. ቴሌቢር\nምርጫ: ";
    }
    cin >> service;
    return service;
}
/* ================= MAIN ================= */

int main()
{
    // One background color for the whole app:
    string bgColor = "0;37;44"; // white text, blue background

    User users[5] =
    {
        {1111, 5000, "0911111111"},
        {2222, 7000, "0922222222"},
        {3333, 4000, "0933333333"},
        {4444, 9000, "0944444444"},
        {5555, 3000, "0955555555"}
    };

    int atmCash = 20000;

    int lang = chooseLanguage(bgColor);
    int service = chooseService(lang, bgColor);

    if (service == 1)
    {
        runCardSession(users, 5, atmCash, lang, bgColor);
    }
    else if (service == 2 || service == 3)
    {
        runPhoneBasedSession(users, 5, atmCash, lang, bgColor);
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