/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

#include "mosaic.h"

#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/ScrolledW.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>

#include <Xm/Protocols.h>

#include "libhtmlw/HTML.h"

extern Widget tolevel;
extern mo_window *current_win;

typedef enum
{
  mo_plaintext = 0, mo_formatted_text, mo_html, mo_latex, mo_postscript,
  mo_mif
} mo_format_token;

static XmxOptionMenuStruct format_opts[] =
{
  { "Plain Text",     mo_plaintext,      XmxSet },
  { "Formatted Text", mo_formatted_text, XmxNotSet },
  { "PostScript",     mo_postscript,     XmxNotSet },
  { "HTML",           mo_html,           XmxNotSet },
  { NULL },
};

/* ------------------------------------------------------------------------ */
/* ----------------------------- SAVE WINDOW ------------------------------ */
/* ------------------------------------------------------------------------ */

/* ------------------------- mo_post_save_window -------------------------- */

static XmxCallback (save_win_cb)
{
  char *fname = (char *)malloc (sizeof (char) * 128);
  FILE *fp;
  mo_window *win = mo_fetch_window_by_id 
    (XmxExtractUniqid ((int)client_data));

  mo_busy ();

  XtUnmanageChild (win->save_win);
  
  XmStringGetLtoR (((XmFileSelectionBoxCallbackStruct *)call_data)->value,
                   XmSTRING_DEFAULT_CHARSET,
                   &fname);

  fp = fopen (fname, "w");
  if (!fp)
    {
      XmxMakeErrorDialog (win->save_win, 
                          "Unable to save document.", "Save Error");
      XtManageChild (Xmx_w);
    }
  else
    {
      if (win->save_format == mo_plaintext)
        {
          char *text = HTMLGetText (win->scrolled_win, 0);
          if (text)
            {
              fputs (text, fp);
              free (text);
            }
        }
      else if (win->save_format == mo_formatted_text)
        {
          char *text = HTMLGetText (win->scrolled_win, 1);
          if (text)
            {
              fputs (text, fp);
              free (text);
            }
        }
      else if (win->save_format == mo_postscript)
        {
          char *text = HTMLGetText (win->scrolled_win, 2 + win->font_family);
          if (text)
            {
              fputs (text, fp);
              free (text);
            }
        }
      else if (win->current_node && win->current_node->text)
        {
          /* HTML source */
          fputs (win->current_node->text, fp);
        }
      fclose (fp);
    }

  mo_not_busy ();

  free (fname);
  
  return;
}

static XmxCallback (format_optmenu_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  
  win->save_format = XmxExtractToken ((int)client_data);

  return;
}

mo_status mo_post_save_window (mo_window *win)
{
  XmxSetUniqid (win->id);
  if (!win->save_win)
    {
      Widget frame, workarea, format_label;

      win->save_win = XmxMakeFileSBDialog
        (win->base, "NCSA Mosaic: Save Document", "Name for saved document:",
         save_win_cb, 0);

      /* This makes a frame as a work area for the dialog box. */
      XmxSetArg (XmNmarginWidth, 5);
      XmxSetArg (XmNmarginHeight, 5);
      frame = XmxMakeFrame (win->save_win, XmxShadowEtchedIn);
      workarea = XmxMakeForm (frame);

      format_label = XmxMakeLabel (workarea, "Format for saved document:");
      /* XmxSetArg (XmNwidth, 210); */
      win->format_optmenu = XmxRMakeOptionMenu (workarea, "",
                                                format_optmenu_cb, 
                                                format_opts);
      XmxSetConstraints
        (format_label, XmATTACH_FORM, XmATTACH_FORM, XmATTACH_FORM,
         XmATTACH_NONE, NULL, NULL, NULL, NULL);
      XmxSetConstraints
        (win->format_optmenu->base, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_NONE,
         XmATTACH_FORM, NULL, NULL, NULL, NULL);
    }
  else
    {
      XmFileSelectionDoSearch (win->save_win, NULL);
    }
  
  XmxManageRemanage (win->save_win);
  return mo_succeed;
}

/* ------------------------------------------------------------------------ */
/* -------------------------- SAVEBINARY WINDOW --------------------------- */
/* ------------------------------------------------------------------------ */

/* This is used by libwww/HTFormat.c to present a user interface
   for retrieving files in binary transfer mode.  Obviously a redesign
   of the interface between the GUI and the commo library really needs
   to happen -- hopefully libwww2 will make this easy.  HA!!! */

