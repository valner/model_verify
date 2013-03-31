/******************************************************************************
 * File: state_maker.cpp
 * Description: class for getting all states of C program.
 * Date: March 2013 
 * Author: Valery Bitsoev
 * Email: valner47@gmail.com
*******************************************************************************/

#include "state_maker_v2.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h> 
using namespace std;

const char* Info = "Usage: <binary name> <f::a> <f::b> <g::a> <g::b>\n"
                   "Allowed flags:\n"
                   "-count: prints number of states in console\n"
                   "-file <out file>: redirect output to specified file\n\n"
                   "Author: Valery Bitsoev. March 2013\n";
/**
 * @brief checks is string consist of only digits
 * @param string - string to check
 */
bool isNumber(char * string)
{
    for(int i=0;string[i]!='\0';i++)
        if(!isdigit(string[i]))
            return false;
    return true;
}
/**
 * @brief overloaded output operator for IntVariable
 * @param stream - output stream
 * @param right - object to print
 * @return reference to stream
 */
ostream& operator<<(ostream& stream, const IntVariable& right)
{
    if(right.m_init_flag)
    {
        stream<<right.m_value;
    }
    else
    {
        stream<<"#";
    }
    return stream;
}

/**
 * @brief class constructor
 * @param filename - name of output file
 * @param count_flag - flag, indicates if need print state number
 */
StateMaker::StateMaker(char* filename, bool count_flag, char* lts_filename): m_states_number(0)
                                      , m_count_flag(count_flag) 
                                      , m_file(filename)
                                      , m_old()
                                      , m_lts(false)
                                      , m_lts_file(lts_filename)
{
    if(!filename)
    {
        m_file.open("states.txt");
    }
    if(lts_filename)
    {
        if(m_lts_file.bad())
        {
            // filename specified, but can't open file for write
            cerr<<"Can't create lts file"<<endl;
        }
        else
        {
            // successful openned file 
            m_lts = true;
        }
    }
}

/**
 * @brief class destructor
 */
StateMaker::~StateMaker()
{
    m_file.close();
}

/**
 * @brief generates and prints states in console or file
 * @param f_a initial value of f::a
 * @param f_b initial value of f::b
 * @param g_a initial value of g::a
 * @param g_b initial value of g::b
 */
void StateMaker::PrintStates(int f_a, int f_b, int g_a, int g_b)
{
    // fill f data
    FuncVars f;
    f.a = f_a;
    f.b = f_b;
    f.counter = 0;

    // fill g data
    FuncVars g;
    g.a = g_a;
    g.b = g.b;
    g.counter = 0;

    // global variable
    IntVariable h;
    if (m_lts)
    {
        m_lts_file<<"digraph G{"<<endl;
    }
    // start generating
    GenerateStates(f, g, h);
    if(m_count_flag) cout<<"Result states number: "<<m_states_number<<endl;
    if (m_lts)
    {
        m_lts_file<<"}"<<endl;
    }
}


int StateMaker::CalcHash(unsigned int fcounter, unsigned int gcounter, IntVariable h)
{
    int tmp;
    if (!h.m_init_flag)
        tmp = 19;
    else
        tmp = h.m_value;
    return tmp + fcounter*20 + gcounter*20*20;
}
/** 
 * @brief generates and prints all states after state described by f, g and h
 * @param f - state of function f
 * @param g - state of function g
 * @param h - state of global variable
 */
void StateMaker::GenerateStates(FuncVars f, FuncVars g, IntVariable h)
{
    if(m_old.count(StateDiff(f.counter, g.counter, h))!=0)
        return;
    m_old.insert(StateDiff(f.counter, g.counter, h));
    if(m_lts)
    {
        m_lts_file<<"    "<<CalcHash(f.counter, g.counter, h)<<" [label=\""<<f.counter<<" ";
        m_lts_file<<g.counter<<" "<<h<<" "<<f.x<<" "<<f.y<<" "<<g.x<<" "<<g.y<<"\"];"<<endl;
    }
    ++m_states_number;
    // print to file
    m_file<<f.counter<<", "<<g.counter<<", "<<h<<", "<<f.x<<", "<<f.y<<", "<<g.x<<", "<<g.y<<endl;
    // activated thread with f
    if(f.counter < 11) StepInF(f,g,h);
    // activated thread with g
    if(g.counter < 17) StepInG(f,g,h);
}

/**
 * @brief do step by f code
 * @param f - state of function f
 * @param g - state of function g
 * @param g - state of global variable
 */
