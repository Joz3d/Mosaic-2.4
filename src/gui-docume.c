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
#include "libhtmlw/HTML.h"

extern char *cached_url;
extern int binary_transfer;
#ifdef PEM_AUTH
extern int post_gave_encrypt;
#endif /* PEM_AUTH */
extern char *startup_document, *home_document;
extern Display *dsp;

int loading_inlined_images = 0;
char *url_base_override = NULL;
int interrupted = 0;

extern char *mo_post_pull_er_over (char *url, char *content_type, 
                                   char *post_data, 
                                   char **texthead);

/****************************************************************************
 * name:    mo_snarf_scrollbar_values
 * purpose: Store current viewing state in the current node, in case
 *          we want to return to the same location later.
 * inputs:  
 *   - mo_window *win: Current window.
 * returns: 
 *   mo_succeed
 *   (mo_fail if no current node exists)
 * remarks: 
 *   Snarfs current docid position in the HTML widget.
 ****************************************************************************/
static mo_status mo_snarf_scrollbar_values (mo_window *win)
{
  /* Make sure we have a node. */
  if (!win->current_node)
    return mo_fail;

  win->current_node->docid = HTMLPositionToId(win->scrolled_win, 0, 3);

  /* Do the cached stuff thing. */
  win->current_node->cached_stuff = HTMLGetWidgetInfo (win->scrolled_win);

  return mo_succeed;
}


/* ---------------------- mo_reset_document_headers ----------------------- */

static mo_status mo_reset_document_headers (mo_window *win)
{
  if (win->current_node)
    {
      XmxTextSetString (win->title_text, win->current_node->title);
      XmxTextSetString (win->url_text, win->current_node->url);
    }

  return mo_succeed;
}

/* --------------------------- mo_back_possible --------------------------- */

/* This could be cached, but since it shouldn't take too long... */
static void mo_back_possible (mo_window *win)
{
  XmxSetSensitive (win->back_button, XmxSensitive);
  XmxRSetSensitive (win->menubar, mo_back, XmxSensitive);
  return;
}


/****************************************************************************
 * name:    mo_back_impossible
 * purpose: Can't go back (nothing in the history list).
 ****************************************************************************/
mo_status mo_back_impossible (mo_window *win)
{
  XmxSetSensitive (win->back_button, XmxNotSensitive);
  XmxRSetSensitive (win->menubar, mo_back, XmxNotSensitive);
  return mo_succeed;
}

static void mo_forward_possible (mo_window *win)
{
  XmxSetSensitive (win->forward_button, XmxSensitive);
  XmxRSetSensitive (win->menubar, mo_forward, XmxSensitive);
  return;
}


/****************************************************************************
 * name:    mo_forward_impossible
 * purpose: Can't go forward (nothing in the history list).
 ****************************************************************************/
mo_status mo_forward_impossible (mo_window *win)
{
  XmxSetSensitive (win->forward_button, XmxNotSensitive);
  XmxRSetSensitive (win->menubar, mo_forward, XmxNotSensitive);
  return mo_succeed;
}

/* ---------------------- mo_annotate_edit_possible ----------------------- */

static void mo_annotate_edit_possible (mo_window *win)
{
  XmxRSetSensitive (win->menubar, mo_annotate_edit, XmxSensitive);
  XmxRSetSensitive (win->menubar, mo_annotate_delete, XmxSensitive);
  return;
}

static void mo_annotate_edit_impossible (mo_window *win)
{
  XmxRSetSensitive (win->menubar, mo_annotate_edit, XmxNotSensitive);
  XmxRSetSensitive (win->menubar, mo_annotate_delete, XmxNotSensitive);
  return;
}


/* ------------------------------------------------------------------------ */

