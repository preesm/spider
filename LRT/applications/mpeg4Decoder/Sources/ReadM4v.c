/*****************************************************************************
   *
   *  MPEG4DSP developped in IETR image lab
   *
   *
   *
   *              Jean-Francois NEZAN <jnezan@insa-rennes.Fr>
   *              Mickael Raulet <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/gro/IMA/th3/temp/index.htm
   *
   *
   *
   *
   *  This program is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
   *
   *  This program is distributed in the hope that it will be useful,
   *  but WITHOUT ANY WARRANTY; without even the implied warranty of
   *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   *  GNU General Public License for more details.
   *
   *  You should have received a copy of the GNU General Public License
   *  along with this program; if not, write to the Free Software
   *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
   *
   * $Id$
   *
   **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--- Global Variables ---*/
FILE        *ptfile = NULL;
static int  file_size ;
int         Filesize (FILE *f) ;
static int  nb_octet_already_read = 0 ;

void readm4v_init (char *file_path)
{
	if ((ptfile = fopen(file_path, "rb")) == NULL)
	{
      printf("Cannot open m4v_file file '%s' \n", file_path);
      exit( -1);
	}

    file_size = Filesize(ptfile);
    fseek(ptfile, nb_octet_already_read, 0);
}

void readm4v (char *file_path, const int nb_octets_to_read, const int new_buff_i, unsigned char *buffer )
{
	if(ptfile == NULL) readm4v_init(file_path);
    if ( new_buff_i == 1 ) {
        if ( file_size < nb_octets_to_read ) {
            //====RQ : cette partie n'est pas encore terminée======
            //il faut compléter le buffer jusqu'à nb_octets_to_read
            {
                int i ;
                for ( i = 0 ; i < nb_octets_to_read ; i++ ) {
                    buffer [i] = 0 ;
                }
            }
            fread(buffer, sizeof(char), file_size - nb_octet_already_read, ptfile);
            fseek(ptfile, 0, 0);
        } else {
            if ( file_size - nb_octet_already_read > nb_octets_to_read ) {
                fread(buffer, sizeof(char), nb_octets_to_read, ptfile);
                nb_octet_already_read += (unsigned long)(nb_octets_to_read);
            } else {
                fread(buffer, sizeof(char), file_size - nb_octet_already_read, ptfile);
                fseek(ptfile, 0, 0);
                nb_octet_already_read = file_size - nb_octet_already_read ;
                fread(buffer + nb_octet_already_read, sizeof(char), nb_octets_to_read - nb_octet_already_read, ptfile);
                nb_octet_already_read = (unsigned long)nb_octets_to_read - nb_octet_already_read ;
            }
        }
        //printf("nb_octet_already_read  %d\n", nb_octet_already_read);
    }
}

int Filesize ( FILE *f )
{
    long    oldfp, fsize ;
    
    oldfp = ftell(f);
    if ( oldfp < 0L ) 
        return -1 ;
    if ( 0 != fseek(f, 0, SEEK_END) ) 
        return -1 ;
    fsize = ftell(f);
    if ( fsize < 0 ) 
        return -1 ;
    if ( 0 != fseek(f, oldfp, SEEK_SET) ) 
        return -1 ;
    return fsize ;
}
