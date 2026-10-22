#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ================= COLOR DEFINITIONS =================
enum ConsoleColor
{
    RED = 12,     // errors
    GREEN = 10,   // success
    YELLOW = 14,  // prompts
    BLUE = 9,     // info headers
    MAGENTA = 13, // patient type / labels
    CYAN = 11,    // section titles
    WHITE = 15    // default
};

#ifdef _WIN32
void setColor(ConsoleColor color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
#else
#define setColor(c)
#endif

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
    char patientType[20];
};

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

int main()
{
    if (!login())
        return 0;

    int choice;

    setColor(BLUE);
    cout << "========================================\n";
    cout << " HOSPITAL PATIENT RECORD SYSTEM\n";
    cout << "========================================\n";
    setColor(WHITE);

    do
    {
        setColor(YELLOW);
        cout << "\n1. Add New Patient";
        cout << "\n2. View Patient by ID";
        cout << "\n3. Update Patient Record";
        cout << "\n4. Delete Patient Record";
        cout << "\n5. Display All Patients";
        cout << "\n6. Search Patient by Name";
        cout << "\n7. Search Patient by Age";
        cout << "\n8. BMI Stats Summary";
        cout << "\n9. Exit\n";
        setColor(WHITE);

        cout << "\nEnter your choice: ";
        cin >> choice;

        system("cls");

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
            setColor(GREEN);
            cout << "Exiting program...\n";
            setColor(WHITE);
            break;
        default:
            setColor(RED);
            cout << "Invalid choice!\n";
            setColor(WHITE);
        }

        if (choice != 9)
        {
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            system("cls");
        }

    } while (choice != 9);

    return 0;
}

// ================= LOGIN SYSTEM =================
bool login()
{
    const string correctUser = "admin";
    const string correctPass = "1235";
    string user, pass;

    setColor(CYAN);
    cout << "=== Login Required ===\n";
    setColor(WHITE);

    cout << "Username: ";
    cin >> user;
    cout << "Password: ";
    cin >> pass;

    if (user == correctUser && pass == correctPass)
    {
        setColor(GREEN);
        cout << "\nLogin successful!\n";
        setColor(WHITE);
        system("cls");
        return true;
    }
    else
    {
        setColor(RED);
        cout << "\nInvalid credentials. Exiting...\n";
        setColor(WHITE);
        return false;
    }
}

// ================= DATA VALIDATION =================
bool validatePatient(Patient p)
{
    if (p.id <= 0)
    {
        setColor(RED);
        cout << "ID must be positive!\n";
        setColor(WHITE);
        return false;
    }
    if (p.age <= 0 || p.age > 120)
    {
        setColor(RED);
        cout << "Invalid age!\n";
        setColor(WHITE);
        return false;
    }
    if (p.bmi < 10 || p.bmi > 50)
    {
        setColor(RED);
        cout << "BMI seems incorrect!\n";
        setColor(WHITE);
        return false;
    }
    if (strlen(p.name) == 0)
    {
        setColor(RED);
        cout << "Name cannot be empty!\n";
        setColor(WHITE);
        return false;
    }
    if (p.sex != 'M' && p.sex != 'F' && p.sex != 'f' && p.sex != 'm')
    {
        setColor(RED);
        cout << "Sex must be M or F!\n";
        setColor(WHITE);
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

    setColor(YELLOW);
    cout << "Enter Patient ID: ";
    setColor(WHITE);
    cin >> p.id;
    cin.ignore();

    setColor(YELLOW);
    cout << "Enter Name: ";
    setColor(WHITE);
    cin.getline(p.name, 50);

    setColor(YELLOW);
    cout << "Enter Age: ";
    setColor(WHITE);
    cin >> p.age;

    setColor(YELLOW);
    cout << "Enter Sex (M/F): ";
    setColor(WHITE);
    cin >> p.sex;
    cin.ignore();

    setColor(YELLOW);
    cout << "Enter BMI: ";
    setColor(WHITE);
    cin >> p.bmi;
    cin.ignore();

    setColor(YELLOW);
    cout << "Enter Blood Type: ";
    setColor(WHITE);
    cin.getline(p.bloodType, 5);

    setColor(YELLOW);
    cout << "Enter Contact Number: ";
    setColor(WHITE);
    cin.getline(p.contact, 20);

    setColor(YELLOW);
    cout << "Enter Patient Type (Emergency/Referral/Regular): ";
    setColor(WHITE);
    cin.getline(p.patientType, 20);

    p.active = true;

    if (!validatePatient(p))
        return;

    file.seekp((p.id - 1) * sizeof(Patient), ios::beg);
    file.write((char *)&p, sizeof(Patient));
    file.close();

    setColor(GREEN);
    cout << "\nPatient record added successfully.\n";
    setColor(WHITE);
}

// ================= VIEW PATIENT =================
void viewPatient()
{
    int id;
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    setColor(YELLOW);
    cout << "Enter Patient ID: ";
    setColor(WHITE);
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        setColor(RED);
        cout << "\nPatient not found.\n";
        setColor(WHITE);
    }
    else
    {
        setColor(CYAN);
        cout << "\nID: " << p.id;
        cout << "\nName: " << p.name;
        cout << "\nAge: " << p.age;
        cout << "\nSex: " << p.sex;
        cout << "\nBMI: " << p.bmi;
        cout << "\nPatient Type: " << p.patientType;
        cout << "\nBlood Type: " << p.bloodType;
        cout << "\nContact: " << p.contact << endl;
        setColor(WHITE);
    }

    file.close();
}

