



/* 2013-09-03 17:45:24, application Decoder_HIGH_level_IP, processor PC_decod1 type=pentiumOS */

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#define uchar unsigned char
#define ushort unsigned short
#define ulong unsigned long
#define uint unsigned int
#define prec_synchro int
#define stream unsigned char
#define image_type unsigned char
#define dpb_type unsigned char



#include <mpeg.h>
#include <lrt_prototypes.h>
#include <hwQueues.h>

int Zero_PC_decod1_out;
/* `alloc_(REVERSE_EVENT,init_vlc_tables_P_PC_decod1_DCT3D_P,4096)' */
REVERSE_EVENT init_vlc_tables_P_PC_decod1_DCT3D_P[4096];
/* `alloc_(REVERSE_EVENT,init_vlc_tables_I_PC_decod1_DCT3D_I,4096)' */
REVERSE_EVENT init_vlc_tables_I_PC_decod1_DCT3D_I[4096];


// Buffer containing the entire video bitstream.
#define BITSTREAM_BUFFER_SIZE				161887
uchar Readm4v_new_buffer[BITSTREAM_BUFFER_SIZE];

// Doubled buffer
int readm4v_double_buffering_buffer_out_size = BITSTREAM_BUFFER_SIZE * 2;
stream readm4v_double_buffering_buffer_out[BITSTREAM_BUFFER_SIZE * 2];


/* `alloc_(int,readm4v_double_buffering_pos_o,1)' */
//int readm4v_double_buffering_pos_o[1];
/* `alloc_(stream,readm4v_double_buffering_buffer_out,829440)' *//* `alloc_(int,readm4v_double_buffering_buffer_out_size,1)' */

/* `alloc_(int,VideoObjectLayer_ysize_o,1)' */
int VideoObjectLayer_ysize_o[1];
/* `alloc_(int,VideoObjectLayer_xsize_o,1)' */
int VideoObjectLayer_xsize_o[1];
/* `alloc_(int,VideoObjectLayer_pos_o,1)' */
int VideoObjectLayer_pos_o[1];
/* `alloc_(uchar,VideoObjectLayer_vop_complexity,5)' */
uchar VideoObjectLayer_vop_complexity[5];
/* `alloc_(struct_VOLsimple,VideoObjectLayer_VOLsimple,1)' */
struct_VOLsimple VideoObjectLayer_VOLsimple[1];
/* `alloc_(int,VideoObjectPlane_vop_coding_type,1)' */
int VideoObjectPlane_vop_coding_type[1];
/* `alloc_(struct_VOP,VideoObjectPlane_VOP,1)' */
struct_VOP VideoObjectPlane_VOP[1];
/* `alloc_(int,VideoObjectPlane_pos,1)' */
int VideoObjectPlane_pos[1];
/* `alloc_(stream,Choix_I_P_haut_niveau_CondI9_o,829440)' *//* `alloc_(int,Choix_I_P_haut_niveau_CondI9_o_size,1)' */
int Choix_I_P_haut_niveau_CondI9_o_size[1];
stream Choix_I_P_haut_niveau_CondI9_o[829440];
/* `alloc_(struct_VOP,Choix_I_P_haut_niveau_CondI8_o,1)' */
struct_VOP Choix_I_P_haut_niveau_CondI8_o[1];
/* `alloc_(struct_VOLsimple,Choix_I_P_haut_niveau_CondI7_o,1)' */
struct_VOLsimple Choix_I_P_haut_niveau_CondI7_o[1];
/* `alloc_(vector,Choix_I_P_haut_niveau_CondI5_o,1620)' */
vector Choix_I_P_haut_niveau_CondI5_o[1620];
/* `alloc_(int,Choix_I_P_haut_niveau_CondI4_o,1)' */
int Choix_I_P_haut_niveau_CondI4_o[1];
/* `alloc_(int,Choix_I_P_haut_niveau_CondI3_o,2)' */
int Choix_I_P_haut_niveau_CondI3_o[2];
/* `alloc_(uchar,Choix_I_P_haut_niveau_CondI2_o,1371648)' */
uchar Choix_I_P_haut_niveau_CondI2_o[1371648];
/* `alloc_(uchar,Choix_I_P_haut_niveau_CondI1_o,342912)' */
uchar Choix_I_P_haut_niveau_CondI1_o[342912];
/* `alloc_(uchar,Choix_I_P_haut_niveau_CondI0_o,342912)' */
uchar Choix_I_P_haut_niveau_CondI0_o[342912];
/* `alloc_(int,Choix_I_P_haut_niveau_img_2_o,1)' */
int Choix_I_P_haut_niveau_img_2_o[1];
/* `alloc_(int,Choix_I_P_haut_niveau_img_4_o,1)' */
int Choix_I_P_haut_niveau_img_4_o[1];
/* `alloc_(int,Choix_I_P_haut_niveau_img_1_o,1)' */
int Choix_I_P_haut_niveau_img_1_o[1];
/* `alloc_(int,Choix_I_P_haut_niveau_img_0_o,1)' */
int Choix_I_P_haut_niveau_img_0_o[1];
/* `alloc_(int,Choix_I_P_haut_niveau_decode_P_frame_address,1)' */
int Choix_I_P_haut_niveau_decode_P_frame_address[1];
/* `alloc_(int,Choix_I_P_haut_niveau_decode_P_frame_pos_fin_vlc,1)' */
int Choix_I_P_haut_niveau_decode_P_frame_pos_fin_vlc[1];
/* `alloc_(uchar,Choix_I_P_haut_niveau_decode_P_frame_stock_mb_type_P,1620)' */
uchar Choix_I_P_haut_niveau_decode_P_frame_stock_mb_type_P[1620];
/* `alloc_(int,Choix_I_P_haut_niveau_decode_I_frame_address,1)' */
int Choix_I_P_haut_niveau_decode_I_frame_address[1];
/* `alloc_(int,Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc,1)' */
int Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc[1];
/* `alloc_(int,Choix_I_P_haut_niveau_CondO6_o,1)' */
int Choix_I_P_haut_niveau_CondO6_o[1];
/* `alloc_(uchar,Choix_I_P_haut_niveau_CondO3_o,1371648)' */
uchar Choix_I_P_haut_niveau_CondO3_o[1371648];
/* `alloc_(uchar,Choix_I_P_haut_niveau_CondO2_o,342912)' */
uchar Choix_I_P_haut_niveau_CondO2_o[342912];
/* `alloc_(uchar,Choix_I_P_haut_niveau_CondO1_o,342912)' */
uchar Choix_I_P_haut_niveau_CondO1_o[342912];
/* `alloc_(int,Choix_I_P_haut_niveau_CondO0_o,1)' */
int Choix_I_P_haut_niveau_CondO0_o[1];
/* `alloc_(int,Choix_I_P_haut_niveau_CondO5_o,1)' */
int Choix_I_P_haut_niveau_CondO5_o[1];
/* `alloc_(int,Display_CondI6_o,1)' */
int Display_CondI6_o[1];
/* `alloc_(int,Display_CondI5_o,1)' */
int Display_CondI5_o[1];
/* `alloc_(uchar,Display_CondI3_o,1371648)' */
uchar Display_CondI3_o[1371648];
/* `alloc_(uchar,Display_CondI2_o,342912)' */
uchar Display_CondI2_o[342912];
/* `alloc_(uchar,Display_CondI1_o,342912)' */
uchar Display_CondI1_o[342912];
/* `alloc_(int,Display_CondI4_o,1)' */
int Display_CondI4_o[1];
/* `alloc_(vector,Choix_I_P_haut_niveau_CondO7_o,1620)' */
vector Choix_I_P_haut_niveau_CondO7_o[1620];
/* `alloc_(int,Choix_I_P_haut_niveau_CondO4_o,2)' */
int Choix_I_P_haut_niveau_CondO4_o[2];
/* `alloc_(int,New_buffer_pos_octet,1)' */
int New_buffer_pos_octet[1];
/* `alloc_(int,New_buffer_new_buff_o,1)' */
int New_buffer_new_buff_o[1];
/* `alloc_(image_type,Display_Extract_1_Image_Y_o,685832)' */
image_type Display_Extract_1_Image_Y_o[685832];
/* `alloc_(image_type,Display_Extract_Image_Y_o,685832)' */
image_type Display_Extract_Image_Y_o[685832];
/* `alloc_(stream,mem_buffer_buf,829440)' *//* `alloc_(int,mem_buffer_buf_size,1)' */
int mem_buffer_buf_size[1];
stream mem_buffer_buf[829440];
/* `alloc_(uchar,mem_Y_last_buf,1371648)' */
uchar mem_Y_last_buf[1371648];
/* `alloc_(uchar,mem_V_last_buf,342912)' */
uchar mem_V_last_buf[342912];
/* `alloc_(uchar,mem_U_last_buf,342912)' */
uchar mem_U_last_buf[342912];
/* `alloc_(vector,mem_mv_buf,1620)' */
vector mem_mv_buf[1620];
/* `alloc_(int,mem_2_int_buf,2)' */
int mem_2_int_buf[2];
/* `alloc_(int,mem_pos_octet_buf,1)' */
int mem_pos_octet_buf[1];
/* `alloc_(int,mem_new_buff_buf,1)' */
int mem_new_buff_buf[1];
/* `alias_(mem_buffer_o,mem_buffer_buf,0,829440)' */
#define mem_buffer_o (mem_buffer_buf+0)
/* `alias_(mem_buffer_o_size,mem_buffer_buf_size,0,1)' */
#define mem_buffer_o_size (mem_buffer_buf_size+0)
/* `alias_(mem_Y_last_o,mem_Y_last_buf,0,1371648)' */
#define mem_Y_last_o (mem_Y_last_buf+0)
/* `alias_(mem_V_last_o,mem_V_last_buf,0,342912)' */
#define mem_V_last_o (mem_V_last_buf+0)
/* `alias_(mem_U_last_o,mem_U_last_buf,0,342912)' */
#define mem_U_last_o (mem_U_last_buf+0)
/* `alias_(mem_mv_o,mem_mv_buf,0,1620)' */
#define mem_mv_o (mem_mv_buf+0)
/* `alias_(mem_2_int_o,mem_2_int_buf,0,2)' */
#define mem_2_int_o (mem_2_int_buf+0)
/* `alias_(mem_pos_octet_o,mem_pos_octet_buf,0,1)' */
//#define mem_pos_octet_o (mem_pos_octet_buf+0)
/* `alias_(mem_new_buff_o,mem_new_buff_buf,0,1)' */
//#define mem_new_buff_o (mem_new_buff_buf+0)

