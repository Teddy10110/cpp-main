#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// ===== Colors =====
#define RESET "\033[0m"
#define BG_WHITE "\033[47m"
#define TEXT_BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BOLD "\033[1m"

// ===== Screen =====
void bgcolor()
{
    cout << BG_WHITE << TEXT_BLACK;
    cout << "\033[2J\033[H";
}

// ===== Constants =====
const int MAX_VOTERS = 5000;
const int MAX_CANDIDATES = 10;

// ===== Admin Credentials =====
const string ADMIN_USER = "admin";
const string ADMIN_PASS = "1234";

// ===== Structures =====
struct Candidate
{
    string name;
    string nickname;
    string department;
    string year;
    string description;
    int votes = 0;
};

struct Voter
{
    string name;
    string department;
    string year;
    int studentID;
    int voterID;
    bool hasVoted = false;
};

// ===== Files =====
const string CANDIDATE_FILE = "candidates.txt";
const string VOTER_FILE = "voters.txt";
const string STATE_FILE = "election_state.txt";

// ===== File Functions =====
void saveCandidates(Candidate c[], int total)
{
    ofstream file(CANDIDATE_FILE);
    file << total << endl;
    for (int i = 0; i < total; i++)
    {
        file << c[i].name << "|"
             << c[i].nickname << "|"
             << c[i].department << "|"
             << c[i].year << "|"
             << c[i].description << "|"
             << c[i].votes << endl;
    }
    file.close();
}

void loadCandidates(Candidate c[], int &total)
{
    ifstream file(CANDIDATE_FILE);
    if (!file) return;

    file >> total;
    file.ignore();

    for (int i = 0; i < total; i++)
    {
        getline(file, c[i].name, '|');
        getline(file, c[i].nickname, '|');
        getline(file, c[i].department, '|');
        getline(file, c[i].year, '|');
        getline(file, c[i].description, '|');
        file >> c[i].votes;
        file.ignore();
    }
    file.close();
}

void saveVoters(Voter v[], int total)
{
    ofstream file(VOTER_FILE);
    file << total << endl;
    for (int i = 0; i < total; i++)
    {
        file << v[i].name << "|"
             << v[i].department << "|"
             << v[i].year << "|"
             << v[i].studentID << "|"
             << v[i].voterID << "|"
             << v[i].hasVoted << endl;
    }
    file.close();
}

void loadVoters(Voter v[], int &total, int &nextVoterID)
{
    ifstream file(VOTER_FILE);
    if (!file) return;

    file >> total;
    file.ignore();
    nextVoterID = 1001;

    for (int i = 0; i < total; i++)
    {
        getline(file, v[i].name, '|');
        getline(file, v[i].department, '|');
        getline(file, v[i].year, '|');
        file >> v[i].studentID;
        file.ignore();
        file >> v[i].voterID;
        file.ignore();
        file >> v[i].hasVoted;
        file.ignore();

        if (v[i].voterID >= nextVoterID)
            nextVoterID = v[i].voterID + 1;
    }
    file.close();
}

void saveElectionState(bool electionOver)
{
    ofstream file(STATE_FILE);
    file << electionOver;
    file.close();
}

void loadElectionState(bool &electionOver)
{
    ifstream file(STATE_FILE);
    if (!file) return;
    file >> electionOver;
    file.close();
}

// ===== Utility =====
void sortCandidates(Candidate c[], int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (c[j].votes < c[j + 1].votes)
                swap(c[j], c[j + 1]);
}

// ===== Admin Login =====
bool adminLogin()
{
    bgcolor();
    string u, p;
    cout << "Admin Username: ";
    cin >> u;
    cout << "Password: ";
    cin >> p;

    return (u == ADMIN_USER && p == ADMIN_PASS);
}

