#ifndef CALENDAR_COMMANDS_H
#define CALENDAR_COMMANDS_H

#include <bonobo/bonobo-control.h>
#include "gnome-cal.h"

/* This enum and the following array define the color preferences */

typedef enum {
	COLOR_PROP_OUTLINE_COLOR,	/* Color of calendar outline */
	COLOR_PROP_HEADING_COLOR,	/* Color for headings */
	COLOR_PROP_EMPTY_DAY_BG,	/* Background color for empty days */
	COLOR_PROP_MARK_DAY_BG,		/* Background color for days with appointments */
	COLOR_PROP_PRELIGHT_DAY_BG,	/* Background color for prelighted day */
	COLOR_PROP_DAY_FG,		/* Color for day numbers */
	COLOR_PROP_CURRENT_DAY_FG,	/* Color for current day's number */
	COLOR_PROP_TODO_NOT_DUE_YET,    /* Color for Todo items not yet due */
	COLOR_PROP_TODO_DUE_TODAY,      /* Color for Todo items due today */ 
	COLOR_PROP_TODO_OVERDUE,        /* Color for Todo items that are overdue */
	COLOR_PROP_LAST			/* Number of color properties */
} ColorProp;

struct color_prop {
	int r;		/* Values are in [0, 65535] */
	int g;
	int b;
	char *label;	/* Label for properties dialog */
	char *key;	/* Key for gnome_config */
};

extern struct color_prop color_props[];


#define COOKIE_USER_HOME_DIR ((char *) -1)

 
/* Calendar preferences */

extern int day_begin, day_end;
extern char *user_name;
extern int am_pm_flag;
extern int week_starts_on_monday;

/* alarm stuff */
#if 0
extern CalendarAlarm alarm_defaults[4];
#endif
extern gboolean beep_on_display;
extern gboolean enable_aalarm_timeout;
extern guint audio_alarm_timeout;
extern const guint MAX_AALARM_TIMEOUT;
extern gboolean enable_snooze;
extern guint snooze_secs;
extern const guint MAX_SNOOZE_SECS;



/* This tells all the calendars to reload the config settings. */
void update_all_config_settings (void);

/* Returns a pointer to a statically-allocated string with a representation of the specified color.
 * Values must be in [0, 65535].
 */
char *build_color_spec (int r, int g, int b);

/* Parses a color specification of the form "#%04x%04x%04x" and returns the color components. */
void parse_color_spec (char *spec, int *r, int *g, int *b);

/* Calls build_color_spec() for the color in the specified property number */
char *color_spec_from_prop (ColorProp propnum);

GnomeCalendar *new_calendar (char *full_name);

void calendar_set_uri (GnomeCalendar *gcal, char *calendar_file);


/*----------------------------------------------------------------------*/
/* FIXME -- where should this stuff go?                                 */
/*----------------------------------------------------------------------*/

void init_calendar (void);

void calendar_control_activate (BonoboControl *control,
				GnomeCalendar *cal);
void calendar_control_deactivate (BonoboControl *control);

void quit_cmd (BonoboUIComponent *uih, void *user_data, const char *path);

/*extern char *user_calendar_file;*/
extern char *user_name;
extern char *full_name;
extern int debug_alarms;

#endif /* CALENDAR_COMMANDS_H */
