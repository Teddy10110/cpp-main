#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ================= COLORS =================
#define RESET "\033[0m"
#define RED "\033[31m"     // errors
#define GREEN "\033[32m"   // success
#define YELLOW "\033[33m"  // prompts
#define BLUE "\033[34m"    // info headers
#define MAGENTA "\033[35m" // patient type / labels
#define CYAN "\033[36m"    // section titles

struct Patient
{
    int id;
    char name[50];
    int age;
    char sex;
    float bmi;
    bool active;
    char bloodType[5];
    char contact[20];
    char patientType[20]; // "Regular", "Emergency", "Referral"
};
// ================= file name =========================

const char FILE_NAME[] = "patients.dat";

// ================= FUNCTION PROTOTYPES =================
bool login();
void addPatient();
void viewPatient();
void updatePatient();
void deletePatient();
void displayAllPatients();
void searchPatientByName();
void searchPatientByAge();
void bmiStatsSummary();
bool validatePatient(Patient p);
void clear();

int main()
{
    if (!login())
        return 0;

    int choice;

    cout << CYAN << "========================================\n"
         << RESET;
    cout << CYAN << " HOSPITAL PATIENT RECORD SYSTEM\n"
         << RESET;
    cout << CYAN << "========================================\n"
         << RESET;

    do
    {
        cout << "\n"
             << YELLOW << "1. Add New Patient" << RESET;
        cout << "\n"
             << YELLOW << "2. View Patient by ID" << RESET;
        cout << "\n"
             << YELLOW << "3. Update Patient Record" << RESET;
        cout << "\n"
             << YELLOW << "4. Delete Patient Record" << RESET;
        cout << "\n"
             << YELLOW << "5. Display All Patients" << RESET;
        cout << "\n"
             << YELLOW << "6. Search Patient by Name" << RESET;
        cout << "\n"
             << YELLOW << "7. Search Patient by Age" << RESET;
        cout << "\n"
             << YELLOW << "8. BMI Stats Summary" << RESET;
        cout << "\n"
             << YELLOW << "9. Exit" << RESET;

        cout << "\n\n"
             << YELLOW << "Enter your choice: " << RESET;
        cin >> choice;

        //clear(); // use "cls" on Windows

        switch (choice)
        {
        case 1:
            addPatient();
            break;
        case 2:
            viewPatient();
            break;
        case 3:
            updatePatient();
            break;
        case 4:
            deletePatient();
            break;
        case 5:
            displayAllPatients();
            break;
        case 6:
            searchPatientByName();
            break;
        case 7:
            searchPatientByAge();
            break;
        case 8:
            bmiStatsSummary();
            break;
        case 9:
            cout << GREEN << "Exiting program...\n"
                 << RESET;
            break;
        default:
            cout << RED << "Invalid choice!\n"
                 << RESET;
        }

        if (choice != 9)
        {
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            //clear();
        }

    } while (choice != 9);

    return 0;
}
// ================= clear screan =================
void clear()
{
#ifdef _WIN32
    std::system("cls"); // {Link: Use system("cls") on Windows
#else
    // ANSI escape code to clear screen and move cursor home
    std::cout << "\033[2J\033[H";
#endif
}
// ================= LOGIN SYSTEM =================
bool login()
{
    const string correctUser = "admin";
    const string correctPass = "1234";
    string user, pass;

    cout << CYAN << "=== Login Required ===\n"
         << RESET;
    cout << YELLOW << "Username: " << RESET;
    cin >> user;
    cout << YELLOW << "Password: " << RESET;
    cin >> pass;

    if (user == correctUser && pass == correctPass)
    {
        cout << GREEN << "\nLogin successful!\n"
             << RESET;
        //clear();
        return true;
    }
    else
    {
        cout << RED << "\nInvalid credentials. Exiting...\n"
             << RESET;
        return false;
    }
}

// ================= DATA VALIDATION =================
bool validatePatient(Patient p)
{
    if (p.id <= 0)
    {
        cout << RED << "ID must be positive!\n"
             << RESET;
        return false;
    }
    if (p.age <= 0 || p.age > 120)
    {
        cout << RED << "Invalid age!\n"
             << RESET;
        return false;
    }
    if (p.bmi < 10 || p.bmi > 50)
    {
        cout << RED << "BMI seems incorrect!\n"
             << RESET;
        return false;
    }
    if (strlen(p.name) == 0)
    {
        cout << RED << "Name cannot be empty!\n"
             << RESET;
        return false;
    }
    if (p.sex != 'M' && p.sex != 'F')
    {
        cout << RED << "Sex must be M or F!\n"
             << RESET;
        return false;
    }
    return true;
}

