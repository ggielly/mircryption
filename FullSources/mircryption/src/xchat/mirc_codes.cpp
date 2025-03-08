/*
  Name: mirc_codes.cpp
  Author: Gregor Jehle <gjehle@gmail.com>
  Date: 27.12.03 21:05
  Description: enables stripping of all mirc colorcodes (if text_stripcolor is enabled in xchat)
  Usage: void mirc_codes::clean(char*)
*/


#include "mirc_codes.h"

bool mirc_codes::is_number(const int test_this)
{
  // checks if the ascii value given in 'test' is a number
  if(test_this >= 48 && test_this <= 57)
  {
    return true;
  }
  return false;
}

bool mirc_codes::is_comma(const int test_this)
{
  // checks if the ascii value given in 'test' is a comma
  if(test_this == 44)
  {
    return true;
  }
  return false;
}


void mirc_codes::clean(char* textline)
{
  int cleanpos = 0;              // remember the position of the last "valid" char
  int switch_condition = 0;
  for(int i=0; textline[i] != '\0'; ++i)
  {
    switch_condition = static_cast<int>(textline[i]);
    switch(switch_condition)
    {
        // mirc colorcodes
        // b k  o  r  u
        // 2 3 15 22 31 dec
        // 2 3 17 26 37 oct 
        case 2:     // bold
        case 15:    // reset
        case 22:    // reverse
        case 31:    // underline
                     // we don't like these, just ignore them
                break;
        case 3:     // color
                     // that one is a bit delicate
                     // first, check if there's a number at the next position
                     // if so, jump to that position and do the same for number or comma
                     // walk through all possible cases.. and that's the point it's starting to get ugly
                if(is_number(textline[i+1]))
                {
                  // n
                  i++;
                  if(is_number(textline[i+1]))
                  {
                    // nn
                    i++;
                    if(is_comma(textline[i+1]))
                    {
                      // nn,
                      i++;
                      if(is_number(textline[i+1]))
                      {
                        // nn,n
                        i++;
                        if(is_number(textline[i+1]))
                        {
                          // nn,nn
                          // worst case scenario O.o
                          i++;
                        }
                      }
                    }
                  }
                  else if(is_comma(textline[i+1]))
                  {
                    // n,
                    i++;
                    if(is_number(textline[i+1]))
                    {
                      // n,n
                      i++;
                      if(is_number(textline[i+1]))
                      {
                        // n,nn
                        i++;
                      }
                    }
                  }
                }
                break;
        default:
                // that's what we want
                // a good, clean.. text
                textline[cleanpos] = textline[i];
                cleanpos++;
                break;
    }
  }
  textline[cleanpos] = '\0';
}