//void rt_decode_I_frame(
//		const unsigned char *data,
//		const struct_VOLsimple *VOLsimple,
//		const int position,
//		struct_VOP *VOP,
//		REVERSE_EVENT *DCT3D_I,
//		int *pos_fin_vlc,
//		int *address,
//		unsigned char *Lum,
//		unsigned char *Cb,
//		unsigned char *Cr,
//		int *keyframes);


void init_vlc_tables_P(REVERSE_EVENT *DCT3D_P);
void init_vlc_tables_I(REVERSE_EVENT *DCT3D_I);
void readm4v_init(char *file_path);
void Init_SDL(int edge,int frame_width,int frame_height);
void SDL_Display(int edge,int frame_width,int frame_height,unsigned char *Y,unsigned char *V,unsigned char *U);
void CloseSDLDisplay();
void readm4v(const int nb_octets_to_read,const int new_buff_i,uchar *buffer);
void readm4v_double_buffering(const int nb_octets_to_read,const int pos_octet,const int new_buff_i,const unsigned char *buffer_new,const unsigned char *buffer_state,unsigned char *buffer_out,int *position_o );
void VideoObjectLayer(const int pos_i,int length_bit,const uchar *data,struct_VOLsimple *VOLsimple,uchar *vop_complexity,int *pos_o,int *vol_width,int *vol_height);
int VideoObjectPlaneI(int position_vol,const unsigned char *data,const struct_VOLsimple *VOLsimple,const unsigned char *vop_complexity,int *pos,struct_VOP *VOP,int  *vop_coding_type );
static int firstframe = 0;
void decode_I_frame(const unsigned char *data,const struct_VOLsimple *VOLsimple,const int position,struct_VOP *VOP,REVERSE_EVENT *DCT3D_I,int *pos_fin_vlc,int *address,unsigned char *Lum,unsigned char *Cb,unsigned char *Cr,int *keyframes);
void decode_P_frame(const unsigned char *data,const struct_VOLsimple *VOLsimple,const int position,struct_VOP *VOP,REVERSE_EVENT *DCT3D_I,REVERSE_EVENT *DCT3D_P,unsigned char *stock_mb_type_P,int *pos_fin_vlc,int *address,unsigned char *Lum,unsigned char *Cb,unsigned char *Cr,int *key_frames,vector *save_mv);
void New_buffer(const int nb_octets_to_read,const int pos,int *new_buff_o,int *pos_octet);
void extract_picture(int xsize,int ysize,int edge,int Crop,uchar *img_luma_in,uchar *img_Cb_in,uchar *img_Cr_in,int address_pic,uchar *img_luma_out,uchar *img_Cb_out,uchar *img_Cr_out);



