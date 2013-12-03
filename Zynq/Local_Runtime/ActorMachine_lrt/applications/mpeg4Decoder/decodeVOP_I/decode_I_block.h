#include <mpeg.h>

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

void action_decode_bloc_intra();