static void mo_set_text (Widget w, char *txt, char *ans, int id, 
                         char *target_anchor, void *cached_stuff)
{
  /* Any data transfer that takes place in here must be inlined
     image loading. */
  loading_inlined_images = 1;
  interrupted = 0;
  mo_set_image_cache_nuke_threshold ();
  if (Rdata.annotations_on_top)
    HTMLSetText (w, txt, ans ? ans : "\0", "\0", id, target_anchor, 
                 cached_stuff);
  else
    HTMLSetText (w, txt, "\0", ans ? ans : "\0", id, target_anchor, 
                 cached_stuff);
  loading_inlined_images = 0;
  interrupted = 0;
  mo_gui_done_with_icon ();
}


/****************************************************************************
 * name:    mo_do_window_text (PRIVATE)
 * purpose: Set a window's text and do lots of other housekeeping
 *          and GUI-maintenance things.
 * inputs:  
 *   - mo_window *win: The current window.
 *   - char      *url: The URL for the text; assumed to be canonicalized
 *                     and otherwise ready for inclusion in history lists,
 *                     the window's overhead URL display, etc.
 *   - char      *txt: The new text for the window.
 *   - char  *txthead: The start of the malloc'd block of text corresponding
 *                     to txt.
 *   - int register_visit: If TRUE, then this text should be registerd
 *                         as a new node in the history list.  If FALSE,
 *                         then we're just moving around in the history list.
 *   - char      *ref: Reference (possible title) for this text.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This is the mother of all functions in Mosaic.  Probably should be
 *   rethought and broken down.
 ****************************************************************************/
