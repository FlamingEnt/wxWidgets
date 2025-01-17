/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockpm.c
 * Purpose: GSocket: PM part
 * CVSID:   $Id: gsockpm.c,v 1.1 2001/05/20 13:25:28 SN Exp $
 * ------------------------------------------------------------------------- */

#include "wx/setup.h"

#if wxUSE_SOCKETS

#include <stdlib.h>
#include "wx/unix/gsockunx.h"
#include "wx/gsocket.h"

extern int wxAppAddSocketHandler(int handle, int mask,
				 void (*callback)(void*), void * gsock);

extern void wxAppRemoveSocketHandler(int handle);

#define wxSockReadMask  0x01
#define wxSockWriteMask 0x02

static void _GSocket_PM_Input(void *data)
{
    GSocket *socket = (GSocket *) data;
    _GSocket_Detected_Read(socket);
}

static void _GSocket_PM_Output(void *data)
{
    GSocket *socket = (GSocket *) data;
    _GSocket_Detected_Write(socket);
}

int _GSocket_GUI_Init(GSocket *socket)
{
    int *m_id;
    socket->m_gui_dependent = (char *)malloc(sizeof(int)*2);
    m_id = (int *)(socket->m_gui_dependent);

    m_id[0] = -1;
    m_id[1] = -1;
    return TRUE;
}

void _GSocket_GUI_Destroy(GSocket *socket)
{
    free(socket->m_gui_dependent);
}

void _GSocket_Install_Callback(GSocket *socket, GSocketEvent event)
{
    int *m_id = (int *)(socket->m_gui_dependent);
    int c;

    if (socket->m_fd == -1)
        return;

    switch (event)
    {
        case GSOCK_LOST:       /* fall-through */
        case GSOCK_INPUT:      c = 0; break;
        case GSOCK_OUTPUT:     c = 1; break;
        case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
        default: return;
    }

    if (m_id[c] != -1)
        wxAppRemoveSocketHandler(m_id[c]);

    if (c == 0)
    {
	m_id[0] = wxAppAddSocketHandler(socket->m_fd, wxSockReadMask,
					_GSocket_PM_Input, (void *)socket);
    }
    else
    {
	m_id[1] = wxAppAddSocketHandler(socket->m_fd, wxSockWriteMask,
					_GSocket_PM_Output, (void *)socket);
    }
}

void _GSocket_Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
    int *m_id = (int *)(socket->m_gui_dependent);
    int c;
    switch (event)
    {
	case GSOCK_LOST:       /* fall-through */
	case GSOCK_INPUT:      c = 0; break;
	case GSOCK_OUTPUT:     c = 1; break;
	case GSOCK_CONNECTION: c = ((socket->m_server) ? 0 : 1); break;
	default: return;
    }
    if (m_id[c] != -1)
        wxAppRemoveSocketHandler(m_id[c]);

    m_id[c] = -1;
}

void _GSocket_Enable_Events(GSocket *socket)
{
    _GSocket_Install_Callback(socket, GSOCK_INPUT);
    _GSocket_Install_Callback(socket, GSOCK_OUTPUT);
}

void _GSocket_Disable_Events(GSocket *socket)
{
    _GSocket_Uninstall_Callback(socket, GSOCK_INPUT);
    _GSocket_Uninstall_Callback(socket, GSOCK_OUTPUT);
}

#else /* !wxUSE_SOCKETS */

/* some compilers don't like having empty source files */
static int wxDummyGsockVar = 0;

#endif /* wxUSE_SOCKETS/!wxUSE_SOCKETS */
