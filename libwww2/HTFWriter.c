/*		FILE WRITER				HTFWrite.h
**		===========
**
**	This version of the stream object just writes to a C file.
**	The file is assumed open and left open.
**
**	Bugs:
**		strings written must be less than buffer size.
*/

#include "HTFWriter.h"

#include "HTFormat.h"
#include "HTAlert.h"
#include "HTFile.h"
#include "HText.h"
#include "tcp.h"
#include "HTCompressed.h"

/*		Stream Object
**		------------
*/

struct _HTStream {
	CONST HTStreamClass *	isa;
	
	FILE *			fp;
        char * fnam;
	char * 			end_command;
        int compressed;
        int interrupted;
        int write_error;
};

/* #define TRACE 1 */

/* MOSAIC: We now pick up some external variables, handled
   in src/mo-www.c: */
extern int force_dump_to_file;
extern char *force_dump_filename;
/* If force_dump_to_file is high, we know we want to dump the
   data into a file already named by force_dump_filename and not
   do anything else. */

/* If this is high, then we just want to dump the thing to a file;
   the file is named by force_dump_filename. */
extern int binary_transfer;

/*_________________________________________________________________________
**
**			A C T I O N 	R O U T I N E S
*/

/*	Character handling
**	------------------
*/

PRIVATE void HTFWriter_put_character ARGS2(HTStream *, me, char, c)
{
  int rv;

  if (me->write_error)
    return;

  /* Make sure argument to putc is in range 0-255, to avoid weirdness
     with rv == -1 == EOF when it's not supposed to. */
  rv = putc ((int)(unsigned char)c, me->fp);

  if (rv == EOF)
    {
      HTProgress ("Error writing to temporary file.");
      me->write_error = 1;
    }
}


/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE void HTFWriter_put_string ARGS2(HTStream *, me, CONST char*, s)
{
  int rv;

  if (me->write_error)
    return;

  rv = fputs(s, me->fp);
  if (rv == EOF)
    {
      HTProgress ("Error writing to temporary file.");
      me->write_error = 1;
    }
}


/*	Buffer write.  Buffers can (and should!) be big.
**	------------
*/
PRIVATE void HTFWriter_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
  int rv;

  if (me->write_error)
    return;

  rv = fwrite(s, 1, l, me->fp); 
  if (rv != l)
    {
      HTProgress ("Error writing to temporary file.");
      me->write_error = 1;
    }
}


/*	Free an HTML object
**	-------------------
**
**	Note that the SGML parsing context is freed, but the created
**	object is not,
**	as it takes on an existence of its own unless explicitly freed.
*/
PRIVATE void HTFWriter_free ARGS1(HTStream *, me)
{
  HText *text;

  /* I dunno if this is necessary... */
  if (me->interrupted)
    {
      free (me->fnam);
      free (me);
      return;
    }

  if (me->write_error)
    {
      char *cmd = (char *)malloc ((strlen (me->fnam) + 32));
      sprintf (cmd, "/bin/rm -f %s &", me->fnam);
      system (cmd);
      free (cmd);

      HTProgress ("Insufficient temporary disk space; could not transfer data.");

      free (me->fnam);
      free (me);
      return;
    }

  fflush (me->fp);
  fclose (me->fp);

  /* We do want to be able to handle compressed inlined images,
     but we don't want transparent uncompression to take place
     in binary transfer mode. */
  if (!binary_transfer && me->compressed != COMPRESSED_NOT)
    {
      if (TRACE)
        fprintf (stderr, "[HTFWriter] Hi there; compressed is %d, fnam is '%s'\n",
                 me->compressed, me->fnam);
      HTCompressedFileToFile (me->fnam, me->compressed);
    }

  if (force_dump_to_file)
    {
      if (!binary_transfer)
        goto done;
    }

  /* Now, me->end_command can either be something starting with
     "<mosaic-internal-reference" or it can be a real command.
     Deal with appropriately. */
  if (me->end_command)
    {
      /* Check for forced dump condition.  The left paren comes
         from the construction of me->end_command as a compound shell
         command below. */
      if (strstr (me->end_command, "mosaic-internal-dump"))
        {
          rename_binary_file (me->fnam);
        }
      else if (!strstr (me->end_command, "mosaic-internal-reference"))
        {
          HTProgress("Spawning external viewer.");
          system (me->end_command);
        }
      else
        {
          /* Internal reference, aka HDF file.  Just close output file. */
        }
    }
  else
    {
      /* No me->end_command; just close the file. */
    }

  /* Construct dummy HText thingie so Mosaic knows
     not to try to access this "document". */
  text = HText_new ();
  HText_beginAppend (text);
  /* If it's a real internal reference, tell Mosaic. */
  if (me->end_command)
    {
      if (strstr (me->end_command, "mosaic-internal-reference"))
        {
          HText_appendText (text, me->end_command);
        }
      else
        {
          HText_appendText (text, "<mosaic-access-override>\n");
        }
      free (me->end_command);
    }
  else
    {
      /* No me->end_command; just override the access. */
      HText_appendText (text, "<mosaic-access-override>\n");
    }
  HText_endAppend (text);

 done:
  if (binary_transfer)
    rename_binary_file (me->fnam);

 really_done:
  free (me->fnam);
  free (me);

  return;
}