static mo_status mo_do_window_text (mo_window *win, char *url, char *txt,
                                    char *txthead,
                                    int register_visit, char *ref)
{
  char *line, *ans;

  mo_set_current_cached_win (win);

  if (Rdata.track_pointer_motion)
    {
      XmString xmstr = XmStringCreateLtoR (" ", XmSTRING_DEFAULT_CHARSET);
      XtVaSetValues
        (win->tracker_label,
         XmNlabelString, (XtArgVal)xmstr,
         NULL);
      XmStringFree (xmstr);
    }
      
  /* If !register_visit, we're just screwing around with current_node
     already, so don't bother snarfing scrollbar values. */
  if (register_visit)
    mo_snarf_scrollbar_values (win);

  /* cached_url HAS to be set here, since Resolve counts on it. */
  cached_url = mo_url_canonicalize (url, "");
  win->cached_url = cached_url;

  mo_here_we_are_son (url);
  
  {
    /* Since mo_fetch_annotation_links uses the communications code,
       we need to play games with binary_transfer. */
    int tmp = binary_transfer;
    binary_transfer = 0;
    ans = mo_fetch_annotation_links (url,
                                     Rdata.annotations_on_top);
    binary_transfer = tmp;
  }

  /* If there is a BASE tag in the document that contains a "real"
     URL, this will be non-NULL by the time we exit and base_callback
     will have been called. */
  url_base_override = NULL;

  {
    int id = 0, freeta = 0;
    void *cached_stuff = NULL;
    char *target_anchor = win->target_anchor;

    if ((!register_visit) && win->current_node)
      {
        id = win->current_node->docid;
        cached_stuff = win->current_node->cached_stuff;
      }

    /* If the window doesn't have a target anchor already,
       see if there's one in this node's URL. */
    if ((!target_anchor || !(*target_anchor)) && win->current_node)
      {
        target_anchor = mo_url_extract_anchor (win->current_node->url);
        freeta = 1;
      }

    if (!txt || !txthead)
      {
#if 0
        txt = strdup ("<h1>Uh oh, I'm stumped</h1> Mosaic has encountered a situation in which it doesn't know what to do.  Please back up and try again, or send a bug report including full details about what you're trying to do to <b>mosaic-x@ncsa.uiuc.edu</b>.  We thank you for your support.");
#endif
        /* Just make it look OK...  band-aid city. */
        txt = strdup ("\0");
        txthead = txt;
      }

    mo_set_text (win->scrolled_win, txt, ans, id, target_anchor,
                 cached_stuff);

    if (win->target_anchor)
      free (win->target_anchor);

    win->target_anchor = NULL;

    if (freeta)
      free (target_anchor);
  }

  if (url_base_override)
    {
      /* Get the override URL -- this should be all we need to do here. */
      url = url_base_override;
      mo_here_we_are_son (url);
    }

  /* Every time we view the document, we reset the search_start
     struct so searches will start at the beginning of the document. */
  ((ElementRef *)win->search_start)->id = 0;

  /* CURRENT_NODE ISN'T SET UNTIL HERE (assuming register_visit is 1). */
  /* Now that WbNtext has been set, we can pull out WbNtitle. */
  /* First, check and see if we have a URL.  If not, we probably
     are only jumping around inside a document. */
  if (url && *url)
    {
      if (register_visit)
        mo_record_visit (win, url, txt, txthead, ref);
      else
        {
          /* At the very least we want to pull out the new title,
             if one exists. */
          if (win->current_node)
            {
              if (win->current_node->title)
                free (win->current_node->title);
              win->current_node->title = mo_grok_title (win, url, ref);
            }
        }
    }
  
  mo_reset_document_headers (win);

  if (win->history_list && win->current_node)
    {
      XmListSelectPos (win->history_list, win->current_node->position, False);
      XmListSetBottomPos (win->history_list, win->current_node->position);
    }

  /* Update source text if necessary. */
  if (win->source_text && XtIsManaged(win->source_text) &&
      win->current_node)
    {
      XmxTextSetString (win->source_text, win->current_node->text);
      XmxTextSetString (win->source_url_text, win->current_node->url);
    }

  if (win->current_node && win->current_node->previous != NULL)
    mo_back_possible (win);
  else
    mo_back_impossible (win);
  
  if (win->current_node && win->current_node->next != NULL)
    mo_forward_possible (win);
  else
    mo_forward_impossible (win);

  if (win->current_node && 
      mo_is_editable_annotation (win, win->current_node->text))
    mo_annotate_edit_possible (win);
  else
    mo_annotate_edit_impossible (win);

  mo_not_busy ();

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_set_win_current_node
 * purpose: Given a window and a node, set the window's current node.
 *          This assumes node is already all put together, in the history
 *          list for the window, etc.
 * inputs:  
 *   - mo_window *win: The current window.
 *   - mo_node  *node: The node to use.
 * returns: 
 *   Result of calling mo_do_window_text.
 * remarks: 
 *   This routine is meant to be used to move forward, backward,
 *   and to arbitrarily locations in the history list.
 ****************************************************************************/
mo_status mo_set_win_current_node (mo_window *win, mo_node *node)
{
  void *to_free = NULL;
  mo_status r;
  
  mo_snarf_scrollbar_values (win);

  if (win->current_node && win->reloading)
    {
      to_free = win->current_node->cached_stuff;

      win->current_node->cached_stuff = NULL;
    }

  win->current_node = node;

  mo_busy ();
  mo_set_current_cached_win (win);
  r = mo_do_window_text (win, win->current_node->url, 
                         win->current_node->text, 
                         win->current_node->texthead,
                         FALSE, win->current_node->ref);

  if (win->reloading)
    {
      if (to_free)
        HTMLFreeWidgetInfo (to_free);
      
      win->reloading = 0;
    }

  return r;
}


/****************************************************************************
 * name:    mo_reload_window_text
 * purpose: Reload the current window's text by pulling it over the
 *          network again.
 * inputs:  
 *   - mo_window *win: The current window.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This frees the current window's texthead.  This calls mo_pull_er_over
 *   directly, and needs to be smarter about handling HDF, etc.
 ****************************************************************************/
mo_status mo_reload_window_text (mo_window *win, int reload_images_also)
{
  mo_busy ();

  mo_set_current_cached_win (win);

  /* Uh oh, this is trouble... */
  if (!win->current_node)
    return mo_load_window_text 
      (win, startup_document ? startup_document : home_document, NULL);

  /* Free all images in the current document. */
  if (Rdata.reload_reloads_images || reload_images_also)
    mo_zap_cached_images_here (win);

  /* Free the current document's text. */
  /* REALLY we shouldn't do this until we've verified we have new text that's
     actually good here -- for instance, if we have a document on display,
     then go to binary transfer mode, then do reload, we should pick up the
     access override here and keep the old copy up on screen. */
  if (win->current_node->texthead != NULL)
    {
      free (win->current_node->texthead);
      win->current_node->texthead = NULL;
    }

  /* Set binary_transfer as per current window. */
  binary_transfer = win->binary_transfer;
  mo_set_current_cached_win (win);
  interrupted = 0;

#ifdef PEM_AUTH
	/*
	 * For http requests only, we may have a mandatory encrypt
	 * state.  Try and enforce that here.
	 */
	if ((!post_gave_encrypt)&&
		(strncmp(win->current_node->url, "http:", 5) == 0))
	{
	    int encrypt_ok;

	    switch(win->encrypt_state)
	    {
		case mo_no_encrypt:
			break;
		case mo_pem_encrypt:
			encrypt_ok = mo_url_set_encryption("PEM", NULL,
				win->current_node->url);
			break;
		case mo_pgp_encrypt:
			encrypt_ok = mo_url_set_encryption("PGP", NULL,
				win->current_node->url);
			break;
	    }
	}
	post_gave_encrypt = 0;
#endif /* PEM_AUTH */

  win->current_node->text = mo_pull_er_over (win->current_node->url, 
                                             &win->current_node->texthead);
  {
    /* Check use_this_url_instead from HTAccess.c. */
    /* IS THIS GOOD ENOUGH FOR THIS CASE??? */
    extern char *use_this_url_instead;
    if (use_this_url_instead)
      {
        win->current_node->url = use_this_url_instead;
      }
  }
#ifdef HAVE_HDF
  if (win->current_node->text && 
      strncmp (win->current_node->text, "<mosaic-internal-reference", 26) == 0)
    {
      char *text = mo_decode_internal_reference 
        (win->current_node->url, win->current_node->text,
         mo_url_extract_anchor (win->current_node->url));
      win->current_node->text = text;
      win->current_node->texthead = text;
    }
#endif 
  
  /* Clear out the cached stuff, if any exists. */
  win->reloading = 1;

  mo_set_win_current_node (win, win->current_node);

  win->reloading = 0;

  return mo_succeed;
}



/****************************************************************************
 * name:    mo_refresh_window_text
 * purpose: Reload the current window's text without pulling it over the net.
 * inputs:  
 *   - mo_window *win: The current window.
 * returns: 
 *   mo_succeed
 * remarks: 
 ****************************************************************************/
mo_status mo_refresh_window_text (mo_window *win)
{
  mo_busy ();

  mo_set_current_cached_win (win);

  if (!win->current_node)
    {
      mo_not_busy ();
      return mo_fail;
    }

  /* Clear out the cached stuff, if any exists. */
  win->reloading = 1;

  mo_set_win_current_node (win, win->current_node);

  mo_not_busy ();

  return mo_succeed;
}



/****************************************************************************
 * name:    mo_load_window_text
 * purpose: Given a window and a raw URL, load the window.  The window
 *          is assumed to already exist with a document inside, etc.
 * inputs:  
 *   - mo_window *win: The current window.
 *   - char      *url: The URL to load.
 *   - char      *ref: The reference ("parent") URL.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This is getting ugly.
 ****************************************************************************/
mo_status mo_load_window_text (mo_window *win, char *url, char *ref)
{
  char *newtext = NULL, *newtexthead = NULL;
  mo_busy ();

  win->target_anchor = mo_url_extract_anchor (url);

  /* If we're just referencing an anchor inside a document,
     do the right thing. */
  if (url && *url == '#')
    {
      /* Now we make a copy of the current text and make sure we ask
         for a new mo_node and entry in the history list. */
      /* IF we're not dealing with an internal reference. */
      if (strncmp (url, "#hdfref;", 8) &&
          strncmp (url, "#hdfdtm;", 8))
        {
          if (win->current_node)
            {
              newtext = strdup (win->current_node->text);
              newtexthead = newtext;
            }
          else
            {
              newtext = strdup ("lose");
              newtexthead = newtext;
            }
        }
      url = mo_url_canonicalize_keep_anchor 
        (url, win->current_node ? win->current_node->url : "");
    }
  else
    {
      /* Get a full address for this URL. */
      /* Under some circumstances we may not have a current node yet
         and may wish to just run with it... so check for that. */
      if (win->current_node && win->current_node->url)
        {
          url = mo_url_canonicalize_keep_anchor (url, win->current_node->url);
        }

      /* Set binary_transfer as per current window. */
      binary_transfer = win->binary_transfer;
      mo_set_current_cached_win (win);

      {
        char *canon = mo_url_canonicalize (url, "");
        interrupted = 0;

#ifdef PEM_AUTH
	/*
	 * For http requests only, we may have a mandatory encrypt
	 * state.  Try and enforce that here.
	 */
	if ((!post_gave_encrypt)&&(strncmp(canon, "http:", 5) == 0))
	{
	    int encrypt_ok;

	    switch(win->encrypt_state)
	    {
		case mo_no_encrypt:
			break;
		case mo_pem_encrypt:
			encrypt_ok = mo_url_set_encryption("PEM", NULL, canon);
			break;
		case mo_pgp_encrypt:
			encrypt_ok = mo_url_set_encryption("PGP", NULL, canon);
			break;
	    }
	}
	post_gave_encrypt = 0;
#endif /* PEM_AUTH */

        newtext = mo_pull_er_over (canon, &newtexthead);
        free (canon);
      }

      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
            
            /* Go get another target_anchor. */
            if (win->target_anchor)
              free (win->target_anchor);
            win->target_anchor = mo_url_extract_anchor (url);
          }
      }
    }

