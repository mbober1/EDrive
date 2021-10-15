#include "utils.hpp"


void * memcpy (void * destination, const void * source, size_t num)
{
    unsigned char* tmp_ptr_1 = (unsigned char*)destination;
    unsigned char* tmp_ptr_2 = (unsigned char*)source;
    
    for (size_t i = 0; i < num; i++)
    {
        tmp_ptr_1[i] = tmp_ptr_2[i];
    }
    
    return destination;
}


void * memset (void * ptr, int value, size_t num)
{
    unsigned char* tmp_ptr = (unsigned char*)ptr;

    for (size_t i = 0; i < num; i++)
    {
        tmp_ptr[0] = value;
    }

    return ptr;
}


int memcmp (const void * ptr1, const void * ptr2, size_t num)
{
    unsigned char* tmp_ptr_1 = (unsigned char*)ptr1;
    unsigned char* tmp_ptr_2 = (unsigned char*)ptr2;

    int result = 0;

    // not same memory block
    if (ptr1 != ptr2)
    {
        while (num > 0)
        {
            if (*tmp_ptr_1 != *tmp_ptr_2)
            {
                result = (*tmp_ptr_1 > *tmp_ptr_2) ? 1:-1;
                break;
            }
            num--;
            tmp_ptr_1++;
            tmp_ptr_2++;
        }
    }

    return result;
}