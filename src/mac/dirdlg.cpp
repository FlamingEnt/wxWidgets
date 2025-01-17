/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.cpp
// Purpose:     wxDirDialog
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: dirdlg.cpp,v 1.11 2002/05/04 12:38:51 VZ Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dirdlg.h"
#endif

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/dirdlg.h"

#include "wx/cmndata.h"

#include "wx/mac/private.h"

#ifdef __DARWIN__
  #include <Carbon/Carbon.h>
#else
  #include <Navigation.h>
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxDirDialog, wxDialog)
#endif

wxDirDialog::wxDirDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& WXUNUSED(pos),
                         const wxSize& WXUNUSED(size),
                         const wxString& WXUNUSED(name))
{
	wxASSERT_MSG( NavServicesAvailable() , "Navigation Services are not running" ) ;
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
	m_path = defaultPath;
}

int wxDirDialog::ShowModal()
{
	NavDialogOptions		mNavOptions;
	NavObjectFilterUPP		mNavFilterUPP = NULL;
	NavPreviewUPP			mNavPreviewUPP = NULL ;
	NavReplyRecord			mNavReply;
	AEDesc*					mDefaultLocation = NULL ;
	bool					mSelectDefault = false ;
	
	::NavGetDefaultDialogOptions(&mNavOptions);

	mNavFilterUPP	= nil;
	mNavPreviewUPP	= nil;
	mSelectDefault	= false;
	mNavReply.validRecord				= false;
	mNavReply.replacing					= false;
	mNavReply.isStationery				= false;
	mNavReply.translationNeeded			= false;
	mNavReply.selection.descriptorType = typeNull;
	mNavReply.selection.dataHandle		= nil;
	mNavReply.keyScript					= smSystemScript;
	mNavReply.fileTranslation			= nil;
	
	// Set default location, the location
	//   that's displayed when the dialog
	//   first appears
	
	if ( mDefaultLocation ) {
		
		if (mSelectDefault) {
			mNavOptions.dialogOptionFlags |= kNavSelectDefaultLocation;
		} else {
			mNavOptions.dialogOptionFlags &= ~kNavSelectDefaultLocation;
		}
	}
	
	OSErr err = ::NavChooseFolder(
						mDefaultLocation,
						&mNavReply,
						&mNavOptions,
						NULL,
						mNavFilterUPP,
						0L);							// User Data
	
	if ( (err != noErr) && (err != userCanceledErr) ) {
		m_path = "" ;
		return wxID_CANCEL ;
	}

	if (mNavReply.validRecord) {		// User chose a folder
	
		FSSpec	folderInfo;
		FSSpec  outFileSpec ;
		AEDesc specDesc ;
		
		OSErr err = ::AECoerceDesc( &mNavReply.selection , typeFSS, &specDesc);
		if ( err != noErr ) {
			m_path = "" ;
			return wxID_CANCEL ;
		}			
		folderInfo = **(FSSpec**) specDesc.dataHandle;
		if (specDesc.dataHandle != nil) {
			::AEDisposeDesc(&specDesc);
		}

//			mNavReply.GetFileSpec(folderInfo);
		
			// The FSSpec from NavChooseFolder is NOT the file spec
			// for the folder. The parID field is actually the DirID
			// of the folder itself, not the folder's parent, and
			// the name field is empty. We must call PBGetCatInfo
			// to get the parent DirID and folder name
		
		Str255		name;
		CInfoPBRec	thePB;			// Directory Info Parameter Block
		thePB.dirInfo.ioCompletion	= nil;
		thePB.dirInfo.ioVRefNum		= folderInfo.vRefNum;	// Volume is right
		thePB.dirInfo.ioDrDirID		= folderInfo.parID;		// Folder's DirID
		thePB.dirInfo.ioNamePtr		= name;
		thePB.dirInfo.ioFDirIndex	= -1;	// Lookup using Volume and DirID
		
		err = ::PBGetCatInfoSync(&thePB);
		if ( err != noErr ) {
			m_path = "" ;
			return wxID_CANCEL ;
		}			
											// Create cannonical FSSpec
		::FSMakeFSSpec(thePB.dirInfo.ioVRefNum, thePB.dirInfo.ioDrParID,
					   name, &outFileSpec);
						
		// outFolderDirID = thePB.dirInfo.ioDrDirID;
		m_path = wxMacFSSpec2MacFilename( &outFileSpec ) ;
		return wxID_OK ;
	}
	return wxID_CANCEL;
}