#ifdef HAVE_HDF
  /* If a target anchor exists, and if it's an HDF reference, then
     go decode the HDF reference and call mo_do_window_text on the
     resulting text. */
  if (win->target_anchor &&
      strncmp (win->target_anchor, "hdfref;", 7) == 0 &&
      strlen (win->target_anchor) > 8)
    {
      char *text;
      text = (char *)mo_decode_hdfref (url, win->target_anchor);
      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
            mo_load_window_text(win, url, ref);
            return;
          }
      }
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  /* An hdfdtm reference means that we should blast the referenced
     HDF data object over the output DTM port to Collage.  Currently
     this can only be an image; in the future we'll do SDS's, etc. */
  else if (win->target_anchor &&
           strncmp (win->target_anchor, "hdfdtm;", 7) == 0 &&
           strlen (win->target_anchor) > 8)
    {
#ifdef HAVE_DTM
      /* We specifically want to make sure that the anchor is allowed
         to stay in the URL, so we don't canonicalize to strip it out. */
      mo_do_hdf_dtm_thang (url, &(win->target_anchor[7]));
#endif

      if (win->target_anchor)
        free (win->target_anchor);
      win->target_anchor = NULL;

      mo_gui_done_with_icon ();
      mo_not_busy ();
    }
  /* Assuming we have HDF, the only thing mosaic-internal-reference
     currently can be is pointer to an HDF file. */
  else if (newtext && strncmp (newtext, "<mosaic-internal-reference", 26) == 0)
    {
      char *text;
      text = mo_decode_internal_reference (url, newtext, win->target_anchor);
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  else
#endif
  /* Now, if it's a telnet session, there should be no need
     to do anything else.  Also check for override in text itself. */
  if (strncmp (url, "telnet:", 7) == 0 || strncmp (url, "tn3270:", 7) == 0 ||
      strncmp (url, "rlogin:", 7) == 0 ||
      (newtext && strncmp (newtext, "<mosaic-access-override>", 24) == 0))
    {
      /* We don't need this anymore. */
      free (newtext);

      /* We still want a global history entry but NOT a 
         window history entry. */
      mo_here_we_are_son (url);
      /* ... and we want to redisplay the current window to
         get the effect of the history entry today, not tomorrow. */
      mo_redisplay_window (win);
      /* We're not busy anymore... */
      mo_gui_done_with_icon ();
      mo_not_busy ();
    }
  else if (newtext)
    {
      /* Not a telnet session and not an override, but text present
         (the "usual" case): */

      /* Set the window text. */
      mo_do_window_text (win, url, newtext, newtexthead, 1, ref);
    }
  else
    {
      /* No text at all. */
      mo_gui_done_with_icon ();
      mo_not_busy ();
    }

 outtahere:
  return mo_succeed;
}