/* Thanks to Martha Weinberg (lyonsm@hpwcsvp.mayfield.hp.com) for
   idea and code starting point. */
static char *temp_binary_fnam;

static XmxCallback (savebinary_cancel_cb)
{
  char *cmd;
  mo_window *win = mo_fetch_window_by_id
    (XmxExtractUniqid ((int)client_data));

  cmd = (char *)malloc ((strlen (temp_binary_fnam) + 32));
  sprintf (cmd, "/bin/rm -f %s &", temp_binary_fnam);
  if ((system (cmd)) != 0)
    {
      XmxMakeErrorDialog (win->base,
                          "Unable to remove local copy of binary file.", 
                          "Remove Error");
      XtManageChild (Xmx_w);
    }

  /* This was dup'd down below... */
  free (temp_binary_fnam);

  free (cmd);
  
  return;
}

static XmxCallback (savebinary_win_cb)
{
  char *fname = (char *)malloc (sizeof (char) * 128);
  char *cmd;
  mo_window *win = mo_fetch_window_by_id
    (XmxExtractUniqid ((int)client_data));

  mo_busy ();

  XtUnmanageChild (win->savebinary_win);

  XmStringGetLtoR (((XmFileSelectionBoxCallbackStruct *)call_data)->value,
                   XmSTRING_DEFAULT_CHARSET,
                   &fname);

  cmd = (char *)malloc ((strlen (temp_binary_fnam) + strlen (fname) + 16));
  sprintf (cmd, "/bin/mv %s %s", temp_binary_fnam, fname);
  if ((system (cmd)) != 0)
    {
      XmxMakeErrorDialog (win->base,
                          "Unable to save binary file.", "Save Error");
      XtManageChild (Xmx_w);
    }
  
  /* This was dup'd down below... */
  free (temp_binary_fnam);

  free (cmd);
  free (fname);
  mo_not_busy ();
  
  return;
}

static mo_status mo_post_savebinary_window (mo_window *win)
{
  XmxSetUniqid (win->id);
  if (!win->savebinary_win)
    {
      XmxSetArg (XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
      win->savebinary_win = XmxMakeFileSBDialog
        (win->base, "NCSA Mosaic: Save Binary File To Local Disk", 
         "Name for binary file on local disk:",
         savebinary_win_cb, 0);
      XmxAddCallback (win->savebinary_win, XmNcancelCallback, 
                      savebinary_cancel_cb, 0);
    }
  else
    {
      XmFileSelectionDoSearch (win->savebinary_win, NULL);
    }

  XmxManageRemanage (win->savebinary_win);
  return mo_succeed;
}

void rename_binary_file (char *fnam)
{
  mo_window *win = current_win;
  temp_binary_fnam = strdup (fnam);
  mo_post_savebinary_window (win);
}

/* ---------------------- mo_post_open_local_window ----------------------- */

static XmxCallback (open_local_win_cb)
{
  char *fname = NULL;
  char *url;
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));

  XtUnmanageChild (win->open_local_win);
  
  XmStringGetLtoR (((XmFileSelectionBoxCallbackStruct *)call_data)->value,
                   XmSTRING_DEFAULT_CHARSET,
                   &fname);

  url = mo_url_canonicalize_local (fname);
  if (url[strlen(url)-1] == '/')
    url[strlen(url)-1] = '\0';
  mo_load_window_text (win, url, NULL);

  free (fname);

  return;
}

mo_status mo_post_open_local_window (mo_window *win)
{
  XmxSetUniqid (win->id);
  if (!win->open_local_win)
    {
      Widget frame, workarea, format_label;

      win->open_local_win = XmxMakeFileSBDialog
        (win->base, "NCSA Mosaic: Open Local Document", 
         "Name of local document to open:",
         open_local_win_cb, 0);
    }
  else
    {
      XmFileSelectionDoSearch (win->open_local_win, NULL);
    }
  
  XmxManageRemanage (win->open_local_win);
  return mo_succeed;
}

/* ----------------------- mo_post_open_window ------------------------ */