void StateMaker::StepInF(FuncVars f, FuncVars g, IntVariable h)
{
    if(m_lts)
    {
        m_lts_file<<"    "<<CalcHash(f.counter, g.counter, h)<<" -> ";
    }
    switch(f.counter)
    {
        //int x,y
        case 0:
            ++f.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"int x,y;\" color = \"red\"];"<<endl;
            }
            break;
        // x = 6
        case 1:
            f.x = 6;
            ++f.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"x=6;\" color = \"red\"];"<<endl;
            }
            break;
        // y = 1
        case 2:
            f.y = 1;
            ++f.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"y=1;\" color = \"red\"];"<<endl;
            }
            break;
        // h = 6
        case 3: 
            h = 6;
            ++f.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"h=6;\" color = \"red\"];"<<endl;
            }
            break;
        // h = x
        case 4:
            h = f.x;
            ++f.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"h=x;\" color = \"red\"];"<<endl;
            }
            break;
        // if (y>2)
        case 5: 
            f.counter = 7;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"!(y>2);\" color = \"red\"];"<<endl;
            }
            break;
        // if (y<8)
        case 7:
            ++f.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"y<8;\" color = \"red\"];"<<endl;
            }
            break;
        // if (y>5)
        case 8:
            f.counter = 10;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"!(y>5);\" color = \"red\"];"<<endl;
            }
            break;
        // x = 5
        case 10:
            f.x = 5;
            ++f.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"x=5;\" color = \"red\"];"<<endl;
            }
            break;
        // end of f
        case 11:
        // in other cases
        default:
            assert(!"bad f.counter value)");
        break;
    }
    GenerateStates(f,g,h);
}


/**
 * @brief do step by g code
 * @param f - state of function f
 * @param g - state of function g
 * @param g - state of global variable
 */
void StateMaker::StepInG(FuncVars f, FuncVars g, IntVariable h)
{
    if(m_lts)
    {
        m_lts_file<<"    "<<CalcHash(f.counter, g.counter, h)<<" -> ";
    }
    switch(g.counter)
    {
        // int x,y
        case 0:
            ++g.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"int x,y;\" color = \"blue\"];"<<endl;
            }
            break;
        // x = 9
        case 1:
            g.x = 9;
            ++g.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"x = 9;\" color = \"blue\"];"<<endl;
            }
            break;
        // y = 0;
        case 2:
            g.y = 10;
            ++g.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"y = 0;\" color = \"blue\"];"<<endl;
            }
            break;
        // h = 1
        case 3:
            h = 1;
            ++g.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"h = 1;\" color = \"blue\"];"<<endl;
            }
            break;
        // if(x<2)
        case 4:
            g.counter = 6;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"!(x<2);\" color = \"blue\"];"<<endl;
            }
            break;
        // h = 2
        case 6: 
            h = 2;
            ++g.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"h = 2;\" color = \"blue\"];"<<endl;
            }
            break;
        // if(h>7)
        case 7:
            g.counter = 9;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"!(h>7);\" color = \"blue\"];"<<endl;
            }
            break;
        // y = 4
        case 9: 
            g.y = 4;
            ++g.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"y = 4;\" color = \"blue\"];"<<endl;
            }
            break;
        // while (x>7)
        case 10:
            ++g.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"x>7;\" color = \"blue\"];"<<endl;
            }
            break;
        // if (h>0)
        case 11:
            ++g.counter;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"h>0;\" color = \"blue\"];"<<endl;
            }
            break;
        // break
        case 12:
            g.counter = 17;
            if(m_lts)
            {
                m_lts_file<<CalcHash(f.counter, g.counter, h)<<" [label = \"break;\" color = \"blue\"];"<<endl;
            }
            break;
        // end of g
        case 17:
        // in other cases
        default:
            assert(!"bad g.counter value");
            break;
    }
    GenerateStates(f,g,h);
}


int main(int argc, char** argv)
{
    int i = 1;
    bool bad_args = false;
    char* filename = NULL;
    char* lts_filename = NULL;
    bool count_flag = false;
    int f_a = 0;
    int f_b = 0;
    int g_a = 0;
    int g_b = 0;
    //parse argv
    if(argc > 4 && argc < 11)
    {
        while(--argc>0)
        {
            if(strcmp(argv[i],"-file") == 0)
            {
                // flag for filename
                if(argc < 2)
                {
                    bad_args = true;
                    break;
                }
                filename = argv[i+1];
                --argc;
                i+=2;
            }
            else if(strcmp(argv[i],"-lts") == 0)
            {
                // flag for filename
                if(argc < 2)
                {
                    bad_args = true;
                    break;
                }
                lts_filename = argv[i+1];
                --argc;
                i+=2;
            }
            else if(strcmp(argv[i], "-count") == 0)
            {
                // flag to print states number
                count_flag = true;
                ++i;
            }
            else if(isNumber(argv[i]))
            {
                if(argc < 4)
                {
                    // too little args
                    bad_args = true;
                    break;
                }
                if(!isNumber(argv[i+1]) || !isNumber(argv[i+2]) || !isNumber(argv[i+3]))
                {
                    // should be 4 number is a row
                    bad_args = true;
                    break;
                }
                // fill values
                f_a = atoi(argv[i]);
                f_b = atoi(argv[i+1]);
                g_a = atoi(argv[i+2]);
                g_b = atoi(argv[i+3]);
                argc-=3;
                i+=4;
            }
            else
            {
                bad_args = true;
                break;
            }
        }
    }
    else
    {
        // too little or too many args
        bad_args = true;
    }
    if (bad_args)
    {
        // print info about program
        cout<<Info<<endl;
        return 1;
    }
    StateMaker state_maker(filename, count_flag, lts_filename);
    state_maker.PrintStates(f_a, f_b, g_a, g_b);
}
