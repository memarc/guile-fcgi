/******************************************************************************
 *  
 *       Filename: src/main.c
 *       Project : 
 *       Author  : Sergey A. Dolin
 *       CVS     : $Author$ $Date$
 *       Creation: Tue Feb 13 17:21:31 YEKT 2001
 *       Purpose :
 *  --------------------------------------------------------------------------
 *       Modified: Sun Mar 25 19:34:53 YEKST 2001 by Sergey A. Dolin
 *       Bugs    : 
 *       Exports : 
 *       Using   : 
 *  
 *   Copyright (C) 2000 Sergey A. Dolin
 *    
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *    
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *  
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *    
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif /*HAVE_CONFIG_H*/
#include <libguile.h>
#include <fcgi_stdio.h>
#include <fcgiapp.h>
#include <string.h>

/****************************************************************************/
/*                SMOBS      interface                                      */
/****************************************************************************/

static long tagFCGX_Stream;
#define SCM_FCGX_STREAMP(smob) (SCM_NIMP(smob) && SCM_CAR(smob)==tagFCGX_Stream)
#define SCM_FCGX_STREAM(smob) ((FCGX_Stream*)SCM_CDR(smob))

static long tagFCGX_Request;
#define SCM_FCGX_REQUESTP(smob) (SCM_NIMP(smob) && SCM_CAR(smob)==tagFCGX_Request)
#define SCM_FCGX_REQUEST(smob) (FCGX_Request*)SCM_CDR(smob)

long fcgi_ptype=0;

/****************************************************************************/
/* FCGX Interface                                                           */
/****************************************************************************/

/*DLLAPI int FCGX_IsCGI(void);*/
SCM_DEFINE (scm_FCGX_IsCGI,"FCGX:IsCGI",0,0,0,
          (),
" * FCGX_IsCGI --\n"
" *      Returns #t iff this process appears to be a CGI process\n"
" *      rather than a FastCGI process.\n"
)
{
  return (FCGX_IsCGI())?SCM_BOOL_T:SCM_BOOL_F;
}
/*DLLAPI int FCGX_Init(void);*/
SCM_DEFINE (scm_FCGX_Init,"FCGX:Init",0,0,0,
          (),
" * FCGX_Init --\n"
" *      Initialize the FCGX library.  Call in multi-threaded apps\n"
" *      before calling FCGX_Accept_r().\n"
" *      Returns #f upon success.\n"
)
{
  return (FCGX_Init()==0)?SCM_BOOL_T:SCM_BOOL_F;
}


/*DLLAPI int FCGX_OpenSocket(const char *path, int backlog);*/
SCM_DEFINE (scm_FCGX_OpenSocket,"FCGX:OpenSocket",2,0,0,
          (SCM path, SCM backlog),
" * FCGX:OpenSocket --\n"
" *  Returns the socket's file descriptor or #f on error.\n"
)
{
  int rs;

  SCM_ASSERT(
	  (SCM_NIMP(path) && (SCM_STRINGP(path))), 
	  path, SCM_ARG1, s_scm_FCGX_OpenSocket);
  SCM_ASSERT((SCM_IMP(backlog) && SCM_INUMP(backlog)),
	  backlog, SCM_ARG2, s_scm_FCGX_OpenSocket);

  rs=FCGX_OpenSocket(
		                 SCM_CHARS(path),
						 SCM_INUM(backlog));

  return (rs==-1)?SCM_BOOL_F:SCM_MAKINUM(rs);
}
/*DLLAPI int FCGX_InitRequest(FCGX_Request *request, int sock, int flags);*/
SCM_DEFINE (scm_FCGX_InitRequest,"FCGX:InitRequest!",3,0,0,
          (SCM request, SCM sock, SCM flags),
" * FCGX:InitRequest! --\n"
" * 	sock is a file descriptor returned by FCGX_OpenSocket() or 0 (default).\n"
" * 	The only supported flag at this time is FCGI_FAIL_ON_INTR.\n"
" * 	Returns #t upon success.\n"
)
{
  FCGX_Request *req;
  int ret;

  SCM_ASSERT((SCM_FCGX_REQUESTP(request)), request, SCM_ARG1, s_scm_FCGX_InitRequest);
  SCM_ASSERT((SCM_IMP(sock) && SCM_INUMP(sock)), sock, SCM_ARG2, s_scm_FCGX_InitRequest);
  SCM_ASSERT((SCM_IMP(flags) && SCM_INUMP(flags)), flags, SCM_ARG3, s_scm_FCGX_InitRequest);

  req=SCM_FCGX_REQUEST(request);

  ret=FCGX_InitRequest(req,SCM_INUM(sock),SCM_INUM(flags));

  return SCM_BOOL(ret==0);
}
/*DLLAPI int FCGX_Accept_r(FCGX_Request *request);*/
SCM_DEFINE (scm_FCGX_Accept_r,"FCGX:Accept_r!",1,0,0,
          (SCM request),
" * FCGX:Accept_r --\n"
" *      Accept a new request (multi-thread safe).  Be sure to call\n"
" * 	FCGX_Init() first.\n"
" * Results: #t on success. Possible reason of fail is  \n"
" *             library not initialized or socket error  \n"
" *             Sorry, for this stupidness. It's FCXG developers'\n"
" *             fault I can't improve. (Seems like they are working\n"
" *             for Microsoft or are Soviet car developers) \n"
" * Side effects:\n"
" *      Finishes the request accepted by (and frees any\n"
" *      storage allocated by) the previous call to FCGX_Accept.\n"
" *      Creates input, output, and error streams and\n"
" *      assigns them to *in, *out, and *err respectively.\n"
" *      Creates a parameters data structure to be accessed\n"
" *      via getenv(3) (if assigned to environ) or by FCGX_GetParam\n"
" *      and assigns it to *envp.\n"
" *      DO NOT retain pointers to the envp array or any strings\n"
" *      contained in it (e.g. to the result of calling FCGX_GetParam),\n"
" *      since these will be freed by the next call to FCGX_Finish\n"
" *      or FCGX_Accept.\n"
)
{
  SCM_ASSERT((SCM_FCGX_REQUESTP(request)), request, SCM_ARG1, s_scm_FCGX_Accept_r);

  /*-9998 not initialized*/
  /*-9999 or so, claim to FCGX developers*/
  return SCM_BOOL(0==FCGX_Accept_r(SCM_FCGX_REQUEST(request)));
}