mo_status mo_post_load_window_text (mo_window *win, char *url, 
                                    char *content_type, char *post_data, 
                                    char *ref)
{
  char *newtext = NULL, *newtexthead = NULL;
  mo_busy ();

  win->target_anchor = mo_url_extract_anchor (url);

  /* If we're just referencing an anchor inside a document,
     do the right thing. */
  if (url && *url == '#')
    {
      /* Now we make a copy of the current text and make sure we ask
         for a new mo_node and entry in the history list. */
      /* IF we're not dealing with an internal reference. */
      if (strncmp (url, "#hdfref;", 8) &&
          strncmp (url, "#hdfdtm;", 8))
        {
          if (win->current_node)
            {
              newtext = strdup (win->current_node->text);
              newtexthead = newtext;
            }
          else
            {
              newtext = strdup ("lose");
              newtexthead = newtext;
            }
        }
      url = mo_url_canonicalize_keep_anchor 
        (url, win->current_node ? win->current_node->url : "");
    }
  else
    {
      /* Get a full address for this URL. */
      /* Under some circumstances we may not have a current node yet
         and may wish to just run with it... so check for that. */
      if (win->current_node && win->current_node->url)
        {
          url = mo_url_canonicalize_keep_anchor (url, win->current_node->url);
        }

      /* Set binary_transfer as per current window. */
      binary_transfer = win->binary_transfer;
      mo_set_current_cached_win (win);

      {
        char *canon = mo_url_canonicalize (url, "");
        interrupted = 0;

#ifdef PEM_AUTH
	/*
	 * For http requests only, we may have a mandatory encrypt
	 * state.  Try and enforce that here.
	 */
	if ((!post_gave_encrypt)&&(strncmp(canon, "http:", 5) == 0))
	{
	    int encrypt_ok;

	    switch(win->encrypt_state)
	    {
		case mo_no_encrypt:
			break;
		case mo_pem_encrypt:
			encrypt_ok = mo_url_set_encryption("PEM", NULL, canon);
			break;
		case mo_pgp_encrypt:
			encrypt_ok = mo_url_set_encryption("PGP", NULL, canon);
			break;
	    }
	}
	post_gave_encrypt = 0;
#endif /* PEM_AUTH */

        newtext = mo_post_pull_er_over (canon, content_type, post_data, 
                                        &newtexthead);
        free (canon);
      }

      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
          }
      }
    }

