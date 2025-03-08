/*
  Name: mirc_codes.h
  Author: hadez@darklab.org
  Date: 27.12.03 21:05
  Description: see mirc_codes.cpp
*/

#ifndef MIRC_CODES_H
#define MIRC_CODES_H
class mirc_codes
{
public:
    static void clean(char*);     // strip all codes from char*
private:
    static bool is_number(const int);    // check for number \__ used by colorcode check
    static bool is_comma(const int);     // check for comma  /
};
#endif
