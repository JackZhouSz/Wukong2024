#include "../include/RodNetwork.h"
#include <Eigen/CholmodSupport>

void RodNetwork::initializeFromFile(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        std::cerr << "Cannot open file " << filename << std::endl;
        return;
    }
    int n_vtx, n_edges;
    in >> n_vtx >> n_edges;
    std::vector<TV> vtx_positions(n_vtx);
    for (int i = 0; i < n_vtx; i++)
    {
        in >> vtx_positions[i][0] >> vtx_positions[i][1] >> vtx_positions[i][2];
    }
    std::vector<std::array<int, 2>> edges(n_edges);
    for (int i = 0; i < n_edges; i++)
    {
        in >> edges[i][0] >> edges[i][1];
    }
    in.close();

    int sub_div = 0;
    std::vector<Eigen::Triplet<T>> w_entry;
    int full_dof_cnt = 0;
    int node_cnt = 0;
    int rod_cnt = 0;

    auto addCrossingData = [&](int crossing_idx, int rod_idx, int location)
    {
        rod_crossings[crossing_idx]->is_fixed = true;
        rod_crossings[crossing_idx]->rods_involved.push_back(rod_idx);
        rod_crossings[crossing_idx]->on_rod_idx[rod_idx] = location;
    };

    std::vector<TV> nodal_positions;
    for (int i = 0; i < n_vtx; i++)
    {
        addCrossingPoint(nodal_positions, vtx_positions[i], full_dof_cnt, node_cnt);
    }

    for (int i = 0; i < n_edges; i++)
    {
        addAStraightRod(nodal_positions[edges[i][0]], nodal_positions[edges[i][1]], 
            edges[i][0], edges[i][1],
            sub_div, full_dof_cnt, node_cnt, rod_cnt);
    }

    rest_states = deformed_states;
    dq = VectorXT::Zero(deformed_states.rows());

    TV min_corner, max_corner;
    computeBoundingBox(min_corner, max_corner);
    std::cout << "min_corner: " << min_corner.transpose() << std::endl;
    std::cout << "max_corner: " << max_corner.transpose() << std::endl;
    for (int i = 0; i < deformed_states.rows() / 3; i++)
    {
        TV x = deformed_states.segment<3>(i * 3);
        if (x[0] > max_corner[0] - 1e-4)
        {
            dirichlet_data[i * 3] = deformed_states[i * 3] + 0.1;
            
            dirichlet_data[i * 3 + 1] = 0;
            dirichlet_data[i * 3 + 2] = 0;
        }
        if (x[0] < min_corner[0] + 1e-4)
        {
            dirichlet_data[i * 3] = 0.0;
            dirichlet_data[i * 3 + 1] = 0;
            dirichlet_data[i * 3 + 2] = 0;
        }
    }
    
}

T RodNetwork::computeResidual(Eigen::Ref<VectorXT> residual)
{
    VectorXT dq_projected = dq;
    if(!run_diff_test)
        iterateDirichletDoF([&](int offset, T target)
        {
            dq_projected[offset] = target;
        });
    
    deformed_states = rest_states + dq_projected;

    // for (auto& rod : rods)
    // {
    //     rod->reference_angles = deformed_states.template segment(rod->theta_dof_start_offset, 
    //         rod->indices.size() - 1);
    //     if (!run_diff_test)
    //         rod->rotateReferenceFrameToLastNewtonStepAndComputeReferenceTwsit();
    // }

    // for (auto& crossing : rod_crossings)
    // {
    //     crossing->omega = deformed_states.template segment<3>(crossing->dof_offset);
    // }
        // crossing->updateRotation(deformed_states.template segment<3>(crossing->dof_offset));

    if (add_stretching)
        addStretchingForce(residual);
    // if (add_bending && add_twisting)
    // {
    //     add3DBendingAndTwistingForce(full_residual);
    // }
    // if (add_rigid_joint)
    //     addJointBendingAndTwistingForce(full_residual);
    
    
    if (!run_diff_test)
        iterateDirichletDoF([&](int offset, T target)
        {
            residual[offset] = 0;
        });
    
    return residual.norm();
}