#ifdef HAVE_HDF
  /* If a target anchor exists, and if it's an HDF reference, then
     go decode the HDF reference and call mo_do_window_text on the
     resulting text. */
  if (win->target_anchor &&
      strncmp (win->target_anchor, "hdfref;", 7) == 0 &&
      strlen (win->target_anchor) > 8)
    {
      char *text;
      text = (char *)mo_decode_hdfref (url, win->target_anchor);
      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
            mo_load_window_text(win, url, ref);
            return;
          }
      }
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  /* An hdfdtm reference means that we should blast the referenced
     HDF data object over the output DTM port to Collage.  Currently
     this can only be an image; in the future we'll do SDS's, etc. */
  else if (win->target_anchor &&
           strncmp (win->target_anchor, "hdfdtm;", 7) == 0 &&
           strlen (win->target_anchor) > 8)
    {
#ifdef HAVE_DTM
      /* We specifically want to make sure that the anchor is allowed
         to stay in the URL, so we don't canonicalize to strip it out. */
      mo_do_hdf_dtm_thang (url, &(win->target_anchor[7]));
#endif

      if (win->target_anchor)
        free (win->target_anchor);
      win->target_anchor = NULL;

      mo_gui_done_with_icon ();
      mo_not_busy ();
    }
  /* Assuming we have HDF, the only thing mosaic-internal-reference
     currently can be is pointer to an HDF file. */
  else if (newtext && strncmp (newtext, "<mosaic-internal-reference", 26) == 0)
    {
      char *text;
      text = mo_decode_internal_reference (url, newtext, win->target_anchor);
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  else
#endif
  /* Now, if it's a telnet session, there should be no need
     to do anything else.  Also check for override in text itself. */
  if (strncmp (url, "telnet:", 7) == 0 || strncmp (url, "tn3270:", 7) == 0 ||
      strncmp (url, "rlogin:", 7) == 0 ||
      (newtext && strncmp (newtext, "<mosaic-access-override>", 24) == 0))
    {
      /* We don't need this anymore. */
      free (newtext);

      /* We still want a global history entry but NOT a 
         window history entry. */
      mo_here_we_are_son (url);
      /* ... and we want to redisplay the current window to
         get the effect of the history entry today, not tomorrow. */
      mo_redisplay_window (win);
      /* We're not busy anymore... */
      mo_gui_done_with_icon ();
      mo_not_busy ();
    }
  else if (newtext)
    {
      /* Not a telnet session and not an override, but text present
         (the "usual" case): */

      /* Set the window text. */
      mo_do_window_text (win, url, newtext, newtexthead, 1, ref);
    }
  else
    {
      /* No text at all. */
      mo_gui_done_with_icon ();
      mo_not_busy ();
    }

 outtahere:
  return mo_succeed;
}





