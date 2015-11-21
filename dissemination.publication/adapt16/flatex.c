/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 * flatex.c -
 *    Flatten a latex file into a single file, by explicitly including 
 *  the files inclued by \include and \input commands. Also, if bibtex is
 *  beeing used, then includes the .bbl file into the resulting file. Thus,
 *  creating a stand alone latex file that can be emailed to someone else. 
 *
 * Compile    : gcc -o flatex flatex.c
 * Tested on  : Linux + gcc   
 * By         : Sariel Har-Peled 
 * Email      : sariel@math.tau.ac.il
 * WEB Page   : http://www.math.tau.ac.il/~sariel/flatex.html
 * Status     : You can do whatever you like with this program. please 
 *              email me bugs & suggestions. 
 *
 * To do      : Add support to the includeonly command.
 *-----------------------------------------------------------------------
 * FLATEX  1.21, 1994, 1996, by Sariel Har-Peled.
 *
 * flatex - create a single latex file with no include/inputs
 *
 *       flatex [-v] [-x FileName] [files]
 *               -v      Verbose, display file structure.
 *               -x      Unflatex: extract files from archive
 *               -q      Quiet mode. Cleaner output but -x can not be used.
 *               -b      Do not insert bibiliography file(.bbl)
 * 
 * Flatex page: http://www.math.tau.ac.il/~sariel/flatex.html
 *-----------------------------------------------------------------------
 * History:
 *    26/8/96, 1.21
 *         Fixed bug with includegraphics command.
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include  <stdlib.h>
#include  <stdio.h>
#include  <string.h>
#include  <math.h>
#include  <ctype.h>


/*======================================================================
 * Static constants.
\*======================================================================*/
#define  LINE_SIZE 1000
#define  FALSE     0
#define  TRUE      1
#define  USE_ARGUMENT( X )   ((void)X)


/*======================================================================
 * Types
\*======================================================================*/
typedef struct {
  char    verbose;
  char    fBibInsert, fQuiet;
  int     cSpecialInputLevel;
  char    szFullName[ LINE_SIZE ];
}  structFlags;


/*======================================================================
 * Static prototypes.
\*======================================================================*/
static void        flatIt( FILE         * flOut,
                           char         * szInName,
		           int            level,
	                   structFlags  * pFlags );
static void        replaceExt( char     * str, char    * ext );


/*======================================================================
 * Start of Code
\*======================================================================*/


static void        spacesByLevel( int     level )
{
  while  ( level > 0 ) {
    printf( "    " );
    level--;
  }
}


static void          printHelp( void )
{
    printf( "flatex - create a single latex file with no include/inputs\n" );
    printf( "\n\tflatex [-v] [-x FileName] [files]\n" );
    printf( "\t\t-v\tVerbose, display file structure.\n" );
    printf( "\t\t-x\tUnflatex: extract files from archive\n" );
    printf( "\t\t-q\tQuiet mode. Cleaner output but -x can not be used.\n" );
    printf( "\t\t-b\tDo not insert bibiliography file(.bbl)\n" );
    printf( "\nFlatex page: http://www.math.tau.ac.il/~sariel/flatex.html\n" );
    printf( "\n" );
}


static void      * myMalloc( unsigned int      size )
{
    void         * ptr;

    ptr = malloc( size );
    if  ( ptr == NULL ) {
        fprintf( stderr, "Not enough memory" );
        exit( -1 );
    }

    return    ptr;
}


static void        handleIncludeCommand( char         * line,
                                         char         * lpszInclude,
                                         FILE         * flOut,
                                         int            level,
                                         structFlags  * pFlags )
{
    char       * lpszBrace, * lpszName, * lpszEndBrace;
    char         ch, fInput = 0;

    lpszBrace = NULL;

    if  ( strncmp( lpszInclude, "\\input", 6 ) == 0 ) {
        lpszBrace = lpszInclude + 6;
	fInput = 1;
    } else
        if  ( strncmp( lpszInclude, "\\include", 8 ) == 0 ) {
            lpszBrace = lpszInclude + 8;
        }

    ch = *lpszInclude;
    *lpszInclude = 0;
    fputs( line, flOut );
    *lpszInclude = ch;

    lpszEndBrace = strchr( lpszBrace, '}' );
    if  ( *lpszBrace != '{'  ||  lpszEndBrace == NULL ) {
        fprintf( stderr, "ERROR: Expected brace not found.\n\n\tline:%s\n",
                 line );
        exit( -1 );
    }

    *lpszEndBrace = 0;
    lpszName = (char *)myMalloc( LINE_SIZE );
    strcpy( lpszName, lpszBrace + 1 );
    if  ( ! fInput )  
        replaceExt( lpszName, ".tex" );

    flatIt( flOut, lpszName, level + 1, pFlags );

    lpszEndBrace++;
    while  ( *lpszEndBrace ) {
        *line++ = *lpszEndBrace++;
    }
    *line = 0;

    free( lpszName );
}


