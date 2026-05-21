#include "globals.h"

/* I'm fucking sick of cargo culting the menu dispatch. I tried passing it as the first arg to every command function and it took me hours to debug.
 *
* Just one global */
char current_username[64] = "\0";
