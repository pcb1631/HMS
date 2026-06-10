#include "inventory.h"
#include "../utils/tui.h"
#include "../utils/globals.h"
#include <stdio.h>

Value inventory_edit(void)
{
	Config stock;
	char path[4096];
	const char * disallow[] = {""};
	
	sprintf(path, "%s/stock.txt", DATABASE_PATH);
	parse_config_file(path, &stock);
	config_editor(stock, path, disallow, 0);
	return make_int(0);
}

Value inventory_edit_txteditor(void)
{
	char path[4096];
	sprintf(path, "%s/stock.txt", DATABASE_PATH);
	text_editor(path, 0);
	return make_int(0);
}