/*DLLAPI void FCGX_Finish_r(FCGX_Request *request);*/
SCM_DEFINE (scm_FCGX_Finish_r,"FCGX:Finish_r",1,0,0,
          (SCM request),
" * FCGX:Finish_r --\n"
" *      Finish the request (multi-thread safe).\n"
" * Side effects:\n"
" *      Finishes the request accepted by (and frees any\n"
" *      storage allocated by) the previous call to FCGX_Accept.\n"
" *      DO NOT retain pointers to the envp array or any strings\n"
" *      contained in it (e.g. to the result of calling FCGX_GetParam),\n"
" *      since these will be freed by the next call to FCGX_Finish\n"
" *      or FCGX_Accept.\n"
)
{
  SCM_ASSERT((SCM_FCGX_REQUESTP(request)), request, SCM_ARG1, s_scm_FCGX_Finish_r);
  FCGX_Finish_r(SCM_FCGX_REQUEST(request));
  return (SCM_UNDEFINED);
}
static SCM
make_env_assoc(FCGX_ParamArray envp)
{
	SCM env=SCM_EOL;
	SCM cenv=SCM_EOL;
	char **p=envp;
	while (*p){
	  char *val;
	  SCM pair;

	  val=strchr(*p,'=');
	  if (val){
		pair=scm_cons(scm_makfromstr(*p,val-*p,0),scm_makfrom0str(val+1));
		if (env==SCM_EOL){
		  env=scm_cons(pair,SCM_EOL);
		  cenv=env;}
		else{
		  SCM new_pair=scm_cons(pair,SCM_EOL);
		  SCM_SETCDR(cenv,new_pair);
		  cenv=new_pair;
		}
	  }
	  ++p;
	}
	return env;
}
/*DLLAPI int FCGX_Accept( FCGX_Stream **in, FCGX_Stream **out, FCGX_Stream **err, FCGX_ParamArray *envp);*/
          /*;(SCM in, SCM out, SCM err, SCM envp),*/
