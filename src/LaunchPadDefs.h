//
// LaunchPadDefs.h
//
#ifndef _LAUNCH_PAD_DEFS_H
#define _LAUNCH_PAD_DEFS_H

#include <Debug.h>
#include <SupportDefs.h>
#include <AppFileInfo.h>

const char LAUNCH_PAD_SIGNATURE[] = "application/x-vnd.HK-LaunchPad";
const char LAUNCH_PAD_SIGNATURE_MINOR[] = "x-vnd.HK-LaunchPad";

const uint32	kMsgCreateNewPad		= 'CtNP';
const uint32	kMsgRemoveItem			= 'RmvI';
const uint32	kMsgRemovePane			= 'RmvP';
const uint32	kMsgInsertPaneAt		= 'InsP';
const uint32	kMsgLaunchItem			= 'LncI';
const uint32	kMsgLaunchRef			= 'LncR';
const uint32	kMsgChangeDisplayMode	= 'ChPa';
const uint32	kMsgChangePaneLayout	= 'ChPl';
const uint32	kMsgDraggedFromDockPane	= 'DgFP';
const uint32	kMsgFromDockPane		= 'MgFP';
const uint32	kMsgSaveSettings		= 'SvSt';
const uint32	kMsgWindowClosed		= 'WClD';

// Settings stored as BMessages
const uint32	kMsgSettings			= 'LPSt';
const uint32	kMsgPadSetting			= 'PdSt';
const uint32	kMsgPaneSetting			= 'PnSt';

// Type of the pane content
const uint32	kPaneTypeEntryRef		= 'EtRf';
const uint32	kPaneTypeEmpty			= (uint32)NULL;

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
