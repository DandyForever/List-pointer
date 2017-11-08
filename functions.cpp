#include "list_t.h"
#include "stack_t.h"

#define assert(condition)                                                   \
        if (!condition)                                                     \
        {                                                                   \
        printf("ASSERTION %s in %s (%d)\n", #condition, __FILE__, __LINE__);\
        abort();                                                            \
        }

//--------------------------------------------------------
//! Macro to choose DEBUG mode
//!
//! @note write '1' to 'if' to use DEBUG mode
//! @note write '0' to 'if' not to use DEBUG mode
//--------------------------------------------------------
#if (0)
    #define ASSERT_LIST(); \
            if (!ListOK ())\
            {                 \
            Dump();    \
            assert(0);        \
            }
#else
    #define ASSERT_LIST();
#endif


List_t::List_t ():
    canaryleft (Crashcan1),

    head (nullptr),
    tail (nullptr),
    size (0),

    canaryright (Crashcan2)
    {
        ASSERT_LIST()
    }

List_t::~List_t ()
{
    ASSERT_LIST()

    while (head != tail)
    {
        Elem* head_c = head;
        head = head->next;

        delete[] (head_c);
    }
    delete[] (head);

    head = nullptr;
    tail = nullptr;
    size = 0;
}

bool List_t::ListElemOK ()
{
    bool isOK = 1;

    Elem* current = head;

    for (int i = 0; i < size; i++)
    {
        if (current != head)
        {
            if (current->prev->next != current)
                isOK = 1;
        }

        else
        {
            if (current->prev != nullptr)
                isOK = 0;
        }

        if (current != tail)
        {
            if (current->next->prev != current)
                isOK = 1;
        }

        else
        {
            if (current->next != nullptr)
                isOK = 0;
        }

        if (current == nullptr)
            isOK = 0;

        if (current->canaryleft != Crashcan1 || current->canaryright != Crashcan2)
            isOK = 0;

        current = current->next;
    }

    if (current != nullptr)
        isOK = 0;

    return isOK;
}

bool List_t::ListOK ()
{
    return this && size >= 0 && canaryleft == Crashcan1 && canaryright == Crashcan2 && ListElemOK ();
}

bool List_t::dump (const int value, FILE* output_file)
{
    fprintf (output_file, "%d ", value);

    return 1;
}

bool List_t::dump (const double value, FILE* output_file)
{
    if (isnan (value))
    {
        fprintf (output_file, "%f\\nnot a number ", value);

        return 0;
    }

    else
    {
        fprintf (output_file, " %f ", value);

        return 1;
    }
}

bool List_t::dump (const void* value, FILE* output_file)
{
    ListElem_t* val = (ListElem_t*) value;

    if (val == nullptr)
    {
        fprintf (output_file, " %p\\nzero pointer ", val);

        return 0;
    }

    else
    {
        fprintf (output_file, " %p ", val);

        return 1;
    }
}

void List_t::Dump ()
{
    FILE* output_file = fopen ("Dump.dot", "w");
    assert (output_file);

    fprintf (output_file, "digraph List\n{\n\trankdir = LR\n");
    fprintf (output_file, "\t\tnode [ shape = \"box\", color = \"black\" ]\n");
    fprintf (output_file, "\t\tedge [ color = \"black\" ]\n\n");

    if (canaryleft != Crashcan1)
        fprintf (output_file, "\tCanaryLEFT [ label = \"CanaryLEFT = %d\\nBUT EXPECTED %d\","
                 "color = \"red\" ]\n", canaryleft, Crashcan1);
    else
        fprintf (output_file, "\tCanaryLEFT [ label = \"CanaryLEFT = %d\" ]\n", canaryleft);

    if (canaryright != Crashcan2)
        fprintf (output_file, "\tCanaryRIGHT [ label = \"CanaryRIGHT = %d\\nBUT EXPECTED %d\","
                 "color = \"red\" ]\n", canaryright, Crashcan2);
    else
        fprintf (output_file, "\tCanaryRIGHT [ label = \"CanaryRIGHT = %d\" ]\n", canaryright);

    if (size >= 0)
        fprintf (output_file, "\tsize [ label = \"size = %d\" ]\n\n", size);
    else
        fprintf (output_file, "\tsize [ label = \"size = %d\\n<0\", color = \"red\"]\n\n", size);

    Elem* current = head;
    assert (current);

    int number = 1;

    while (current != nullptr)
    {
        fprintf (output_file, "\telem%d [ shape = \"record\", label = \"<pointer>%d\\n", number, number);

        if (current == head)
        {
            fprintf (output_file, "HEAD\\n");
        }

        if (current == tail)
        {
            fprintf (output_file, "TAIL\\n");
        }

        fprintf (output_file, "%p | {<prev> prev\\n %p | value\\n "
                 , current, current->prev);

        bool isnormal = dump (current->value, output_file);

        if (current->canaryleft != Crashcan1 || current->canaryright != Crashcan2)
            isnormal = 0;

        fprintf (output_file, "| <next> next\\n%p} | {canaryLEFT\\n%d | canaryRIGHT\\n%d} \", color = "
                 , current->next, current->canaryleft, current->canaryright);

        if (isnormal)
            fprintf (output_file, "\"blue\" ]\n");

        else
            fprintf (output_file, "\"red\" ]\n");

        number++;
        current = current->next;
    }

    fprintf (output_file, "\n");
    current = head;

    number = 1;

    while (current != nullptr)
    {
        if (current->prev != nullptr)
            fprintf (output_file, "\telem%d:<prev> -> elem%d:<pointer>\n", number, number - 1);

        if (current->next != nullptr)
            fprintf (output_file, "\telem%d:<next> -> elem%d:<pointer> [ color = \"green\" ]\n", number, number + 1);

        number++;
        current = current->next;
    }


    fprintf (output_file, "}");
    fclose (output_file);


    system ("dot -Tpng Dump.dot -o Dump.png");
}


void List_t::PushBack (const ListElem_t value)
{
    ASSERT_LIST()

    if (size >= Max_list_size)
    {
        printf ("NOT ENOUGH MEMORY\n");
        assert (0);
    }

    Elem* elem = new Elem;

    if (size == 0)
    {
        head = elem;
        tail = elem;

        elem->next = nullptr;
        elem->prev = nullptr;
        elem->value = value;
    }

    else
    {
        tail->next = elem;

        elem->prev = tail;
        elem->next = nullptr;
        elem->value = value;

        tail = elem;
    }
    size++;

    ASSERT_LIST()
}


void List_t::PushFront (const ListElem_t value)
{
    ASSERT_LIST()

    if (size >= Max_list_size)
    {
        printf ("NOT ENOUGH MEMORY\n");
        assert (0);
    }

    Elem* elem = new Elem;

    if (size == 0)
    {
        head = elem;
        tail = elem;

        elem->next = nullptr;
        elem->prev = nullptr;
        elem-> value = value;
    }

    else
    {
        head->prev = elem;

        elem->next = head;
        elem->prev = nullptr;
        elem->value = value;

        head = elem;
    }
    size++;

    ASSERT_LIST()
}

ListElem_t List_t::PopBack ()
{
    ASSERT_LIST()

    if (size == 0)
    {
        printf ("LIST IS EMPTY\n");
        assert (0)
    }

    ListElem_t value = tail->value;
    Elem* elem = tail;

    if (size == 1)
    {
        head = nullptr;
        tail = nullptr;
    }

    else
    {
        tail->prev->next = nullptr;

        tail = tail->prev;
    }
    size--;

    delete elem;

    ASSERT_LIST()

    return value;
}

ListElem_t List_t::PopFront ()
{
    ASSERT_LIST()

    if (size == 0)
    {
        printf ("LIST IS EMPTY\n");
        assert (0);
    }

    ListElem_t value = head->value;
    Elem* elem = head;

    if (size == 1)
    {
        head = nullptr;
        tail = nullptr;
    }

    else
    {
        head->next->prev = nullptr;

        head = head->next;
    }
    size--;

    delete elem;

    ASSERT_LIST()

    return value;
}

void List_t::Insert (Elem* pointer, const ListElem_t value)
{
    ASSERT_LIST()

    if (size >= Max_list_size)
    {
        printf ("NOT ENOUGH MEMORY\n");
        assert (0);
    }

    if (pointer == tail)
        PushBack (value);

    else if (pointer->next->prev != pointer)
    {
        printf ("BAD POINTER %p\n", pointer);//value
        assert (0);
    }

    else
    {
        Elem* elem = new Elem;

        elem->next = pointer->next;
        elem->prev = pointer;
        elem->value = value;

        elem->next->prev = elem;
        elem->prev->next = elem;

        size++;
    }

    ASSERT_LIST()
}

ListElem_t List_t::Erase (Elem* pointer)
{
    ASSERT_LIST()

    if (size == 0)
    {
        printf ("LIST IS EMPTY\n");
        assert (0);
    }

    if (pointer == tail)
        return PopBack ();

    else if (pointer == head)
        return PopFront ();

    else if (pointer->next->prev == pointer && pointer->prev->next == pointer)
    {
        Elem* elem = pointer;
        ListElem_t value = elem->value;

        elem->prev->next = elem->next;
        elem->next->prev = elem->prev;

        delete elem;

        size--;

        ASSERT_LIST()

        return value;
    }
}

Elem* List_t::HeadEl ()
{
    ASSERT_LIST()

    return head;
}

Elem* List_t::TailEl ()
{
    ASSERT_LIST()

    return tail;
}

Elem* List_t::NextEl (Elem* elem)
{
    ASSERT_LIST()

    if (elem->next->prev != elem)
    {
        printf ("BAD POINTER %p\n", elem);
        assert (0);
    }

    return elem->next;
}

Elem* List_t::PrevEl (Elem* elem)
{
    ASSERT_LIST()

    if (elem->prev->next != elem)
    {
        printf ("BAD POINTER %p\n", elem);
        assert (0);
    }

    return elem->prev;
}

ListElem_t List_t::ValEl (Elem* elem)
{
    ASSERT_LIST()

    if (elem != tail && elem != head && elem->next->prev != elem)
    {
        printf ("BAD POINTER %p\n", elem);
        assert (0);
    }

    return elem->value;
}

int List_t::Size ()
{
    ASSERT_LIST()

    return size;
}

Elem* List_t::InOrder ()
{
    ASSERT_LIST()

    Elem buffer[size] = {};

    int i = 0;

    for (Elem* current = head; current != nullptr; current = current->next)
    {
        buffer[i++] = *current;
    }

    ASSERT_LIST()

    return buffer;
}