//#define M4V_FILE_PATH		"C:\\work\\COMPA\\decodeur_mp4\\Sequences\\SIMPLE\\P-VOP\\jvc011.m4v"
#define M4V_FILE_PATH		"C:\\work\\COMPA\\decodeur_mp4\\Sequences\\SIMPLE\\I-VOP\\san001.m4v"

typedef struct DisplayData{
	int	XDIM, YDIM;
	UINT8	*Y, *U, *V;
} DisplayData;



void init_SDL(){
	Zero_PC_decod1_out = 0;
	init_vlc_tables_P(init_vlc_tables_P_PC_decod1_DCT3D_P);
	init_vlc_tables_I(init_vlc_tables_I_PC_decod1_DCT3D_I);

	Choix_I_P_haut_niveau_CondO4_o[0]=0;
	Choix_I_P_haut_niveau_CondO4_o[1]=0;

	Init_SDL(16, 720, 576);
}


void readBitstream(){
	// Opening video file.
	readm4v_init(M4V_FILE_PATH);

	// Reading the bitstream.
	int newBuffer = 1;
	readm4v(BITSTREAM_BUFFER_SIZE, newBuffer, Readm4v_new_buffer);

	int mem_pos_octet_o = readm4v_double_buffering_buffer_out_size;
	int readm4v_double_buffering_pos_o;

	// TODO: comments...
	readm4v_double_buffering(
			BITSTREAM_BUFFER_SIZE,
			mem_pos_octet_o,
			newBuffer,
			Readm4v_new_buffer,
			(uchar *)mem_buffer_o,
			(uchar *)readm4v_double_buffering_buffer_out,
			&readm4v_double_buffering_pos_o);

	VideoObjectLayer(
			readm4v_double_buffering_pos_o,
			414720 * 8* 2,
			readm4v_double_buffering_buffer_out,
			VideoObjectLayer_VOLsimple,
			VideoObjectLayer_vop_complexity,
			VideoObjectLayer_pos_o,
			VideoObjectLayer_xsize_o,
			VideoObjectLayer_ysize_o);
}


void close_SDL(){
	CloseSDLDisplay();
}


void setVOPType(){
	VideoObjectPlaneI(
			VideoObjectLayer_pos_o[0],
			readm4v_double_buffering_buffer_out,
			VideoObjectLayer_VOLsimple,
			VideoObjectLayer_vop_complexity,
			VideoObjectPlane_pos,
			VideoObjectPlane_VOP,
			VideoObjectPlane_vop_coding_type);

	// Sending the VOP type to the global runtime.
	OS_InfoQPush_UINT32(*VideoObjectPlane_vop_coding_type);
}


void readVOP(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	VideoObjectPlaneI(
			VideoObjectLayer_pos_o[0],
			readm4v_double_buffering_buffer_out,
			VideoObjectLayer_VOLsimple,
			VideoObjectLayer_vop_complexity,
			VideoObjectPlane_pos,
			VideoObjectPlane_VOP,
			VideoObjectPlane_vop_coding_type);

	// Sending VOP data.
	write_output_fifo(action->fifo_out_id[0], sizeof(struct_VOP), (UINT8*)&VideoObjectPlane_VOP);
}


void displayVOP(){
	static UINT8 flag;
	if (flag == 0){
		init_SDL();
		flag++;
	}


	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();
	DisplayData	data;
	// Receiving values.
	read_input_fifo(action->fifo_in_id[0], sizeof(DisplayData), (UINT8*)&data);

	// Displaying only if Choix_I_P_haut_niveau_CondO6_o[0] is 1 or 2
	switch(Choix_I_P_haut_niveau_CondO6_o[0]){
	case 1:
	case 2:
		SDL_Display(16, data.XDIM, data.YDIM, data.Y, data.U, data.V);
	}
}


void decodeVOPInput(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	// Receiving VOP data.
	read_input_fifo(action->fifo_in_id[0], sizeof(struct_VOP), (UINT8*)&VideoObjectPlane_VOP);

	// Sending display data.
	write_output_fifo(action->fifo_out_id[0], sizeof(struct_VOP), (UINT8*)&VideoObjectPlane_VOP);
}

void decodeVOPSwitch(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	// Receiving VOP data.
	read_input_fifo(action->fifo_in_id[0], sizeof(struct_VOP), (UINT8*)&VideoObjectPlane_VOP);

	// Sending display data.
	write_output_fifo(action->fifo_out_id[0], sizeof(struct_VOP), (UINT8*)&VideoObjectPlane_VOP);
}