SCM_DEFINE (scm_FCGX_Accept,"FCGX:Accept",0,0,0,
	(),
" * FCGX:Accept --\n"
" *      Accept a new request (NOT multi-thread safe).\n"
" * Results: (in out err env) or #f\n"
" * Side effects:\n"
" *      Finishes the request accepted by (and frees any\n"
" *      storage allocated by) the previous call to FCGX_Accept.\n"
" *      Creates input, output, and error streams and\n"
" *      assigns them to *in, *out, and *err respectively.\n"
" *      Creates a parameters data structure to be accessed\n"
" *      via getenv(3) (if assigned to environ) or by FCGX_GetParam\n"
" *      and assigns it to *envp.\n"
" *      DO NOT retain pointers to the envp array or any strings\n"
" *      contained in it (e.g. to the result of calling FCGX_GetParam),\n"
" *      since these will be freed by the next call to FCGX_Finish\n"
" *      or FCGX_Accept.\n"
)
{
  int ret;
  FCGX_ParamArray envp;
  FCGX_Stream* in;
  FCGX_Stream* out;
  FCGX_Stream* err;
  SCM ret_list;

  ret=FCGX_Accept(&in,&out,&err,&envp);
  
  if (0==ret){
	SCM i,o,e;

	SCM_DEFER_INTS;
	SCM_NEWSMOB(i,tagFCGX_Stream,in);
	SCM_NEWSMOB(o,tagFCGX_Stream,out);
	SCM_NEWSMOB(e,tagFCGX_Stream,err);
	ret_list=SCM_LIST4(i,o,e,make_env_assoc(envp));
	SCM_ALLOW_INTS;

	return ret_list;
  }

  return SCM_BOOL_F;
}

/*DLLAPI void FCGX_Finish(void);*/
SCM_DEFINE (scm_FCGX_Finish,"FCGX:Finish",0,0,0,
          (),
" * FCGX:Finish --\n"
" *      Finish the current request (NOT multi-thread safe).\n"
" * Side effects:\n"
" *      Finishes the request accepted by (and frees any\n"
" *      storage allocated by) the previous call to FCGX_Accept.\n"
" *      DO NOT retain pointers to the envp array or any strings\n"
" *      contained in it (e.g. to the result of calling FCGX_GetParam),\n"
" *      since these will be freed by the next call to FCGX_Finish\n"
" *      or FCGX_Accept.\n"
)
{
  FCGX_Finish();
  return (SCM_UNDEFINED);
}
/*DLLAPI int FCGX_StartFilterData(FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_StartFilterData,"FCGX:StartFilterData",1,0,0,
          (SCM stream),
" * FCGX:StartFilterData --\n"
" *      stream is an input stream for a FCGI_FILTER request.\n"
" *      stream is positioned at EOF on FCGI_STDIN.\n"
" *      Repositions stream to the start of FCGI_DATA.\n"
" *      If the preconditions are not met (e.g. FCGI_STDIN has not\n"
" *      been read to EOF) sets the stream error code to\n"
" *      FCGX_CALL_SEQ_ERROR.\n"
" * Results:\n"
" *      0 for a normal return, < 0 for error\n"
)
{

  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_StartFilterData);

  return SCM_BOOL(FCGX_StartFilterData(SCM_FCGX_STREAM(stream)));
}
/*DLLAPI void FCGX_SetExitStatus(int status, FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_SetExitStatus,"FCGX:SetExitStatus",2,0,0,
          (SCM status, SCM stream),
" * FCGX:SetExitStatus --\n"
" *      Sets the exit status for stream's request. The exit status\n"
" *      is the status code the request would have exited with, had\n"
" *      the request been run as a CGI program.  You can call\n"
" *      SetExitStatus several times during a request; the last call\n"
" *      before the request ends determines the value.\n"
)
{
  SCM_ASSERT((SCM_IMP(status) && SCM_INUMP(status)), status, SCM_ARG1, s_scm_FCGX_SetExitStatus);
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG2, s_scm_FCGX_SetExitStatus);
  FCGX_SetExitStatus(SCM_INUM(status),SCM_FCGX_STREAM(stream));
  return (SCM_UNDEFINED);
}
#if 0
/*DLLAPI char *FCGX_GetParam(const char *name, FCGX_ParamArray envp);*/
SCM_DEFINE (scm_*FCGX_GetParam,"*FCGX_GetParam",2,0,0,
          (SCM name, SCM envp),
" * FCGX:GetParam -- obtain value of FCGI parameter in environment\n"
" * Results:\n"
" *      environment envp.  Caller must not mutate the result\n"
" *      or retain it past the end of this request.\n"
)
{
  SCM_ASSERT((SCM_NIMP(name) && (SCM_STRINGP(name))), name, SCM_ARG1, s_scm_*FCGX_GetParam);
  SCM_ASSERT((SCM_NIMP(envp)), envp, SCM_ARG2, s_scm_*FCGX_GetParam);
);
  return (SCM_MAKE_CHAR());
}
#endif