static char        isBefore( char    * lpszA, char    * lpszB )
{
    if  ( lpszB == NULL )
        return  TRUE;
    if  ( lpszA == NULL )
        return  FALSE;

    if  ( (int)( lpszA -lpszB ) < 0 ) {
        return   TRUE;
     }
    return   FALSE;
}


static FILE      * fopenTex( char         * file, 
                             char         * mode )
{
    FILE     * fl;

    fl = fopen( file, mode );
    if  ( fl != NULL ) 
      return fl;
    
    replaceExt( file, ".tex" );
    fl = fopen( file, mode );

    return  fl;
}


static char      isTexFileExists( char        * file )
{
    FILE     * fl;

    fl = fopenTex( file, "rt" );
    if  ( fl != NULL ) {
      fclose( fl );
      return  1;
    }

    return  0;
}


static void        addTexExt( char    * file )
{
    FILE      * fl;

    fl = fopenTex( file, "rt");
    if  ( fl != NULL )
      fclose( fl );
}


static char     is_str_prefix( char    * str, char * prefix )
{
    int len;

    if  ( str == NULL  ||  prefix == NULL )
        return  0;

    len = strlen( prefix );
    
    return  (strncmp( str, prefix, len ) == 0);
}


static void        flatIt( FILE         * flOut,
                           char         * pSzInName,
		           int            level,
                           structFlags  * pFlags )
{
    FILE      * flIn;
    char      * str, * lpszInput, * lpszInclude, * line, * lpszRem, *inc;
    char      * lpszLine, * lpszRemark, * lpszBib, * lpszBibStyle;
    char      * lpszNewCommand, * lpszName;
    char        cont;
    char        repFlag;
    char        szInName[ 100 ];
    char        fInclude;

    strcpy( szInName, pSzInName );

    addTexExt( szInName );
    if  ( ! pFlags->fQuiet ) 
      fprintf( flOut, "%%%cflatex input: [%s]\n", 
               pFlags->cSpecialInputLevel > 0? '*' : ' ', 
               szInName ); 
    if  ( pFlags->verbose ) {
        printf( "\t" );
        spacesByLevel( level );
        printf( "%s\n", szInName );
    }

    line = (char *)myMalloc( LINE_SIZE );
    lpszLine = (char *)myMalloc( LINE_SIZE );
    lpszRemark = (char *)myMalloc( LINE_SIZE );

    flIn = fopenTex( szInName, "rt" );
    if   ( flIn == NULL ) {
        fprintf( stderr, "Unable to open file: %s\n", szInName );
        exit( -1 );
    }

    *lpszRemark = 0;
    while  ( ! feof( flIn ) ) {
        str = fgets( line, LINE_SIZE, flIn );
        if  ( str == NULL )
            break;

        fInclude = FALSE;

        strcpy( lpszLine, line );

        lpszRem = strchr( line, '%' );
        if  ( lpszRem != NULL ) {
            strcpy( lpszRemark, lpszRem );
            *lpszRem = 0;
        }

        do  {
            cont = 0;
            lpszInput = strstr( line,   "\\input" );

            lpszBib = strstr( line, "\\bibliography" );
            lpszBibStyle = strstr( line, "\\bibliographystyle" );

            if  ( pFlags->fBibInsert  &&
                  ( lpszBib != NULL  ||  lpszBibStyle != NULL ) ) {
                lpszName = (char *)myMalloc( LINE_SIZE );
                
                strcpy( lpszName, lpszLine );
                strcpy( lpszLine, pFlags->fQuiet? "%" : "%FLATEX-REM:" );
                strcat( lpszLine, lpszName );

                if  ( lpszBibStyle != NULL ) {
                    strcpy( lpszName, pFlags->szFullName );
                    replaceExt( lpszName, ".bbl" );
 
                    pFlags->cSpecialInputLevel++;
                    flatIt( flOut, lpszName, level + 1, pFlags );
                    pFlags->cSpecialInputLevel--;
                    
                    if  ( pFlags->verbose ) {
                        printf( "\t" );
                        spacesByLevel( level + 1 );
                        printf( "(Bibiliography)\n" );
                    }
                }
                break;
            }
 
            inc = line;
            do { 
                repFlag = 0;
                lpszInclude = strstr( inc, "\\include" );

                if  ( is_str_prefix( lpszInclude, "\\includeversion" )  
                      ||  is_str_prefix( lpszInclude, 
                                         "\\includegraphics"  ) ) {
                    repFlag = 1;
                    inc = lpszInclude + 1;
                    continue;
                }
                
                if  ( is_str_prefix( lpszInclude, "\\includeonly" ) ) {
                    fprintf( stderr, "WARNING: \"\\includeonly\" command "
                             "ignored\n" );
                    inc = lpszInclude + 1;
                    repFlag = 1;
                    continue;
                }
                if  ( lpszInclude != NULL  &&  isalpha( lpszInclude[ 8 ] ) ) {
                    fprintf( stderr,
                      "\nWarning: include-like(?) command ignored"
                             " at line:\n\t%s", lpszLine );
                    inc = lpszInclude + 1;
                    repFlag = 1;
                    continue;
                }
            }  while  ( repFlag );
 
            if  ( isBefore( lpszInput, lpszInclude ) )
                lpszInclude = lpszInput;

            if  ( lpszInclude != NULL ) {
                lpszNewCommand = strstr( line, "\\newcommand" );
                if (  lpszNewCommand == NULL )  {
                    handleIncludeCommand( line, lpszInclude, flOut, level,
                                          pFlags );
                    cont = 1;
                    fInclude = TRUE;
                }
            }
        } while  ( cont );
        if  ( fInclude ) {
            strcat( line, lpszRemark );            
            fputs( line, flOut );
        } else
            fputs( lpszLine, flOut );
    }

    fclose( flIn );
    fputs( "\n", flOut );

    if  ( ! pFlags->fQuiet ) 
      fprintf( flOut, "%% flatex input end: [%s]\n", szInName );

    free( line );
    free( lpszLine );
    free( lpszRemark );
}


