#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Book_Loan
{
    int Book_ID;
    char Borrower_Name[30];
    int Days_Borrowed;
};

void Display_Book(Book_Loan Data[], int Amount){
    for (int i = 0; i < Amount; i++)
    {
        cout << "The Borrower's Name: " << Data[i].Borrower_Name << endl;
    }
}

int main(){
    int Numbers_Of_Books_Borrowed;
    cout << "How many Books have been borrowed: " ;
    cin >> Numbers_Of_Books_Borrowed; 
    Book_Loan Books_Loand[Numbers_Of_Books_Borrowed];
    for (int i = 0; i < Numbers_Of_Books_Borrowed; i++){
        cout << "Enter the Book_ID for the " << i + 1 <<" th: ";
        cin >> Books_Loand[i].Book_ID;

        cout << "Enter the Borrower_Name for the " << i + 1 <<" th: ";
        // consume leftover newline and read the full line (can contain spaces)
        getline(cin >> ws, Books_Loand[i].Borrower_Name);

        cout << "Enter the Days_Borrowed for the " << i + 1 <<" th: ";
        cin >> Books_Loand[i].Days_Borrowed;
    }


    Display_Book(Books_Loand, Numbers_Of_Books_Borrowed);

    return 0;
}