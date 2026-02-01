#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <conio.h> // for getch()
using namespace std;

/* ================= FULL-SCREEN BACKGROUND & SCREEN CLEAR ================= */

void setColor(const string &code) { cout << "\033[" << code << "m"; }
void resetColor() { cout << "\033[0m"; }
void fullScreenBackground(const string &colorCode) {
    setColor(colorCode);
    cout << "\033[2J\033[H"; // clear screen & move cursor to top-left
}

/* ================= STRUCT ================= */
struct User {
    string name;
    string accountNumber;
    int pin;
    int balance;
    string phone;
};

/* ================= FILE HELPERS ================= */
int loadATMCash() {
    ifstream f("atmCash.txt");
    int cash = 0; if (f.is_open()) { f >> cash; f.close(); }
    return cash;
}
void saveATMCash(int cash) {
    ofstream f("atmCash.txt"); f << cash; f.close();
}
bool loadUser(const string &accountNumber, User &u) {
    ifstream f("account" + accountNumber + ".txt");
    if (!f.is_open()) return false;
    getline(f, u.name);
    getline(f, u.accountNumber);
    f >> u.pin >> u.balance; f.ignore();
    getline(f, u.phone);
    f.close(); return true;
}
void saveUser(const User &u) {
    ofstream f("account" + u.accountNumber + ".txt");
    f << u.name << "\n" << u.accountNumber << "\n" << u.pin << "\n" << u.balance << "\n" << u.phone << "\n";
    f.close();
}
void logTransaction(const string &accountNumber, const string &action, int amount, int balance, const string &otherAcc = "") {
    ofstream f("transactions" + accountNumber + ".txt", ios::app);
    time_t now = time(0); char* dt = ctime(&now); dt[strlen(dt)-1]='\0';
    f << dt << " - " << action;
    if (!otherAcc.empty()) f << " " << otherAcc;
    f << " - " << amount << " ETB - Balance " << balance << "\n";
    f.close();
}

/* ================= HIDDEN PIN INPUT ================= */
int inputHiddenPIN() {
    int pin = 0; char ch;
    while (true) {
        ch = getch();
        if (ch == '\r') break; // Enter
        if (ch >= '0' && ch <= '9') { pin = pin*10 + (ch-'0'); cout << "*"; }
    }
    cout << endl; return pin;
}

/* ================= RECEIPT ================= */
void printReceipt(const User &u, const string &action, int amount, int balance, int lang, const string &otherAcc = "") {
    if (lang == 1) {
        cout << "\n----- RECEIPT -----\n" << "User: " << u.name << "\nAccount: " << u.accountNumber
             << "\nTransaction: " << action;
        if (!otherAcc.empty()) cout << " " << otherAcc;
        cout << "\nAmount: " << amount << " ETB\nBalance: " << balance << " ETB\n-------------------\n";
    } else {
        cout << "\n----- ደረሰኝ -----\n" << "ተጠቃሚ: " << u.name << "\nመለያ ቁጥር: " << u.accountNumber
             << "\nግብይት: " << action;
        if (!otherAcc.empty()) cout << " " << otherAcc;
        cout << "\nመጠን: " << amount << " ብር\nቀሪ ሂሳብ: " << balance << " ብር\n-------------------\n";
    }
}
void askAndPrintReceipt(const User &u, const string &action, int amount, int balance, int lang, const string &otherAcc="") {
    int choice;
    if (lang == 1) { cout << "\nDo you want receipt? 1.Yes 2.No: "; }
    else { cout << "\nደረሰኝ ይፈልጋሉ? 1.አዎ 2.አይ: "; }
    cin >> choice;
    if (choice == 1) printReceipt(u, action, amount, balance, lang, otherAcc);
}

/* ================= USER FUNCTIONS ================= */
void showBalance(const User &u, int lang) {
    if (lang==1) cout << "\nUser: " << u.name << " | Account: " << u.accountNumber << "\nBalance: " << u.balance << " ETB\n";
    else cout << "\nተጠቃሚ: " << u.name << " | መለያ ቁጥር: " << u.accountNumber << "\nቀሪ ሂሳብ: " << u.balance << " ብር\n";
}

