/*
 * Copyright (c) 2009 Kross Windz <krosswindz@gmail.com>.
 * All rights reserved.
 */

#include <curses.h>
#include <stdint.h>
#include <stdlib.h>

#define VER_MAJ 0
#define VER_MIN 2

#define SEC_MINUTES 60
#define MIN_HOURS 60
#define MSEC_SECONDS 1000
#define MSEC_MINUTES MSEC_SECONDS * SEC_MINUTES
#define MSEC_HOURS MSEC_MINUTES * MIN_HOURS

static void display_header (void);
static void get_menu_choice (void);
static void get_offset (void);
static void get_time (void);
static void print_menu (int);
static void timecalc (void);

static int mt_h = 0;
static int mt_m = 0;
static int mt_ms = 0;
static int mt_s = 0;
static int t_h = 0;
static int t_m = 0;
static int t_ms = 0;
static int t_s = 0;
static int os_h = 0;
static int os_m = 0;
static int os_ms = 0;
static int os_s = 0;
static volatile int os_neg = 0;
static volatile int loop = 1;
static WINDOW *win_header;
static WINDOW *win_main;

int
main (void)
{
	initscr ();
	cbreak ();
	noecho ();
	refresh ();
	win_header = newwin (4, 80, 0, 0);
	win_main = newwin (20, 80, 4, 0);
	keypad (win_main, TRUE);
	display_header ();
	while (loop == 1)
		get_menu_choice ();

	delwin (win_main);
	delwin (win_header);
	endwin ();
	return EXIT_SUCCESS;
}

static void
display_header (void)
{
	wprintw (win_header, "TmG Time calculator v%d.%d\n", VER_MAJ, VER_MIN);
	wprintw (win_header, "Copyright (c) 2009 Kross Windz "
			"<krosswindz@gmail.com>\n");
	wprintw (win_header, "All rights reserved.\n");
	wrefresh (win_header);

	return;
}

static void
get_menu_choice (void)
{
	int c;
	int choice = -1;
	static int select = 0;

	print_menu (select);
	while (1) {
		c = wgetch (win_main);
		switch (c) {
			case KEY_DOWN:
				select++;
				break;

			case KEY_UP:
				select--;
				if (select < 0)
					select = 2;

				break;

			case 10:
				choice = select;
				break;

			default:
				choice = c - 48;
		}

		select %= 3;
		print_menu (select);
		if (choice != -1)
			break;
	}

	switch (choice) {
		case 0:
			get_offset ();
			break;

		case 1:
			get_time ();
			break;

		case 2:
			loop = 0;
			break;

		default:
			choice = select;
	}

	select = choice;

	return;
}

static void
get_offset (void)
{
	char c = '\0';

	werase (win_main);
	echo ();
	wprintw (win_main, "Is offset negative (y/N): ");
	wrefresh (win_main);
	wscanw (win_main, "%c", &c);
	if ((c == 'y') || (c == 'Y'))
		os_neg = 1;
	else
		os_neg = 0;

	wprintw (win_main, "Enter offset (hh:mm:ss.mil): ");
	wrefresh (win_main);
	wscanw (win_main, "%02d:%02d:%02d.%03d", &os_h, &os_m, &os_s, &os_ms);
	noecho ();
	timecalc ();

	return;
}

static void
get_time (void)
{
	werase (win_main);
	echo ();
	wprintw (win_main, "Enter time (hh:mm:ss.mil): ");
	wrefresh (win_main);
	wscanw (win_main, "%02d:%02d:%02d.%03d", &t_h, &t_m, &t_s, &t_ms);
	noecho();
	timecalc ();

	return;
}

static void
print_menu (int highlight)
{
	werase (win_main);
	mvwprintw (win_main, 19, 0, "Use Up/Down arrow keys to mov Up/down. "
			"Press Enter or number to select choice.");
	mvwprintw (win_main, 0, 0, "Menu\n====\n");
	if (highlight == 0) {
		wattron (win_main, A_REVERSE);
		mvwprintw (win_main, 2, 0, "0 - Enter time offset (offset: ");
		if (os_neg == 1)
			wprintw (win_main, "-");

		wprintw (win_main, "%02d:%02d:%02d.%03d)\n", os_h, os_m, os_s,
				os_ms);
		wattroff (win_main, A_REVERSE);
	} else {
		mvwprintw (win_main, 2, 0, "0 - Enter time offset (offset: ");
		if (os_neg == 1)
			wprintw (win_main, "-");

		wprintw (win_main, "%02d:%02d:%02d.%03d)\n", os_h, os_m, os_s,
				os_ms);
	}

	if (highlight == 1) {
		wattron (win_main, A_REVERSE);
		mvwprintw (win_main, 3, 0, "1 - Enter time (time: %02d:%02d:"
				"%02d.%03d)\n", t_h, t_m, t_s, t_ms);
		wattroff (win_main, A_REVERSE);
	} else {
		mvwprintw (win_main, 3, 0, "1 - Enter time (time: %02d:%02d:"
				"%02d.%03d)\n", t_h, t_m, t_s, t_ms);
	}

	if (highlight == 2) {
		wattron (win_main, A_REVERSE);
		mvwprintw (win_main, 4, 0, "2 - Exit calculator.\n");
		wattroff (win_main, A_REVERSE);
	} else {
		mvwprintw (win_main, 4, 0, "2 - Exit calculator.\n");
	}

	mvwprintw (win_main, 6, 0, "Modified time: %02d:%02d:%02d.%03d\n",
			mt_h, mt_m, mt_s, mt_ms);
	wrefresh (win_main);
	return;
}

static void
timecalc (void)
{
	uint64_t os, t;
	int temp_h, temp_m, temp_s, temp_ms;

	mt_h = mt_m = mt_s = mt_ms = 0;
	os = os_ms + os_s * MSEC_SECONDS + os_m * MSEC_MINUTES
		+ os_h * MSEC_HOURS;
	t = t_ms + t_s * MSEC_SECONDS + t_m * MSEC_MINUTES + t_h * MSEC_HOURS;
	if ((os_neg == 1) && (t < os))
		return;

	temp_h = t_h;
	temp_m = t_m;
	temp_s = t_s;
	temp_ms = t_ms;
	switch (os_neg) {
		case 0:
			mt_ms += temp_ms + os_ms;
			if (mt_ms >= 1000) {
				mt_s++;
				mt_ms -= 1000;
			}

			mt_s += temp_s + os_s;
			if (mt_s >= 60) {
				mt_m++;
				mt_s -= 60;
			}

			mt_m += temp_m + os_m;
			if (mt_m >= 60) {
				mt_h++;
				mt_m -= 60;
			}

			mt_h += temp_h + os_h;
			break;

		case 1:
			if (temp_ms < os_ms) {
				temp_s--;
				temp_ms += 1000;
			}

			mt_ms = temp_ms - os_ms;
			if (temp_s < os_s) {
				temp_m--;
				temp_s += 60;
			}

			mt_s = temp_s - os_s;
			if (temp_m < os_m) {
				temp_h--;
				temp_m += 60;
			}

			mt_m = temp_m - os_m;
			mt_h = temp_h - os_h;
	}

	return;
}
