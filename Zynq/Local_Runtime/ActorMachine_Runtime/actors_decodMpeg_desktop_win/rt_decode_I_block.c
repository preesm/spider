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
#include <mpeg.h>
#include <lrt_prototypes.h>

short   iDCT_data [64];
short   InvDCT_f [128];
void    VLCinverseI (const int N, const int posMB, const int pos_i, const unsigned char *RESTRICT data
    , const REVERSE_EVENT *RESTRICT DCT3D_I, const short *RESTRICT BlkA, const short *RESTRICT BlkB
    , const short *RESTRICT BlkC, const struct_VOP *RESTRICT VOP, int *RESTRICT pos_fin_vlc, short *RESTRICT PQF
    , short *RESTRICT QFpred, short *RESTRICT F00pred, short *RESTRICT QPpred, int *RESTRICT prediction_direction) ;
void    InverseACDCpred (const int type, const short *RESTRICT PQF, const short *RESTRICT QFpred, const short F00pred
    , const short QPpred, const struct_VOP *RESTRICT VOP, const int prediction_direction, short *RESTRICT QF
    , int *RESTRICT dc_scaler) ;
void    InverseQuantI (short *RESTRICT QF, const struct_VOP *RESTRICT VOP, const int dc_scaler
    , const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT F, short *RESTRICT BlkXn) ;
void    InverseDCT (const short *RESTRICT F_in, short *RESTRICT f_out) ;
void    interpolate8x8_switch (const int width, const int height, const int edge, const int stride_src, const int x, const int y, const int dx
    , const int dy, const struct_VOP *RESTRICT VOP, const unsigned char *RESTRICT refn, unsigned char *RESTRICT cur) ;
void    VLCinverseP (const REVERSE_EVENT *RESTRICT DCT3D_P, const int pos_i, const unsigned char *RESTRICT data
    , int *RESTRICT pos_fin_vlc, short *RESTRICT PQF) ;
void    InverseQuantP (const short *RESTRICT QF, const struct_VOP *RESTRICT VOP
    , const struct_VOLsimple *RESTRICT VOLsimple, short *RESTRICT F) ;
void    block_inter (short *RESTRICT src1, unsigned char *RESTRICT src2, short *RESTRICT dst) ;
void 	InverseDCT_optim (short *RESTRICT F);
void block_sat ( const short *RESTRICT src, unsigned char *RESTRICT dst );


void rt_decode_bloc_intra()
{
	/*!
	   intra-block processing.

	   	\param[in] k	 				: block number in macroblock
	   	\param[in] data 				: bitstream (double buffer output)
	   	\param[in] VOP 					: input VOP
	   	\param[in] pos_i		 			: position in bitstream
	   	\param[in] DCT3D_I 				: inverse VLC intra coefficients
	   	\param[in] VOLsimple			: input VOL
	   	\param[in] MB_pos				: current macroblock number
	   	\param[in] Blk_A				: neighboring block 'A'
	   	\param[in] Blk_B				: neighboring block 'B'
	   	\param[in] Blk_C				: neighboring block 'C'
	   	\param[out] InverseQuant_Blk_X	: block inverse quantization
	   	\param[out] block_sat_X			: processed output block
	   	\param[out] pos_fin_vlc			: position in bitstream after block processing
	   */

	UINT8 error;
	OS_TCB tcb;
	error = OSTaskQuery(OS_PRIO_SELF, &tcb);

	//--- Reading inputs.
	int 				k, MB_pos, pos_i;
	UINT8*				data;
	REVERSE_EVENT		DCT3D_I;
	struct_VOLsimple 	VOLsimple;
	short				Blk_A, Blk_B, Blok_C;
	struct_VOP			VOP;

	read_input_fifo(tcb.fifo_in[0], sizeof(int), &k, &error);
	read_input_fifo(tcb.fifo_in[1], sizeof(int), &MB_pos, &error);
	read_input_fifo(tcb.fifo_in[2], sizeof(int), &pos_i, &error);
	read_input_fifo(tcb.fifo_in[3], sizeof(INT8U), data, &error);
	read_input_fifo(tcb.fifo_in[4], sizeof(REVERSE_EVENT), &DCT3D_I, &error);
	read_input_fifo(tcb.fifo_in[5], sizeof(struct_VOLsimple), &VOLsimple, &error);
	read_input_fifo(tcb.fifo_in[6], sizeof(short), &Blk_A, &error);
	read_input_fifo(tcb.fifo_in[7], sizeof(short), &Blk_B, &error);
	read_input_fifo(tcb.fifo_in[8], sizeof(short), &Blk_C, &error);
	read_input_fifo(tcb.fifo_in[9], sizeof(struct_VOP), &VOP, &error);
	//---

	short   			InvDCT_f[128], DCpred_QFpred[7], DCpred_F00pred, DCpred_QPpred;
	int     			DCpred_prediction_direction;
	int 				pos_fin_vlc;

	VLCinverseI(5 - k, MB_pos, pos_i, data, DCT3D_I, Blk_A, Blk_B, Blk_C, VOP, pos_fin_vlc, InvDCT_f, DCpred_QFpred
        , DCpred_F00pred, DCpred_QPpred, DCpred_prediction_direction);

    InverseACDCpred(k, InvDCT_f, DCpred_QFpred, DCpred_F00pred [0], DCpred_QPpred [0], VOP
        , DCpred_prediction_direction [0], iDCT_data, InverseACDCpred_dc_scaler);

    short InverseQuant_Blk_X;
    InverseQuantI(iDCT_data, VOP, InverseACDCpred_dc_scaler [0], VOLsimple, InvDCT_f, &InverseQuant_Blk_X);

    InverseDCT_optim(InvDCT_f);

    INT8U	block_sat_X;
    block_sat(InvDCT_f, &block_sat_X);


    //--- Writing outputs.
    write_output_fifo(tcb.fifo_out[0], sizeof(int), &pos_fin_vlc, &error);
    write_output_fifo(tcb.fifo_out[0], sizeof(short), &InverseQuant_Blk_X, &error);
    write_output_fifo(tcb.fifo_out[0], sizeof(INT8U), &block_sat_X, &error);
    //---
}