/*DLLAPI int FCGX_GetChar(FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_GetChar,"FCGX:GetChar",1,0,0,
          (SCM stream),
" * FCGX:GetChar --\n"
" *      Reads a byte from the input stream and returns it.\n"
" * Results: char or eof-object\n"
)
{
  int ret;

  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_GetChar);
  ret=FCGX_GetChar(SCM_FCGX_STREAM(stream));

  return (ret==EOF)?SCM_EOF_VAL:SCM_MAKE_CHAR(ret);
}
/*DLLAPI int FCGX_UnGetChar(int c, FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_UnGetChar,"FCGX:UnGetChar",2,0,0,
          (SCM c, SCM stream),
" * FCGX:UnGetChar --\n"
" *      Pushes back the character c onto the input stream.  One\n"
" *      character of pushback is guaranteed once a character\n"
" *      has been read.  No pushback is possible for EOF.\n"
" * Results:\n"
)
{
  int ret;
  SCM_ASSERT((SCM_IMP(c) && SCM_CHARP(c)), c, SCM_ARG1, s_scm_FCGX_UnGetChar);
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG2, s_scm_FCGX_UnGetChar);
  ret=FCGX_UnGetChar(SCM_CHAR(c),SCM_FCGX_STREAM(stream));
  return (ret==EOF)?SCM_EOF_VAL:SCM_MAKE_CHAR(ret);
}
/*DLLAPI int FCGX_GetStr(char *str, int n, FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_GetStr,"FCGX:GetStr",2,0,0,
          (SCM n, SCM stream),
" * FCGX:GetStr --\n"
" *      Reads up to n consecutive bytes from the input stream\n"
" *      into the string.  Performs no interpretation\n"
" *      of the input bytes.\n"
" * Results:\n"
" *      the end of input has been reached.\n"
)
{
  //SCM_ASSERT((SCM_NIMP(str) && (SCM_STRINGP(str))), str, SCM_ARG1, s_scm_FCGX_GetStr);
  char *buf;
  SCM ret;
  int count;
  int nn;

  SCM_ASSERT((SCM_IMP(n) && SCM_INUMP(n)), n, SCM_ARG1, s_scm_FCGX_GetStr);
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG2, s_scm_FCGX_GetStr);

  nn=SCM_INUM(n);
  /*TODO: calloc?*/
  buf=scm_must_malloc(nn+1,s_scm_FCGX_GetStr);
  count=FCGX_GetStr(buf,nn,SCM_FCGX_STREAM(stream));

  ret=(count==0 && nn>0)?SCM_EOF_VAL:scm_makfromstr(buf,count,0);
  free(buf);

  return ret;
}
/*DLLAPI char *FCGX_GetLine(char *str, int n, FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_GetLine,"FCGX:GetLine",2,0,0,
          (SCM n, SCM stream),
" * FCGX:GetLine --\n"
" *      Reads up to n-1 consecutive bytes from the input stream\n"
" *      into the character array str.  Stops before n-1 bytes\n"
" *      have been read if '\n' or EOF is read.  The terminating '\n'\n"
" *      is copied to str.  After copying the last byte into str,\n"
" *      stores a '\0' terminator.\n"
" * Results:\n"
" *      str otherwise.\n"
)
{
  char *buf;
  char *rets; 
  SCM ret;
  int nn;

  SCM_ASSERT((SCM_IMP(n) && SCM_INUMP(n)), n, SCM_ARG1, s_scm_FCGX_GetLine);
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG2, s_scm_FCGX_GetLine);

  nn=SCM_INUM(n);
  /*TODO: calloc?*/
  buf=scm_must_malloc(nn,s_scm_FCGX_GetLine);
  rets=FCGX_GetLine(buf,nn,SCM_FCGX_STREAM(stream));
  ret=(!rets)?SCM_EOF_VAL:scm_makfrom0str(rets);
  free(buf);

  return ret;
}
/*DLLAPI  int FCGX_HasSeenEOF(FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_HasSeenEOF,"FCGX:HasSeenEOF",1,0,0,
          (SCM stream),
" * FCGX:HasSeenEOF --\n"
" *      Returns EOF if end-of-file has been detected while reading\n"
" *      from stream; otherwise returns 0.\n"
" *      Note that FCGX_HasSeenEOF(s) may return 0, yet an immediately\n"
" *      following FCGX_GetChar(s) may return EOF.  This function, like\n"
" *      the standard C stdio function feof, does not provide the\n"
" *      ability to peek ahead.\n"
" * Results:\n"
)
{
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_HasSeenEOF);
  return SCM_BOOL(FCGX_HasSeenEOF(SCM_FCGX_STREAM(stream))==EOF);
}
/*DLLAPI int FCGX_PutChar(int c, FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_PutChar,"FCGX:PutChar",2,0,0,
          (SCM c, SCM stream),
" * FCGX:PutChar --\n"
" *      Writes a byte to the output stream.\n"
" * Results: char or #f on error\n"
)
{
  SCM_ASSERT((SCM_IMP(c) && SCM_CHARP(c)), c, SCM_ARG1, s_scm_FCGX_PutChar);
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG2, s_scm_FCGX_PutChar);

  return (EOF==FCGX_PutChar(SCM_CHAR(c),SCM_FCGX_STREAM(stream)))?SCM_BOOL_F:c;
}
/*DLLAPI int FCGX_PutStr(const char *str, int n, FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_PutStr,"FCGX:PutStr",3,0,0,
          (SCM str, SCM n, SCM stream),
" * FCGX:PutStr --\n"
" *      Writes n consecutive bytes from the character array str\n"
" *      into the output stream.  Performs no interpretation\n"
" *      of the output bytes.\n"
" * Results:\n"
" *      Number of bytes written (n) for normal return,\n"
" *     #f if an error occurred.\n"
)
{
  int ret;

  SCM_ASSERT((SCM_NIMP(str) && (SCM_STRINGP(str))), str, SCM_ARG1, s_scm_FCGX_PutStr);
  SCM_ASSERT((SCM_IMP(n) && SCM_INUMP(n)), n, SCM_ARG2, s_scm_FCGX_PutStr);
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG3, s_scm_FCGX_PutStr);

  ret=FCGX_PutStr(SCM_CHARS(str),SCM_INUM(n),SCM_FCGX_STREAM(stream));
  return (EOF==ret)?SCM_BOOL_F:SCM_MAKINUM(ret);
}
/*DLLAPI int FCGX_PutS(const char *str, FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_PutS,"FCGX:PutS",2,0,0,
          (SCM str, SCM stream),
" * FCGX:PutS --\n"
" *      Writes a null-terminated character string to the output stream.\n"
" *      In deed only simulates narive FCGX_PutS by FCGX_PutStr couse of\n"
" *      guile always know length of sring.\n"
" * Results:\n"
" *      number of bytes written for normal return,\n"
" *      #f if an error occurred.\n"
)
{
  int ret;
  SCM_ASSERT((SCM_NIMP(str) && (SCM_STRINGP(str))), str, SCM_ARG1, s_scm_FCGX_PutS);
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG2, s_scm_FCGX_PutS);

  //ret=FCGX_PutS(SCM_CHARS(str),SCM_FCGX_STREAM(stream));
  ret=FCGX_PutStr(SCM_CHARS(str),SCM_LENGTH(str),SCM_FCGX_STREAM(stream));
  return (EOF==ret)?SCM_BOOL_F:SCM_MAKINUM(ret);
}
#if 0
/*TODO: implement */
/*DLLAPI int FCGX_FPrintF(FCGX_Stream *stream, const char *format, ...);*/
SCM_DEFINE (scm_FCGX_PrintF,"FCGX:PrintF",2,0,1,
          (SCM stream, SCM format, SCM rest),
" * FCGX:PrintF --\n"
" *      Performs printf-style output formatting and writes the results\n"
" *      to the output stream.\n"
" * Results:\n"
" *      number of bytes written for normal return,\n"
" *      EOF (-1) if an error occurred.\n"
)
{
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_PrintF);
  SCM_ASSERT((SCM_NIMP(format) && (SCM_STRINGP(format))), format, SCM_ARG2, s_scm_FCGX_PrintF);
  return (SCM_MAKINUM(FCGX_FPrintF(SCM_FCGX_STREAM(stream),
		  SCM_CHARS(format))));
}

