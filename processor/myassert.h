#ifndef MYASSERT_H_INCLUDED
#define MYASSERT_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#define DEBUG 1

#ifdef DEBUG

#define ASSERT_OK(type, instance) \
    if (!instance) \
    { \
        printf(#type " failed in %s in function %s, line %d", __FILE__, __PRETTY_FUNCTION__ , __LINE__); \
        abort(); \
    } else if (!type##_ok(instance)) \
    { \
        type##_dump(instance, #instance); \
        printf(#type " failed in %s in function %s, line %d", __FILE__, __PRETTY_FUNCTION__ , __LINE__); \
        abort(); \
    }

#else

#define ASSERT_OK(type, instance)

#endif

#endif // MYASSERT_H_INCLUDED
