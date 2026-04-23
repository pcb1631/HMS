#include "commands.h"

Value make_user(Value * list)
{



    generate_file_structure(list[0].data.as_string, list[1].data.as_string);
    return make_int(0);
}