/****************************************************************************
 * name:    mo_duplicate_window_text
 * purpose: Given an old window and a new window, make a copy of the text
 *          in the old window and install it in the new window.
 * inputs:  
 *   - mo_window *oldw: The old window.
 *   - mo_window *neww: The new window.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This is how windows are cloned: a new window is created and this
 *   call sets up its contents.
 ****************************************************************************/
mo_status mo_duplicate_window_text (mo_window *oldw, mo_window *neww)
{
  /* We can get away with just cloning text here and forgetting
     about texthead, obviously, since we're making a new copy. */
  char *newtext;

  if (!oldw->current_node)
    return mo_fail;

  newtext = strdup (oldw->current_node->text);

  mo_do_window_text 
    (neww, strdup (oldw->current_node->url), 
     newtext, newtext, TRUE, 
     oldw->current_node->ref ? strdup (oldw->current_node->ref) : NULL);

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_access_document
 * purpose: Given a URL, access the document by loading the current 
 *          window's text.
 * inputs:  
 *   - mo_window *win: The current window.
 *   - char      *url: URL to access.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This should be the standard call for accessing a document.
 ****************************************************************************/
mo_status mo_access_document (mo_window *win, char *url)
{
  mo_busy ();

  mo_set_current_cached_win (win);

  mo_load_window_text (win, url, NULL);

  return mo_succeed;
}


mo_status mo_post_access_document (mo_window *win, char *url,
                                   char *content_type, char *post_data)
{
  mo_busy ();

  mo_set_current_cached_win (win);

  mo_post_load_window_text (win, url, content_type, post_data, NULL);

  return mo_succeed;
}
