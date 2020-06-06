#include "../../../MexTK/mex.h"

#define EVENT_DATASIZE 128
#define TM_DATA -(50 * 4) - 4

// Structure Definitions
typedef struct EventMenus EventMenus;
typedef struct MenuInfo
{
    char *optionName;
    char **optionValues;
    int optionValuesNum;
} MenuInfo;
typedef struct EventMatchData
{
    unsigned int timer : 2;
    unsigned int matchType : 3;
    unsigned int playMusic : 1;
    unsigned int hideGo : 1;
    unsigned int hideReady : 1;
    unsigned int isCreateHUD : 1;
    unsigned int isDisablePause : 1;
    unsigned int timerRunOnPause : 1;    // 0x01
    unsigned int isHidePauseHUD : 1;     // 0x02
    unsigned int isShowLRAStart : 1;     // 0x04
    unsigned int isCheckForLRAStart : 1; // 0x08
    unsigned int isShowZRetry : 1;       // 0x10
    unsigned int isCheckForZRetry : 1;   // 0x20
    unsigned int isShowAnalogStick : 1;  // 0x40
    unsigned int isShowScore : 1;        // 0x80
    unsigned int isRunStockLogic : 1;    // 0x20
    unsigned int isDisableHit : 1;       // 0x20
    unsigned int useKOCounter : 1;
    s8 playerKind;                    // -1 = use selected fighter
    s8 cpuKind;                       // -1 = no CPU
    s16 stage;                        // -1 = use selected stage
    unsigned int timerSeconds : 32;   // 0xFFFFFFFF
    unsigned int timerSubSeconds : 8; // 0xFF
    void *onCheckPause;
    void *onMatchEnd;
} EventMatchData;
typedef struct EventInfo
{
    char *eventName;
    char *eventDescription;
    char *eventControls;
    char *eventTutorial;
    u8 isChooseCPU;
    u8 isSelectStage;
    u8 scoreType;
    u8 callbackPriority;
    void (*eventOnFrame)(GOBJ *gobj);
    void (*eventOnInit)(GOBJ *gobj);
    EventMatchData *matchData;
    MenuInfo *menuInfo;
    int menuOptionNum;
    int defaultOSD;
} EventInfo;
typedef struct EventPage
{
    char *name;
    int eventNum;
    EventInfo **events;
} EventPage;
typedef struct TMData
{
    JOBJDesc *messageJoint;
} TMData;
typedef struct EventMenu
{
    EventInfo *eventInfo;
    Text *menu;
    Text *controls;
    Text *description;
    s32 cursor;
    u8 *options[50];
} MenuData;

typedef struct EventOption
{
    u8 option_kind;       // the type of option this is; string, integers, etc
    u8 option_val;        // value of this option
    EventMenus *menu;     // pointer to the menu that pressing A opens
    char *option_name;    // pointer to a string
    char **option_values; // pointer to an array of strings
} EventOption;

typedef struct EventMenus
{
    u8 option_num;        // number of options this menu contains
    u8 menu_width;        // how wide to make the menu
    u8 is_using;          // bool used to know if this menu is focused
    u8 cursor;            // index of the option currently selected
    EventOption *options; // pointer to all of this menu's options
    EventMenus *prev;     // pointer to previous menu, used at runtime
    JOBJ *joint;          // pointer to this menus joint background
    Text *text;           // pointer to this options text
} EventMenus;

// Function prototypes
EventInfo *GetEvent(int page, int event);
void EventInit(int page, int eventID, MatchData *matchData);
void EventLoad();
void EventMenu_Init(EventInfo *eventInfo);
void EventMenu_Think(GOBJ *eventMenu);
void EventMenu_Draw(GOBJ *eventMenu);
int Text_AddSubtextManual(Text *text, char *string, int posx, int posy, int scalex, int scaley);

static EventOption EvFreeOptions_Main[];
static EventOption EvFreeOptions_General[];
static EventMenus EvFreeMenu_General;

// definitions
#define OPTKIND_MENU 0
#define OPTKIND_STRING 1
#define OPTKIND_INT 2
#define OPTKIND_FLOAT 3