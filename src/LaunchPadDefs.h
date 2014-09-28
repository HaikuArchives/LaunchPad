// $Id: LaunchPadDefs.h,v 1.2 2000/06/28 02:26:24 hiromasa Exp $
#ifndef _LAUNCH_PAD_DEFS_H
#define _LAUNCH_PAD_DEFS_H

#include <be/support/Debug.h>
#include <be/support/SupportDefs.h>
#include <be/storage/AppFileInfo.h>

extern const char LAUNCH_PAD_SIGNATURE[];
extern const char LAUNCH_PAD_SIGNATURE_MINOR[];

enum {
	kMsgCreateNewPad		= 'CtNP',
	kMsgRemoveItem			= 'RmvI',
	kMsgRemovePane			= 'RmvP',
	kMsgInsertPaneAt		= 'InsP',
	kMsgLaunchItem			= 'LncI',
	kMsgLaunchRef			= 'LncR',
	kMsgChangeDisplayMode	= 'ChPa',
	kMsgChangePaneLayout	= 'ChPl',
	kMsgDraggedFromDockPane	= 'DgFP',
	kMsgFromDockPane		= 'MgFP',
	kMsgSaveSettings		= 'SvSt',
	kMsgWindowClosed		= 'WClD',
	kMsgMouseWatchDog		= 'MsWd',
};

// Settings stored as BMessages
enum {
	kMsgSettings			= 'LPSt',
	kMsgPadSetting			= 'PdSt',
	kMsgPaneSetting			= 'PnSt',
};

// Type of the pane content
enum {
	kPaneTypeEntryRef		= 'EtRf',
	kPaneTypeEmpty			= (uint32)NULL,
};

// Pane appearance
enum PaneAppearance {
	kLargeIconWithCaption,
	kLargeIconWithoutCaption,
	kMiniIconWithCaption,
	kMiniIconWithoutCaption,
	kCaptionOnly,
	kUserDefined
};

// Global func.
void GetAppVersion( version_info* version );

#ifdef DEBUG
#  if !DEBUG
#    undef	DEBUG
#  endif
#endif

#endif // _LAUNCH_PAD_DEFS_H
