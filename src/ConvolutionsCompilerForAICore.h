//
// (c) Copyright 2019 Xilinx Inc. All Rights Reserved.
// Created by pchatara on 5/21/19.
//

#ifndef HALIDE_CODEGEN_AI_CORE_H
#define HALIDE_CODEGEN_AI_CORE_H

#include "CodeGen_C.h"
#include "Module.h"
#include "Scope.h"
#include "Interval.h"

#include <cmath>
#include <list>
#include <utility>
#include <vector>
using namespace std;

namespace Halide {

    namespace Internal {

        class AIVectorMacOperation {
        public:
            string operand1_group_name;
            string operand2_group_name;
            int simd_columns;
            int simd_lanes;
            vector<pair<int, int>> operand_offsets;

            AIVectorMacOperation(string op1_group_name, string op2_group_name, int cols, int lanes) {
                operand1_group_name = std::move(op1_group_name);
                operand2_group_name = std::move(op2_group_name);
                simd_columns = cols;
                simd_lanes = lanes;
            }
        };


        class LogicalMemoryGroup {
        public:
            //Id's of loads in their reuse graph
            vector<int> ids;

            //Extent denotes the range of logical memory group
            Interval extent;

            // Size of the group
            int extent_size;

            //Offsets of loads from the beginning of the group
            vector<int> offsets;

            //Name of the group
            string name;

            //temporary field to check allocations (helpful for reducing live
            // ranges)
            vector<int> allocations; //1 means: Range: [(VLEN)*1, (VLEN)*2) are allocated
        };

        class ReuseGraph {

            //Number of elements in the reuse graph
            int num_elements_;

            //Adjacency list
            list<int> *adjacency_list_;

            //Method to do DFS exploration recursively
            void DFSUtil(int v, bool visited[], vector<int> &group);

          public:

            //These groups are formed after performing DFS on the reuse graph
            //Each element of a group has temporal reuse with atleast another
            //element in the same group.
            vector<LogicalMemoryGroup> logical_memory_groups_;

            //All loads involved in the reuse graph. This is used to retrieve
            // back a particular load given its id in the reuse graph
            vector<const Load*> loads_in_the_reuse_graph_;

            // Constructor
            ReuseGraph(int V, vector<const Load*> loads);

            //Method to add an edge to the graph
            void AddEdge(int v, int w);

            //This method is used to extract connected components using DFS
            void ExtractConnectedComponents();
        };


        /** The special IR is called as Triplet IR where each statement corresponds to a triplet.
         * First element of a triplet -- a store operation related to output
         * Second element of a triplet -- a load operation related to output
         * Third element of a triplet -- a vector of pair-wise loads related to MAC operations
         */
        typedef struct TripletIRNode {
            //vector of loads of all candidate (parsable) statements
            vector<const Load *> initial_loads_;

            //vector of pair wise loads of MACs of all candidate (parsable) statements
            vector<pair<const Load *, const Load *>> pair_wise_loads_;

            //vector of stores of all candidate (parsable) statements
            vector<const Store *> stores_;


            vector<AIVectorMacOperation *> fused_vector_macs;
        } TripletIRNode;

        /** This class is a front-end parser to find a set of suitable statements
         * inside the halide program, and convert them into a special IR to ease out analysis,
         * transformations, and code generation for AI core.
         * We expect statements of form: Output = Output + Sum of MAC operations
         * The special IR is called as Triplet IR where each statement corresponds to a triplet.
         * First element of a triplet -- a store operation related to output
         * Second element of a triplet -- a load operation related to output
         * Third element of a triplet -- a vector of pair-wise loads related to MAC operations
         */
        class TripletIRParser : public IRGraphVisitor {
        public:

            TripletIRNode node;

            //Header used to append every debug message
            string debug_header_;

            //Number of SIMD lanes (derived from vector loads/store/MAC operations)
            int num_simd_lanes_ = -1;

            //Size of data type in bits (derived from operand data types)
            int data_type_size_in_bits_ = -1;

            //Length of strides of memory operations (derived from loads/stores of all statements)
            int memory_operations_stride_length_ = -1;

            //Function to return number of simd columns (leveraging post-add)
            int GetNumSIMDColumns();

        protected:
            using IRVisitor::visit;

            /** Implementing virtual methods of IRGraphVisitor to parse halide statements
             * to generate Triplet IR
             */
            void visit(const Mul *op) override;

            void visit(const Load *op) override;

            void visit(const Add *op) override;

            void visit(const Store *op) override;

            //Temporary variables used while parsing
            const Load *tmp_mac_operand_1_, *tmp_mac_operand_2_;
            bool tmp_which_operand_ = false;
        };

        /** This class acts as a like a compiler for convolutions on a single AI core.
         * It transforms a set of statements into a special IR, then analyze the
         * reuse patterns, transform the computations, and finally generate code
         * leveraging vector intrinsics of a Xilinx AI core.
         */
        class ConvolutionsCompilerForAICore : public CodeGen_C {
        public:
            /** Initialize the compiler including a code generator
             * pointing at a particular output
             * stream (e.g. a file, or std::cout) */
            ConvolutionsCompilerForAICore(std::ostream &dest,
                              Target target,
                              OutputKind output_kind);