void withdrawCash(User &u, int &atmCash, int lang) {
    int amount; 
    if(lang==1) cout << "Enter withdrawal amount: "; else cout << "መጠን ያስገቡ (ብር): ";cin >> amount;
    if(amount>u.balance){ cout << (lang==1?"Insufficient balance\n":"በቂ ቀሪ ሂሳብ የለም\n"); return; }
    if(amount>atmCash){ cout << (lang==1?"ATM cannot dispense this amount\n":"ATM ከአገልግሎት ውጭ ነው\n"); return; }
    u.balance -= amount; atmCash -= amount;
    saveUser(u); saveATMCash(atmCash);
    logTransaction(u.accountNumber,"Withdraw",amount,u.balance);
    askAndPrintReceipt(u,"Withdraw",amount,u.balance,lang);
}

void depositCash(User &u,int lang){
    int amount;
    if(lang==1) cout << "Enter deposit amount: "; else cout << "መጠን ያስገቡ (ብር): ";
    cin >> amount; if(amount<=0){cout << "Invalid amount\n"; return;}
    u.balance += amount;
    saveUser(u);
    logTransaction(u.accountNumber,"Deposit",amount,u.balance);
    askAndPrintReceipt(u,"Deposit",amount,u.balance,lang);
}

void transferMoney(User &u,int lang){
    string receiverAcc; cout << (lang==1?"Enter receiver account number: ":"የተቀበለውን መለያ ቁጥር ያስገቡ: "); cin >> receiverAcc;
    User receiver; if(!loadUser(receiverAcc,receiver)){cout << (lang==1?"Receiver not found\n":"ተቀበለው ተጠቃሚ አልተገኘም\n"); return;}
    int amount; cout << (lang==1?"Enter amount to transfer: ":"መጠን ያስገቡ: "); cin >> amount;
    if(amount>u.balance){cout << (lang==1?"Insufficient balance\n":"በቂ ቀሪ ሂሳብ የለም\n"); return;}
    u.balance -= amount; receiver.balance += amount;
    saveUser(u); saveUser(receiver);
    logTransaction(u.accountNumber,"TransferTo",amount,u.balance,receiverAcc);
    logTransaction(receiver.accountNumber,"ReceivedFrom",amount,receiver.balance,u.accountNumber);
    askAndPrintReceipt(u,"Transfer",amount,u.balance,lang,receiverAcc);
}

void showTransactions(const User &u,int lang){
    ifstream f("transactions"+u.accountNumber+".txt"); string line;
    if(lang==1) cout << "\nTransaction History:\n"; else cout << "\nየግብይት ታሪክ:\n";
    while(getline(f,line)) cout << line << "\n"; f.close();
}

void changePIN(User &u,int lang){
    int oldPin,newPin;
    cout << (lang==1?"Enter old PIN: ":"የቀድሞ PIN ያስገቡ: "); cin >> oldPin;
    if(oldPin!=u.pin){cout << (lang==1?"Incorrect PIN\n":"የተሳሳተ PIN\n"); return;}
    cout << (lang==1?"Enter new PIN: ":"አዲስ PIN ያስገቡ: "); cin >> newPin; u.pin=newPin;
    saveUser(u); cout << (lang==1?"PIN changed successfully\n":"PIN ተቀይሯል\n");
}