bool RodNetwork::advanceOneStep(int step)
{
    std::cout << "===================STEP " << step << "===================" << std::endl;
    VectorXT residual = VectorXT::Zero(deformed_states.rows());
    T residual_norm = computeResidual(residual);
    
    std::cout << "[NEWTON] iter " << step << "/" 
        << max_newton_iter << ": residual_norm " 
        << residual_norm << " tol: " << newton_tol << std::endl;

    if (residual_norm < newton_tol || step == max_newton_iter)
    {
        return true;
    }

    T du_norm = 1e10;
    du_norm = lineSearchNewton(residual);
    return false;
}


// bool RodNetwork::staticSolve()
// {
//     int cnt = 0;
//     T residual_norm = 1e10, dq_norm = 1e10;
    
//     while (true)
//     {
//         VectorXT residual(deformed_states.rows());
//         residual.setZero();
        
//         computeResidual(residual);

//         // auto f_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - time ).count();

//         residual_norm = residual.norm();
//         if (verbose)
//             std::cout << "residual_norm " << residual_norm << std::endl;
        
//         // std::cout << residual_norm << std::endl;
//         // std::getchar();

//         if (residual_norm < newton_tol)
//             break;
        
//         dq_norm = lineSearchNewton(residual);


//         if(cnt == max_newton_iter || dq_norm > 1e10)
//             break;
//         cnt++;
//     }
    
//     // if (verbose)
//         std::cout << "# of newton solve: " << cnt << " exited with |g|: " << residual_norm << "|dq|: " << dq_norm  << std::endl;
    
//     if (cnt == max_newton_iter || dq_norm > 1e10 || residual_norm > 1)
//         return false;
//     return true;
// }

T RodNetwork::lineSearchNewton(const VectorXT& residual)
{
    bool verbose = false;
    
    VectorXT ddq(deformed_states.rows());

    StiffnessMatrix K;
    buildSystemDoFMatrix(K);
    bool success = linearSolve(K, residual, ddq);
    if (!success)
        return 1e16;

    T norm = ddq.norm();
    // std::cout << norm << std::endl;
    // if (norm < 1e-6) return norm;
    T alpha = 1;
    T E0 = computeTotalEnergy();
    // std::cout << "E0: " << E0 << std::endl;
    int cnt = 0;
    std::vector<T> Es;
    VectorXT dq_current = dq;
    while(true)
    {
        dq = dq_current + alpha * ddq;
        T E1 = computeTotalEnergy();
        
        if (E1 - E0 < 0) 
            break;
        alpha *= T(0.5);
        cnt += 1;
        if (cnt > ls_max)
            break;
    }
    
    // for (auto& crossing : rod_crossings)
    // {
    //     Vector<T, 3> new_omega = dq.template segment<3>(crossing->reduced_dof_offset);
    //     crossing->updateRotation(new_omega);
    //     dq.template segment<3>(crossing->reduced_dof_offset).setZero();
    // }
    
    return norm;   
}


T RodNetwork::computeTotalEnergy()
{
    VectorXT dq_projected = dq;
    
    if(!run_diff_test)
        iterateDirichletDoF([&](int offset, T target)
        {
            dq_projected[offset] = target;
        });
   
    deformed_states = rest_states + dq_projected;

    // for (auto& rod : rods)
    // {
    //     rod->reference_angles = deformed_states.template segment(rod->theta_dof_start_offset, 
    //         rod->indices.size() - 1);
    // }
    // for (auto& crossing : rod_crossings)
    //     crossing->omega = deformed_states.template segment<3>(crossing->dof_offset);

    T total_energy = 0;
    T E_stretching = 0, E_bending_twisting = 0, E_contact = 0;
    
    if (add_stretching)
        E_stretching += addStretchingEnergy();
    // if (add_bending && add_twisting)
    //     E_bending_twisting = add3DBendingAndTwistingEnergy();
    
    // if (add_rigid_joint)
    //     E_bending_twisting += addJointBendingAndTwistingEnergy();
    
    total_energy = E_stretching + E_bending_twisting + E_contact;
    
    if (verbose)
        std::cout << "E_stretching " << E_stretching << 
        " E_bending_twisting " << E_bending_twisting <<  
        " E_contact " << E_contact << std::endl;
    return total_energy;
}

