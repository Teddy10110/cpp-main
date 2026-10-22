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
#define bg_BLUE "\033[45m"
#define MAGENTA "\033[35m" // patient type / labels
#define CYAN "\033[36m"    // section titles
void screanColor ()
{
    cout << bg_BLUE ;
    cout << "\033[2J\033[H";
}

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
    screanColor();
    if (!login())
        return 0;

    int choice;
    screanColor();

    cout << CYAN << "========================================\n"
         ;
    cout << CYAN << " HOSPITAL PATIENT RECORD SYSTEM\n"
         ;
    cout << CYAN << "========================================\n"
         ;

    do
    {
        screanColor();
        cout << "\n"
             << YELLOW << "1. Add New Patient" ;
        cout << "\n"
             << YELLOW << "2. View Patient by ID" ;
        cout << "\n"
             << YELLOW << "3. Update Patient Record" ;
        cout << "\n"
             << YELLOW << "4. Delete Patient Record" ;
        cout << "\n"
             << YELLOW << "5. Display All Patients" ;
        cout << "\n"
             << YELLOW << "6. Search Patient by Name" ;
        cout << "\n"
             << YELLOW << "7. Search Patient by Age" ;
        cout << "\n"
             << YELLOW << "8. BMI Stats Summary" ;
        cout << "\n"
             << YELLOW << "9. Exit" ;

        cout << "\n\n"
             << YELLOW << "Enter your choice: " ;
        cin >> choice;

        // clear(); // use "cls" on Windows

        switch (choice)
        {
        case 1:
        screanColor();
            addPatient();
            break;
        case 2:
        screanColor();
            viewPatient();
            break;
        case 3:
        screanColor();
            updatePatient();
            break;
        case 4:
        screanColor();
            deletePatient();
            break;
        case 5:
        screanColor();
            displayAllPatients();
            break;
        case 6:
        screanColor();
            searchPatientByName();
            break;
        case 7:
        screanColor();
            searchPatientByAge();
            break;
        case 8:
        screanColor();
            bmiStatsSummary();
            break;
        case 9:
        screanColor();
            cout << GREEN << "Exiting program...\n"
                 ;
            break;
        default:
            cout << RED << "Invalid choice!\n"
                 ;
        }

        if (choice != 9)
        {
            screanColor();
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            // clear();
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
    screanColor();
    const string correctUser = "admin";
    const string correctPass = "1234";
    string user, pass;

    cout << CYAN << "=== Login Required ===\n"
         ;
    cout << YELLOW << "Username: " ;
    cin >> user;
    cout << YELLOW << "Password: " ;
    cin >> pass;

    if (user == correctUser && pass == correctPass)
    {
        screanColor();
        cout << GREEN << "\nLogin successful!\n"
             ;
        // clear();
        return true;
    }
    else
    {
        screanColor();
        cout << RED << "\nInvalid credentials. Exiting...\n"
             ;
        return false;
    }
}

// ================= DATA VALIDATION =================
bool validatePatient(Patient p)
{
    if (p.id <= 0)
    {
        cout << RED << "ID must be positive!\n"
             ;
        return false;
    }
    if (p.age <= 0 || p.age > 120)
    {
        cout << RED << "Invalid age!\n"
             ;
        return false;
    }
    if (p.bmi < 10 || p.bmi > 50)
    {
        cout << RED << "BMI seems incorrect!\n"
             ;
        return false;
    }
    if (strlen(p.name) == 0)
    {
        cout << RED << "Name cannot be empty!\n"
             ;
        return false;
    }
    if (p.sex != 'M' && p.sex != 'F')
    {
        cout << RED << "Sex must be M or F!\n"
             ;
        return false;
    }
    return true;
}

// ================= ADD PATIENT =================
void addPatient()
{
    screanColor();
    Patient p;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);

    if (!file)
    {
        file.open(FILE_NAME, ios::binary | ios::out);
        file.close();
        file.open(FILE_NAME, ios::binary | ios::in | ios::out);
    }

    cout << YELLOW << "Enter Patient ID: " ;
    cin >> p.id;
    cin.ignore();

    cout << YELLOW << "Enter Name: " ;
    cin.getline(p.name, 50);

    cout << YELLOW << "Enter Age: " ;
    cin >> p.age;
    cin.ignore();

    cout << YELLOW << "Enter Sex (M/F): " ;
    cin >> p.sex;
    cin.ignore();

    cout << YELLOW << "Enter BMI: " ;
    cin >> p.bmi;
    cin.ignore();

    cout << YELLOW << "Enter Blood Type: " ;
    cin.getline(p.bloodType, 5);

    cout << YELLOW << "Enter Contact Number: " ;
    cin.getline(p.contact, 20);

    cout << YELLOW << "Enter Patient Type (Emergency/Referral/Regular): " ;
    cin.getline(p.patientType, 20);

    p.active = true;

    if (!validatePatient(p))
        return;

    file.seekp((p.id - 1) * sizeof(Patient), ios::beg);
    file.write((char *)&p, sizeof(Patient));
    file.close();

    cout << GREEN << "\nPatient record added successfully.\n"
         ;
}

// ================= VIEW PATIENT =================
void viewPatient()
{
    screanColor();
    int id;
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    cout << YELLOW << "Enter Patient ID: " ;
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        screanColor();
        cout << RED << "\nPatient not found.\n"
             ;
    }
    else
    {
        screanColor();
        cout << CYAN << "\nID: "  << p.id;
        cout << CYAN << "\nName: "  << p.name;
        cout << CYAN << "\nAge: "  << p.age;
        cout << CYAN << "\nSex: "  << p.sex;
        cout << CYAN << "\nBMI: "  << p.bmi;
        cout << MAGENTA << "\nPatient Type: "  << p.patientType;
        cout << GREEN << "\nBlood Type: "  << p.bloodType;
        cout << GREEN << "\nContact: "  << p.contact << endl;
    }

    file.close();
}

