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

/* This document should be included in whatever source document
   sets up the Intrinsics.  It is in a separate file so it doesn't
   clutter up that file.  So sue me. */

#ifndef __MOSAIC_XRESOURCES_H__

/* ----------------------------- X Resources ------------------------------ */
#define offset(x) XtOffset (AppDataPtr, x)

static XtResource resources[] = {
  /* Whether Mosaic reads and writes global history from 
     ~/.mosaic-global-history
     and thus provides persistent history tracking. */
  { "useGlobalHistory", "UseGlobalHistory", XtRBoolean, sizeof (Boolean),
      offset (use_global_history), XtRString, "True" },
  /* Whether titles will be displayed wherever URL\'s are normally
     displayed. */
  { "displayURLsNotTitles", "DisplayURLsNotTitles", XtRBoolean, 
      sizeof (Boolean),
      offset (display_urls_not_titles), XtRString, "False" },
  /* Default width for a Document View window.  This will change as windows
     are cloned. */
  { "defaultWidth", "DefaultWidth", XtRInt, sizeof (int),
      offset (default_width), XtRString, "640" },
  /* Default height for a Document View window. */
  { "defaultHeight", "DefaultHeight", XtRInt, sizeof (int),
      offset (default_height), XtRString, "700" },
  /* Startup document. */
  { "homeDocument", "HomeDocument", XtRString, sizeof (char *),
      offset (home_document), XtRString, 
      HOME_PAGE_DEFAULT },
  { "confirmExit", "ConfirmExit", XtRBoolean, sizeof (Boolean),
      offset (confirm_exit), XtRString, "True" },
  /* THIS USED TO BE mailCommand BUT IS NOW sendmailCommand. */
  { "sendmailCommand", "SendmailCommand", XtRString, sizeof (char *),
      offset (sendmail_command), XtRString, "/usr/lib/sendmail" },
  /* Ignore this.  Stealth feature. */
  { "mailFilterCommand", "MailFilterCommand", XtRString, sizeof (char *),
      offset (mail_filter_command), XtRString, NULL },
  { "printCommand", "PrintCommand", XtRString, sizeof (char *),
      offset (print_command), XtRString, "lpr" },
  { "xtermCommand", "XtermCommand", XtRString, sizeof (char *),
      offset (xterm_command), XtRString, 
#ifdef _AIX
      "aixterm -v",
#else /* not _AIX */
      "xterm",
#endif /* not _AIX */
    },
  { "globalHistoryFile", "GlobalHistoryFile", XtRString, 
      sizeof (char *),
      offset (global_history_file), XtRString, ".mosaic-global-history" },
  { "defaultHotlistFile", "DefaultHotlistFile", XtRString, 
      sizeof (char *),
      offset (default_hotlist_file), XtRString, ".mosaic-hotlist-default" },
  { "personalAnnotationDirectory", "PersonalAnnotationDirectory", XtRString, 
      sizeof (char *),
      offset (private_annotation_directory), XtRString, 
      ".mosaic-personal-annotations" },
  /* Whether selections should be fancy, by default. */
  { "fancySelections", "FancySelections", XtRBoolean,
      sizeof (Boolean), offset (default_fancy_selections), 
      XtRString, "False" },
  { "defaultAuthorName", "DefaultAuthorName", XtRString, sizeof (char *),
      offset (default_author_name), XtRString, NULL },
  { "annotationsOnTop", "AnnotationsOnTop", XtRBoolean, sizeof (Boolean),
      offset (annotations_on_top), XtRString, "False" },

  { "colorsPerInlinedImage", "ColorsPerInlinedImage", XtRInt, sizeof (int),
      offset (colors_per_inlined_image), XtRString, "50" },
  { "trackVisitedAnchors", "TrackVisitedAnchors", XtRBoolean, sizeof (Boolean),
      offset (track_visited_anchors), XtRString, "True" },

  { "uncompressCommand", "UncompressCommand", XtRString, sizeof (char *), 
      offset (uncompress_command), XtRString, "uncompress" },
  { "gunzipCommand", "GunzipCommand", XtRString, 
      sizeof (char *), offset (gunzip_command), XtRString, "gunzip -f -n" },

  { "recordCommandLocation", "RecordCommandLocation", XtRString, 
      sizeof (char *), offset (record_command_location), XtRString,
#ifdef __hpux
      "/usr/audio/bin/srecorder" },
#else
#if defined(__sgi)
      "/usr/sbin/recordaiff" },
