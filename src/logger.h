#include<iostream>
#include<bits/stdc++.h>
#include<sys/stat.h> 
#include<fstream>
#include<sstream>

using namespace std;

class Logger{

    string logFile = "log";
    ofstream fout;
    
    public:

    Logger();
    void log(string logString);
};

extern Logger logger;