/*DLLAPI int FCGX_VFPrintF(FCGX_Stream *stream, const char *format, va_list arg);*/
SCM_DEFINE (scm_FCGX_VFPrintF,"FCGX:VFPrintF",2,0,1,
          (SCM stream, SCM format, SCM arg),
""
)
{
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_VFPrintF);
  SCM_ASSERT((SCM_NIMP(format) && (SCM_STRINGP(format))), format, SCM_ARG2, s_scm_FCGX_VFPrintF);
  SCM_ASSERT(, arg, SCM_ARG3, s_scm_FCGX_VFPrintF);
);
  return (SCM_MAKINUM());
}
#endif

/*DLLAPI int FCGX_FFlush(FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_FFlush,"FCGX:FFlush",1,0,0,
          (SCM stream),
" * FCGX:FFlush --\n"
" *      Flushes any buffered output.\n"
" *      Server-push is a legitimate application of FCGX_FFlush.\n"
" *      Otherwise, FCGX_FFlush is not very useful, since FCGX_Accept\n"
" *      does it implicitly.  Calling FCGX_FFlush in non-push applications\n"
" *      results in extra writes and therefore reduces performance.\n"
" *      AWARE: stream can be disposed by Finish(_r). If you get\n"
" *             segfault -- you are here!.\n"
" * Results:\n"
" *      #f if an error occurred.\n"
)
{
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_FFlush);

  return (-1==FCGX_FFlush(SCM_FCGX_STREAM(stream)));
}
/*DLLAPI int FCGX_FClose(FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_FClose,"FCGX:FClose",1,0,0,
          (SCM stream),
" * FCGX:FClose --\n"
" *      Closes the stream.  For writers, flushes any buffered\n"
" *      output.\n"
" *      Close is not a very useful operation since FCGX_Accept\n"
" *      does it implicitly.  Closing the out stream before the\n"
" *      err stream results in an extra write if there's nothing\n"
" *      in the err stream, and therefore reduces performance.\n"
" *      AWARE: stream can be disposed by Finish(_r). If you get\n"
" *             segfault -- you are here!.\n"
" * Results:\n"
" *      EOF (-1) if an error occurred.\n"
)
{
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_FClose);

  return (-1==FCGX_FClose(SCM_FCGX_STREAM(stream)));
}
/*DLLAPI int FCGX_GetError(FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_GetError,"FCGX:GetError",1,0,0,
          (SCM stream),
" * FCGX:GetError --\n"
" *      Return the stream error code.  0 means no error, > 0\n"
" *      is an errno(2) error, < 0 is an FastCGI error.\n"
" *      AWARE: stream can be disposed by Finish(_r). If you get\n"
" *             segfault -- you are here!.\n"
)
{
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_GetError);
  return SCM_MAKINUM(FCGX_GetError(SCM_FCGX_STREAM(stream)));
}
/*DLLAPI void FCGX_ClearError(FCGX_Stream *stream);*/
SCM_DEFINE (scm_FCGX_ClearError,"FCGX:ClearError",1,0,0,
          (SCM stream),
" * FCGX:ClearError --\n"
" *      Clear the stream error code and end-of-file indication.\n"
" *      AWARE: stream can be disposed by Finish(_r). If you get\n"
" *             segfault -- you are here!.\n"
)
{
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_ClearError);
  FCGX_ClearError(SCM_FCGX_STREAM(stream));
  return (SCM_UNDEFINED);
}
/****************************************************************************/
/*                    fcgi: interface                                       */
/****************************************************************************/
static int acceptCalled = 0;
static int IsCGI = 0;
static SCM saved_in=SCM_BOOL_F;
static SCM saved_out=SCM_BOOL_F;
static SCM saved_err=SCM_BOOL_F;
static SCM start_of_accept=SCM_BOOL_F;
extern char** environ;
static char** saved_environ=NULL;

