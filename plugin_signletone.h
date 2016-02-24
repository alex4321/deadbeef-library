#include "plugin_base.h"

#define DEADBEEF_PLUGIN(Class) \
    public: \
        static Class* Get(DB_functions_t* deadbeef = NULL); \

#define DEADBEEF_PLUGIN_IMPLEMENTATION(Class) \
    Class* Class::Get(DB_functions_t* deadbeef) { \
        if (Class::instance == NULL) Class::instance = new Class(deadbeef); \
        return (Class*)Class::instance; \
    }