// ================= UPDATE PATIENT =================
void updatePatient()
{
    int id;
    Patient p;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);

    setColor(YELLOW);
    cout << "Enter Patient ID to update: ";
    setColor(WHITE);
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        setColor(RED);
        cout << "\nPatient not found.\n";
        setColor(WHITE);
    }
    else
    {
        cin.ignore();
        setColor(YELLOW);
        cout << "Enter New Name: ";
        setColor(WHITE);
        cin.getline(p.name, 50);

        setColor(YELLOW);
        cout << "Enter New Age: ";
        setColor(WHITE);
        cin >> p.age;

        setColor(YELLOW);
        cout << "Enter New Sex (M/F): ";
        setColor(WHITE);
        cin >> p.sex;
        cin.ignore();

        setColor(YELLOW);
        cout << "Enter New BMI: ";
        setColor(WHITE);
        cin >> p.bmi;
        cin.ignore();

        setColor(YELLOW);
        cout << "Enter New Blood Type: ";
        setColor(WHITE);
        cin.getline(p.bloodType, 5);

        setColor(YELLOW);
        cout << "Enter New Contact: ";
        setColor(WHITE);
        cin.getline(p.contact, 20);

        setColor(YELLOW);
        cout << "Enter Patient Type (Emergency/Referral/Regular): ";
        setColor(WHITE);
        cin.getline(p.patientType, 20);

        if (!validatePatient(p))
            return;

        file.seekp((id - 1) * sizeof(Patient), ios::beg);
        file.write((char *)&p, sizeof(Patient));

        setColor(GREEN);
        cout << "\nPatient record updated successfully.\n";
        setColor(WHITE);
    }

    file.close();
}

// ================= DELETE PATIENT =================
void deletePatient()
{
    int id;
    Patient p;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);

    setColor(YELLOW);
    cout << "Enter Patient ID to delete: ";
    setColor(WHITE);
    cin >> id;

    file.seekg((id - 1) * sizeof(Patient), ios::beg);
    file.read((char *)&p, sizeof(Patient));

    if (!file || p.id != id || !p.active)
    {
        setColor(RED);
        cout << "\nPatient not found.\n";
        setColor(WHITE);
    }
    else
    {
        p.active = false;

        file.seekp((id - 1) * sizeof(Patient), ios::beg);
        file.write((char *)&p, sizeof(Patient));

        setColor(GREEN);
        cout << "\nPatient record deleted successfully.\n";
        setColor(WHITE);
    }

    file.close();
}

// ================= DISPLAY ALL =================
void displayAllPatients()
{
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    setColor(CYAN);
    cout << "\nALL PATIENT RECORDS\n";
    setColor(WHITE);

    while (file.read((char *)&p, sizeof(Patient)))
    {
        if (p.active)
        {
            setColor(MAGENTA);
            cout << "\n--------------------";
            setColor(CYAN);
            cout << "\nID: " << p.id;
            cout << "\nName: " << p.name;
            cout << "\nAge: " << p.age;
            cout << "\nSex: " << p.sex;
            cout << "\nBMI: " << p.bmi;
            cout << "\nPatient Type: " << p.patientType;
            cout << "\nBlood Type: " << p.bloodType;
            cout << "\nContact: " << p.contact;
            setColor(WHITE);
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

    cin.ignore();
    setColor(YELLOW);
    cout << "Enter name to search: ";
    setColor(WHITE);
    cin.getline(searchName, 50);

    bool found = false;
    while (file.read((char *)&p, sizeof(Patient)))
    {
        if (p.active && strcasecmp(p.name, searchName) == 0)
        {
            setColor(MAGENTA);
            cout << "\n--------------------";
            setColor(CYAN);
            cout << "\nID: " << p.id;
            cout << "\nName: " << p.name;
            cout << "\nAge: " << p.age;
            cout << "\nSex: " << p.sex;
            cout << "\nBMI: " << p.bmi;
            cout << "\nPatient Type: " << p.patientType;
            cout << "\nBlood Type: " << p.bloodType;
            cout << "\nContact: " << p.contact;
            setColor(WHITE);
            found = true;
        }
    }

    if (!found)
    {
        setColor(RED);
        cout << "\nNo patient found with that name.\n";
        setColor(WHITE);
    }

    file.close();
}

// ================= SEARCH BY AGE =================
void searchPatientByAge()
{
    int searchAge;
    Patient p;
    ifstream file(FILE_NAME, ios::binary);

    setColor(YELLOW);
    cout << "Enter age to search: ";
    setColor(WHITE);
    cin >> searchAge;

    bool found = false;
    while (file.read((char *)&p, sizeof(Patient)))
    {
        if (p.active && p.age == searchAge)
        {
            setColor(MAGENTA);
            cout << "\n--------------------";
            setColor(CYAN);
            cout << "\nID: " << p.id;
            cout << "\nName: " << p.name;
            cout << "\nAge: " << p.age;
            cout << "\nSex: " << p.sex;
            cout << "\nBMI: " << p.bmi;
            cout << "\nPatient Type: " << p.patientType;
            cout << "\nBlood Type: " << p.bloodType;
            cout << "\nContact: " << p.contact;
            setColor(WHITE);
            found = true;
        }
    }

    if (!found)
    {
        setColor(RED);
        cout << "\nNo patient found with that age.\n";
        setColor(WHITE);
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

    setColor(CYAN);
    cout << "\nBMI STATS SUMMARY";
    setColor(WHITE);
    setColor(YELLOW);
    cout << "\nUnderweight: " << underweight;
    cout << "\nNormal: " << normal;
    cout << "\nOverweight: " << overweight;
    cout << "\nObese: " << obese << endl;
    setColor(WHITE);

    file.close();
}