static void        replaceExt( char     * str, char    * ext )
{
    int        len, ind;

    len = strlen( str );
    ind = len - 1;
    while  ( ind >= 0  &&  str[ ind ] != '.'  &&  str[ ind ] != '\\' &&
             str[ ind ] != '/' )
        ind--;

    if  ( ind >= 0  &&  str[ ind ] == '.' ) {
        str[ ind ] = 0;
    }

    strcat( str, ext );
}

static char         strCmpPrefixAndCopy( char      * line, 
                                         char      * str,
                                         char      * outName )
{
  char       * pos, * pPreLine;

  pPreLine = line;

  pos = strstr( line, str );
  if  ( pos == NULL )
    return  0;

  line = pos + strlen( str );
  strcpy( outName, line );
  pos = strchr( outName, ']' );

  if  ( pos == NULL ) {
    fprintf( stderr, "Error encountered in line: [%s]", pPreLine );
    exit( -1 );
  }
  *pos = 0;

  return  1;
}


static void        writeFile( FILE     * flIn, 
                              char     * pOutName, 
                              int        level )
{
  FILE       * flOut;
  char       * lpszLine;
  char         line[ LINE_SIZE ], outName[ LINE_SIZE ];
  char         flag;

  outName[ 0 ] = 0;

  if  ( pOutName == NULL ) {
    flOut = NULL;
    printf( "Scanning for flatex archive start...\n" );
  } else {
    flOut = fopen( pOutName, "wt" );
    if  ( flOut == NULL ) {
      fprintf( stderr, "Unable to open file: %s", pOutName );
      exit( -1 );
    }
    spacesByLevel( level );
    printf( "[%s]\n", pOutName );
  }

  do {
    lpszLine = fgets( line, LINE_SIZE, flIn );
    if  ( lpszLine == NULL ) 
      break;

    flag = strCmpPrefixAndCopy( line, "% flatex input end: [", outName ); 
    if  ( flag ) {
      if  ( flOut == NULL ) {
        fprintf( stderr, "Something is wrong!!!!\n" );
        exit( -1 );
      }
      //spacesByLevel( level );
      //     printf( "/\n" );
      //printf( "Writing [%s] done\n", outName );
      break;
    }

    flag = strCmpPrefixAndCopy( line, "% flatex input: [", outName ); 
    if  ( flag ) {
        writeFile( flIn, outName, level + 1 );
        if  ( flOut != NULL ) 
          fprintf( flOut, "\\input{%s}\n", outName );
    } else {
        flag = strCmpPrefixAndCopy( line, "%*flatex input: [", outName ); 
        if  ( flag ) {
            writeFile( flIn, outName, level + 1 );
        } else {
            if  ( flOut != NULL ) {
                if  ( strncmp( line, "%FLATEX-REM:", 12 ) == 0 ) 
                    fputs( line + 12, flOut );
                else
                    fputs( line, flOut );
            }
        }
    }
  }  while  ( ! feof( flIn ) );
  
  if  ( flOut != NULL ) 
    fclose( flOut );  
}