// ================= ADD PATIENT =================
void addPatient()
{
    Patient p;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);

    if (!file)
    {
        file.open(FILE_NAME, ios::binary | ios::out);
        file.close();
        file.open(FILE_NAME, ios::binary | ios::in | ios::out);
    }

    cout << YELLOW << "Enter Patient ID: " << RESET;
    cin >> p.id;
    cin.ignore();

    cout << YELLOW << "Enter Name: " << RESET;
    cin.getline(p.name, 50);

    cout << YELLOW << "Enter Age: " << RESET;
    cin >> p.age;
    cin.ignore();

    cout << YELLOW << "Enter Sex (M/F): " << RESET;
    cin >> p.sex;
    cin.ignore();

    cout << YELLOW << "Enter BMI: " << RESET;
    cin >> p.bmi;
    cin.ignore();

    cout << YELLOW << "Enter Blood Type: " << RESET;
    cin.getline(p.bloodType, 5);

    cout << YELLOW << "Enter Contact Number: " << RESET;
    cin.getline(p.contact, 20);

    cout << YELLOW << "Enter Patient Type (Emergency/Referral/Regular): " << RESET;
    cin.getline(p.patientType, 20);

    p.active = true;

    if (!validatePatient(p))
        return;

    file.seekp((p.id - 1) * sizeof(Patient), ios::beg);
    file.write((char *)&p, sizeof(Patient));
    file.close();

    cout << GREEN << "\nPatient record added successfully.\n"
         << RESET;
}

// ================= VIEW PATIENT =================
void viewPatient()
{
    int id;
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    cout << YELLOW << "Enter Patient ID: " << RESET;
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        cout << RED << "\nPatient not found.\n"
             << RESET;
    }
    else
    {
        cout << CYAN << "\nID: " << RESET << p.id;
        cout << CYAN << "\nName: " << RESET << p.name;
        cout << CYAN << "\nAge: " << RESET << p.age;
        cout << CYAN << "\nSex: " << RESET << p.sex;
        cout << CYAN << "\nBMI: " << RESET << p.bmi;
        cout << MAGENTA << "\nPatient Type: " << RESET << p.patientType;
        cout << GREEN << "\nBlood Type: " << RESET << p.bloodType;
        cout << GREEN << "\nContact: " << RESET << p.contact << endl;
    }

    file.close();
}

// ================= UPDATE PATIENT =================
void updatePatient()
{
    int id;
    Patient p;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);

    cout << YELLOW << "Enter Patient ID to update: " << RESET;
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        cout << RED << "\nPatient not found.\n"
             << RESET;
    }
    else
    {
        cin.ignore();
        cout << YELLOW << "Enter New Name: " << RESET;
        cin.getline(p.name, 50);

        cout << YELLOW << "Enter New Age: " << RESET;
        cin >> p.age;

        cout << YELLOW << "Enter New Sex (M/F): " << RESET;
        cin >> p.sex;
        cin.ignore();

        cout << YELLOW << "Enter New BMI: " << RESET;
        cin >> p.bmi;
        cin.ignore();

        cout << YELLOW << "Enter New Blood Type: " << RESET;
        cin.getline(p.bloodType, 5);

        cout << YELLOW << "Enter New Contact: " << RESET;
        cin.getline(p.contact, 20);

        cout << YELLOW << "Enter Patient Type (Emergency/Referral/Regular): " << RESET;
        cin.getline(p.patientType, 20);

        if (!validatePatient(p))
            return;

        file.seekp((id - 1) * sizeof(Patient), ios::beg);
        file.write((char *)&p, sizeof(Patient));

        cout << GREEN << "\nPatient record updated successfully.\n"
             << RESET;
    }

    file.close();
}

