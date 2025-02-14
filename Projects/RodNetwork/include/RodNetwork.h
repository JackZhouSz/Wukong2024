#ifndef RODNETWORK_H
#define RODNETWORK_H


#include <utility> 
#include <iostream>
#include <fstream>
#include <Eigen/Geometry>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <tbb/tbb.h>
#include <unordered_map>
#include <iomanip>


#include "VecMatDef.h"
#include "Rod.h"

class RodNetwork
{
public:
	using VectorXT = Matrix<T, Eigen::Dynamic, 1>;
	using MatrixXT = Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
	using VectorXi = Vector<int, Eigen::Dynamic>;
	using MatrixXi = Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;
	using VtxList = std::vector<int>;
	using StiffnessMatrix = Eigen::SparseMatrix<T>;
	using Entry = Eigen::Triplet<T>;
	using TV = Vector<T, 3>;
	using TV2 = Vector<T, 2>;
	using TM2 = Matrix<T, 2, 2>;
	using TV3 = Vector<T, 3>;
	using IV = Vector<int, 3>;
	using IV2 = Vector<int, 2>;
	using TM = Matrix<T, 3, 3>;


public:

	T ROD_A = 2.5e-4;
	T ROD_B = 2.5e-4;

	VectorXT deformed_states;
    VectorXT rest_states;
	VectorXT dq;

	std::vector<Rod*> rods;
	std::vector<RodCrossing*> rod_crossings;
	std::unordered_map<int, T> dirichlet_data;

	bool add_stretching = true;

	bool verbose = false;
	bool run_diff_test = false;
	int max_newton_iter = 500;
    int ls_max = 12;
	T newton_tol = 1e-5;

private:
	template <class OP>
	void iterateDirichletDoF(const OP& f) {
		for (auto dirichlet: dirichlet_data)
			f(dirichlet.first, dirichlet.second);
	}

	template<int dim_row=2, int dim_col=2>
    void addHessianEntry(
        std::vector<Entry>& triplets,
        const std::vector<int>& vtx_idx, 
        const MatrixXT& hessian, 
        int shift_row = 0, int shift_col=0)
    {
        
        for (int i = 0; i < vtx_idx.size(); i++)
        {
            int dof_i = vtx_idx[i];
            for (int j = 0; j < vtx_idx.size(); j++)
            {
                int dof_j = vtx_idx[j];
                for (int k = 0; k < dim_row; k++)
                    for (int l = 0; l < dim_col; l++)
                        triplets.emplace_back(
                                dof_i * dim_row + k + shift_row, 
                                dof_j * dim_col + l + shift_col, 
                                hessian(i * dim_row + k, j * dim_col + l)
                            );                
            }
        }
    }
	

public:
	RodNetwork() {} 
	~RodNetwork() {} 

	T computeTotalEnergy();

    T computeResidual(Eigen::Ref<VectorXT> residual);
    
    void projectDirichletDoFMatrix(StiffnessMatrix& A, const std::unordered_map<int, T>& data);
    
    void buildSystemDoFMatrix(StiffnessMatrix& K);

    bool linearSolve(StiffnessMatrix& K, const VectorXT& residual, VectorXT& du);

    T lineSearchNewton(const VectorXT& residual);

	bool advanceOneStep(int step);

	void computeBoundingBox(TV& bottom_left, TV& top_right);

	// Stretching.cpp

	T addStretchingEnergy();
	void addStretchingForce(Eigen::Ref<VectorXT> residual);
	void addStretchingHessian(std::vector<Entry>& entry_K);


	// initialize from file
	void initializeFromFile(const std::string& filename);

	// code for initializing rods
	void addAStraightRod(const TV& from, const TV& to, 
		int from_idx, int to_idx,
        int sub_div, int& full_dof_cnt, int& node_cnt, int& rod_cnt);

	void addCrossingPoint(std::vector<TV>& existing_nodes, 
		const TV& point, int& full_dof_cnt, int& node_cnt);
};


#endif