static XmxCallback (open_win_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  char *url;

  switch (XmxExtractToken ((int)client_data))
    {
    case 0:
      XtUnmanageChild (win->open_win);
      url = XmxTextGetString (win->open_text);
      mo_convert_newlines_to_spaces (url);
      if (!strstr (url, ":"))
        {
          char *freeme = url;
          url = mo_url_canonicalize_local (url);
          free (freeme);
        }
      mo_load_window_text (win, url, NULL);
      /* The following free breaks things under the following conditions:

         Start Mosaic with home page something that can't be fetched.
         Interrupt, hit 'open', enter a URL, fetch document.
         Follow relative link. */
      /* free (url); */
      break;
    case 1:
      XtUnmanageChild (win->open_win);
      /* Do nothing. */
      break;
    case 2:
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("docview-menubar-file.html#open"),
         NULL, NULL);
      break;
    case 3:
      XmxTextSetString (win->open_text, "");
      break;
    }

  return;
}

mo_status mo_post_open_window (mo_window *win)
{
  if (!win->open_win)
    {
      Widget dialog_frame;
      Widget dialog_sep, buttons_form;
      Widget open_form, label;
      
      /* Create it for the first time. */
      XmxSetUniqid (win->id);
      win->open_win = XmxMakeFormDialog 
        (win->base, "NCSA Mosaic: Open Document");
      dialog_frame = XmxMakeFrame (win->open_win, XmxShadowOut);

      /* Constraints for base. */
      XmxSetConstraints 
        (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
      
      /* Main form. */
      open_form = XmxMakeForm (dialog_frame);
      
      label = XmxMakeLabel (open_form, "URL To Open: ");
      XmxSetArg (XmNwidth, 310);
      win->open_text = XmxMakeTextField (open_form);
      XmxAddCallbackToText (win->open_text, open_win_cb, 0);
      
      dialog_sep = XmxMakeHorizontalSeparator (open_form);
      
      buttons_form = XmxMakeFormAndFourButtons
        (open_form, open_win_cb, "Open", "Clear", "Dismiss", "Help...", 
         0, 3, 1, 2);

      /* Constraints for open_form. */
      XmxSetOffsets (label, 14, 0, 10, 0);
      XmxSetConstraints
        (label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
         NULL, NULL, NULL, NULL);
      XmxSetOffsets (win->open_text, 10, 0, 5, 10);
      XmxSetConstraints
        (win->open_text, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, NULL, NULL, label, NULL);
      XmxSetArg (XmNtopOffset, 10);
      XmxSetConstraints 
        (dialog_sep, XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, 
         XmATTACH_FORM,
         win->open_text, buttons_form, NULL, NULL);
      XmxSetConstraints 
        (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
    }
  
  XmxManageRemanage (win->open_win);
  
  return mo_succeed;
}

#ifdef HAVE_DTM

/* ---------------------- mo_send_document_over_dtm ----------------------- */

mo_status mo_send_document_over_dtm (mo_window *win)
{
  /* Always pass across pretty-formatted text for the moment. */
  char *text;

  if (!win->current_node)
    return mo_fail;

  text = HTMLGetText (win->scrolled_win, 1);
  
  mo_dtm_send_text (win, win->current_node->title, text);
  free (text);

  return mo_succeed;
}

/* ----------------------- mo_post_dtmout_window ------------------------ */

static XmxCallback (dtmout_win_cb)
{
  mo_window *win = mo_fetch_window_by_id 
    (XmxExtractUniqid ((int)client_data));
  char *port;

  switch (XmxExtractToken ((int)client_data))
    {
    case 0:
      XtUnmanageChild (win->dtmout_win);
      mo_busy ();
      port = XmxTextGetString (win->dtmout_text);
      if (port && *port)
        {
          mo_dtm_out (port);

          /* Now we can't open another outport, but we can send documents... */
          {
            mo_window *w = NULL;
            while (w = mo_next_window (w))
              {
                mo_set_dtm_menubar_functions (w);
              }
          }
        }
      mo_not_busy ();
      break;
    case 1:
      XtUnmanageChild (win->dtmout_win);
      /* Do nothing. */
      break;
    case 2:
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("docview-menubar-file.html#dtmbroadcast"),
         NULL, NULL);
      break;
    }

  return;
}

mo_status mo_post_dtmout_window (mo_window *win)
{
  if (!win->dtmout_win)
    {
      Widget dialog_frame;
      Widget dialog_sep, buttons_form;
      Widget dtmout_form, label;
      
      /* Create it for the first time. */
      XmxSetUniqid (win->id);
      win->dtmout_win = XmxMakeFormDialog 
        (win->base, "NCSA Mosaic: Open DTM Outport");
      dialog_frame = XmxMakeFrame (win->dtmout_win, XmxShadowOut);

      /* Constraints for base. */
      XmxSetConstraints 
        (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
      
      /* Main form. */
      dtmout_form = XmxMakeForm (dialog_frame);
      
      label = XmxMakeLabel (dtmout_form, "DTM Output Port: ");
      XmxSetArg (XmNwidth, 240);
      win->dtmout_text = XmxMakeTextField (dtmout_form);
      XmxAddCallbackToText (win->dtmout_text, dtmout_win_cb, 0);
      
      dialog_sep = XmxMakeHorizontalSeparator (dtmout_form);
      
      buttons_form = XmxMakeFormAndThreeButtons
        (dtmout_form, dtmout_win_cb, "Open", "Dismiss", "Help...", 
         0, 1, 2);

      /* Constraints for dtmout_form. */
      XmxSetOffsets (label, 14, 0, 10, 0);
      XmxSetConstraints
        (label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
         NULL, NULL, NULL, NULL);
      XmxSetOffsets (win->dtmout_text, 10, 0, 5, 10);
      XmxSetConstraints
        (win->dtmout_text, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, NULL, NULL, label, NULL);
      XmxSetArg (XmNtopOffset, 10);
      XmxSetConstraints 
        (dialog_sep, XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, 
         XmATTACH_FORM,
         win->dtmout_text, buttons_form, NULL, NULL);
      XmxSetConstraints 
        (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
    }
  
  XmxManageRemanage (win->dtmout_win);
  
  return mo_succeed;
}
#endif /* HAVE_DTM */

/* ------------------------- mo_post_mail_window -------------------------- */

static XmxCallback (mail_win_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  char *to, *subj, *text = 0, *content_type;
  int free_text;

  switch (XmxExtractToken ((int)client_data))
    {
    case 0:
      XtUnmanageChild (win->mail_win);

      mo_busy ();

      to = XmxTextGetString (win->mail_to_text);
      if (!to)
        return;
      if (to[0] == '\0')
        return;

      subj = XmxTextGetString (win->mail_subj_text);
      if (!subj)
        subj = strdup ("\0");

      if (win->mail_format == mo_plaintext)
        {
          text = HTMLGetText (win->scrolled_win, 0);
          content_type = "text/plain";
          free_text = 1;
        }
      else if (win->mail_format == mo_formatted_text)
        {
          text = HTMLGetText (win->scrolled_win, 1);
          content_type = "text/plain";
          free_text = 1;
        }
      else if (win->mail_format == mo_postscript)
        {
          text = HTMLGetText (win->scrolled_win, 2 + win->font_family);
          content_type = "application/postscript";
          free_text = 1;
        }
      else if (win->current_node && win->current_node->text)
        {
          /* HTML source. */
          text = win->current_node->text;
          content_type = "text/x-html";
          free_text = 0;
        }

      if (text)
        mo_send_mail_message 
          (text, to, subj, content_type,
           win->current_node ? win->current_node->url : NULL);
      
      if (free_text && text)
        free (text);
      free (to);
      free (subj);
      
      mo_not_busy ();
      
      break;
    case 1:
      XtUnmanageChild (win->mail_win);
      /* Do nothing. */
      break;
    case 2:
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("docview-menubar-file.html#mailto"), 
         NULL, NULL);
      break;
    }

  return;
}

static XmxCallback (mail_fmtmenu_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  
  win->mail_format = XmxExtractToken ((int)client_data);

  return;
}

mo_status mo_post_mail_window (mo_window *win)
{
  if (!win->mail_win)
    {
      Widget dialog_frame;
      Widget dialog_sep, buttons_form;
      Widget mail_form, to_label, subj_label;
      Widget frame, workarea, format_label;
      
      /* Create it for the first time. */
      XmxSetUniqid (win->id);
      win->mail_win = XmxMakeFormDialog 
        (win->base, "NCSA Mosaic: Mail Document");
      dialog_frame = XmxMakeFrame (win->mail_win, XmxShadowOut);

      /* Constraints for base. */
      XmxSetConstraints 
        (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
      
      /* Main form. */
      mail_form = XmxMakeForm (dialog_frame);
      
      to_label = XmxMakeLabel (mail_form, "Mail To: ");
      XmxSetArg (XmNwidth, 335);
      win->mail_to_text = XmxMakeTextField (mail_form);
      
      subj_label = XmxMakeLabel (mail_form, "Subject: ");
      win->mail_subj_text = XmxMakeTextField (mail_form);

      {
        XmxSetArg (XmNmarginWidth, 5);
        XmxSetArg (XmNmarginHeight, 5);
        frame = XmxMakeFrame (mail_form, XmxShadowEtchedIn);
        workarea = XmxMakeForm (frame);
        
        format_label = XmxMakeLabel (workarea, "Format for mailed document:");
        win->mail_fmtmenu = XmxRMakeOptionMenu (workarea, "",
                                                mail_fmtmenu_cb, 
                                                format_opts);

        XmxSetConstraints
          (format_label, XmATTACH_FORM, XmATTACH_FORM, XmATTACH_FORM,
           XmATTACH_NONE, NULL, NULL, NULL, NULL);
        XmxSetConstraints
          (win->mail_fmtmenu->base, XmATTACH_FORM, XmATTACH_FORM, 
           XmATTACH_NONE,
           XmATTACH_FORM, NULL, NULL, NULL, NULL);
      }

      dialog_sep = XmxMakeHorizontalSeparator (mail_form);
      
      buttons_form = XmxMakeFormAndThreeButtons
        (mail_form, mail_win_cb, "Mail", "Dismiss", "Help...", 0, 1, 2);

      /* Constraints for mail_form. */
      XmxSetOffsets (to_label, 14, 0, 10, 0);
      XmxSetConstraints
        (to_label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
         NULL, NULL, NULL, NULL);
      XmxSetOffsets (win->mail_to_text, 10, 0, 5, 10);
      XmxSetConstraints
        (win->mail_to_text, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, NULL, NULL, to_label, NULL);

      XmxSetOffsets (subj_label, 14, 0, 10, 0);
      XmxSetConstraints
        (subj_label, XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_FORM, 
         XmATTACH_NONE,
         win->mail_to_text, NULL, NULL, NULL);
      XmxSetOffsets (win->mail_subj_text, 10, 0, 5, 10);
      XmxSetConstraints
        (win->mail_subj_text, XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, win->mail_to_text, NULL, subj_label, NULL);

      XmxSetOffsets (frame, 10, 0, 10, 10);
      XmxSetConstraints
        (frame, XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM,
         win->mail_subj_text, NULL, NULL, NULL);

      XmxSetArg (XmNtopOffset, 10);
      XmxSetConstraints 
        (dialog_sep, XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, 
         XmATTACH_FORM,
         frame, buttons_form, NULL, NULL);
      XmxSetConstraints 
        (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
    }
  
  XmxManageRemanage (win->mail_win);
  
  return mo_succeed;
}

/* ----------------------- mo_post_print_window ------------------------ */

static XmxCallback (print_win_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  char *lpr, *fnam, *cmd;
  FILE *fp;

  switch (XmxExtractToken ((int)client_data))
    {
    case 0:
      XtUnmanageChild (win->print_win);

      mo_busy ();

      lpr = XmxTextGetString (win->print_text);
      if (!lpr)
        return;
      if (lpr[0] == '\0')
        return;

      fnam = mo_tmpnam();

      fp = fopen (fnam, "w");
      if (!fp)
        goto oops;
      if (win->print_format == mo_plaintext)
        {
          char *text = HTMLGetText (win->scrolled_win, 0);
          if (text)
            {
              fputs (text, fp);
              free (text);
            }
        }
      else if (win->print_format == mo_formatted_text)
        {
          char *text = HTMLGetText (win->scrolled_win, 1);
          if (text)
            {
              fputs (text, fp);
              free (text);
            }
        }
      else if (win->print_format == mo_postscript)
        {
          char *text = HTMLGetText (win->scrolled_win, 2 + win->font_family);
          if (text)
            {
              fputs (text, fp);
              free (text);
            }
        }
      else if (win->current_node && win->current_node->text)
        {
          /* HTML source */
          fputs (win->current_node->text, fp);
        }
      fclose (fp);

      cmd = (char *)malloc ((strlen (lpr) + strlen (fnam) + 24));
      sprintf (cmd, "%s %s", lpr, fnam);
      if ((system (cmd)) != 0)
        {
          XmxMakeErrorDialog 
            (win->base, 
             "Unable to print document;\nplease check the value of X resource\nprintCommand.", 
             "Print Error");
          XtManageChild (Xmx_w);
        }
      free (cmd);
      
    oops:
      free (lpr);
      
      cmd = (char *)malloc ((strlen (fnam) + 32) * sizeof (char));
      sprintf (cmd, "/bin/rm -f %s &", fnam);
      system (cmd);

      free (fnam);
      free (cmd);

      mo_not_busy ();
      
      break;
    case 1:
      XtUnmanageChild (win->print_win);
      /* Do nothing. */
      break;
    case 2:
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("docview-menubar-file.html#print"),
         NULL, NULL);
      break;
    }

  return;
}

static XmxCallback (print_fmtmenu_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  
  win->print_format = XmxExtractToken ((int)client_data);

  return;
}

mo_status mo_post_print_window (mo_window *win)
{
  if (!win->print_win)
    {
      Widget dialog_frame;
      Widget dialog_sep, buttons_form;
      Widget print_form, print_label;
      Widget frame, workarea, format_label;
      
      /* Create it for the first time. */
      XmxSetUniqid (win->id);
      win->print_win = XmxMakeFormDialog 
        (win->base, "NCSA Mosaic: Print Document");
      dialog_frame = XmxMakeFrame (win->print_win, XmxShadowOut);

      /* Constraints for base. */
      XmxSetConstraints 
        (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
      
      /* Main form. */
      print_form = XmxMakeForm (dialog_frame);
      
      print_label = XmxMakeLabel (print_form, "Print Command: ");
      XmxSetArg (XmNwidth, 270);
      win->print_text = XmxMakeTextField (print_form);
      XmxTextSetString (win->print_text, Rdata.print_command);
      
      {
        XmxSetArg (XmNmarginWidth, 5);
        XmxSetArg (XmNmarginHeight, 5);
        frame = XmxMakeFrame (print_form, XmxShadowEtchedIn);
        workarea = XmxMakeForm (frame);
        
        format_label = XmxMakeLabel (workarea, "Format for printed document:");
        win->print_fmtmenu = XmxRMakeOptionMenu (workarea, "",
                                                print_fmtmenu_cb, 
                                                format_opts);
        XmxSetConstraints
          (format_label, XmATTACH_FORM, XmATTACH_FORM, XmATTACH_FORM,
           XmATTACH_NONE, NULL, NULL, NULL, NULL);
        XmxSetConstraints
          (win->print_fmtmenu->base, XmATTACH_FORM, XmATTACH_FORM, 
           XmATTACH_NONE,
           XmATTACH_FORM, NULL, NULL, NULL, NULL);
      }

      dialog_sep = XmxMakeHorizontalSeparator (print_form);
      
      buttons_form = XmxMakeFormAndThreeButtons
        (print_form, print_win_cb, "Print", "Dismiss", "Help...", 0, 1, 2);

      /* Constraints for print_form. */
      XmxSetOffsets (print_label, 14, 0, 10, 0);
      XmxSetConstraints
        (print_label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
         NULL, NULL, NULL, NULL);
      XmxSetOffsets (win->print_text, 10, 0, 5, 10);
      XmxSetConstraints
        (win->print_text, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, NULL, NULL, print_label, NULL);

      XmxSetOffsets (frame, 10, 0, 10, 10);
      XmxSetConstraints
        (frame, XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM,
         win->print_text, NULL, NULL, NULL);

      XmxSetArg (XmNtopOffset, 10);
      XmxSetConstraints 
        (dialog_sep, XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, 
         XmATTACH_FORM,
         frame, buttons_form, NULL, NULL);
      XmxSetConstraints 
        (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
    }
  
  XmxManageRemanage (win->print_win);
  
  return mo_succeed;
}

/* ----------------------- mo_post_source_window ------------------------ */

static XmxCallback (source_win_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  switch (XmxExtractToken ((int)client_data))
    {
    case 0:
      XtUnmanageChild (win->source_win);
      break;
    case 1:
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("docview-menubar-file.html#documentsource"),
         NULL, NULL);
      break;
    }

  return;
}

mo_status mo_post_source_window (mo_window *win)
{
  if (!win->source_win)
    {
      Widget dialog_frame;
      Widget dialog_sep, buttons_form;
      Widget source_form, label;
      
      /* Create it for the first time. */
      XmxSetUniqid (win->id);
      win->source_win = XmxMakeFormDialog 
        (win->base, "NCSA Mosaic: Document Source");
      dialog_frame = XmxMakeFrame (win->source_win, XmxShadowOut);

      /* Constraints for base. */
      XmxSetConstraints 
        (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
      
      /* Main form. */
      source_form = XmxMakeForm (dialog_frame);
      
      label = XmxMakeLabel (source_form, "Uniform Resource Locator: ");
      XmxSetArg (XmNcursorPositionVisible, False);
      XmxSetArg (XmNeditable, False);
      win->source_url_text = XmxMakeText (source_form);
      
      /* Info window: text widget, not editable. */
      XmxSetArg (XmNscrolledWindowMarginWidth, 10);
      XmxSetArg (XmNscrolledWindowMarginHeight, 10);
      XmxSetArg (XmNcursorPositionVisible, False);
      XmxSetArg (XmNeditable, False);
      XmxSetArg (XmNeditMode, XmMULTI_LINE_EDIT);
      XmxSetArg (XmNrows, 15);
      XmxSetArg (XmNcolumns, 80);
      win->source_text = XmxMakeScrolledText (source_form);
      
      dialog_sep = XmxMakeHorizontalSeparator (source_form);
      
      buttons_form = XmxMakeFormAndTwoButtonsSqueezed
        (source_form, source_win_cb, "Dismiss", "Help...", 0, 1);

      /* Constraints for source_form. */
      XmxSetOffsets (label, 13, 0, 10, 0);
      XmxSetConstraints
        (label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
         NULL, NULL, NULL, NULL);
      XmxSetOffsets (win->source_url_text, 10, 0, 5, 10);
      XmxSetConstraints
        (win->source_url_text, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, NULL, NULL, label, NULL);
      XmxSetConstraints 
        (XtParent (win->source_text), XmATTACH_WIDGET, XmATTACH_WIDGET, 
         XmATTACH_FORM, XmATTACH_FORM, win->source_url_text, dialog_sep, 
         NULL, NULL);
      XmxSetArg (XmNtopOffset, 10);
      XmxSetConstraints 
        (dialog_sep, XmATTACH_NONE, XmATTACH_WIDGET, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, buttons_form, NULL, NULL);
      XmxSetConstraints 
        (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
    }
  
  XmxManageRemanage (win->source_win);
  if (win->current_node)
    {
      XmxTextSetString (win->source_text, win->current_node->text);
      XmxTextSetString (win->source_url_text, win->current_node->url);
    }

  return mo_succeed;
}

/* ----------------------- mo_post_search_window ------------------------ */

static XmxCallback (search_win_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  switch (XmxExtractToken ((int)client_data))
    {
    case 0: /* search */
      {
        char *str = XmxTextGetString (win->search_win_text);

        if (str && *str)
          {
            int backward = XmToggleButtonGetState (win->search_backwards_toggle);
            int caseless = XmToggleButtonGetState (win->search_caseless_toggle);
            int rc;

            if (!backward)
              {
                /* Either win->search_start->id is 0, in which case the search
                   should start from the beginning, or it's non-0, in which case
                   at least one search step has already been taken.
                   If the latter, it should be incremented so as to start
                   the search after the last hit.  Right? */
                if (((ElementRef *)win->search_start)->id)
                  {
                    ((ElementRef *)win->search_start)->id =
                      ((ElementRef *)win->search_end)->id;
                    ((ElementRef *)win->search_start)->pos =
                      ((ElementRef *)win->search_end)->pos;
                  }
              }

            rc = HTMLSearchText 
              (win->scrolled_win, str, 
               (ElementRef *)win->search_start, 
               (ElementRef *)win->search_end, 
               backward, caseless);

            if (rc == -1)
              {
                /* No match was found. */
                if (((ElementRef *)win->search_start)->id)
                  XmxMakeInfoDialog 
                    (win->search_win, "Sorry, no more matches in this document.",
                     "NCSA Mosaic: Find Result");
                else
                  XmxMakeInfoDialog 
                    (win->search_win, "Sorry, no matches in this document.",
                     "NCSA Mosaic: Find Result");
                XmxManageRemanage (Xmx_w);
              }
            else
              {
                /* Now search_start and search_end are starting and ending
                   points of the match. */
		HTMLGotoId(win->scrolled_win,
			((ElementRef *)win->search_start)->id);

                /* Set the selection. */
                HTMLSetSelection (win->scrolled_win, (ElementRef *)win->search_start,
                                  (ElementRef *)win->search_end);

              } /* found a target */
          } /* if str && *str */
      } /* case */
      break;
    case 1: /* reset */
      /* Clear out the search text. */
      XmxTextSetString (win->search_win_text, "");

      /* Subsequent searches start at the beginning. */
      ((ElementRef *)win->search_start)->id = 0;

      /* Reposition document at top of screen. */
      HTMLGotoId(win->scrolled_win, 0);
      break;
    case 2: /* dismiss */
      XtUnmanageChild (win->search_win);
      break;
    case 3: /* help */
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("docview-menubar-file.html#search"),
         NULL, NULL);
      break;
    }

  return;
}

mo_status mo_post_search_window (mo_window *win)
{
  if (!win->search_win)
    {
      Widget dialog_frame;
      Widget dialog_sep, buttons_form;
      Widget search_form, label;
      
      /* Create it for the first time. */
      XmxSetUniqid (win->id);
      win->search_win = XmxMakeFormDialog 
        (win->base, "NCSA Mosaic: Find In Document");
      dialog_frame = XmxMakeFrame (win->search_win, XmxShadowOut);

      /* Constraints for base. */
      XmxSetConstraints 
        (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
      
      /* Main form. */
      search_form = XmxMakeForm (dialog_frame);
      
      label = XmxMakeLabel (search_form, "Find string in document: ");
      XmxSetArg (XmNcolumns, 25);
      win->search_win_text = XmxMakeText (search_form);
      XmxAddCallbackToText (win->search_win_text, search_win_cb, 0);

      win->search_caseless_toggle = XmxMakeToggleButton 
        (search_form, "Caseless Search", NULL, 0);
      XmxSetToggleButton (win->search_caseless_toggle, XmxSet);
      win->search_backwards_toggle = XmxMakeToggleButton 
        (search_form, "Backwards Search", NULL, 0);

      dialog_sep = XmxMakeHorizontalSeparator (search_form);
      
      buttons_form = XmxMakeFormAndFourButtons
        (search_form, search_win_cb, 
         "Find", "Reset", "Dismiss", "Help...", 0, 1, 2, 3);

      /* Constraints for search_form. */
      XmxSetOffsets (label, 13, 0, 10, 0);
      /* Label attaches top to form, bottom to nothing,
         left to form, right to nothing. */
      XmxSetConstraints
        (label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
         NULL, NULL, NULL, NULL);
      XmxSetOffsets (win->search_win_text, 10, 0, 5, 8);
      /* search_win_text attaches top to form, bottom to nothing,
         left to label, right to form. */
      XmxSetConstraints
        (win->search_win_text, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, NULL, NULL, label, NULL);

      /* search_caseless_toggle attaches top to search_win_text, bottom to nothing,
         left to position, right to position. */
      XmxSetConstraints
        (win->search_caseless_toggle, XmATTACH_WIDGET, XmATTACH_NONE,
         XmATTACH_WIDGET, XmATTACH_NONE, 
         win->search_win_text, NULL, label, NULL);
      XmxSetOffsets (win->search_caseless_toggle, 8, 0, 2, 0);

      /* search_backwards_toggle attaches top to search_caseless_toggle,
         bottom to nothing, left to position, right to position. */
      XmxSetConstraints
        (win->search_backwards_toggle, XmATTACH_WIDGET, XmATTACH_NONE,
         XmATTACH_WIDGET, XmATTACH_NONE, win->search_caseless_toggle, 
         NULL, label, NULL);
      XmxSetOffsets (win->search_backwards_toggle, 8, 0, 2, 0);

      XmxSetOffsets (dialog_sep, 8, 0, 0, 0);
      /* dialog_sep attaches top to search_backwards_toggle,
         bottom to buttons_form, left to form, right to form */
      XmxSetConstraints 
        (dialog_sep, XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, 
         XmATTACH_FORM,
         win->search_backwards_toggle, buttons_form, NULL, NULL);
      XmxSetConstraints 
        (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
    }
  
  XmxManageRemanage (win->search_win);
  
  return mo_succeed;
}
