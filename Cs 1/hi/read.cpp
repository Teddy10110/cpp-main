#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;
    struct bobo
    {
        int studentid;
        char student_name[20];
        float cgpa;
    };
    
int main(){
   
    ifstream read;
    cout<< "Enter the file name: " ;
    char filename[15];
    cin >> filename;
    read.open(filename, ios::in);
    
    bobo biruk;
    cout << "Enter the id of the student: ";
    int sid;
    cin >> sid;
    read.seekg((sid-1)*sizeof(biruk));
    read.read((char*)&biruk,sizeof(biruk));
    cout << "The cgpa of the student is: " ;
    cout << biruk.studentid << endl;
    cout << biruk.student_name << endl;
    cout << biruk.cgpa << endl;

    read.close();


    

    getch();
    
    
}