void RodNetwork::buildSystemDoFMatrix(StiffnessMatrix& K)
{
    std::vector<Entry> entry_K;
    VectorXT dq_projected = dq;
    if(!run_diff_test)
        iterateDirichletDoF([&](int offset, T target)
        {
            dq_projected[offset] = target;
        });

    deformed_states = rest_states + dq_projected;
    // for (auto& rod : rods)
    // {
    //     rod->reference_angles = deformed_states.template segment(rod->theta_dof_start_offset, rod->indices.size() - 1);
    // }
    // for (auto& crossing : rod_crossings)
    //     crossing->omega = deformed_states.template segment<3>(crossing->dof_offset);

    if (add_stretching)
        addStretchingHessian(entry_K);
    // if (add_bending && add_twisting)
    // {
    //     add3DBendingAndTwistingK(entry_K);
    // }
    // if (add_rigid_joint)
    //     addJointBendingAndTwistingK(entry_K);
    

    K.setFromTriplets(entry_K.begin(), entry_K.end());
    
    if(!run_diff_test)
        projectDirichletDoFMatrix(K, dirichlet_data);
    
    K.makeCompressed();
}

void RodNetwork::projectDirichletDoFMatrix(StiffnessMatrix& A, const std::unordered_map<int, T>& data)
{
    for (auto iter : data)
    {
        A.row(iter.first) *= 0.0;
        A.col(iter.first) *= 0.0;
        A.coeffRef(iter.first, iter.first) = 1.0;
    }

}

bool RodNetwork::linearSolve(StiffnessMatrix& K, 
    const VectorXT& residual, VectorXT& du)
{
    
    Eigen::CholmodSupernodalLLT<StiffnessMatrix, Eigen::Lower> solver;
    
    T alpha = 1e-6;
    StiffnessMatrix H(K.rows(), K.cols());
    H.setIdentity(); H.diagonal().array() = 1e-10;
    K += H;
    solver.analyzePattern(K);
    
    int indefinite_count_reg_cnt = 0, invalid_search_dir_cnt = 0, invalid_residual_cnt = 0;
    int i = 0;
    T dot_dx_g = 0.0;
    for (; i < 50; i++)
    {
        solver.factorize(K);
        // std::cout << "factorize" << std::endl;
        if (solver.info() == Eigen::NumericalIssue)
        {
            K.diagonal().array() += alpha;
            alpha *= 10;
            indefinite_count_reg_cnt++;
            continue;
        }

        du = solver.solve(residual);
        
        dot_dx_g = du.normalized().dot(residual.normalized());

        int num_negative_eigen_values = 0;
        int num_zero_eigen_value = 0;

        bool positive_definte = num_negative_eigen_values == 0;
        bool search_dir_correct_sign = dot_dx_g > 1e-6;
        if (!search_dir_correct_sign)
        {   
            invalid_search_dir_cnt++;
        }
        
        // bool solve_success = true;
        // bool solve_success = (K * du - residual).norm() / residual.norm() < 1e-6;
        bool solve_success = du.norm() < 1e3;
        
        if (!solve_success)
            invalid_residual_cnt++;
        // std::cout << "PD: " << positive_definte << " direction " 
        //     << search_dir_correct_sign << " solve " << solve_success << std::endl;

        if (positive_definte && search_dir_correct_sign && solve_success)
        {
            
            if (verbose)
            {
                std::cout << "\t===== Linear Solve ===== " << std::endl;
                std::cout << "\tnnz: " << K.nonZeros() << std::endl;
                // std::cout << "\t takes " << t.elapsed_sec() << "s" << std::endl;
                std::cout << "\t# regularization step " << i 
                    << " indefinite " << indefinite_count_reg_cnt 
                    << " invalid search dir " << invalid_search_dir_cnt
                    << " invalid solve " << invalid_residual_cnt << std::endl;
                std::cout << "\tdot(search, -gradient) " << dot_dx_g << std::endl;
                std::cout << "\t======================== " << std::endl;
                
            }
            return true;
        }
        else
        {
            K.diagonal().array() += alpha;
            alpha *= 10;
        }
    }
    if (verbose)
    {
        std::cout << "\t===== Linear Solve ===== " << std::endl;
        std::cout << "\tnnz: " << K.nonZeros() << std::endl;
        // std::cout << "\t takes " << t.elapsed_sec() << "s" << std::endl;
        std::cout << "\t# regularization step " << i 
            << " indefinite " << indefinite_count_reg_cnt 
            << " invalid search dir " << invalid_search_dir_cnt
            << " invalid solve " << invalid_residual_cnt << std::endl;
        std::cout << "\tdot(search, -gradient) " << dot_dx_g << std::endl;
        std::cout << "\t======================== " << std::endl;
        
    }
    return false;
}


