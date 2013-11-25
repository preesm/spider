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

#include "decode_I_block.h"

//short   iDCT_data [64];
//short   InvDCT_f [128];

static UINT8 data[829440];
void action_decode_intra_block()
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

	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	//--- Reading inputs.
	int 				k, MB_pos, pos_i;
//	UINT8*				data;
	REVERSE_EVENT		DCT3D_I;
	struct_VOLsimple 	VOLsimple;
	short				Blk_A, Blk_B, Blk_C;
	struct_VOP			VOP;

	read_input_fifo(action->fifo_in_id[0], sizeof(int), (UINT8*)&k);
	read_input_fifo(action->fifo_in_id[1], sizeof(int), (UINT8*)&MB_pos);
	read_input_fifo(action->fifo_in_id[2], sizeof(int), (UINT8*)&pos_i);
	read_input_fifo(action->fifo_in_id[3], sizeof(UINT8), (UINT8*)data);
	read_input_fifo(action->fifo_in_id[4], sizeof(REVERSE_EVENT), (UINT8*)&DCT3D_I);
	read_input_fifo(action->fifo_in_id[5], sizeof(struct_VOLsimple), (UINT8*)&VOLsimple);
	read_input_fifo(action->fifo_in_id[6], sizeof(short), (UINT8*)&Blk_A);
	read_input_fifo(action->fifo_in_id[7], sizeof(short), (UINT8*)&Blk_B);
	read_input_fifo(action->fifo_in_id[8], sizeof(short), (UINT8*)&Blk_C);
	read_input_fifo(action->fifo_in_id[9], sizeof(struct_VOP), (UINT8*)&VOP);
	//---

	short   			iDCT_data [64];
	short   			InvDCT_f[128], DCpred_QFpred[7], DCpred_F00pred, DCpred_QPpred;
	int     			DCpred_prediction_direction, InverseACDCpred_dc_scaler;
	int 				pos_fin_vlc;

	VLCinverseI(
		5 - k, 		MB_pos, 		pos_i, 				data, 			&DCT3D_I,
		&Blk_A, 	&Blk_B, 		&Blk_C, 			&VOP, 			&pos_fin_vlc,
		InvDCT_f, 	DCpred_QFpred, 	&DCpred_F00pred, 	&DCpred_QPpred, &DCpred_prediction_direction);

    InverseACDCpred(
    	k, 	InvDCT_f, 						DCpred_QFpred, 	DCpred_F00pred, 	DCpred_QPpred,
    	&VOP,DCpred_prediction_direction,	iDCT_data, 		&InverseACDCpred_dc_scaler);

    short InverseQuant_Blk_X;
    InverseQuantI(
    	iDCT_data, 	&VOP, 	InverseACDCpred_dc_scaler, &VOLsimple, InvDCT_f,
    	&InverseQuant_Blk_X);

    InverseDCT_optim(InvDCT_f);

    UINT8	block_sat_X;
    block_sat(InvDCT_f, &block_sat_X);


    //--- Writing outputs.
    write_output_fifo(action->fifo_out_id[0], sizeof(int), (UINT8*)&pos_fin_vlc);
    write_output_fifo(action->fifo_out_id[1], sizeof(short), (UINT8*)&InverseQuant_Blk_X);
    write_output_fifo(action->fifo_out_id[2], sizeof(UINT8), (UINT8*)&block_sat_X);
    //---
}
