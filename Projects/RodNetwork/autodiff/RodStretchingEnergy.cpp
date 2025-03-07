#include "RodStretchingEnergy.h"

void computeRodStretchingEnergy(double ks, const Eigen::Matrix<double,3,1> & X1, const Eigen::Matrix<double,3,1> & X2, const Eigen::Matrix<double,3,1> & x1, const Eigen::Matrix<double,3,1> & x2, 
	double& energy){
	double _i_var[24];
	_i_var[0] = (X2(1,0))-(X1(1,0));
	_i_var[1] = (X2(0,0))-(X1(0,0));
	_i_var[2] = (x2(1,0))-(x1(1,0));
	_i_var[3] = (x2(0,0))-(x1(0,0));
	_i_var[4] = (X2(2,0))-(X1(2,0));
	_i_var[5] = (_i_var[0])*(_i_var[0]);
	_i_var[6] = (_i_var[1])*(_i_var[1]);
	_i_var[7] = (x2(2,0))-(x1(2,0));
	_i_var[8] = (_i_var[2])*(_i_var[2]);
	_i_var[9] = (_i_var[3])*(_i_var[3]);
	_i_var[10] = (_i_var[4])*(_i_var[4]);
	_i_var[11] = (_i_var[6])+(_i_var[5]);
	_i_var[12] = (_i_var[7])*(_i_var[7]);
	_i_var[13] = (_i_var[9])+(_i_var[8]);
	_i_var[14] = (_i_var[11])+(_i_var[10]);
	_i_var[15] = (_i_var[13])+(_i_var[12]);
	_i_var[16] = std::sqrt(_i_var[14]);
	_i_var[17] = std::sqrt(_i_var[15]);
	_i_var[18] = 0.5;
	_i_var[19] = (_i_var[17])-(_i_var[16]);
	_i_var[20] = (_i_var[18])*(ks);
	_i_var[21] = (_i_var[19])*(_i_var[19]);
	_i_var[22] = (_i_var[20])/(_i_var[16]);
	_i_var[23] = (_i_var[22])*(_i_var[21]);
	energy = _i_var[23];
}
void computeRodStretchingEnergyGradient(double ks, const Eigen::Matrix<double,3,1> & X1, const Eigen::Matrix<double,3,1> & X2, const Eigen::Matrix<double,3,1> & x1, const Eigen::Matrix<double,3,1> & x2, 
	Eigen::Matrix<double, 6, 1>& energygradient){
	double _i_var[39];
	_i_var[0] = (X2(1,0))-(X1(1,0));
	_i_var[1] = (X2(0,0))-(X1(0,0));
	_i_var[2] = (x2(1,0))-(x1(1,0));
	_i_var[3] = (x2(0,0))-(x1(0,0));
	_i_var[4] = (X2(2,0))-(X1(2,0));
	_i_var[5] = (_i_var[0])*(_i_var[0]);
	_i_var[6] = (_i_var[1])*(_i_var[1]);
	_i_var[7] = (x2(2,0))-(x1(2,0));
	_i_var[8] = (_i_var[2])*(_i_var[2]);
	_i_var[9] = (_i_var[3])*(_i_var[3]);
	_i_var[10] = (_i_var[4])*(_i_var[4]);
	_i_var[11] = (_i_var[6])+(_i_var[5]);
	_i_var[12] = (_i_var[7])*(_i_var[7]);
	_i_var[13] = (_i_var[9])+(_i_var[8]);
	_i_var[14] = (_i_var[11])+(_i_var[10]);
	_i_var[15] = (_i_var[13])+(_i_var[12]);
	_i_var[16] = 0.5;
	_i_var[17] = std::sqrt(_i_var[14]);
	_i_var[18] = std::sqrt(_i_var[15]);
	_i_var[19] = (_i_var[16])*(ks);
	_i_var[20] = 2;
	_i_var[21] = (_i_var[18])-(_i_var[17]);
	_i_var[22] = (_i_var[19])/(_i_var[17]);
	_i_var[23] = (_i_var[20])*(_i_var[18]);
	_i_var[24] = 1;
	_i_var[25] = (_i_var[22])*(_i_var[21]);
	_i_var[26] = (_i_var[24])/(_i_var[23]);
	_i_var[27] = (_i_var[20])*(_i_var[25]);
	_i_var[28] = (_i_var[27])*(_i_var[26]);
	_i_var[29] = (_i_var[28])*(_i_var[3]);
	_i_var[30] = (_i_var[28])*(_i_var[2]);
	_i_var[31] = (_i_var[28])*(_i_var[7]);
	_i_var[32] = -1;
	_i_var[33] = (_i_var[20])*(_i_var[29]);
	_i_var[34] = (_i_var[20])*(_i_var[30]);
	_i_var[35] = (_i_var[20])*(_i_var[31]);
	_i_var[36] = (_i_var[33])*(_i_var[32]);
	_i_var[37] = (_i_var[34])*(_i_var[32]);
	_i_var[38] = (_i_var[35])*(_i_var[32]);
	energygradient(0,0) = _i_var[36];
	energygradient(1,0) = _i_var[37];
	energygradient(2,0) = _i_var[38];
	energygradient(3,0) = _i_var[33];
	energygradient(4,0) = _i_var[34];
	energygradient(5,0) = _i_var[35];
}
void computeRodStretchingEnergyHessian(double ks, const Eigen::Matrix<double,3,1> & X1, const Eigen::Matrix<double,3,1> & X2, const Eigen::Matrix<double,3,1> & x1, const Eigen::Matrix<double,3,1> & x2, 
	Eigen::Matrix<double, 6, 6>& energyhessian){
	double _i_var[67];
	_i_var[0] = (x2(1,0))-(x1(1,0));
	_i_var[1] = (x2(0,0))-(x1(0,0));
	_i_var[2] = (x2(2,0))-(x1(2,0));
	_i_var[3] = (_i_var[0])*(_i_var[0]);
	_i_var[4] = (_i_var[1])*(_i_var[1]);
	_i_var[5] = (_i_var[2])*(_i_var[2]);
	_i_var[6] = (_i_var[4])+(_i_var[3]);
	_i_var[7] = (_i_var[6])+(_i_var[5]);
	_i_var[8] = (X2(1,0))-(X1(1,0));
	_i_var[9] = (X2(0,0))-(X1(0,0));
	_i_var[10] = std::sqrt(_i_var[7]);
	_i_var[11] = 2;
	_i_var[12] = (X2(2,0))-(X1(2,0));
	_i_var[13] = (_i_var[8])*(_i_var[8]);
	_i_var[14] = (_i_var[9])*(_i_var[9]);
	_i_var[15] = (_i_var[11])*(_i_var[10]);
	_i_var[16] = (_i_var[12])*(_i_var[12]);
	_i_var[17] = (_i_var[14])+(_i_var[13]);
	_i_var[18] = (_i_var[15])*(_i_var[15]);
	_i_var[19] = 1;
	_i_var[20] = (_i_var[17])+(_i_var[16]);
	_i_var[21] = (_i_var[19])/(_i_var[18]);
	_i_var[22] = std::sqrt(_i_var[20]);
	_i_var[23] = 0.5;
	_i_var[24] = -(_i_var[21]);
	_i_var[25] = (_i_var[10])-(_i_var[22]);
	_i_var[26] = (_i_var[23])*(ks);
	_i_var[27] = (_i_var[19])/(_i_var[15]);
	_i_var[28] = (_i_var[24])*(_i_var[11]);
	_i_var[29] = (_i_var[11])*(_i_var[25]);
	_i_var[30] = (_i_var[26])/(_i_var[22]);
	_i_var[31] = (_i_var[28])*(_i_var[27]);
	_i_var[32] = (_i_var[30])*(_i_var[29]);
	_i_var[33] = (_i_var[30])*(_i_var[11]);
	_i_var[34] = (_i_var[27])*(_i_var[27]);
	_i_var[35] = (_i_var[32])*(_i_var[31]);
	_i_var[36] = (_i_var[34])*(_i_var[33]);
	_i_var[37] = (_i_var[36])+(_i_var[35]);
	_i_var[38] = (_i_var[11])*(_i_var[1]);
	_i_var[39] = (_i_var[11])*(_i_var[2]);
	_i_var[40] = (_i_var[11])*(_i_var[0]);
	_i_var[41] = (_i_var[38])*(_i_var[37]);
	_i_var[42] = (_i_var[39])*(_i_var[37]);
	_i_var[43] = (_i_var[32])*(_i_var[27]);
	_i_var[44] = (_i_var[38])*(_i_var[38]);
	_i_var[45] = (_i_var[40])*(_i_var[40]);
	_i_var[46] = (_i_var[39])*(_i_var[39]);
	_i_var[47] = (_i_var[40])*(_i_var[41]);
	_i_var[48] = -1;
	_i_var[49] = (_i_var[38])*(_i_var[42]);
	_i_var[50] = (_i_var[43])*(_i_var[11]);
	_i_var[51] = (_i_var[44])*(_i_var[37]);
	_i_var[52] = (_i_var[40])*(_i_var[42]);
	_i_var[53] = (_i_var[45])*(_i_var[37]);
	_i_var[54] = (_i_var[46])*(_i_var[37]);
	_i_var[55] = (_i_var[48])*(_i_var[47]);
	_i_var[56] = (_i_var[48])*(_i_var[49]);
	_i_var[57] = (_i_var[51])+(_i_var[50]);
	_i_var[58] = (_i_var[48])*(_i_var[52]);
	_i_var[59] = (_i_var[53])+(_i_var[50]);
	_i_var[60] = (_i_var[54])+(_i_var[50]);
	_i_var[61] = (_i_var[48])*(_i_var[55]);
	_i_var[62] = (_i_var[48])*(_i_var[56]);
	_i_var[63] = (_i_var[48])*(_i_var[57]);
	_i_var[64] = (_i_var[48])*(_i_var[58]);
	_i_var[65] = (_i_var[48])*(_i_var[59]);
	_i_var[66] = (_i_var[48])*(_i_var[60]);
	energyhessian(0,0) = _i_var[57];
	energyhessian(1,0) = _i_var[61];
	energyhessian(2,0) = _i_var[62];
	energyhessian(3,0) = _i_var[63];
	energyhessian(4,0) = _i_var[55];
	energyhessian(5,0) = _i_var[56];
	energyhessian(0,1) = _i_var[61];
	energyhessian(1,1) = _i_var[59];
	energyhessian(2,1) = _i_var[64];
	energyhessian(3,1) = _i_var[55];
	energyhessian(4,1) = _i_var[65];
	energyhessian(5,1) = _i_var[58];
	energyhessian(0,2) = _i_var[62];
	energyhessian(1,2) = _i_var[64];
	energyhessian(2,2) = _i_var[60];
	energyhessian(3,2) = _i_var[56];
	energyhessian(4,2) = _i_var[58];
	energyhessian(5,2) = _i_var[66];
	energyhessian(0,3) = _i_var[63];
	energyhessian(1,3) = _i_var[55];
	energyhessian(2,3) = _i_var[56];
	energyhessian(3,3) = _i_var[57];
	energyhessian(4,3) = _i_var[47];
	energyhessian(5,3) = _i_var[49];
	energyhessian(0,4) = _i_var[55];
	energyhessian(1,4) = _i_var[65];
	energyhessian(2,4) = _i_var[58];
	energyhessian(3,4) = _i_var[47];
	energyhessian(4,4) = _i_var[59];
	energyhessian(5,4) = _i_var[52];
	energyhessian(0,5) = _i_var[56];
	energyhessian(1,5) = _i_var[58];
	energyhessian(2,5) = _i_var[66];
	energyhessian(3,5) = _i_var[49];
	energyhessian(4,5) = _i_var[52];
	energyhessian(5,5) = _i_var[60];
}