void RodNetwork::computeBoundingBox(TV& min_corner, TV& max_corner)
{
    min_corner.setConstant(1e6);
    max_corner.setConstant(-1e6);

    for (auto& rod : rods)
    {
        int cnt = 0;
        for (int idx : rod->indices)
        {
            TV x;
            rod->x(idx, x);
            for (int d = 0; d < 3; d++)
            {
                max_corner[d] = std::max(max_corner[d], x[d]);
                min_corner[d] = std::min(min_corner[d], x[d]);
            }
        }
    }   
}

// ========================== rod construction helpers ==========================
void RodNetwork::addCrossingPoint(std::vector<TV>& existing_nodes, 
    const TV& point, int& full_dof_cnt, int& node_cnt)
{
    rod_crossings.push_back(new RodCrossing(node_cnt, std::vector<int>())); 
    deformed_states.conservativeResize(full_dof_cnt + 3);
    deformed_states.segment<3>(full_dof_cnt) = point;
    existing_nodes.push_back(point);
    full_dof_cnt += 3;
    node_cnt++;
}


void RodNetwork::addAStraightRod(const TV& from, const TV& to,  
    int from_idx, int to_idx,
    int sub_div, int& full_dof_cnt, int& node_cnt, int& rod_cnt)
{

    std::unordered_map<int, IV> offset_map;
    std::vector<TV> sub_points;
    std::vector<int> node_idx;
    node_idx.push_back(from_idx);
    // sub_points.push_back(from);
    int cnt = 0;
    if (sub_div)
    {
        int n_sub_nodes = sub_div + 1;
        T length_sub = (to - from).norm() / T(sub_div);
        TV loop_point = from;
        TV loop_left = from;
        TV length_vec = (to - from).normalized();
        for (int j = 0; j < n_sub_nodes - 1; j++)
        {
            sub_points.push_back(loop_left + length_sub * length_vec);
            loop_left = sub_points.back();
            node_idx.push_back(node_cnt + cnt);
            cnt++;
        }
    }
    node_idx.push_back(to_idx);
    // sub_points.push_back(to);
                
    deformed_states.conservativeResize(full_dof_cnt + (sub_points.size()) * (3));

    Rod* rod = new Rod(deformed_states, rest_states, rod_cnt, false, ROD_A, ROD_B);

    for (int i = 0; i < sub_points.size(); i++)
    {
        // offset_map[node_cnt] = IV::Zero();
        //push Lagrangian DoF    
        deformed_states.segment<3>(full_dof_cnt) = sub_points[i];
        // for (int d = 0; d < 3; d++)
        // {
        //     offset_map[node_cnt][d] = full_dof_cnt++;  
        // }
        node_cnt++;
    }

    // rod->offset_map = offset_map;
    rod->indices = node_idx;
    
    rods.push_back(rod);
    rod_cnt++;
}
