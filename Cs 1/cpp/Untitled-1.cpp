#include <limits>

std::cin >> Books_Loand[i].Borrower_Name;                           // previous extraction
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard leftover
std::getline(std::cin, Books_Loand[i].Borrower_Name);               // now safe

cin.ignore(numeric_limits<streamsize>::max(), '\n');