        protected:

            //Header used to append every debug message
            string debug_header_;

            //Number of SIMD lanes (derived from vector loads/store/MAC operations)
            int num_simd_lanes_ = -1;

            //Size of data type in bits (derived from operand data types)
            int data_type_size_in_bits_ = -1;

            //Number of SIMD columsn per lane
            // (derived from SIMD lanes and data type)
            int num_simd_columns_per_lane_ = -1;

            //Length of strides of memory operations (derived from loads/stores of all statements)
            int memory_operations_stride_length_ = -1;

            //Total loads in the innermost loop
            int total_loads_ = 0;

            //Total stores in the innermost loop
            int total_stores_ = 0;

            //Total MAC operations in the innermost loop
            int total_mac_operations_ = 0;

            int is_temporal_logical_group_smaller_than_256b_ = -1;

            //Using visit functions of c code generator in our code generators
            using CodeGen_C::visit;

            //Implemeting virtual method "visit" taking ramp as input
            void visit(const Ramp *op) override;

            //Implemeting virtual method "visit" taking call operation as input
            void visit(const Call *op) override;

            //Implemeting virtual method "visit" taking assert statement as input
            void visit(const AssertStmt *op) override;

            //Implemeting virtual method "visit" taking for-loop as input
            void visit(const For *op) override;

            //Implemeting virtual method "visit" taking block of statements as input
            void visit(const Block *block) override;

            //Implemeting virtual method "visit" taking min statement as input
            void visit(const Min *op) override;


            //Step-1 of compiler: Extract straight line of code
            void ExtractStatementsFromBlock(
                    const Block *op, bool &does_block_has_straight_line_of_code,
                    vector<Stmt> &statements);

            //Step-2 of compiler: Parse statements
            void ParseStatements(
                    vector<Stmt> &statements,
                    vector<TripletIRNode> &statements_in_triplet_form);

            //Step-3: Fuse statements writing to same output to reduce writes
            // back to memory
            void FuseStatementsWritingToSameOutput(
                    vector<TripletIRNode> &statements_in_triplet_form,
                    vector<TripletIRNode> &statements_after_fusion);

            //Step-4: Perform reuse analysis
            ReuseGraph BuildTemporalReuseGraphAndLogicalMemoryGroups(
                    vector<TripletIRNode> &statements);

            ReuseGraph BuildSpatialReuseGraphAndLogicalMemoryGroups(
                    vector<TripletIRNode> &statements);

            //Step-5: Fuse vector MAC operations to leverage post-add
            void FuseVectorMacOperations(vector<TripletIRNode> &statements,
                    ReuseGraph &temporal_reuse_graph, ReuseGraph
                    &spatial_reuse_graph);

            //Step-6: Finally, generate code
            void GenerateCode(vector<TripletIRNode> &statements,
                              ReuseGraph &temporal_reuse_graph,
                              ReuseGraph &spatial_reuse_graph,
                              bool eliminate_initial_loads = true);

            void PrintStatementsInTripletForm(
                    vector<TripletIRNode> &statements);

            int GetReuseAmongVectorLoads(
                    const Load *a, const Load *b);

            int GetSpatialDistanceAmongVectorBroadcasts(
                    const Load *a, const Load *b);


            LogicalMemoryGroup* GetLogicalMemoryGroupInformationGivenALoad(
                    const Load *load, ReuseGraph &temporal_reuse_graph,
                    ReuseGraph &spatial_reuse_graph,
                    bool &is_the_load_in_temporal_reuse_graph,
                    int &target_load_offset_from_group_base_address);

            LogicalMemoryGroup *
            GetLogicalMemoryGroupInformationGivenTheNameOfGroup(
                    string target_group_name,
                    ReuseGraph &temporal_reuse_graph,
                    ReuseGraph &spatial_reuse_graph,
                    bool &is_the_load_in_temporal_reuse_graph);

            void ValidateOffsetsAndReturnStepOfFusedMACOperations(
                    AIVectorMacOperation* fused_vector_mac,
                    int &mac_operand1_offset_step,
                    int &mac_operand2_offset_step);


            void DeclareAndInitializeLoads(
                    LogicalMemoryGroup *load_logical_memory_group,
                    int load_offset_from_logical_memory_group_base_address,
                    bool is_temporal,
                    string load_name);


            void BuildRequiredVectorDataTypes(
                    string &mac_intrinsic_name_in_string,
                    string &mul_intrinsic_name_in_string,
                    string &accumulator_type_in_string,
                    string &operand_type_in_string);

            void GetPermuteCoefficients(
                    int offset_from_memory_group_base,
                    int offset_step,
                    bool is_operand_load_in_temporal_group,
                    string &start,
                    string &offset_lo,
                    string &offset_hi,
                    string &step,
                    string &mini_square);

            void convert_statements_in_block_to_vector_of_statements(
                    Stmt s, vector<Stmt> &v);

            vector<Stmt> convert_statements_in_block_to_vector_of_statements(
                    Stmt s);


        };
    }

}
#endif //HALIDE_CODEGEN_AI_CORE_H
