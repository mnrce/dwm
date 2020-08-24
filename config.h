/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

/*   Display modes of the tab bar: never shown, always shown, shown only in */
/*   monocle mode in presence of several windows.                           */
/*   Modes after showtab_nmodes are disabled                                */
enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab            = showtab_auto; /* Default tab bar show mode */
static const int toptab             = 1;            /* 0 means bottom tab bar */

static const char *fonts[]          = { "BlexMono Nerd Font:size=11" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"alacritty", "--class", "spterm", "-e", "tmux", "new-session", "-A", "-s", "Scratchpad", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

/* default layout per tags */
/* The first element is for all-tag view, following i-th element corresponds to */
/* tags[i]. Layout is referred using the layouts array index.*/
static int def_layouts[1 + LENGTH(tags)]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                   instance                 title       tags mask     isfloating   monitor */
	{ "Gimp",                   NULL,                    NULL,        0,           1,           -1 },
	{ "firefox",                NULL,                    NULL,        1 << 0,      0,           -1 },
	{ "Nightly",                NULL,                    NULL,        1 << 0,      0,           -1 },
	{ NULL,                     "spterm",                NULL,        SPTAG(0),    1,           -1 },
	{ "persepolis",             NULL,                    NULL,        0,           1,           -1 },
	{ "GoldenDict",             NULL,                    NULL,        0,           1,           -1 },
	{ "firefox",                "Places",                NULL,        0,           1,           -1 },
	{ "firefox",                "Browser",               NULL,        0,           1,           -1 },
	{ "firefox",                "Toolkit",               NULL,        0,           1,           -1 },
	{ "Nightly",                "Places",                NULL,        0,           1,           -1 },
	{ "Nightly",                "Browser",               NULL,        0,           1,           -1 },
	{ "Nightly",                "Toolkit",               NULL,        0,           1,           -1 },
	{ "torbrowser-launcher",    "torbrowser-launcher",   NULL,        0,           1,           -1 },
	{ "Tor Browser",            "Places",                NULL,        0,           1,           -1 },
	{ "Tor Browser",            "Browser",               NULL,        0,           1,           -1 },
	{ "Tor Browser",            "Toolkit",               NULL,        0,           1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	/* { "><>",      NULL }, */    /* no layout function means floating behavior */
	{ "|M|",      centeredmaster },
	{ "[M]",      monocle },
	/* { ">M>",      centeredfloatingmaster }, */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "alacritty", NULL };


static Key keys[] = {
	/* modifier                     key                        function        argument */
	{ MODKEY,                       XK_d,                      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return,                 spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,                      togglebar,      {0} },
	{ MODKEY,                       XK_j,                      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,                      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,                      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,                      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,                      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,                      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Tab,                    view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,                      killclient,     {0} },
	/* { MODKEY,                       XK_t,                      setlayout,      {.v = &layouts[0]} }, */
	/* { MODKEY,                       XK_f,                      setlayout,      {.v = &layouts[1]} }, */
	/* { MODKEY,                       XK_m,                      setlayout,      {.v = &layouts[2]} }, */
	{ MODKEY,                       XK_w,                      cyclelayout,    {.i = -1 } },
	/* { MODKEY,                       XK_period,                 cyclelayout,    {.i = +1 } }, */
	{ MODKEY,                       XK_u,                      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_o,                      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,                  zoom,           {0} },
	{ MODKEY|ShiftMask,             XK_space,                  togglefloating, {0} },
	{ MODKEY,                       XK_f,                      togglefullscr,  {0} },
	{ MODKEY,                       XK_s,                      togglesticky,   {0} },
	{ MODKEY,                       XK_0,                      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,                  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,                 focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,                  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,                 tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_BackSpace,              togglescratch,  {.ui = 0 } },
	TAGKEYS(                        XK_1,                                      0)
	TAGKEYS(                        XK_2,                                      1)
	TAGKEYS(                        XK_3,                                      2)
	TAGKEYS(                        XK_4,                                      3)
	TAGKEYS(                        XK_5,                                      4)
	TAGKEYS(                        XK_6,                                      5)
	TAGKEYS(                        XK_7,                                      6)
	TAGKEYS(                        XK_8,                                      7)
	TAGKEYS(                        XK_9,                                      8)
	{ MODKEY|ShiftMask,             XK_e,                      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_r,                      quit,           {1} },

	{ 0,                            XF86XK_AudioRaiseVolume,   spawn,          SHCMD("pactl set-sink-volume @DEFAULT_SINK@ +2%") },
	{ 0,                            XF86XK_AudioLowerVolume,   spawn,          SHCMD("pactl set-sink-volume @DEFAULT_SINK@ -2%") },
	{ 0,                            XF86XK_AudioMute,          spawn,          SHCMD("pactl set-sink-mute @DEFAULT_SINK@ toggle") },
	{ 0,                            XF86XK_AudioMicMute,       spawn,          SHCMD("pactl set-source-mute 1 toggle") },
	/* use `loginctl lock-session` or `xset s activate` to lock the screen instead of calling slock directly */
	/* this will make sure that xss-lock won't try to lock again on dpms */
	{ MODKEY,                       XK_semicolon,              spawn,          SHCMD("loginctl lock-session") },
	{ 0,                            XK_Print,                  spawn,          SHCMD("maim | xclip -selection clipboard -t image/png") },
	{ MODKEY,                       XK_Print,                  spawn,          SHCMD("maim -s | xclip -selection clipboard -t image/png") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTabBar,            0,              Button1,        focuswin,       {0} },
};