static SCM scm_fcgi_make_port(SCM st);
static SCM scm_fcgi_finish();

inline SCM
fcgi_make_stream(FCGX_Stream* stream)
{
  SCM_RETURN_NEWSMOB(tagFCGX_Stream,stream);
}
static int
fcgi_fill_input (SCM_PORT)
{
//  scm_port *pt=SCM_PTAB_ENTRY (port);
//;  FCGX_Stream *s=SCM_FCGX_STREAM?(port);
  return EOF;
}
	
static void
fcgi_write (SCM port, const void *data, size_t size)
{
  FCGX_PutStr(data,size,SCM_FCGX_STREAM(SCM_STREAM(port)));
}

SCM_DEFINE (scm_fcgi_accept,"fcgi:accept",0,0,0,
          (),"")
{
  int ret;
  FCGX_ParamArray envp;
  FCGX_Stream* in;
  FCGX_Stream* out;
  FCGX_Stream* err;

  if (0==acceptCalled){
/* First call */
    IsCGI = FCGX_IsCGI();
	if (IsCGI){
	  acceptCalled=1;
	  return SCM_EOF_VAL;
	}
	saved_in=scm_current_input_port();
	saved_out=scm_current_output_port();
	saved_err=scm_current_error_port();
	scm_protect_object(saved_in);
	scm_protect_object(saved_out);
	scm_protect_object(saved_err);
	saved_environ=environ;
	atexit(&scm_fcgi_finish);
  }else if (IsCGI){
	/*emulate CGI's one nly pass*/
	acceptCalled=0;
	return SCM_BOOL_F;
  };

  ret=FCGX_Accept(&in,&out,&err,&envp);
  if (SCM_NFALSEP(start_of_accept))
	scm_unprotect_object(start_of_accept);
  start_of_accept=scm_gettimeofday();
  scm_protect_object(start_of_accept);

  if (0==ret){
	SCM_DEFER_INTS;
	scm_set_current_input_port(scm_fcgi_make_port(fcgi_make_stream(in)));
	scm_set_current_output_port(scm_fcgi_make_port(fcgi_make_stream(out)));
	scm_set_current_error_port(scm_fcgi_make_port(fcgi_make_stream(err)));
	environ=envp;
	SCM_ALLOW_INTS;
	++acceptCalled;
	return SCM_BOOL_T;
  }else{
	return SCM_BOOL_F;
  }
};

