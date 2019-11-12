/* September 2002: Craig Wilkie <craig.wilkie@bigfoot.com>
               General Utilities for Windows Screensavers */

#ifndef OPTIONS_H
#define OPTIONS_H

#define LONG_EDIT_TYPE  0
#define CHECK_TYPE      1

typedef struct _option
{
    long type;
    long *long_value;
    int control_id;
	long default_value;
	long min_value;
	long max_value;
	char *reg_name;

} OPTION;

typedef struct _options_struct
{
    char *screenhack_name;
    OPTION *options;

} OPTIONS_STRUCT;

void read_registry_data(OPTIONS_STRUCT *options);
void write_registry_data(OPTIONS_STRUCT *options);

void set_dialog_options(HWND hDlg, OPTIONS_STRUCT *options);
void get_dialog_options(HWND hDlg, OPTIONS_STRUCT *options);

#endif