/*	End writing
*/

PRIVATE void HTFWriter_end_document ARGS1(HTStream *, me)
{
  if (me->interrupted || me->write_error)
    return;

  fflush(me->fp);
}

PRIVATE void HTFWriter_handle_interrupt ARGS1(HTStream *, me)
{
  char *cmd;

  if (me->write_error)
    goto outtahere;

  /* Close the file, then kill it. */
  fclose (me->fp);

  cmd = (char *)malloc ((strlen (me->fnam) + 32) * sizeof (char));
  sprintf (cmd, "/bin/rm -f %s &", me->fnam);
  system (cmd);
  free (cmd);

  if (TRACE)
    fprintf (stderr, "*** HTFWriter interrupted; killed '%s'\n", me->fnam);
  
 outtahere:
  me->interrupted = 1;

  return;
}


/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStreamClass HTFWriter = /* As opposed to print etc */
{		
	"FileWriter",
	HTFWriter_free,
	HTFWriter_end_document,
	HTFWriter_put_character, 	HTFWriter_put_string,
	HTFWriter_write,
        HTFWriter_handle_interrupt
}; 


/*	Take action using a system command
**	----------------------------------
**
**	Creates temporary file, writes to it, executes system command
**	on end-document.  The suffix of the temp file can be given
**	in case the application is fussy, or so that a generic opener can
**	be used.
**
**      WARNING: If force_dump_to_file is high, pres may be NULL
**      (as we may get called directly from HTStreamStack).
*/
PUBLIC HTStream* HTSaveAndExecute ARGS5(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,	/* Not used */
	HTStream *,		sink,
        HTFormat,               format_in,
        int,                    compressed)	/* Not used */
{
  char *command;
  CONST char * suffix;
  
  HTStream* me;

  me = (HTStream*)malloc(sizeof(*me));
  me->isa = &HTFWriter;  
  me->interrupted = 0;
  me->write_error = 0;
  me->fnam = NULL;
  me->end_command = NULL;
  me->compressed = compressed;

  if (TRACE)
    fprintf (stderr, "[HTSaveAndExecute] me->compressed is '%d'\n",
             me->compressed);
  
  /* Save the file under a suitably suffixed name */
  
  if (!force_dump_to_file)
    {
      extern char *mo_tmpnam (void);

      suffix = HTFileSuffix(pres->rep);
      
      me->fnam = mo_tmpnam();
      if (suffix) 
        {
          char *freeme = me->fnam;
         
          me->fnam = (char *)malloc (strlen (me->fnam) + strlen (suffix) + 8);
          strcpy(me->fnam, freeme);
          strcat(me->fnam, suffix);
          free (freeme);
        }
    }
  else
    {
      me->fnam = strdup (force_dump_filename);
    }

  me->fp = fopen (me->fnam, "w");
  if (!me->fp) 
    {
      HTProgress("Can't open temporary file -- serious problem.");
      me->write_error = 1;
      return me;
    }

  /* If force_dump_to_file is high, we're done here. */
  if (!force_dump_to_file)
    {
      if (!strstr (pres->command, "mosaic-internal-reference"))
        {
          /* If there's a "%s" in the command, or if the command
             is magic... */
          if (TRACE)
            fprintf (stderr, "HTFWriter: pres->command is '%s'\n",
                     pres->command);
          if (strstr (pres->command, "%s") ||
              strstr (pres->command, "mosaic-internal"))
            {
              /* Make command to process file */
              command = (char *)malloc 
                ((strlen (pres->command) + 10 + 3*strlen(me->fnam)) * 
                 sizeof (char));
              
              /* Cute.  pres->command will be something like "xv %s"; me->fnam
                 gets filled in as many times as appropriate.  */
              sprintf (command, pres->command, me->fnam, me->fnam, me->fnam);
              
              me->end_command = (char *)malloc 
                ((strlen (command) + 32 + strlen(me->fnam)) * sizeof (char));
              sprintf (me->end_command, "(%s ; /bin/rm -f %s) &",
                       command, me->fnam);

              free (command);
            }
          else
            {
              /* Make command to process file -- but we have to cat
                 to the viewer's stdin. */
              me->end_command = (char *)malloc 
                ((strlen (pres->command) + 64 + (2 * strlen(me->fnam))) * 
                 sizeof (char));
              sprintf (me->end_command, "((cat %s | %s); /bin/rm -f %s) &",
                       me->fnam, pres->command, me->fnam);
            }
        }
      else
        {
          /* Overload me->end_command to be what we should write out as text
             to communicate back to client code. */
          me->end_command = (char *)malloc
            (strlen ("mosaic-internal-reference") + strlen (me->fnam) + 32);
          sprintf (me->end_command, "<%s \"%s\">\n", "mosaic-internal-reference", me->fnam);
        }
    }
  
  return me;
}