SCM_DEFINE (scm_fcgi_accept_count,"fcgi:accept-count",0,0,0,
          (),"")
{
  return SCM_MAKINUM(acceptCalled);
};

SCM_DEFINE (scm_fcgi_accept_start,"fcgi:accept-start",0,0,0,
          (),"")
{
  return start_of_accept;
};

SCM_DEFINE (scm_fcgi_finish,"fcgi:finish",0,0,0,
          (),"")
{
    if(!acceptCalled || IsCGI) {
        return SCM_UNDEFINED;
    }
    FCGX_Finish();
	scm_set_current_input_port(saved_in);
	scm_set_current_output_port(saved_out);
	scm_set_current_error_port(saved_err);
	scm_unprotect_object(saved_in);
	scm_unprotect_object(saved_out);
	scm_unprotect_object(saved_err);
    saved_in=SCM_BOOL_F;
    saved_out=SCM_BOOL_F;
    saved_err=SCM_BOOL_F;
    environ = saved_environ;
	acceptCalled=0;
	return SCM_UNDEFINED;
}

static SCM
fcgi_port_equalp(SCM p1,SCM p2){
  return (SCM_FCGX_STREAM(SCM_STREAM(p1))==
	  SCM_FCGX_STREAM(SCM_STREAM(p2)))?SCM_BOOL_T:SCM_BOOL_F;
}

static void
fcgi_port_flush(SCM port){
  FCGX_FFlush(SCM_FCGX_STREAM(SCM_STREAM(port)));
}

static SCM
fcgi_port_mark(SCM port){
  scm_gc_mark (SCM_STREAM(port));
  return SCM_BOOL_F;
}

static int
fcgi_port_close(SCM port){
  return FCGX_FClose(SCM_FCGX_STREAM(SCM_STREAM(port)));
};

/*****************************************************************************/
/*             Guile Interface                                               */
/*****************************************************************************/
static int 
print_stream(SCM smob,SCM port,scm_print_state *state)
{
  FCGX_Stream *p=SCM_FCGX_STREAM(smob);
  char buf[256];
  sprintf(buf,"#<FCGX_Stream %p>",p);
  scm_puts(buf,port);
  return 1;
}

static scm_sizet
free_stream(SCM stream){
  /*
   * It's done by Request
  FCGX_Stream *p=SCM_FCGX_STREAM(stream);
  FCGX_FClose(p);
  free(p);
  */
  return sizeof(FCGX_Stream);
}

SCM_DEFINE (scm_FCGX_err,"FCGX:Request-err",1,0,0,
	(SCM request),
" * FCGX:Stream --\n"
" * Returns FCGX Stream object representing ouput stream.\n"
		   )
{
  FCGX_Request *p=SCM_FCGX_REQUEST(request);
  SCM_RETURN_NEWSMOB(tagFCGX_Stream,p->err);
}

SCM_DEFINE (scm_FCGX_in,"FCGX:Request-in",1,0,0,
	(SCM request),
" * FCGX:Stream --\n"
" * Returns FCGX Stream object representing ouput stream.\n"
		   )
{
  FCGX_Request *p=SCM_FCGX_REQUEST(request);
  SCM_RETURN_NEWSMOB(tagFCGX_Stream,p->in);
}