void decodeVOPI(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();
	// Receiving VOP data.
	read_input_fifo(action->fifo_in_id[0], sizeof(struct_VOP), (UINT8*)&VideoObjectPlane_VOP);






	if(firstframe==0){
		firstframe++;
		Choix_I_P_haut_niveau_img_0_o [0] = 0 ;
	}
	else{
		Choix_I_P_haut_niveau_img_0_o [0] = 1 ;
	}

	/*!
	   I-frame processing.

		\param[in] data 			: bitstream (double buffer output)
		\param[in] VOLsimple		: input VOL
		\param[in] pos_i 			: position in bitstream
		\param[in] VOP 				: input VOP
		\param[in] DCT3D_I 			: inverse VLC intra coefficients
		\param[out] pos_o			: position in bitstream after frame processing
		\param[out] address			: address in the image buffer to display
		\param[inout] Lum			: image luminance Y output
		\param[inout] Cb			: image chrominance U output
		\param[inout] Cr			: image chrominance V output
		\param[inout] keyframes		: if it is the first keyframes

	   */
	decode_I_frame(
			Choix_I_P_haut_niveau_CondI9_o,						// data
			Choix_I_P_haut_niveau_CondI7_o,						// VOLSimple
			Choix_I_P_haut_niveau_CondI4_o[0],					// pos_i
			Choix_I_P_haut_niveau_CondI8_o,						// VOP
			init_vlc_tables_I_PC_decod1_DCT3D_I,				// DCT3D_I
			Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc,	// pos_o
			Choix_I_P_haut_niveau_decode_I_frame_address,		// address
			Choix_I_P_haut_niveau_CondI2_o,						// Lum
			Choix_I_P_haut_niveau_CondI0_o,						// Cb
			Choix_I_P_haut_niveau_CondI1_o,						// Cr
			Choix_I_P_haut_niveau_CondI3_o);					// keyframes

	Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_0_o[0];
	memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
	memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
	memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
	Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_decode_I_frame_address[0];
	Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc[0];
	memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
	memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));








	DisplayData	data;

	switch (Choix_I_P_haut_niveau_CondO6_o[0]) {
	  case 1 : {
		{
//		   int XDIM = ((int *) Display_Extract_Image_Y_o)[0] = *Display_CondI5_o;
//		   int YDIM = ((int *) Display_Extract_Image_Y_o)[1] = *Display_CondI6_o;
//		   uchar *Y = Display_Extract_Image_Y_o + 8;
//		   uchar *U = Y + (XDIM + 32) * YDIM;
//		   uchar *V = U + (XDIM + 32) * YDIM/4;
			data.XDIM = ((int*)Display_Extract_Image_Y_o)[0] = Display_CondI5_o[0];
			data.YDIM = ((int*)Display_Extract_Image_Y_o)[1] = Display_CondI6_o[0];
			data.Y = Display_Extract_Image_Y_o + 8;
			data.U = data.Y + (data.XDIM + 32) * data.YDIM;
			data.V = data.U + (data.XDIM + 32) * data.YDIM/4;
		   extract_picture(
				   data.XDIM, data.YDIM,
				   16, Zero_PC_decod1_out,
				   Display_CondI3_o, Display_CondI1_o, Display_CondI2_o, Display_CondI4_o[0],
				   data.Y, data.U, data.V);
		}

		break; }

	  case 2 : {
		{
//		   int XDIM = ((int *) Display_Extract_1_Image_Y_o)[0] = *Display_CondI5_o;
//		   int YDIM = ((int *) Display_Extract_1_Image_Y_o)[1] = *Display_CondI6_o;
//		   uchar *Y = Display_Extract_1_Image_Y_o + 8;
//		   uchar *U = Y + (XDIM + 32) * YDIM;
//		   uchar *V = U + (XDIM + 32) * YDIM/4;
			data.XDIM = ((int*)Display_Extract_1_Image_Y_o)[0] = Display_CondI5_o[0];
			data.YDIM = ((int*)Display_Extract_1_Image_Y_o)[1] = Display_CondI6_o[0];
			data.Y = Display_Extract_1_Image_Y_o + 8;
			data.U = data.Y + (data.XDIM + 32) * data.YDIM;
			data.Y = data.U + (data.XDIM + 32) * data.YDIM/4;
		   extract_picture(
				   data.XDIM, data.YDIM,
				   16, Zero_PC_decod1_out,
				   Display_CondI3_o, Display_CondI1_o, Display_CondI2_o, Display_CondI4_o[0],
				   data.Y, data.U, data.V);
		}
		break; }
	}


	mem_buffer_buf_size[0] = readm4v_double_buffering_buffer_out_size;
	memcpy(mem_buffer_buf, readm4v_double_buffering_buffer_out, 829440*sizeof(stream));
	memcpy(mem_Y_last_buf, Choix_I_P_haut_niveau_CondO3_o, 1371648*sizeof(uchar));
	memcpy(mem_V_last_buf, Choix_I_P_haut_niveau_CondO2_o, 342912*sizeof(uchar));
	memcpy(mem_U_last_buf, Choix_I_P_haut_niveau_CondO1_o, 342912*sizeof(uchar));
	memcpy(mem_mv_buf, Choix_I_P_haut_niveau_CondO7_o, 1620*sizeof(vector));
	memcpy(mem_2_int_buf, Choix_I_P_haut_niveau_CondO4_o, 2*sizeof(int));
	mem_pos_octet_buf[0] = New_buffer_pos_octet[0];
	mem_new_buff_buf[0] = New_buffer_new_buff_o[0];

	switch (Choix_I_P_haut_niveau_CondO6_o[0]){
	  case 1 : {
		{
//		   int XDIM = ((unsigned int *) Display_Extract_Image_Y_o)[0];
//		   int YDIM = ((unsigned int *) Display_Extract_Image_Y_o)[1];
//		   uchar *Y = Display_Extract_Image_Y_o + 8;
//		   uchar *U = Y + (XDIM + 32) * YDIM;
//		   uchar *V = U + (XDIM + 32) * YDIM/4 ;
		   data.XDIM = ((unsigned int *) Display_Extract_Image_Y_o)[0];
		   data.YDIM = ((unsigned int *) Display_Extract_Image_Y_o)[1];
		   data.Y = Display_Extract_Image_Y_o + 8;
		   data.U = data.Y + (data.XDIM + 32) * data.YDIM;
		   data.V = data.U + (data.XDIM + 32) * data.YDIM/4 ;

//		   SDL_Display(16, XDIM, YDIM, Y, U, V);
		}

		break; }

	  case 2 : {
		{
//		   int XDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[0];
//		   int YDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[1];
//		   uchar *Y = Display_Extract_1_Image_Y_o + 8;
//		   uchar *U = Y + (XDIM + 32) * YDIM;
//		   uchar *V = U + (XDIM + 32) * YDIM/4 ;
		   data.XDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[0];
		   data.YDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[1];
		   data.Y = Display_Extract_1_Image_Y_o + 8;
		   data.U = data.Y + (data.XDIM + 32) * data.YDIM;
		   data.V = data.U + (data.XDIM + 32) * data.YDIM/4 ;

//		   SDL_Display(16, XDIM, YDIM, Y, U, V);
		}

		break; }
	}





	// Sending display data.
	write_output_fifo(action->fifo_out_id[0], sizeof(DisplayData), (UINT8*)&data);
}

