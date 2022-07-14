#include "systemc.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

extern string srt;
extern vector<string> tokens_global;

SC_MODULE(Data_Convertor) {
    sc_in<bool> enable;
    /*    sc_out<sc_int<32>> in;
        sc_out<bool> reset, read, write*/;
        //string s ;

        void get_tokens() {
            std::vector<std::string> tokens;
            std::string token;
            std::stringstream ss(srt);
            while (getline(ss, token, '|')) {
                tokens.push_back(token);
            }
            tokens_global = tokens;
        }

        SC_CTOR(Data_Convertor) {
            SC_THREAD(get_tokens);
            sensitive << enable;
        }
};