SCM_DEFINE (scm_FCGX_out,"FCGX:Request-out",1,0,0,
	(SCM request),
" * FCGX:Stream --\n"
" * Returns FCGX Stream object representing ouput stream.\n"
		   )
{
  FCGX_Request *p=SCM_FCGX_REQUEST(request);
  SCM_RETURN_NEWSMOB(tagFCGX_Stream,p->out);
}

SCM_DEFINE (scm_FCGX_env,"FCGX:Request-env",1,0,0,
	(SCM request),
" * FCGX:Stream --\n"
" * Returns FCGX Stream object representing ouput stream.\n"
		   )
{
  FCGX_Request *p=SCM_FCGX_REQUEST(request);
  return (make_env_assoc(p->envp));
}

static int 
print_request(SCM smob,SCM port,scm_print_state *state)
{
  char buf[256];
  sprintf(buf,"#<FCGX_Request %p>",SCM_FCGX_REQUEST(smob));
  scm_puts(buf,port);
  return 1;
}

static scm_sizet
free_request(SCM request){
  FCGX_Request *p=SCM_FCGX_REQUEST(request);
  FCGX_Finish_r(p);
  free(p);
  return sizeof(FCGX_Request);
}

SCM_DEFINE (scm_FCGX_Make_request,"FCGX:Request",0,2,0,
	(SCM sock,SCM flags),
" * FCGX:Request --\n"
" * Make new FCGX Request object.\n"
		   )
{
  SCM code;
  FCGX_Request *req;
  SCM ret;
  
  req=scm_must_malloc(sizeof(FCGX_Request),s_scm_FCGX_Make_request);
  memset(req,0,sizeof(FCGX_Request));

  SCM_NEWSMOB(ret,tagFCGX_Request,NULL);
  code=scm_FCGX_InitRequest(ret,sock,flags);
  if (SCM_FALSEP(code)){
	free(req);
	return SCM_BOOL_F;
  }else{
	return ret;
  }
}

SCM_DEFINE (scm_fcgi_make_port,"fcgi:make-port",1,0,0,
          (SCM stream),"")
{
  long mode_bits;
  FCGX_Stream *str;
  scm_port *pt;
  SCM port;
  SCM_ASSERT((SCM_FCGX_STREAMP(stream)), stream, SCM_ARG1, s_scm_FCGX_ClearError);
  str=SCM_FCGX_STREAM(stream);
  mode_bits=SCM_BUF0|((str->isReader)?SCM_RDNG:SCM_WRTNG)|((str->isClosed)?0:SCM_OPN);

  SCM_NEWCELL(port);
  SCM_DEFER_INTS;
  pt=scm_add_to_port_table (port);
  SCM_SETPTAB_ENTRY (port,pt);
  SCM_SETCAR(port,fcgi_ptype|mode_bits);
  SCM_SETSTREAM (port,stream);
  SCM_ALLOW_INTS;

  return port;
}


SCM_DEFINE (scm_fcgi_make_stream,"fcgi:make-stream",1,0,0,
          (SCM port),"")
{
  SCM_ASSERT(SCM_PORTP(port) && 
	  ((SCM_CAR(port) & fcgi_ptype)==fcgi_ptype),
	  port,
	  SCM_ARG1,
	  s_scm_fcgi_make_stream);

  return SCM_STREAM(port);
}

void
init_fcgi(void)
{
  tagFCGX_Stream=scm_make_smob_type("FCGX:Stream",sizeof(FCGX_Stream)); 
  tagFCGX_Request=scm_make_smob_type("FCGX:Request",sizeof(FCGX_Request)); 
  scm_set_smob_print(tagFCGX_Stream,print_stream);
  scm_set_smob_free(tagFCGX_Stream,free_stream);
  scm_set_smob_print(tagFCGX_Request,print_request);
  scm_set_smob_free(tagFCGX_Request,free_request);

  fcgi_ptype=scm_make_port_type("fcgi:port",fcgi_fill_input,fcgi_write);
  scm_set_port_mark(fcgi_ptype,fcgi_port_mark);
  scm_set_port_equalp(fcgi_ptype,fcgi_port_equalp);
  scm_set_port_flush(fcgi_ptype,fcgi_port_flush);
  scm_set_port_close(fcgi_ptype,fcgi_port_close);
#include "guile-fcgi.x"
}

static void
inner_main (void *closure, int argc, char **argv)
{
  /* module initializations would go here */
  init_fcgi();
  scm_shell (argc, argv);
}

int
main (int argc, char **argv)
{
  scm_boot_guile (argc, argv, inner_main, 0);
  return 0; /* never reached */
}
