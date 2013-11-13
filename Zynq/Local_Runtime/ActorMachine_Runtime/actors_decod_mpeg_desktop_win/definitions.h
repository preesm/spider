
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <stdio.h>
#include "decoderTypes.h"

void init_vlc_tables_P(REVERSE_EVENT *DCT3D_P);
void init_vlc_tables_I(REVERSE_EVENT *DCT3D_I);
void readm4v_init(char *file_path);
void Init_SDL(int edge,int frame_width,int frame_height);

void CloseSDLDisplay();
void readm4v(char *file_path, const int nb_octets_to_read,const int new_buff_i,uchar *buffer);
void readm4v_double_buffering(const int nb_octets_to_read,const int pos_octet,const int new_buff_i,const unsigned char *buffer_new,const unsigned char *buffer_state,unsigned char *buffer_out,int *position_o );
void VideoObjectLayer(const int pos_i,int length_bit,const uchar *data,struct_VOLsimple *VOLsimple,uchar *vop_complexity,int *pos_o,int *vol_width,int *vol_height);
int VideoObjectPlaneI(int position_vol,const unsigned char *data,const struct_VOLsimple *VOLsimple,const unsigned char *vop_complexity,int *pos,struct_VOP *VOP,int  *vop_coding_type );

void decode_I_frame(const unsigned char *data,const struct_VOLsimple *VOLsimple,const int position,struct_VOP *VOP,REVERSE_EVENT *DCT3D_I,int *pos_fin_vlc,int *address,unsigned char *Lum,unsigned char *Cb,unsigned char *Cr,int *keyframes);
void decode_P_frame(const unsigned char *data,const struct_VOLsimple *VOLsimple,const int position,struct_VOP *VOP,REVERSE_EVENT *DCT3D_I,REVERSE_EVENT *DCT3D_P,unsigned char *stock_mb_type_P,int *pos_fin_vlc,int *address,unsigned char *Lum,unsigned char *Cb,unsigned char *Cr,int *key_frames,vector *save_mv);
void New_buffer(const int nb_octets_to_read,const int pos,int *new_buff_o,int *pos_octet);


void extract_picture(int xsize,int ysize,int edge,int Crop,uchar *img_luma_in,uchar *img_Cb_in,uchar *img_Cr_in,int address_pic,uchar *img_luma_out,uchar *img_Cb_out,uchar *img_Cr_out);
void SDL_Display(int edge,int frame_width,int frame_height,unsigned char *Y,unsigned char *V,unsigned char *U);






void openFile(char *file_path, FILE* pFile);
void readUpToNextStartCode(FILE * pFile, uchar* buffer, uint* nbBytesRead);

void VisualObjectLayer(stream* buffer);


//typedef struct DisplayData{
//	int	XDIM, YDIM;
//	uchar	*Y, *U, *V;
//} DisplayData;
//


#endif /* DEFINITIONS_H_ */