#else
#if defined (sun)
      "/usr/demo/SOUND/record" },
#else
      "/bin/true" },
#endif /* if sun */
#endif /* if sgi */
#endif /* ifdef */
  { "recordCommand", "RecordCommand", XtRString, sizeof (char *),
      offset (record_command), XtRString,
#ifdef __hpux
      "srecorder -au" },
#else
#if defined(__sgi)
      "recordaiff -n 1 -s 8 -r 8000" },
#else
#if defined (sun)
      "record" },
#else
      "true" },
#endif /* if sun */
#endif /* if sgi */
#endif /* ifdef */

  { "gethostbynameIsEvil", "GethostbynameIsEvil", XtRBoolean, sizeof (Boolean),
      offset (gethostbyname_is_evil), XtRString, "False" },
  { "autoPlaceWindows", "AutoPlaceWindows", XtRBoolean, sizeof (Boolean),
      offset (auto_place_windows), XtRString, "True" },
  { "initialWindowIconic", "InitialWindowIconic", XtRBoolean, sizeof (Boolean),
      offset (initial_window_iconic), XtRString, "False" },

  { "tmpDirectory", "TmpDirectory", XtRString, sizeof (char *),
      offset (tmp_directory), XtRString, NULL },
  { "annotationServer", "AnnotationServer", XtRString, sizeof (char *),
      offset (annotation_server), XtRString, NULL },
  { "catchPriorAndNext", "CatchPriorAndNext", XtRBoolean, sizeof (Boolean),
      offset (catch_prior_and_next), XtRString, "False" },

  /* Proxy gateway resources */
  { "httpProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (http_proxy), XtRString, NULL },
  { "ftpProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (ftp_proxy), XtRString, NULL },
  { "waisProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (wais_proxy), XtRString, NULL },
  { "gopherProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (gopher_proxy), XtRString, NULL },
  { "newsProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (news_proxy), XtRString, NULL },
  { "fileProxy", "ProxyGateway", XtRString, sizeof (char *),
      offset (file_proxy), XtRString, NULL },

  { "fullHostname", "FullHostname", XtRString, sizeof (char *),
      offset (full_hostname), XtRString, NULL },
  { "reverseInlinedBitmapColors", "ReverseInlinedBitmapColors", XtRBoolean,
      sizeof (Boolean),
      offset (reverse_inlined_bitmap_colors), XtRString, "False" },
  { "confirmDeleteAnnotation", "ConfirmDeleteAnnotation", 
      XtRBoolean, sizeof (Boolean),
      offset (confirm_delete_annotation), XtRString, "True"},
  { "tweakGopherTypes", "TweakGopherTypes", XtRBoolean, sizeof (Boolean),
      offset (tweak_gopher_types), XtRString, "True" },

  /* --- new in 2.0 --- */
  { "trackPointerMotion", "TrackPointerMotion", XtRBoolean, sizeof (Boolean),
      offset (track_pointer_motion), XtRString, "True" },
  { "trackFullURLs", "TrackFullURLs", XtRBoolean, sizeof (Boolean),
      offset (track_full_url_names), XtRString, "True" },

  { "hdfMaxImageDimension", "HdfMaxImageDimension", XtRInt, sizeof (int),
      offset (hdf_max_image_dimension), XtRString, "400" },
  { "hdfMaxDisplayedDatasets", "HdfMaxDisplayedDatasets", XtRInt, sizeof (int),
      offset (hdf_max_displayed_datasets), XtRString, "15" },
  { "hdfMaxDisplayedAttributes", "HdfMaxDisplayedAttributes", XtRInt, sizeof (int),
      offset (hdf_max_displayed_attributes), XtRString, "10" },
  { "hdfPowerUser", "HdfPowerUser", XtRBoolean, sizeof (Boolean),
      offset (hdf_power_user), XtRString, "False" },

  { "docsDirectory", "DocsDirectory", XtRString, sizeof (char *),
      offset (docs_directory), XtRString, NULL },

  { "documentsMenuSpecfile", "DocumentsMenuSpecfile", XtRString, sizeof (char *),
      offset (documents_menu_specfile), XtRString, 
      "/usr/local/lib/mosaic/documents.menu" },

  { "reloadReloadsImages", "ReloadReloadsImages", XtRBoolean, sizeof (Boolean),
      offset (reload_reloads_images), XtRString, "False" },
  { "simpleInterface", "SimpleInterface", XtRBoolean, sizeof (Boolean),
      offset (simple_interface), XtRString, "False" },

  { "maxWaisResponses", "MaxWaisResponses", XtRInt, sizeof (int),
      offset (max_wais_responses), XtRString, "200" },
  { "larryBitmaps", "LarryBitmaps", XtRBoolean, sizeof (Boolean),
      offset (larry_bitmaps), XtRString, "False" },
  { "hardinBong", "HardinBong", XtRBoolean, sizeof (Boolean),
      offset (hardin_bong), XtRString, "False" },
  { "delayImageLoads", "DelayImageLoads", XtRBoolean, sizeof (Boolean),
      offset (delay_image_loads), XtRString, "False" },

  { "useDefaultExtensionMap", "UseDefaultExtensionMap", 
      XtRBoolean, sizeof (Boolean),
      offset (use_default_extension_map), XtRString, "True" },
  { "globalExtensionMap", "GlobalExtensionMap", 
      XtRString, sizeof (char *),
      offset (global_extension_map), XtRString, GLOBAL_EXTENSION_MAP },
  { "personalExtensionMap", "PersonalExtensionMap", 
      XtRString, sizeof (char *),
      offset (personal_extension_map), XtRString, ".mime.types" },

  { "useDefaultTypeMap", "UseDefaultTypeMap", 
      XtRBoolean, sizeof (Boolean),
      offset (use_default_type_map), XtRString, "True" },
  { "globalTypeMap", "GlobalTypeMap", 
      XtRString, sizeof (char *),
      offset (global_type_map), XtRString, GLOBAL_TYPE_MAP },
  { "personalTypeMap", "PersonalTypeMap", 
      XtRString, sizeof (char *),
      offset (personal_type_map), XtRString, ".mailcap" },

  { "twirlingTransferIcon", "TwirlingTransferIcon", 
      XtRBoolean, sizeof (Boolean),
      offset (twirling_transfer_icon), XtRString, "True" },
  { "twirlIncrement", "TwirlIncrement", XtRInt, sizeof (int),
      offset (twirl_increment), XtRString, "4096" },

  { "imageCacheSize", "ImageCacheSize", XtRInt, sizeof (int),
      offset (image_cache_size), XtRString, "2048" },

  { "protectMeFromMyself", "ProtectMeFromMyself", 
      XtRBoolean, sizeof (Boolean),
      offset (protect_me_from_myself), XtRString, "False" },

#ifdef __sgi
  { "debuggingMalloc", "DebuggingMalloc", XtRBoolean, sizeof (Boolean),
      offset (debugging_malloc), XtRString, "False" },
#endif

#ifdef PEM_AUTH
  { "pemEncrypt", "PEMEncrypt", XtRString, sizeof (char *),
      offset (pem_encrypt), XtRString, NULL },
  { "pemDecrypt", "PEMDecrypt", XtRString, sizeof (char *),
      offset (pem_decrypt), XtRString, NULL },
  { "pemEntity", "PEMEntity", XtRString, sizeof (char *),
      offset (pem_entity), XtRString, NULL },
  { "pgpEncrypt", "PGPEncrypt", XtRString, sizeof (char *),
      offset (pgp_encrypt), XtRString, NULL },
  { "pgpDecrypt", "PGPDecrypt", XtRString, sizeof (char *),
      offset (pgp_decrypt), XtRString, NULL },
  { "pgpEntity", "PGPEntity", XtRString, sizeof (char *),
      offset (pgp_entity), XtRString, NULL },
#endif /* PEM_AUTH */
};

#undef offset

static XrmOptionDescRec options[] = {
  {"-fn",     "*fontList",            XrmoptionSepArg, NULL},
  {"-ft",     "*XmText*fontList",     XrmoptionSepArg, NULL},
  {"-fm",     "*menubar*fontList",    XrmoptionSepArg, NULL},
  {"-home",   "*homeDocument",        XrmoptionSepArg, NULL},
  {"-ngh",    "*useGlobalHistory",    XrmoptionNoArg,  "False"},
  /* Let Xt strip out -mono from stuff it considers interesting. */
  {"-mono",   "*nothingUseful",       XrmoptionNoArg,  "True"},
  {"-color",  "*nothingUseful",       XrmoptionNoArg,  "True"},
  {"-ghbnie", "*gethostbynameIsEvil", XrmoptionNoArg,  "True"},
  {"-iconic", "*initialWindowIconic", XrmoptionNoArg,  "True"},
  {"-i",      "*initialWindowIconic", XrmoptionNoArg,  "True"},
  /* New in 1.1 */
  /* -nd isn't documented since defaults in the widget still take effect,
     so the benefits of using it are kinda iffy (as if they weren't 
     anyway)... */
  {"-nd",     "*nothingUseful",       XrmoptionNoArg,  "True"},
  {"-tmpdir", "*tmpDirectory",        XrmoptionSepArg, NULL},
  {"-larry",  "*larryBitmaps",        XrmoptionNoArg,  "True"},
  {"-dil",    "*delayImageLoads",     XrmoptionNoArg,  "True"},
  {"-ics",    "*imageCacheSize",      XrmoptionSepArg, NULL},
  {"-protect","*protectMeFromMyself", XrmoptionNoArg,  "True"},
  {"-kraut",  "*mailFilterCommand",   XrmoptionNoArg,  "kraut"},
  {"-joseph", "*hardinBong",          XrmoptionNoArg,  "True"},
  {"-hardin", "*hardinBong",          XrmoptionNoArg,  "True"},
#ifdef __sgi
  {"-dm",     "*debuggingMalloc",     XrmoptionNoArg,  "True"},
#endif
};

static String color_resources[] = {
  "*XmLabel*fontList:   		-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*XmLabelGadget*fontList:	-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*XmScale*fontList:   		-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*XmBulletinBoard*labelFontList:	-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*optionmenu.XmLabelGadget*fontList:	-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*XmPushButton*fontList:		-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmPushButtonGadget*fontList:	-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmToggleButton*fontList:	-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmToggleButtonGadget*fontList:	-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*optionmenu*fontList:		-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmIconGadget*fontList:		-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmBulletinBoard*buttonFontList: -*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*menubar*fontList:   		-*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmPushButton*fontList:  -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmLabelGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmPushButtonGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmCascadeButton*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmCascadeButtonGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmCascadeButton*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmCascadeButtonGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmToggleButton*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmToggleButtonGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*pulldownmenu*fontList:	-*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmList*fontList:	-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmText.fontList:      -*-lucidatypewriter-medium-r-normal-*-14-*-iso8859-1",
  "*XmTextField.fontList: -*-lucidatypewriter-medium-r-normal-*-14-*-iso8859-1",
  
  "*optionmenu*marginHeight: 	0",
  "*optionmenu*marginTop: 		5",
  "*optionmenu*marginBottom: 	5",
  "*optionmenu*marginWidth: 	5",
  "*pulldownmenu*XmPushButton*marginHeight:	1",
  "*pulldownmenu*XmPushButton*marginWidth:	1",
  "*pulldownmenu*XmPushButton*marginLeft:	3",
  "*pulldownmenu*XmPushButton*marginRight:	3",
  "*XmList*listMarginWidth:        3",
  "*menubar*marginHeight: 		1",
  "*menubar.marginHeight: 		0",
  "*menubar*marginLeft:  		1",
  "*menubar.spacing:  		7",
  "*XmMenuShell*marginLeft:  	3",
  "*XmMenuShell*marginRight:  	4",
  "*XmMenuShell*XmToggleButtonGadget*spacing: 	 2",
  "*XmMenuShell*XmToggleButtonGadget*marginHeight:  0",
  "*XmMenuShell*XmToggleButtonGadget*indicatorSize: 12",
  "*XmMenuShell*XmLabelGadget*marginHeight: 4",
  "*XmToggleButtonGadget*spacing: 	4",
  "*XmToggleButton*spacing: 	4",
  "*XmScrolledWindow*spacing: 	0",
  "*XmScrollBar*width: 		        18",
  "*XmScrollBar*height: 		18",
  "*Hbar*height:                        22",
  "*Vbar*width:                         22",
  "*XmScale*scaleHeight: 		20",
  "*XmText*marginHeight:		4",
  "*fsb*XmText*width:                   420",
  "*fsb*XmTextField*width:                   420",
  "*fillOnSelect:			True",
  "*visibleWhenOff:		        True",
  "*XmText*highlightThickness:		0",
  "*XmTextField*highlightThickness:	0",
  "*XmPushButton*highlightThickness:	0",
  "*XmScrollBar*highlightThickness:     0",
  "*highlightThickness:	                0",
  /* "*geometry:                           +400+200", */
  "*keyboardFocusPolicy:                pointer",
  
  "*TitleFont: -adobe-times-bold-r-normal-*-24-*-*-*-*-*-iso8859-1",
  "*Font: -adobe-times-medium-r-normal-*-17-*-*-*-*-*-iso8859-1",
  "*ItalicFont: -adobe-times-medium-i-normal-*-17-*-*-*-*-*-iso8859-1",
  "*BoldFont: -adobe-times-bold-r-normal-*-17-*-*-*-*-*-iso8859-1",
  "*FixedFont: -adobe-courier-medium-r-normal-*-17-*-*-*-*-*-iso8859-1",
  "*Header1Font: -adobe-times-bold-r-normal-*-24-*-*-*-*-*-iso8859-1",
  "*Header2Font: -adobe-times-bold-r-normal-*-18-*-*-*-*-*-iso8859-1",
  "*Header3Font: -adobe-times-bold-r-normal-*-17-*-*-*-*-*-iso8859-1",
  "*Header4Font: -adobe-times-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*Header5Font: -adobe-times-bold-r-normal-*-12-*-*-*-*-*-iso8859-1",
  "*Header6Font: -adobe-times-bold-r-normal-*-10-*-*-*-*-*-iso8859-1",
  "*AddressFont: -adobe-times-medium-i-normal-*-17-*-*-*-*-*-iso8859-1",
  "*PlainFont: -adobe-courier-medium-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*ListingFont: -adobe-courier-medium-r-normal-*-12-*-*-*-*-*-iso8859-1",

  "*AnchorUnderlines:                   1",
  "*VisitedAnchorUnderlines:            1",
  "*DashedVisitedAnchorUnderlines:      True",
  "*VerticalScrollOnRight:              True",

#ifdef __sgi
  "*Foreground:			 	#000000000000",
  "*XmScrollBar*Foreground:             #afafafafafaf",
  "*Background:                         #afafafafafaf",
  "*XmList*Background:     		#afafafafafaf",
  "*XmText*Background:	 	        #afafafafafaf",
  "*TroughColor:                        #545454545454",
  "*XmSelectionBox*Background:	 	#afafafafafaf",
  "*XmMessageBox*Background:	 	#afafafafafaf",
  "*XmLabel*Foreground:                 #1d1d15155b5b",
  "*XmToggleButton*Foreground:          #1d1d15155b5b",
  "*XmPushButton*Foreground:            #5b5b00000000",
  "*logo*Foreground:                    #1d1d15155b5b",
  "*searchindex_button*Foreground:      #1d1d15155b5b",
  "*XmTextField*Background: 		#8c8c8c8c8c8c",
  "*SelectColor:			#ffffffff0000",
  "*HighlightColor:		 	#afafafafafaf",

  "*TopShadowColor:                     #dfdfdfdfdfdf",
  "*XmList*TopShadowColor:              #dfdfdfdfdfdf",
  "*XmText*TopShadowColor:              #dfdfdfdfdfdf",
  "*XmSelectionBox*TopShadowColor:      #dfdfdfdfdfdf",
  "*XmMessageBox*TopShadowColor:        #dfdfdfdfdfdf",
  
  "*visitedAnchorColor:                 #272705055b5b",
  "*anchorColor:                        #00000000b0b0",
  "*activeAnchorFG:                     #ffff00000000",
  "*activeAnchorBG:                     #afafafafafaf",
#else /* not sgi */
  "*Foreground:			 	#000000000000",
  "*XmScrollBar*Foreground:             #bfbfbfbfbfbf",
  "*XmLabel*Foreground:                 #1d1d15155b5b",
  "*XmToggleButton*Foreground:          #1d1d15155b5b",
  "*XmPushButton*Foreground:            #5b5b00000000",
  "*logo*Foreground:                    #1d1d15155b5b",
  "*searchindex_button*Foreground:      #1d1d15155b5b",

  "*Background:                         #bfbfbfbfbfbf",

  "*XmList*Background:     		#bfbfbfbfbfbf",
  "*XmText*Background:	 	        #bfbfbfbfbfbf",
  "*XmSelectionBox*Background:	 	#bfbfbfbfbfbf",
  "*XmMessageBox*Background:	 	#bfbfbfbfbfbf",
  "*XmTextField*Background: 		#9c9c9c9c9c9c",

  "*TopShadowColor:                     #e7e7e7e7e7e7",
  "*XmList*TopShadowColor:              #e7e7e7e7e7e7",
  "*XmText*TopShadowColor:              #e7e7e7e7e7e7",
  "*XmSelectionBox*TopShadowColor:      #e7e7e7e7e7e7",
  "*XmMessageBox*TopShadowColor:        #e7e7e7e7e7e7",
  
  "*TroughColor:                        #646464646464",
  "*SelectColor:			#ffffffff0000",
  "*HighlightColor:		 	#bfbfbfbfbfbf",

  /* Remember to update this in the app-defaults file. */
  "*visitedAnchorColor:                 #3f3f0f0f7b7b",
  "*anchorColor:                        #00000000b0b0",
  "*activeAnchorFG:                     #ffff00000000",
  "*activeAnchorBG:                     #bfbfbfbfbfbf",
#endif
  
  NULL,
};

static String mono_resources[] = {
  "*XmLabel*fontList:   		-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*XmLabelGadget*fontList:	-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*XmScale*fontList:   		-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*XmBulletinBoard*labelFontList:	-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*optionmenu.XmLabelGadget*fontList:	-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*XmPushButton*fontList:		-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmPushButtonGadget*fontList:	-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmToggleButton*fontList:	-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmToggleButtonGadget*fontList:	-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*optionmenu*fontList:		-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmIconGadget*fontList:		-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmBulletinBoard*buttonFontList: -*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*menubar*fontList:   		-*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmPushButton*fontList:  -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmLabelGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmPushButtonGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmCascadeButton*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmCascadeButtonGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmCascadeButton*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmCascadeButtonGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmToggleButton*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmMenuShell*XmToggleButtonGadget*fontList: -*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*pulldownmenu*fontList:	-*-helvetica-bold-o-normal-*-14-*-iso8859-1",
  "*XmList*fontList:	-*-helvetica-medium-r-normal-*-14-*-iso8859-1",
  "*XmText.fontList:      -*-lucidatypewriter-medium-r-normal-*-14-*-iso8859-1",
  "*XmTextField.fontList: -*-lucidatypewriter-medium-r-normal-*-14-*-iso8859-1",

  "*optionmenu*marginHeight: 	0",
  "*optionmenu*marginTop: 		5",
  "*optionmenu*marginBottom: 	5",
  "*optionmenu*marginWidth: 	5",
  "*pulldownmenu*XmPushButton*marginHeight:	1",
  "*pulldownmenu*XmPushButton*marginWidth:	1",
  "*pulldownmenu*XmPushButton*marginLeft:	3",
  "*pulldownmenu*XmPushButton*marginRight:	3",
  "*XmList*listMarginWidth:        3",
  "*menubar*marginHeight: 		1",
  "*menubar.marginHeight: 		0",
  "*menubar*marginLeft:  		1",
  "*menubar.spacing:  		7",
  "*XmMenuShell*marginLeft:  	3",
  "*XmMenuShell*marginRight:  	4",
  "*XmMenuShell*XmToggleButtonGadget*spacing: 	 2",
  "*XmMenuShell*XmToggleButtonGadget*marginHeight:  0",
  "*XmMenuShell*XmToggleButtonGadget*indicatorSize: 12",
  "*XmMenuShell*XmLabelGadget*marginHeight: 4",
  "*XmToggleButtonGadget*spacing: 	4",
  "*XmToggleButton*spacing: 	4",
  "*XmScrolledWindow*spacing: 	0",
  "*XmScrollBar*width: 		        18",
  "*XmScrollBar*height: 		18",
  "*Hbar*height:                        22",
  "*Vbar*width:                         22",
  "*XmScale*scaleHeight: 		20",
  "*XmText*marginHeight:		4",
  "*fsb*XmText*width:                   420",
  "*fsb*XmTextField*width:                   420",
  "*fillOnSelect:			True",
  "*visibleWhenOff:		        True",
  "*XmText*highlightThickness:		0",
  "*XmTextField*highlightThickness:	0",
  "*XmPushButton*highlightThickness:	0",
  "*XmScrollBar*highlightThickness:     0",
  "*highlightThickness:	                0",
  /* "*geometry:                           +400+200", */
  "*keyboardFocusPolicy:                pointer",

  "*TitleFont: -adobe-times-bold-r-normal-*-24-*-*-*-*-*-iso8859-1",
  "*Font: -adobe-times-medium-r-normal-*-17-*-*-*-*-*-iso8859-1",
  "*ItalicFont: -adobe-times-medium-i-normal-*-17-*-*-*-*-*-iso8859-1",
  "*BoldFont: -adobe-times-bold-r-normal-*-17-*-*-*-*-*-iso8859-1",
  "*FixedFont: -adobe-courier-medium-r-normal-*-17-*-*-*-*-*-iso8859-1",
  "*Header1Font: -adobe-times-bold-r-normal-*-24-*-*-*-*-*-iso8859-1",
  "*Header2Font: -adobe-times-bold-r-normal-*-18-*-*-*-*-*-iso8859-1",
  "*Header3Font: -adobe-times-bold-r-normal-*-17-*-*-*-*-*-iso8859-1",
  "*Header4Font: -adobe-times-bold-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*Header5Font: -adobe-times-bold-r-normal-*-12-*-*-*-*-*-iso8859-1",
  "*Header6Font: -adobe-times-bold-r-normal-*-10-*-*-*-*-*-iso8859-1",
  "*AddressFont: -adobe-times-medium-i-normal-*-17-*-*-*-*-*-iso8859-1",
  "*PlainFont: -adobe-courier-medium-r-normal-*-14-*-*-*-*-*-iso8859-1",
  "*ListingFont: -adobe-courier-medium-r-normal-*-12-*-*-*-*-*-iso8859-1",

  "*Foreground:                         black",
  "*Background:                         white",
  "*TopShadowColor:                     black",
  "*BottomShadowColor:                  black",
  "*anchorColor:                        black",
  "*visitedAnchorColor:                 black",
  "*activeAnchorFG:                     black",
  "*activeAnchorBG:                     white",
  "*TroughColor:                        black",
  "*SelectColor:                        black",
  "*AnchorUnderlines:                   1",
  "*VisitedAnchorUnderlines:            1",
  "*DashedVisitedAnchorUnderlines:      True",
  "*VerticalScrollOnRight:              True",
  
  NULL,
};

#define __MOSAIC_XRESOURCES_H__
#endif /* __MOSAIC_XRESOURCES_H__ */