// ================= DELETE PATIENT =================
void deletePatient()
{
    int id;
    Patient p;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);

    cout << YELLOW << "Enter Patient ID to delete: " << RESET;
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        cout << RED << "\nPatient not found.\n"
             << RESET;
    }
    else
    {
        p.active = false;

        file.seekp((id - 1) * sizeof(Patient), ios::beg);
        file.write((char *)&p, sizeof(Patient));

        cout << GREEN << "\nPatient record deleted successfully.\n"
             << RESET;
    }

    file.close();
}

// ================= DISPLAY ALL =================
void displayAllPatients()
{
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    cout << CYAN << "\nALL PATIENT RECORDS\n"
         << RESET;

    while (file.read((char *)&p, sizeof(Patient)))
    {
        if (p.active)
        {
            cout << "\n--------------------";
            cout << CYAN << "\nID: " << RESET << p.id;
            cout << CYAN << "\nName: " << RESET << p.name;
            cout << CYAN << "\nAge: " << RESET << p.age;
            cout << CYAN << "\nSex: " << RESET << p.sex;
            cout << CYAN << "\nBMI: " << RESET << p.bmi;
            cout << MAGENTA << "\nPatient Type: " << RESET << p.patientType;
            cout << GREEN << "\nBlood Type: " << RESET << p.bloodType;
            cout << GREEN << "\nContact: " << RESET << p.contact;
        }
    }

    file.close();
}

// ================= SEARCH BY NAME =================
void searchPatientByName()
{
    char searchName[50];
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    cin.ignore(); // clear buffer
    cout << YELLOW << "Enter name to search: " << RESET;
    cin.getline(searchName, 50);

    bool found = false;
    while (file.read((char *)&p, sizeof(Patient)))
    {
        if (p.active && strcasecmp(p.name, searchName) == 0)
        {
            cout << "\n--------------------";
            cout << CYAN << "\nID: " << RESET << p.id;
            cout << CYAN << "\nName: " << RESET << p.name;
            cout << CYAN << "\nAge: " << RESET << p.age;
            cout << CYAN << "\nSex: " << RESET << p.sex;
            cout << CYAN << "\nBMI: " << RESET << p.bmi;
            cout << MAGENTA << "\nPatient Type: " << RESET << p.patientType;
            cout << GREEN << "\nBlood Type: " << RESET << p.bloodType;
            cout << GREEN << "\nContact: " << RESET << p.contact;
            found = true;
        }
    }

    if (!found)
    {
        cout << RED << "\nNo patient found with that name.\n"
             << RESET;
    }

    file.close();
}

// ================= SEARCH BY AGE =================
void searchPatientByAge()
{
    int searchAge;
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    cout << YELLOW << "Enter age to search: " << RESET;
    cin >> searchAge;

    bool found = false;
    while (file.read((char *)&p, sizeof(Patient)))
    {
        if (p.active && p.age == searchAge)
        {
            cout << "\n--------------------";
            cout << CYAN << "\nID: " << RESET << p.id;
            cout << CYAN << "\nName: " << RESET << p.name;
            cout << CYAN << "\nAge: " << RESET << p.age;
            cout << CYAN << "\nSex: " << RESET << p.sex;
            cout << CYAN << "\nBMI: " << RESET << p.bmi;
            cout << MAGENTA << "\nPatient Type: " << RESET << p.patientType;
            cout << GREEN << "\nBlood Type: " << RESET << p.bloodType;
            cout << GREEN << "\nContact: " << RESET << p.contact;
            found = true;
        }
    }

    if (!found)
    {
        cout << RED << "\nNo patient found with that age.\n"
             << RESET;
    }

    file.close();
}

// ================= BMI STATS SUMMARY =================
void bmiStatsSummary()
{
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    int underweight = 0, normal = 0, overweight = 0, obese = 0;

    while (file.read((char *)&p, sizeof(Patient)))
    {
        if (p.active)
        {
            if (p.bmi < 18.5)
                underweight++;
            else if (p.bmi < 25)
                normal++;
            else if (p.bmi < 30)
                overweight++;
            else
                obese++;
        }
    }

    cout << CYAN << "\nBMI STATS SUMMARY" << RESET;
    cout << GREEN << "\nUnderweight: " << RESET << underweight;
    cout << GREEN << "\nNormal: " << RESET << normal;
    cout << GREEN << "\nOverweight: " << RESET << overweight;
    cout << GREEN << "\nObese: " << RESET << obese << endl;

    file.close();
}
