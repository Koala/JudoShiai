/* -*- mode: C; c-basic-offset: 4;  -*- */

/*
 * Copyright (C) 2006-2011 by Hannu Jokinen
 * Full copyright text is included in the software package.
 */ 

#if defined(__WIN32__) || defined(WIN32)

#define  __USE_W32_SOCKETS
//#define Win32_Winsock

#include <windows.h>
#include <stdio.h>
#include <initguid.h>
#include <winsock2.h>

#else /* UNIX */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#endif /* WIN32 */

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gthread.h>
#include <gdk/gdkkeysyms.h>
#ifdef WIN32
#include <glib/gwin32.h>
#endif

#include "judoweight.h"
#include "comm.h"

/* System-dependent definitions */
#ifndef WIN32
#define closesocket     close
#define SOCKET  int
#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1
#define SOCKADDR_BTH struct sockaddr_rc
#define AF_BTH PF_BLUETOOTH
#define BTHPROTO_RFCOMM BTPROTO_RFCOMM
#endif

void send_packet(struct message *msg);

time_t traffic_last_rec_time;

void copy_packet(struct message *msg)
{
    /* dummy function to make linker happy (defined in comm.c) */
}

gint timeout_callback(gpointer data)
{
    struct message msg;
    extern gint my_address;

    msg.type = MSG_DUMMY;
    msg.sender = my_address;
    msg.u.dummy.application_type = application_type();
    send_packet(&msg);

    return TRUE;
}

gboolean msg_accepted(struct message *m)
{
    switch (m->type) {
    case MSG_EDIT_COMPETITOR:
    case MSG_SCALE:
        return TRUE;
    }
    return FALSE;
}

void msg_received(struct message *input_msg)
{
    gint tatami;
    gint position;
    struct name_data *j;
    gchar  buf[16];

    if (input_msg->sender == my_address)
        return;

    traffic_last_rec_time = time(NULL);
#if 0
    g_print("msg type = %d from %d\n", 
            input_msg->type, input_msg->sender);
#endif
    switch (input_msg->type) {
    case MSG_EDIT_COMPETITOR:
	set_display(&input_msg->u.edit_competitor);
        break;

    case MSG_SCALE:
        g_snprintf(buf, sizeof(buf), "%d.%02d", input_msg->u.scale.weight/1000, (input_msg->u.scale.weight%1000)/10);
        if (weight_entry)
            gtk_button_set_label(GTK_BUTTON(weight_entry), buf);
        break;
    }
}


void send_packet(struct message *msg)
{
    msg_to_queue(msg);
}

gboolean keep_connection(void)
{
    return TRUE;
}

gint get_port(void)
{
    return SHIAI_PORT;
}