// ===== Admin Panel =====
void adminPanel(Candidate candidates[], int &totalCandidates,
                Voter voters[], int totalVoters,
                bool &electionOver)
{
    int choice;
    while (true)
    {
        bgcolor();
        cout << "\n===== ADMIN PANEL =====\n";
        cout << "1. Add Candidate\n";
        cout << "2. View Candidates\n";

        cout << "3. End Election\n";
        cout<<"4. Show voters\n";
         cout << "5. Back\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            Candidate c;
            cout << "Full Name: ";
            getline(cin, c.name);
            cout << "Partyname: ";
            getline(cin, c.nickname);
            cout << "Department: ";
            getline(cin, c.department);
            cout << "Year: ";
            getline(cin, c.year);
            cout << "Description: ";
            getline(cin, c.description);

            candidates[totalCandidates++] = c;
            saveCandidates(candidates, totalCandidates);

            cout << GREEN << "Candidate added!\n" << RESET;
            cin.get();
        }

        else if (choice == 2)
        {
            for (int i = 0; i < totalCandidates; i++)
            {
                cout << "\n" << BOLD << candidates[i].name << " (" << candidates[i].nickname << ")" << RESET << endl;
                cout << "Dept: " << candidates[i].department << endl;
                cout << "Year: " << candidates[i].year << endl;
                cout << "About: " << candidates[i].description << endl;
                cout << "Votes: " << candidates[i].votes << endl;
            }
            cin.get();
        }

        else if (choice == 3)
        {
            electionOver = true;
            saveElectionState(electionOver);
            sortCandidates(candidates, totalCandidates);
            cout << "\nWINNER: " << candidates[0].name << endl;
            cin.get();
        }
           else if (choice == 4)
{
    bgcolor();
    cout << "\n===== REGISTERED VOTERS =====\n";

    if (totalVoters == 0)
    {
        cout << YELLOW << "No voters registered yet.\n" << RESET;
    }
    else
    {
        for (int i = 0; i < totalVoters; i++)
        {
            cout << BOLD << "\nVoter #" << i + 1 << RESET << endl;
            cout << "Voter ID   : " << voters[i].voterID << endl;
            cout << "Name       : " << voters[i].name << endl;
            cout << "Department : " << voters[i].department << endl;
            cout << "Year       : " << voters[i].year << endl;
            cout << "Student ID : " << voters[i].studentID << endl;
            cout << "Status     : "
                 << (voters[i].hasVoted ? GREEN "Voted" RESET
                                        : RED "Not Voted" RESET)
                 << endl;
            cout << "-------------------------------\n";
        }
    }

    cout << "\nPress Enter to continue...";
    cin.get();
}

        else
            return;
    }
}

// ===== MAIN =====
int main()
{
    bgcolor();

    Candidate candidates[MAX_CANDIDATES];
    Voter voters[MAX_VOTERS];

    int totalCandidates = 0;
    int totalVoters = 0;
    int nextVoterID = 1001;
    bool electionOver = false;

    loadCandidates(candidates, totalCandidates);
    loadVoters(voters, totalVoters, nextVoterID);
    loadElectionState(electionOver);

    int choice;
    while (true)
    {
        bgcolor();
        cout << "\n===== SCHOOL ELECTION SYSTEM =====\n";
        cout << "1. Register Voter\n";
        cout << "2. Vote\n";
        cout << "3. View Candidates\n";
        cout << "4. Admin Login\n";
        cout << "5. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            Voter v;
            cout << "Name: ";
            getline(cin, v.name);
            cout << "Department: ";
            getline(cin, v.department);
            cout << "Year: ";
            getline(cin, v.year);
            cout << "Student ID: ";
            cin >> v.studentID;

            v.voterID = nextVoterID++;
            voters[totalVoters++] = v;
            saveVoters(voters, totalVoters);

            cout << GREEN << "Registered! ID: " << v.voterID << RESET << endl;
            cin.ignore();
            cin.get();
        }

        else if (choice == 2)
        {
            int id;
            cout << "Voter ID: ";
            cin >> id;

            for (int i = 0; i < totalVoters; i++)
            {
                if (voters[i].voterID == id && !voters[i].hasVoted)
                {
                    for (int j = 0; j < totalCandidates; j++)
                        cout << j + 1 << ". " << candidates[j].name
                             << " (" << candidates[j].nickname << ")\n";

                    int v;
                    cin >> v;
                    candidates[v - 1].votes++;
                    voters[i].hasVoted = true;

                    saveCandidates(candidates, totalCandidates);
                    saveVoters(voters, totalVoters);
                    break;
                }
            }
            cin.ignore();
            cin.get();
        }
else if (choice == 3)
{
    bgcolor();
    cout << "\n===== CANDIDATE INFORMATION =====\n";

    if (totalCandidates == 0)
    {
        cout << YELLOW << "No candidates available.\n" << RESET;
    }
    else
    {
        for (int i = 0; i < totalCandidates; i++)
        {
            cout << BOLD << "\nCandidate #" << i + 1 << RESET << endl;
            cout << "Full Name   : " << candidates[i].name << endl;
            cout << "Partyname    : " << candidates[i].nickname << endl;
            cout << "Department  : " << candidates[i].department << endl;
            cout << "Year        : " << candidates[i].year << endl;
            cout << "Description : " << candidates[i].description << endl;
            cout << "Votes       : " << candidates[i].votes << endl;
            cout << "-------------------------------\n";
        }
    }

    cout << "\nPress Enter to continue...";
    cin.get();
}


     else if (choice == 4)
        {
            if (adminLogin())
                adminPanel(candidates, totalCandidates, voters, totalVoters, electionOver);
        }
        else
            break;
    }
    return 0;
}
