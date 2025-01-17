/* -------------------------------------------------------------------------
 * Project: Mac Notifier Support
 * Name:    macnotfy.h
 * Author:  Stefan CSomor
 * Purpose: Mac Notifier include file
 * CVSID:   $Id: macnotfy.h,v 1.1 2000/08/10 04:43:04 SC Exp $
 * -------------------------------------------------------------------------
 */
 
#ifndef MAC_NOTIFYERS
#define MAC_NOTIFYERS
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*wxMacNotificationProcPtr)(unsigned long event , void* data ) ;

typedef void *wxMacNotifierTableRef ;
void wxMacCreateNotifierTable() ;
void wxMacDestroyNotifierTable() ;
wxMacNotifierTableRef wxMacGetNotifierTable() ;
void wxMacAddEvent( wxMacNotifierTableRef table , wxMacNotificationProcPtr handler , unsigned long event , void* data , short wakeUp ) ;
void wxMacWakeUp() ;
void wxMacProcessNotifierEvents() ;
void wxMacProcessNotifierAndPendingEvents() ;
void wxMacRemoveAllNotifiersForData( wxMacNotifierTableRef table , void* data ) ;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* MAC_NOTIFYERS */