void decodeVOPSelect(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();
	DisplayData	data;

	// Receiving values.
	read_input_fifo(action->fifo_in_id[0], sizeof(DisplayData), (UINT8*)&data);

	// Sending display data.
	write_output_fifo(action->fifo_out_id[0], sizeof(DisplayData), (UINT8*)&data);
}

void decodeVOPOutput(){

	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();
	DisplayData	data;

	// Receiving values.
	read_input_fifo(action->fifo_in_id[0], sizeof(DisplayData), (UINT8*)&data);

	// Sending display data.
	write_output_fifo(action->fifo_out_id[0], sizeof(DisplayData), (UINT8*)&data);
}








void decodeVOP(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	// Receiving VOP data.
	read_input_fifo(action->fifo_in_id[0], sizeof(struct_VOP), (UINT8*)&VideoObjectPlane_VOP);


	Choix_I_P_haut_niveau_CondI9_o_size[0] = readm4v_double_buffering_buffer_out_size;
	memcpy(Choix_I_P_haut_niveau_CondI9_o, readm4v_double_buffering_buffer_out, 829440*sizeof(stream));
	Choix_I_P_haut_niveau_CondI8_o[0] = VideoObjectPlane_VOP[0];
	Choix_I_P_haut_niveau_CondI7_o[0] = VideoObjectLayer_VOLsimple[0];
	memcpy(Choix_I_P_haut_niveau_CondI5_o, mem_mv_o, 1620*sizeof(vector));
	Choix_I_P_haut_niveau_CondI4_o[0] = VideoObjectPlane_pos[0];
	memcpy(Choix_I_P_haut_niveau_CondI3_o, mem_2_int_o, 2*sizeof(int));
	memcpy(Choix_I_P_haut_niveau_CondI2_o, mem_Y_last_o, 1371648*sizeof(uchar));
	memcpy(Choix_I_P_haut_niveau_CondI1_o, mem_V_last_o, 342912*sizeof(uchar));
	memcpy(Choix_I_P_haut_niveau_CondI0_o, mem_U_last_o, 342912*sizeof(uchar));

	/* switch VOP coding type */
	switch (VideoObjectPlane_vop_coding_type[0]) {

	  case 0 : {
		if(firstframe==0){
			firstframe++;
			Choix_I_P_haut_niveau_img_0_o [0] = 0 ;
		}
		else{
			Choix_I_P_haut_niveau_img_0_o [0] = 1 ;
		}

		/*!
		   I-frame processing.

		   	\param[in] data 			: bitstream (double buffer output)
		   	\param[in] VOLsimple		: input VOL
		   	\param[in] pos_i 			: position in bitstream
		   	\param[in] VOP 				: input VOP
		   	\param[in] DCT3D_I 			: inverse VLC intra coefficients
		   	\param[out] pos_o			: position in bitstream after frame processing
		    \param[out] address			: address in the image buffer to display
		   	\param[inout] Lum			: image luminance Y output
		   	\param[inout] Cb			: image chrominance U output
		   	\param[inout] Cr			: image chrominance V output
		   	\param[inout] keyframes		: if it is the first keyframes

		   */
		decode_I_frame(
				Choix_I_P_haut_niveau_CondI9_o,						// data
				Choix_I_P_haut_niveau_CondI7_o,						// VOLSimple
				Choix_I_P_haut_niveau_CondI4_o[0],					// pos_i
				Choix_I_P_haut_niveau_CondI8_o,						// VOP
				init_vlc_tables_I_PC_decod1_DCT3D_I,				// DCT3D_I
				Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc,	// pos_o
				Choix_I_P_haut_niveau_decode_I_frame_address,		// address
				Choix_I_P_haut_niveau_CondI2_o,						// Lum
				Choix_I_P_haut_niveau_CondI0_o,						// Cb
				Choix_I_P_haut_niveau_CondI1_o,						// Cr
				Choix_I_P_haut_niveau_CondI3_o);					// keyframes

		Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_0_o[0];
		memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
		Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_decode_I_frame_address[0];
		Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc[0];
		memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
		memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));
		break; }

	  case 1 : {
		Choix_I_P_haut_niveau_img_1_o [0] = 1 ;

		decode_P_frame(
				Choix_I_P_haut_niveau_CondI9_o,
				Choix_I_P_haut_niveau_CondI7_o,
				Choix_I_P_haut_niveau_CondI4_o[0],
				Choix_I_P_haut_niveau_CondI8_o,
				init_vlc_tables_I_PC_decod1_DCT3D_I,
				init_vlc_tables_P_PC_decod1_DCT3D_P,
				Choix_I_P_haut_niveau_decode_P_frame_stock_mb_type_P,
				Choix_I_P_haut_niveau_decode_P_frame_pos_fin_vlc,
				Choix_I_P_haut_niveau_decode_P_frame_address,
				Choix_I_P_haut_niveau_CondI2_o,
				Choix_I_P_haut_niveau_CondI0_o,
				Choix_I_P_haut_niveau_CondI1_o,
				Choix_I_P_haut_niveau_CondI3_o,
				Choix_I_P_haut_niveau_CondI5_o);

		Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_1_o[0];
		memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
		Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_decode_P_frame_address[0];
		Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_decode_P_frame_pos_fin_vlc[0];
		memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
		memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));
		break; }

	  case 2 : {
		Choix_I_P_haut_niveau_img_2_o [0] = 0 ;
		Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_2_o[0];
		memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
		Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_img_2_o[0];
		Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_CondI4_o[0];
		memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
		memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));
		break; }

	  case 4 : {
		Choix_I_P_haut_niveau_img_4_o [0] = 0 ;
		Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_4_o[0];
		memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
		Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_img_4_o[0];
		Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_CondI4_o[0];
		memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
		memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));
		break; }
	}

	Display_CondI6_o[0] = VideoObjectLayer_ysize_o[0];
	Display_CondI5_o[0] = VideoObjectLayer_xsize_o[0];
	memcpy(Display_CondI3_o, Choix_I_P_haut_niveau_CondO3_o, 1371648*sizeof(uchar));
	memcpy(Display_CondI2_o, Choix_I_P_haut_niveau_CondO2_o, 342912*sizeof(uchar));
	memcpy(Display_CondI1_o, Choix_I_P_haut_niveau_CondO1_o, 342912*sizeof(uchar));
	Display_CondI4_o[0] = Choix_I_P_haut_niveau_CondO0_o[0];

	New_buffer(414720, Choix_I_P_haut_niveau_CondO5_o[0], New_buffer_new_buff_o, New_buffer_pos_octet);


	DisplayData data; // Data to be sent to the display actor.

	switch (Choix_I_P_haut_niveau_CondO6_o[0]) {
	  case 1 : {
		{
//		   int XDIM = ((int *) Display_Extract_Image_Y_o)[0] = *Display_CondI5_o;
//		   int YDIM = ((int *) Display_Extract_Image_Y_o)[1] = *Display_CondI6_o;
//		   uchar *Y = Display_Extract_Image_Y_o + 8;
//		   uchar *U = Y + (XDIM + 32) * YDIM;
//		   uchar *V = U + (XDIM + 32) * YDIM/4;
			data.XDIM = ((int*)Display_Extract_Image_Y_o)[0] = Display_CondI5_o[0];
			data.YDIM = ((int*)Display_Extract_Image_Y_o)[1] = Display_CondI6_o[0];
			data.Y = Display_Extract_Image_Y_o + 8;
			data.U = data.Y + (data.XDIM + 32) * data.YDIM;
			data.V = data.U + (data.XDIM + 32) * data.YDIM/4;
		   extract_picture(
				   data.XDIM, data.YDIM,
				   16, Zero_PC_decod1_out,
				   Display_CondI3_o, Display_CondI1_o, Display_CondI2_o, Display_CondI4_o[0],
				   data.Y, data.U, data.V);
		}

		break; }

	  case 2 : {
		{
//		   int XDIM = ((int *) Display_Extract_1_Image_Y_o)[0] = *Display_CondI5_o;
//		   int YDIM = ((int *) Display_Extract_1_Image_Y_o)[1] = *Display_CondI6_o;
//		   uchar *Y = Display_Extract_1_Image_Y_o + 8;
//		   uchar *U = Y + (XDIM + 32) * YDIM;
//		   uchar *V = U + (XDIM + 32) * YDIM/4;
			data.XDIM = ((int*)Display_Extract_1_Image_Y_o)[0] = Display_CondI5_o[0];
			data.YDIM = ((int*)Display_Extract_1_Image_Y_o)[1] = Display_CondI6_o[0];
			data.Y = Display_Extract_1_Image_Y_o + 8;
			data.U = data.Y + (data.XDIM + 32) * data.YDIM;
			data.Y = data.U + (data.XDIM + 32) * data.YDIM/4;
		   extract_picture(
				   data.XDIM, data.YDIM,
				   16, Zero_PC_decod1_out,
				   Display_CondI3_o, Display_CondI1_o, Display_CondI2_o, Display_CondI4_o[0],
				   data.Y, data.U, data.V);
		}
		break; }
	}

	mem_buffer_buf_size[0] = readm4v_double_buffering_buffer_out_size;
	memcpy(mem_buffer_buf, readm4v_double_buffering_buffer_out, 829440*sizeof(stream));
	memcpy(mem_Y_last_buf, Choix_I_P_haut_niveau_CondO3_o, 1371648*sizeof(uchar));
	memcpy(mem_V_last_buf, Choix_I_P_haut_niveau_CondO2_o, 342912*sizeof(uchar));
	memcpy(mem_U_last_buf, Choix_I_P_haut_niveau_CondO1_o, 342912*sizeof(uchar));
	memcpy(mem_mv_buf, Choix_I_P_haut_niveau_CondO7_o, 1620*sizeof(vector));
	memcpy(mem_2_int_buf, Choix_I_P_haut_niveau_CondO4_o, 2*sizeof(int));
	mem_pos_octet_buf[0] = New_buffer_pos_octet[0];
	mem_new_buff_buf[0] = New_buffer_new_buff_o[0];

	switch (Choix_I_P_haut_niveau_CondO6_o[0]){
	  case 1 : {
		{
//		   int XDIM = ((unsigned int *) Display_Extract_Image_Y_o)[0];
//		   int YDIM = ((unsigned int *) Display_Extract_Image_Y_o)[1];
//		   uchar *Y = Display_Extract_Image_Y_o + 8;
//		   uchar *U = Y + (XDIM + 32) * YDIM;
//		   uchar *V = U + (XDIM + 32) * YDIM/4 ;
		   data.XDIM = ((unsigned int *) Display_Extract_Image_Y_o)[0];
		   data.YDIM = ((unsigned int *) Display_Extract_Image_Y_o)[1];
		   data.Y = Display_Extract_Image_Y_o + 8;
		   data.U = data.Y + (data.XDIM + 32) * data.YDIM;
		   data.V = data.U + (data.XDIM + 32) * data.YDIM/4 ;

//		   SDL_Display(16, XDIM, YDIM, Y, U, V);
		}

		break; }

	  case 2 : {
		{
//		   int XDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[0];
//		   int YDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[1];
//		   uchar *Y = Display_Extract_1_Image_Y_o + 8;
//		   uchar *U = Y + (XDIM + 32) * YDIM;
//		   uchar *V = U + (XDIM + 32) * YDIM/4 ;
		   data.XDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[0];
		   data.YDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[1];
		   data.Y = Display_Extract_1_Image_Y_o + 8;
		   data.U = data.Y + (data.XDIM + 32) * data.YDIM;
		   data.V = data.U + (data.XDIM + 32) * data.YDIM/4 ;

//		   SDL_Display(16, XDIM, YDIM, Y, U, V);
		}

		break; }
	}

	// Sending display data.
	write_output_fifo(action->fifo_out_id[0], sizeof(DisplayData), (UINT8*)&data);
}