// ================= UPDATE PATIENT =================
void updatePatient()
{
    screanColor();
    int id;
    Patient p;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);

    cout << YELLOW << "Enter Patient ID to update: " ;
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        screanColor();
        cout << RED << "\nPatient not found.\n"
             ;
    }
    else
    {
        screanColor();
        cin.ignore();
        cout << YELLOW << "Enter New Name: " ;
        cin.getline(p.name, 50);

        cout << YELLOW << "Enter New Age: " ;
        cin >> p.age;

        cout << YELLOW << "Enter New Sex (M/F): " ;
        cin >> p.sex;
        cin.ignore();

        cout << YELLOW << "Enter New BMI: " ;
        cin >> p.bmi;
        cin.ignore();

        cout << YELLOW << "Enter New Blood Type: " ;
        cin.getline(p.bloodType, 5);

        cout << YELLOW << "Enter New Contact: " ;
        cin.getline(p.contact, 20);

        cout << YELLOW << "Enter Patient Type (Emergency/Referral/Regular): " ;
        cin.getline(p.patientType, 20);

        if (!validatePatient(p))
            return;

        file.seekp((id - 1) * sizeof(Patient), ios::beg);
        file.write((char *)&p, sizeof(Patient));

        cout << GREEN << "\nPatient record updated successfully.\n"
             ;
    }

    file.close();
}

// ================= DELETE PATIENT =================
void deletePatient()
{
    screanColor();
    int id;
    Patient p;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);

    cout << YELLOW << "Enter Patient ID to delete: " ;
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        screanColor();
        cout << RED << "\nPatient not found.\n"
             ;
    }
    else
    {
        screanColor();
        p.active = false;

        file.seekp((id - 1) * sizeof(Patient), ios::beg);
        file.write((char *)&p, sizeof(Patient));

        cout << GREEN << "\nPatient record deleted successfully.\n"
             ;
    }

    file.close();
}

// ================= DISPLAY ALL =================
void displayAllPatients()
{
    screanColor();
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    cout << CYAN << "\nALL PATIENT RECORDS\n"
         ;

    while (file.read((char *)&p, sizeof(Patient)))
    {
        screanColor();
        if (p.active)
        {
            screanColor();
            cout << "\n--------------------";
            cout << CYAN << "\nID: "  << p.id;
            cout << CYAN << "\nName: "  << p.name;
            cout << CYAN << "\nAge: "  << p.age;
            cout << CYAN << "\nSex: "  << p.sex;
            cout << CYAN << "\nBMI: "  << p.bmi;
            cout << MAGENTA << "\nPatient Type: "  << p.patientType;
            cout << GREEN << "\nBlood Type: "  << p.bloodType;
            cout << GREEN << "\nContact: "  << p.contact;
        }
    }

    file.close();
}

// ================= SEARCH BY NAME =================
void searchPatientByName()
{
    screanColor();
    char searchName[50];
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    cin.ignore(); // clear buffer
    cout << YELLOW << "Enter name to search: " ;
    cin.getline(searchName, 50);

    bool found = false;
    while (file.read((char *)&p, sizeof(Patient)))
    {
        screanColor();
        if (p.active && strcasecmp(p.name, searchName) == 0)
        {
            screanColor();
            cout << "\n--------------------";
            cout << CYAN << "\nID: "  << p.id;
            cout << CYAN << "\nName: "  << p.name;
            cout << CYAN << "\nAge: "  << p.age;
            cout << CYAN << "\nSex: "  << p.sex;
            cout << CYAN << "\nBMI: "  << p.bmi;
            cout << MAGENTA << "\nPatient Type: "  << p.patientType;
            cout << GREEN << "\nBlood Type: "  << p.bloodType;
            cout << GREEN << "\nContact: "  << p.contact;
            found = true;
        }
    }

    if (!found)
    {
        screanColor();
        cout << RED << "\nNo patient found with that name.\n"
             ;
    }

    file.close();
}

// ================= SEARCH BY AGE =================
void searchPatientByAge()
{
    screanColor();
    int searchAge;
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    cout << YELLOW << "Enter age to search: " ;
    cin >> searchAge;

    bool found = false;
    while (file.read((char *)&p, sizeof(Patient)))
    {
        screanColor();
        if (p.active && p.age == searchAge)
        {
            screanColor();
            cout << "\n--------------------";
            cout << CYAN << "\nID: "  << p.id;
            cout << CYAN << "\nName: "  << p.name;
            cout << CYAN << "\nAge: "  << p.age;
            cout << CYAN << "\nSex: "  << p.sex;
            cout << CYAN << "\nBMI: "  << p.bmi;
            cout << MAGENTA << "\nPatient Type: "  << p.patientType;
            cout << GREEN << "\nBlood Type: "  << p.bloodType;
            cout << GREEN << "\nContact: "  << p.contact;
            found = true;
        }
    }

    if (!found)
    {
        screanColor();
        cout << RED << "\nNo patient found with that age.\n"
             ;
    }

    file.close();
}

// ================= BMI STATS SUMMARY =================
void bmiStatsSummary()
{
    screanColor();
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

    cout << CYAN << "\nBMI STATS SUMMARY" ;
    cout << GREEN << "\nUnderweight: "  << underweight;
    cout << GREEN << "\nNormal: "  << normal;
    cout << GREEN << "\nOverweight: "  << overweight;
    cout << GREEN << "\nObese: "  << obese << endl;

    file.close();

}
