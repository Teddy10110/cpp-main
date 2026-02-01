#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

// ===== Colors =====
#define RESET "\033[0m"
#define BG_WHITE "\033[47m"
#define TEXT_BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
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

// ===== File Names =====
const string CANDIDATE_FILE = "candidates.txt";
const string VOTER_FILE = "voters.txt";
const string STATE_FILE = "election_state.txt";

// ===== File Functions =====
void saveCandidates(Candidate c[], int total)
{
    ofstream file(CANDIDATE_FILE);
    file << total << endl;
    for (int i = 0; i < total; i++)
        file << c[i].name << "|" << c[i].votes << endl;
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

    cout << "\n===== ADMIN LOGIN =====\n";
    cout << "Username: ";
    cin >> u;
    cout << "Password: ";
    cin >> p;

    if (u == ADMIN_USER && p == ADMIN_PASS)
    {
        cout << GREEN << "Login successful!\n" << RESET;
        return true;
    }
    cout << RED << "Wrong credentials!\n" << RESET;
    return false;
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
        cout << "2. Remove Candidate\n";
        cout << "3. View Candidates\n";
        cout << "4. View Voters\n";
        cout << "5. Reset Election\n";
        cout << "6. End Election & Results\n";
        cout << "7. Back\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            if (totalCandidates >= MAX_CANDIDATES)
            {
                cout << RED << "Candidate limit reached!\n" << RESET;
                cin.get();
                continue;
            }
            cout << "Candidate name: ";
            getline(cin, candidates[totalCandidates].name);
            candidates[totalCandidates].votes = 0;
            totalCandidates++;
            saveCandidates(candidates, totalCandidates);
            cout << GREEN << "Candidate added!\n" << RESET;
            cin.get();
        }

        else if (choice == 2)
        {
            for (int i = 0; i < totalCandidates; i++)
                cout << i + 1 << ". " << candidates[i].name << endl;

            int r;
            cout << "Remove number: ";
            cin >> r;

            if (r >= 1 && r <= totalCandidates)
            {
                for (int i = r - 1; i < totalCandidates - 1; i++)
                    candidates[i] = candidates[i + 1];
                totalCandidates--;
                saveCandidates(candidates, totalCandidates);
                cout << GREEN << "Candidate removed!\n" << RESET;
            }
            cin.ignore();
            cin.get();
        }

        else if (choice == 3)
        {
            for (int i = 0; i < totalCandidates; i++)
                cout << candidates[i].name << " (" << candidates[i].votes << ")\n";
            cin.get();
        }

        else if (choice == 4)
        {
            for (int i = 0; i < totalVoters; i++)
                cout << voters[i].voterID << " | " << voters[i].name
                     << " | " << (voters[i].hasVoted ? "Voted" : "Not Voted") << endl;
            cin.get();
        }

        else if (choice == 5)
        {
            for (int i = 0; i < totalCandidates; i++)
                candidates[i].votes = 0;
            for (int i = 0; i < totalVoters; i++)
                voters[i].hasVoted = false;

            electionOver = false;
            saveCandidates(candidates, totalCandidates);
            saveVoters(voters, totalVoters);
            saveElectionState(electionOver);

            cout << GREEN << "Election reset!\n" << RESET;
            cin.get();
        }

        else if (choice == 6)
        {
            electionOver = true;
            saveElectionState(electionOver);

            sortCandidates(candidates, totalCandidates);
            cout << "\n===== RESULTS =====\n";
            for (int i = 0; i < totalCandidates; i++)
                cout << candidates[i].name << " -> " << candidates[i].votes << endl;
            cin.get();
        }

        else if (choice == 7)
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
            if (electionOver)
            {
                cout << RED << "Election closed!\n" << RESET;
                cin.get();
                continue;
            }
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

            cout << GREEN << "Registered! Voter ID: " << v.voterID << RESET << endl;
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
                        cout << j + 1 << ". " << candidates[j].name << endl;

                    int v;
                    cin >> v;
                    if (v >= 1 && v <= totalCandidates)
                    {
                        candidates[v - 1].votes++;
                        voters[i].hasVoted = true;
                        saveCandidates(candidates, totalCandidates);
                        saveVoters(voters, totalVoters);
                        cout << GREEN << "Vote cast!\n" << RESET;
                    }
                }
            }
            cin.ignore();
            cin.get();
        }

        else if (choice == 3)
        {
            for (int i = 0; i < totalCandidates; i++)
                cout << candidates[i].name << endl;
            cin.get();
        }

        else if (choice == 4)
        {
            if (adminLogin())
                adminPanel(candidates, totalCandidates,
                           voters, totalVoters, electionOver);
        }

        else if (choice == 5)
            break;
    }
    return 0;
}