void decoder(){
	Zero_PC_decod1_out = 0;
	init_vlc_tables_P(init_vlc_tables_P_PC_decod1_DCT3D_P);
	init_vlc_tables_I(init_vlc_tables_I_PC_decod1_DCT3D_I);
	readm4v_init(M4V_FILE_PATH);
	readm4v_double_buffering_buffer_out_size=414720*2;
	Choix_I_P_haut_niveau_CondO4_o[0]=0;
	Choix_I_P_haut_niveau_CondO4_o[1]=0;
	*mem_pos_octet_o=829440;
	*mem_new_buff_o=1;

//	int ii;
//	for(ii = 0; ii < 100; ii++)
	for (;;)
	{
		readm4v(414720, mem_new_buff_o[0], Readm4v_new_buffer);

		readm4v_double_buffering(
				414720,
				mem_pos_octet_o[0],
				mem_new_buff_o[0],
				Readm4v_new_buffer,
				(uchar *)mem_buffer_o,
				(uchar *)readm4v_double_buffering_buffer_out,
				readm4v_double_buffering_pos_o);

		VideoObjectLayer(
				readm4v_double_buffering_pos_o[0],
				414720 * 8* 2,
				readm4v_double_buffering_buffer_out,
				VideoObjectLayer_VOLsimple,
				VideoObjectLayer_vop_complexity,
				VideoObjectLayer_pos_o,
				VideoObjectLayer_xsize_o,
				VideoObjectLayer_ysize_o);

		VideoObjectPlaneI(
				VideoObjectLayer_pos_o[0],
				readm4v_double_buffering_buffer_out,
				VideoObjectLayer_VOLsimple,
				VideoObjectLayer_vop_complexity,
				VideoObjectPlane_pos,
				VideoObjectPlane_VOP,
				VideoObjectPlane_vop_coding_type);

		Choix_I_P_haut_niveau_CondI9_o_size[0] = readm4v_double_buffering_buffer_out_size;
		memcpy(Choix_I_P_haut_niveau_CondI9_o, readm4v_double_buffering_buffer_out, 829440*sizeof(stream));
		Choix_I_P_haut_niveau_CondI8_o[0] = VideoObjectPlane_VOP[0];
		Choix_I_P_haut_niveau_CondI7_o[0] = VideoObjectLayer_VOLsimple[0];
		memcpy(Choix_I_P_haut_niveau_CondI5_o, mem_mv_o, 1620*sizeof(vector));
		Choix_I_P_haut_niveau_CondI4_o[0] = VideoObjectPlane_pos[0];
		memcpy(Choix_I_P_haut_niveau_CondI3_o, mem_2_int_o, 2*sizeof(int));
		memcpy(Choix_I_P_haut_niveau_CondI2_o, mem_Y_last_o, 1371648*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondI1_o, mem_V_last_o, 342912*sizeof(uchar));
		memcpy(Choix_I_P_haut_niveau_CondI0_o, mem_U_last_o, 342912*sizeof(uchar));



		/* switch VOP coding type */
		switch (VideoObjectPlane_vop_coding_type[0]) {

		  case 0 : {
			if(firstframe==0){
				firstframe++;
				Choix_I_P_haut_niveau_img_0_o [0] = 0 ;
			}
			else{
				Choix_I_P_haut_niveau_img_0_o [0] = 1 ;
			}

			/*!
			   I-frame processing.

			   	\param[in] data 			: bitstream (double buffer output)
			   	\param[in] VOLsimple		: input VOL
			   	\param[in] pos_i 			: position in bitstream
			   	\param[in] VOP 				: input VOP
			   	\param[in] DCT3D_I 			: inverse VLC intra coefficients
			   	\param[out] pos_o			: position in bitstream after frame processing
			    \param[out] address			: address in the image buffer to display
			   	\param[inout] Lum			: image luminance Y output
			   	\param[inout] Cb			: image chrominance U output
			   	\param[inout] Cr			: image chrominance V output
			   	\param[inout] keyframes		: if it is the first keyframes

			   */
			decode_I_frame(
					Choix_I_P_haut_niveau_CondI9_o,						// data
					Choix_I_P_haut_niveau_CondI7_o,						// VOLSimple
					Choix_I_P_haut_niveau_CondI4_o[0],					// pos_i
					Choix_I_P_haut_niveau_CondI8_o,						// VOP
					init_vlc_tables_I_PC_decod1_DCT3D_I,				// DCT3D_I
					Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc,	// pos_o
					Choix_I_P_haut_niveau_decode_I_frame_address,		// address
					Choix_I_P_haut_niveau_CondI2_o,						// Lum
					Choix_I_P_haut_niveau_CondI0_o,						// Cb
					Choix_I_P_haut_niveau_CondI1_o,						// Cr
					Choix_I_P_haut_niveau_CondI3_o);					// keyframes

			Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_0_o[0];
			memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
			memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
			memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
			Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_decode_I_frame_address[0];
			Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc[0];
			memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
			memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));
			break; }

		  case 1 : {
			Choix_I_P_haut_niveau_img_1_o [0] = 1 ;

			decode_P_frame(
					Choix_I_P_haut_niveau_CondI9_o,
					Choix_I_P_haut_niveau_CondI7_o,
					Choix_I_P_haut_niveau_CondI4_o[0],
					Choix_I_P_haut_niveau_CondI8_o,
					init_vlc_tables_I_PC_decod1_DCT3D_I,
					init_vlc_tables_P_PC_decod1_DCT3D_P,
					Choix_I_P_haut_niveau_decode_P_frame_stock_mb_type_P,
					Choix_I_P_haut_niveau_decode_P_frame_pos_fin_vlc,
					Choix_I_P_haut_niveau_decode_P_frame_address,
					Choix_I_P_haut_niveau_CondI2_o,
					Choix_I_P_haut_niveau_CondI0_o,
					Choix_I_P_haut_niveau_CondI1_o,
					Choix_I_P_haut_niveau_CondI3_o,
					Choix_I_P_haut_niveau_CondI5_o);

			Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_1_o[0];
			memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
			memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
			memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
			Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_decode_P_frame_address[0];
			Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_decode_P_frame_pos_fin_vlc[0];
			memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
			memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));
			break; }

		  case 2 : {
			Choix_I_P_haut_niveau_img_2_o [0] = 0 ;
			Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_2_o[0];
			memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
			memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
			memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
			Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_img_2_o[0];
			Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_CondI4_o[0];
			memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
			memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));
			break; }

		  case 4 : {
			Choix_I_P_haut_niveau_img_4_o [0] = 0 ;
			Choix_I_P_haut_niveau_CondO6_o[0] = Choix_I_P_haut_niveau_img_4_o[0];
			memcpy(Choix_I_P_haut_niveau_CondO3_o, Choix_I_P_haut_niveau_CondI2_o, 1371648*sizeof(uchar));
			memcpy(Choix_I_P_haut_niveau_CondO2_o, Choix_I_P_haut_niveau_CondI1_o, 342912*sizeof(uchar));
			memcpy(Choix_I_P_haut_niveau_CondO1_o, Choix_I_P_haut_niveau_CondI0_o, 342912*sizeof(uchar));
			Choix_I_P_haut_niveau_CondO0_o[0] = Choix_I_P_haut_niveau_img_4_o[0];
			Choix_I_P_haut_niveau_CondO5_o[0] = Choix_I_P_haut_niveau_CondI4_o[0];
			memcpy(Choix_I_P_haut_niveau_CondO7_o, Choix_I_P_haut_niveau_CondI5_o, 1620*sizeof(vector));
			memcpy(Choix_I_P_haut_niveau_CondO4_o, Choix_I_P_haut_niveau_CondI3_o, 2*sizeof(int));
			break; }
		}

		Display_CondI6_o[0] = VideoObjectLayer_ysize_o[0];
		Display_CondI5_o[0] = VideoObjectLayer_xsize_o[0];
		memcpy(Display_CondI3_o, Choix_I_P_haut_niveau_CondO3_o, 1371648*sizeof(uchar));
		memcpy(Display_CondI2_o, Choix_I_P_haut_niveau_CondO2_o, 342912*sizeof(uchar));
		memcpy(Display_CondI1_o, Choix_I_P_haut_niveau_CondO1_o, 342912*sizeof(uchar));
		Display_CondI4_o[0] = Choix_I_P_haut_niveau_CondO0_o[0];

		New_buffer(414720, Choix_I_P_haut_niveau_CondO5_o[0], New_buffer_new_buff_o, New_buffer_pos_octet);

		switch (Choix_I_P_haut_niveau_CondO6_o[0]) {
		  case 1 : {
			{
			   int XDIM = ((int *) Display_Extract_Image_Y_o)[0] = *Display_CondI5_o;
			   int YDIM = ((int *) Display_Extract_Image_Y_o)[1] = *Display_CondI6_o;
			   uchar *Y = Display_Extract_Image_Y_o + 8;
			   uchar *U = Y + (XDIM + 32) * YDIM;
			   uchar *V = U + (XDIM + 32) * YDIM/4;
			   extract_picture(
					   XDIM, YDIM,
					   16, Zero_PC_decod1_out,
					   Display_CondI3_o, Display_CondI1_o, Display_CondI2_o, Display_CondI4_o[0],
					   Y, U, V);
			}

			break; }

		  case 2 : {
			{
			   int XDIM = ((int *) Display_Extract_1_Image_Y_o)[0] = *Display_CondI5_o;
			   int YDIM = ((int *) Display_Extract_1_Image_Y_o)[1] = *Display_CondI6_o;
			   uchar *Y = Display_Extract_1_Image_Y_o + 8;
			   uchar *U = Y + (XDIM + 32) * YDIM;
			   uchar *V = U + (XDIM + 32) * YDIM/4;
			   extract_picture(
					   XDIM, YDIM,
					   16, Zero_PC_decod1_out,
					   Display_CondI3_o, Display_CondI1_o, Display_CondI2_o, Display_CondI4_o[0],
					   Y, U, V);
			}
			break; }
		}

		mem_buffer_buf_size[0] = readm4v_double_buffering_buffer_out_size[0];
		memcpy(mem_buffer_buf, readm4v_double_buffering_buffer_out, 829440*sizeof(stream));
		memcpy(mem_Y_last_buf, Choix_I_P_haut_niveau_CondO3_o, 1371648*sizeof(uchar));
		memcpy(mem_V_last_buf, Choix_I_P_haut_niveau_CondO2_o, 342912*sizeof(uchar));
		memcpy(mem_U_last_buf, Choix_I_P_haut_niveau_CondO1_o, 342912*sizeof(uchar));
		memcpy(mem_mv_buf, Choix_I_P_haut_niveau_CondO7_o, 1620*sizeof(vector));
		memcpy(mem_2_int_buf, Choix_I_P_haut_niveau_CondO4_o, 2*sizeof(int));
		mem_pos_octet_buf[0] = New_buffer_pos_octet[0];
		mem_new_buff_buf[0] = New_buffer_new_buff_o[0];

		switch (Choix_I_P_haut_niveau_CondO6_o[0]){
		  case 1 : {
			{
			   int XDIM = ((unsigned int *) Display_Extract_Image_Y_o)[0];
			   int YDIM = ((unsigned int *) Display_Extract_Image_Y_o)[1];
			   uchar *Y = Display_Extract_Image_Y_o + 8;
			   uchar *U = Y + (XDIM + 32) * YDIM;
			   uchar *V = U + (XDIM + 32) * YDIM/4 ;
			   SDL_Display(16, XDIM, YDIM, Y, U, V);
			}

			break; }

		  case 2 : {
			{
			   int XDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[0];
			   int YDIM = ((unsigned int *) Display_Extract_1_Image_Y_o)[1];
			   uchar *Y = Display_Extract_1_Image_Y_o + 8;
			   uchar *U = Y + (XDIM + 32) * YDIM;
			   uchar *V = U + (XDIM + 32) * YDIM/4 ;
			   SDL_Display(16, XDIM, YDIM, Y, U, V);
			}

			break; }
		}
	}

	int kk;
	for (kk = 0; kk < 100000000; ++kk) {}
}
