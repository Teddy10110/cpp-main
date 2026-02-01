#include <iostream>

using namespace std;

int main(){
    int again = 1;

    int lang = 1;
    while (again == 1)
    {
        int choice;
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
            cout << "hi BIRUK";
        }
        else if (choice == 2)
        {
            cout << "hi ERMI";
        }
        else if (choice == 3)
        {
            cout << "hi every one";
        }
        else
        {
            break;
        }

        cout << (lang == 1 ? "\nAnother transaction? (Y/N): "
                 : "\nሌላ ግብይት? (Y/N): ");
        cout << "Enter 1 to continou, 2 to stop";
        cin >> again;
        cin.clear();
        cin.ignore(10000, '\n');

    }
    

    

}