static void        flatOutFile( char         * fileName, 
                                structFlags  * pFlags )
{
  FILE       * flIn;

  USE_ARGUMENT( pFlags );

  flIn = fopen( fileName, "rt" );
  if  ( flIn == NULL ) {
    fprintf( stderr, "Unable to open file: %s", fileName );
    exit( -1 );
  }
  
  writeFile( flIn, NULL, 0 );

  fclose( flIn );
}


static void        flatFile( char         * fileName, 
                             structFlags  * pFlags )
{
    char             * szInName, * szOutName;
    int                inLen;
    FILE             * flOut;

    szInName = (char *)myMalloc( LINE_SIZE );
    szOutName = (char *)myMalloc( LINE_SIZE );

    strcpy( szInName, fileName );
    if  ( ! isTexFileExists( szInName ) ) {
      fprintf( stderr, "--Unable to open file: [%s]\n", fileName );
      exit( -1 );
    }

    inLen = strlen( szInName );
    if  ( inLen < 4  ||  ( szInName[ inLen ] != '.'  &&
            strcmp( szInName + inLen - 4, ".tex" ) != 0 ) ) {
        strcat( szInName, ".tex" );
    }

    printf( "input file: [%s]\n", szInName );

    strcpy( pFlags->szFullName, szInName );

    strcpy( szOutName, szInName );
    replaceExt( szOutName, ".flt" );

    flOut = fopen( szOutName, "wt" );
    if   ( flOut == NULL ) {
        fprintf( stderr, "Unable to open file: %s", szOutName );
        exit( -1 );
    }

    flatIt( flOut, szInName, 0, pFlags );

    fclose( flOut );
    
    printf( "\n\tFile: \"%s\" generated\n", szOutName );
}


static char    isFlag( char     * str, char       ch )
{
    if  ( str[ 0 ] == '-'  &&
          ( str[ 1 ] == ch  || str[ 1 ] == toupper( ch ) )
          &&  ( str[ 2 ] == 0 ) )
      return  TRUE;

    return  FALSE;
}


int       main( int    argc, char   * argv[] )
{
    int                   ind;
    structFlags           sFlags;

    printf( "FLATEX  1.21, 1994, 1996, by Sariel Har-Peled.\n\n" );
    if   ( argc == 1 )
        printHelp();

    sFlags.verbose = FALSE;
    sFlags.fBibInsert = TRUE;
    sFlags.cSpecialInputLevel = 0;
    *sFlags.szFullName = 0;
    sFlags.fQuiet = FALSE;

    for   ( ind = 1; ind < argc; ind++ ) {
        if   ( isFlag( argv[ ind ], 'v' ) ) {
            sFlags.verbose = TRUE;
            continue;
        }
        if   ( isFlag( argv[ ind ], 'b' ) ) {
            sFlags.fBibInsert = FALSE;
            continue;
        }
        if   ( isFlag( argv[ ind ], 'q' ) ) {
            sFlags.fQuiet = TRUE;
            continue;
        }
        if   ( isFlag( argv[ ind ], 'x' ) ) {
            flatOutFile( argv[ ind + 1 ], &sFlags );
            ind++;
            continue;
        }

        flatFile( argv[ ind ], &sFlags );
    }
    return   0;
}

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
 *
 * flatex.c - End of File
\*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

