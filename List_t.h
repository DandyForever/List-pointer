#pragma once

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iostream>

typedef char* ListElem_t;

const int Crashcan1 = 666666666, Crashcan2 = 999999999;
const int Max_list_size = 20000, Smallptr = 7;

struct Elem
{
    int canaryleft = Crashcan1;

    ListElem_t value;
    Elem* next;
    Elem* prev;

    int canaryright = Crashcan2;
};

class List_t
{
    private:
        int canaryleft;

        Elem* head;
        Elem* tail;
        int size;

        int canaryright;

        bool dump                     (const int value,    FILE* file_error_info);
        bool dump                     (const double value, FILE* file_error_info);
        bool dump                     (const void* value,  FILE* file_error_info);
    public:
        bool ListElemOK               ();
        bool ListOK                   ();
        void Dump                     ();

        List_t                        ();
        ~List_t                       ();

        Elem* HeadEl                  ();
        Elem* TailEl                  ();
        Elem* NextEl                  (Elem* elem);
        Elem* PrevEl                  (Elem* elem);
        ListElem_t ValEl              (Elem* elem);
        int Size                      ();

        void PushFront                (const ListElem_t value);
        void PushBack                 (const ListElem_t value);
        ListElem_t PopBack            ();
        ListElem_t PopFront           ();
        void Insert                   (Elem* pointer, const ListElem_t value);
        ListElem_t Erase              (Elem* pointer);

        Elem* InOrder                 ();

};