/* ================= ADMIN FUNCTIONS ================= */
bool adminLogin(int lang){
    string id,pin; cout << (lang==1?"Enter Admin ID: ":"የአስተዳደር መለያ ያስገቡ: "); cin >> id;
    cout << (lang==1?"Enter Admin PIN: ":"የአስተዳደር PIN ያስገቡ: "); cin >> pin;
    ifstream f("admin.txt"); string fileId,filePin; f >> fileId >> filePin; f.close();
    if(id==fileId && pin==filePin) return true;
    cout << (lang==1?"Invalid admin credentials\n":"የአስተዳደር መረጃ ትክክል አይደለም\n"); return false;
}
void registerUser(int lang){
    User u; cin.ignore();
    cout << (lang==1?"Enter name: ":"ስም ያስገቡ: "); getline(cin,u.name);
    cout << (lang==1?"Enter phone: ":"ስልክ ቁጥር ያስገቡ: "); getline(cin,u.phone);
    cout << (lang==1?"Enter initial PIN: ":"መነሻ PIN ያስገቡ: "); cin >> u.pin;
    cout << (lang==1?"Enter initial balance: ":"መነሻ ቀሪ ሂሳብ ያስገቡ: "); cin >> u.balance;
    int accNum = rand()%900000 +100000; u.accountNumber=to_string(accNum);
    saveUser(u); ofstream f("transactions"+u.accountNumber+".txt"); f.close();
    cout << (lang==1?"User registered. Account number: ":"ተጠቃሚ ተመዝግቧል መለያ ቁጥር: ") << u.accountNumber << "\n";
}
void resetUserPIN(int lang){
    string acc; cout << (lang==1?"Enter user account number: ":"የተጠቃሚ መለያ ቁጥር ያስገቡ: "); cin >> acc;
    User u; if(!loadUser(acc,u)){cout << (lang==1?"User not found\n":"ተጠቃሚ አልተገኘም\n"); return;}
    int newPin; cout << (lang==1?"Enter new PIN: ":"አዲስ PIN ያስገቡ: "); cin >> newPin; u.pin=newPin; saveUser(u);
    cout << (lang==1?"PIN reset successfully\n":"PIN ተቀይሯል\n");
}
void loadATMCashAdmin(int lang){
    int cash; cout << (lang==1?"Enter amount to add to ATM: ":"ወደ ATM መጨመር የሚፈልጉትን መጠን ያስገቡ: "); cin >> cash;
    int atmCash=loadATMCash(); atmCash+=cash; saveATMCash(atmCash);
    cout << (lang==1?"ATM cash updated\n":"ATM ገንዘብ ተማረከ\n");
}/* ================= USER SESSION ================= */
void userSession(User &u,int &atmCash,int lang){
    int choice; char again;
    do {
        cout << "\n" << (lang==1?"1.Balance 2.Withdraw 3.Deposit 4.Transfer 5.Transactions 6.Change PIN 7.Exit\n":"1.ቀሪ ሂሳብ 2.ገንዘብ ማውጣት 3.ክፍያ 4.ማስተላለፊያ 5.ግብይት ታሪክ 6.PIN መቀየር 7.መውጣት\n") 
             << (lang==1?"Choice: ":"ምርጫ: "); cin >> choice;
        switch(choice){
            case 1: showBalance(u,lang); break;
            case 2: withdrawCash(u,atmCash,lang); break;
            case 3: depositCash(u,lang); break;
            case 4: transferMoney(u,lang); break;
            case 5: showTransactions(u,lang); break;
            case 6: changePIN(u,lang); break;
        }
    } while(choice!=7);
}

/* ================= MAIN ================= */
int main(){
    srand(time(0));
    string bgColor="0;37;44"; // white text, blue background
    fullScreenBackground(bgColor);

    int lang;
    cout << "WELCOME TO ETHIOPIA ATM MACHINE\n1. English\n2. አማርኛ\nChoose Language: "; cin >> lang;
    if(lang!=1) lang=2;

    int userType;
    cout << (lang==1?"Select user type: 1.Customer 2.Admin: ":"የተጠቃሚ አይነት ይምረጡ: 1.ደንበኛ 2.አስተዳደር: "); cin >> userType;

    int atmCash=loadATMCash();

    if(userType==1){
        string acc; cout << (lang==1?"Enter account number: ":"መለያ ቁጥር ያስገቡ: "); cin >> acc;
        User u; if(!loadUser(acc,u)){cout << (lang==1?"User not found\n":"ተጠቃሚ አልተገኘም\n"); return 0;}
        cout << (lang==1?"Enter PIN: ":"PIN ያስገቡ: "); int pin=inputHiddenPIN();
        if(pin!=u.pin){cout << (lang==1?"Incorrect PIN\n":"የተሳሳተ PIN\n"); return 0;}
        userSession(u,atmCash,lang);
    } else if(userType==2){
        if(!adminLogin(lang)) return 0;
        int choice;
        do {
            cout << "\n" << (lang==1?"1.Register User 2.Reset User PIN 3.Load ATM Cash 4.Exit\n":"1.አዲስ ተጠቃሚ መመዝገብ 2.PIN መቀየር 3.ATM ገንዘብ መጨመር 4.መውጣት\n") << (lang==1?"Choice: ":"ምርጫ: ");
            cin >> choice;
            switch(choice){
                case 1: registerUser(lang); break;
                case 2: resetUserPIN(lang); break;
                case 3: loadATMCashAdmin(lang); break;
            }
        } while(choice!=4);
    } else cout << (lang==1?"Invalid selection\n":"ትክክለኛ ምርጫ አልተመረጠም\n");

    resetColor();
    return 0;
}