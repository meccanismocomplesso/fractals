/* February 2006: modified by The-BlueAnt */
/* September 2002: Craig Wilkie <craig.wilkie@bigfoot.com>
               General Utilities for Windows Screensavers */


#include <windows.h>
#include <stdio.h>
#include "options.h"

char registry_string[500];

void set_dialog_options(HWND hDlg, OPTIONS_STRUCT *options)
{
	/* Loop through each of the options in the structure */
	OPTION *option = options->options;

	/* Loop through all the valid options */
	while (option->type != -1)
	{
		if (option->type == LONG_EDIT_TYPE)
		{
			SetDlgItemInt(hDlg, option->control_id, *(option->long_value), FALSE);
		}
		else if (option->type == CHECK_TYPE)
		{
            CheckDlgButton(hDlg, option->control_id, (*(option->long_value) == 1) ? BST_CHECKED : BST_UNCHECKED);
		}
		else
		{
			/* Invalid option type */
		}

		option++;
	}
}

void get_dialog_options(HWND hDlg, OPTIONS_STRUCT *options)
{
	/* Loop through each of the options in the structure */
	OPTION *option = options->options;
	long temp;

	/* Loop through all the valid options */
	while (option->type != -1)
	{
		if (option->type == LONG_EDIT_TYPE)
		{
	        temp = GetDlgItemInt(hDlg, option->control_id, NULL, FALSE);
			if (temp < option->min_value)
			{
				*(option->long_value) = option->min_value;
			}
			else if (temp > option->max_value)
			{
				*(option->long_value) = option->max_value;
			}
			else
			{
				*(option->long_value) = temp;
			}
		}
		else if (option->type == CHECK_TYPE)
		{
			
			*(option->long_value) = (IsDlgButtonChecked(hDlg, option->control_id) == BST_CHECKED) ? 1 : 0;
		}
		else
		{
			/* Invalid option type */
		}

		option++;
	}
}

void read_registry_data(OPTIONS_STRUCT *options)
{
    HKEY key;
    DWORD size;
    DWORD disposition;
    LONG result;

    sprintf(registry_string, "Software\\The-BlueAnt Software\\Screensaver\\%s", options->screenhack_name);
    result = RegCreateKeyEx(HKEY_CURRENT_USER, registry_string, 0,
                                    "ScreenSaverClass",  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                                    NULL,   &key,   &disposition);
    if (result == ERROR_SUCCESS)
    {
        size = sizeof(long);

		/* Loop through each of the options in the structure */
		OPTION *option = options->options;

		/* Loop through all the valid options */
		while (option->type != -1)
		{
			/* Long edit, and check types are both handled the same as far as the registry is concerned */
			if ((option->type == LONG_EDIT_TYPE) || (option->type == CHECK_TYPE))
			{
				long temp;

		        result = RegQueryValueEx(key, option->reg_name, NULL, NULL, (unsigned char *)(&temp), &size);
				*(option->long_value) = (result != ERROR_SUCCESS) ? option->default_value : temp;
			}
			else
			{
				/* Invalid option type */
			}

			option++;
		}

        RegCloseKey(key);
    }
}

void write_registry_data(OPTIONS_STRUCT *options)
{
    HKEY key;
    DWORD size;
    LONG result;
    DWORD disposition;

    sprintf(registry_string, "Software\\The-BlueAnt Software\\Screensaver\\%s", options->screenhack_name);
    result = RegCreateKeyEx(HKEY_CURRENT_USER, registry_string, 0,
                                    "ScreenSaverClass",  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
                                    NULL,   &key,   &disposition);
    if (result == ERROR_SUCCESS)
    {
        size = sizeof(long);

		/* Loop through each of the options in the structure */
		OPTION *option = options->options;

		/* Loop through all the valid options */
		while (option->type != -1)
		{
			/* Long edit, and check types are both handled the same as far as the registry is concerned */
			if ((option->type == LONG_EDIT_TYPE) || (option->type == CHECK_TYPE))
			{
				long temp = *(option->long_value);

		        RegSetValueEx(key, option->reg_name, 0, REG_BINARY, (unsigned char *)(&temp), size); 
			}
			else
			{
				/* Invalid option type */
			}

			option++;
		}

        RegCloseKey(key);
    }
}

