//
// (c) Copyright 2019 Xilinx Inc. All Rights Reserved.
// Created by pchatara on 5/21/19.
//


#include <utility>
#include <iostream>
#include <limits>
#include <fstream>
#include "ConvolutionsCompilerForAICore.h"
#include "CodeGen_Internal.h"
#include "IROperator.h"
#include "Simplify.h"
#include "Substitute.h"
#include "IREquality.h"

using namespace std;

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}



namespace Halide {
    namespace Internal {

        /** This method returns the stride length of a memory operations, for,
         * e.g., contiguous load --> stride length = 1
         */
        int ExtractStrideLengthOfAMemoryOperation(const Expr &e) {
          const auto *r = e.as<Ramp>();
          if (r == nullptr) {
            return -1;
          }

          const auto *i = r->stride.as<IntImm>();
          if (i != nullptr) {
            return static_cast<int>(i->value);
          }

          return -1;
        }

        string GetHexValueOfaDigit(int digit){
            assert(digit >= 0 && digit < 16);
            if(digit <= 9)
                return to_string(digit);
            else if(digit == 10)
                return "A";
            else if(digit == 11)
                return "B";
            else if(digit == 12)
                return "C";
            else if(digit == 13)
                return "D";
            else if(digit == 14)
                return "E";
            else if (digit == 15)
                return "F";
            else
                assert(false);
        }

        /** Implementing virtual method of IRGraphVisitor to extract
         * a pair wise load of a MAC operation.
         */
        void TripletIRParser::visit(const Mul *op) {
            op->a.accept(this);
            op->b.accept(this);
            this->node.pair_wise_loads_.emplace_back(tmp_mac_operand_1_,
                                                     tmp_mac_operand_2_);

            debug(4) << debug_header_ <<"Pair wise load: "
                     << tmp_mac_operand_1_->name << ", "
                     << tmp_mac_operand_2_->name << "\n";
        }

        /** Implementing virtual method of IRGraphVisitor to extract
         * information related to a load operation.
         */
        void TripletIRParser::visit(const Load *op) {
            tmp_which_operand_ == 0 ? tmp_mac_operand_1_ = op
                                    : tmp_mac_operand_2_ = op;

          //Consider the load if it is only a vector load (including scalar broadcast)
          if (op->index.type().is_vector()) {
            int load_operation_simd_lanes = op->index.type().lanes();
            if(num_simd_lanes_ == -1 || data_type_size_in_bits_ == -1) {
              num_simd_lanes_ = load_operation_simd_lanes;
              data_type_size_in_bits_ = op->type.bits();
            } else {
              assert(num_simd_lanes_ == load_operation_simd_lanes);
              assert(data_type_size_in_bits_ == op->type.bits());
            }

            int load_operation_stride_length =
                    ExtractStrideLengthOfAMemoryOperation(op->index);
            if(load_operation_stride_length == -1) {
              debug(0) << debug_header_ << "[Parser Error]: Loads "
                       << "should only be constant strided -- "
                       << op->name << "\n";
              assert(false);
            } else {
              if(this->memory_operations_stride_length_ == -1) {
                this->memory_operations_stride_length_
                        = load_operation_stride_length;
              } else if(this->memory_operations_stride_length_
                        != load_operation_stride_length) {
                debug(0) << debug_header_ << "[Parser Error]: All loads "
                         << "should have same constant stride length -- "
                         << op->name << "\n";
                assert(false);
              }
            }
          }

          tmp_which_operand_ = !tmp_which_operand_;
        }

        /** Implementing virtual method of IRGraphVisitor to extract
         * information related to a initial load operation.
         */
        void TripletIRParser::visit(const Add *op) {

            //Case1: Both operands are multipliers
            if(op->a.as<Mul>() && op->b.as<Mul>()) {
                op->a.accept(this);
                op->b.accept(this);
                return;
            }

            //Case2: One operand is multiplier and another is load
            const Load *initial_load;
            if(op->a.as<Mul>() && op->b.as<Load>()) {
                initial_load = op->b.as<Load>();
                op->a.accept(this);
            } else if (op->a.as<Load>() && op->b.as<Mul>()) {
                initial_load = op->a.as<Load>();
                op->b.accept(this);
            } else {
                debug(0) << debug_header_ << "[Parser Error]: Atleast one "
                         << "one operand of non-MAC operation should be a "
                            "load\n";
                assert(false);
            }


            if(!initial_load->index.type().is_vector()) {
                debug(0) << debug_header_ << "[Parser Error]: Load "
                         << "should be a vector -- " << initial_load->name
                         << "\n";
                assert(false);
            }

            if(!strided_ramp_base(initial_load->index, 1).defined()) {
                debug(0) << debug_header_ << "[Parser Error]: Initial "
                         << "load/store should be only unit strided -- "
                         << initial_load->index << "\n";
                assert(false);
            }

            this->node.initial_loads_.push_back(initial_load);

            //Assumption: We currently handle only one initial value in a statement
            assert(this->node.initial_loads_.size() == 1);

            debug(4) << debug_header_ << "Initial value load: "
                     << this->node.initial_loads_[0]->name  << ","
                     << this->node.initial_loads_[0]->index << "\n";
        }

        /** Implementing virtual method of IRGraphVisitor to extract
         * information related to a store operation.
         */
        void TripletIRParser::visit(const Store *op) {
            if(!op->index.type().is_vector()) {
                debug(0) << debug_header_ << "[Parser Error]: Store operation "
                         << "should be a vector operation -- "
                         << op->name << "\n";

                assert(false);
            }


            if(!strided_ramp_base(op->index, 1).defined()) {
                debug(0) << debug_header_ << "[Parser Error]: Initial "
                         << "load/store should be only unit strided -- "
                         << op->index << "\n";
                assert(false);
            }

            this->node.stores_.push_back(op);

            //Assumption: We currently handle only one store in a statement
            assert(this->node.stores_.size() == 1);

            op->value.accept(this);
        }

        /** Method to return number of SIMD columns available in the hardware
         * based on the number of SIMD lanes and operand data types.
         */
        int TripletIRParser::GetNumSIMDColumns() {

            int num_simd_columns = 0;
            if(data_type_size_in_bits_ == 32) {
                assert(num_simd_lanes_ == 8);
                num_simd_columns = 1;
            } else if(data_type_size_in_bits_ == 16) {
                assert(num_simd_lanes_ == 8 || num_simd_lanes_ == 16);
                num_simd_columns = 32 / num_simd_lanes_;
            } else {
                debug(0) << debug_header_ << "We don't handle any other "
                         << "precision yet:\n";
                assert(false);
            }

            assert(num_simd_columns >= 1);
            return num_simd_columns;
        }

        //Method to do DFS exploration recursively
        void ReuseGraph::DFSUtil(int v, bool visited[], vector<int> &group) {
          // Mark the current node as visited and print it
          visited[v] = true;
          group.push_back(v);

          // Recur for all the vertices
          // adjacent to this vertex
          list<int>::iterator i;
          for(i = adjacency_list_[v].begin();
              i != adjacency_list_[v].end(); ++i) {
            if (!visited[*i])
              DFSUtil(*i, visited, group);
          }
        }

        // Constructor to initialize reuse graph class
        ReuseGraph::ReuseGraph(int V, vector<const Load*> loads) {
          this->num_elements_ = V;
          adjacency_list_ = new list<int>[V];
          this->loads_in_the_reuse_graph_ = std::move(loads);
        }

        //Method to add an edge to the reuse graph
        void ReuseGraph::AddEdge(int v, int w) {
          adjacency_list_[v].push_back(w);
          adjacency_list_[w].push_back(v);
        }

        //This method is used to extract connected components using DFS
        void ReuseGraph::ExtractConnectedComponents() {
          auto *visited = new bool[num_elements_];
          for(int v = 0; v < num_elements_; v++)
            visited[v] = false;

          for (int v=0; v < num_elements_; v++) {
            if (!visited[v]) {
              LogicalMemoryGroup g;
              DFSUtil(v, visited, g.ids);
              logical_memory_groups_.push_back(g);
            }
          }
        }

        /** A helper for block_to_vector below. */
        void ConvolutionsCompilerForAICore::
          convert_statements_in_block_to_vector_of_statements(Stmt s,
                                                vector<Stmt> &v) {
            const auto *b = s.as<Block>();
            if (!b) {
                v.push_back(s);
            } else {
              convert_statements_in_block_to_vector_of_statements(b->first, v);
              convert_statements_in_block_to_vector_of_statements(b->rest, v);
            }
        }

        /** Unpack a block into its component Stmts. */
        vector<Stmt> ConvolutionsCompilerForAICore::
          convert_statements_in_block_to_vector_of_statements(Stmt s) {
            vector<Stmt> result;
            convert_statements_in_block_to_vector_of_statements(s, result);
            return result;
        }

        /** This function returns number of elements in common to both vector loads*/
        int ConvolutionsCompilerForAICore::GetReuseAmongVectorLoads(
                const Load *a, const Load *b) {
          if(a->name == b->name) {
            const auto *load_a_ramp = a->index.as<Ramp>();
            const auto *load_b_ramp = b->index.as<Ramp>();
            int load_a_ramp_stride = ExtractStrideLengthOfAMemoryOperation(
                    a->index);
            int load_b_ramp_stride = ExtractStrideLengthOfAMemoryOperation(
                    b->index);

            assert(load_a_ramp->lanes == load_b_ramp->lanes);
            assert(load_a_ramp_stride == load_b_ramp_stride);

            Expr base_expression_diff = load_a_ramp->base - load_b_ramp->base;
            base_expression_diff = simplify(base_expression_diff);
            const auto *diff_int = base_expression_diff.as<IntImm>();
            if (diff_int) {
              int64_t offset = 0;
              if(diff_int->value > 0)
                offset = (diff_int->value);
              else
                offset = (-1 * diff_int->value);

              int64_t reuse = (load_a_ramp->lanes - offset);

              //TODO: Check this logic later about number of elements in common.
              if(reuse > 0 && reuse % load_a_ramp_stride == 0)
                reuse = reuse / load_a_ramp_stride;
              else if(reuse > 0)
                reuse = reuse / load_a_ramp_stride + 1;

              debug(2) << "Number of Reuse elements b/w"
                       << Expr(a) << " and " << Expr(b) << " are " << reuse
                       << " , " <<offset << "\n";
              if(reuse > 0)
                return (int) reuse;
              else
                return 0;
            } else {
                return 0;
            }
          } else {
              return 0;
          }
        }

        /**This function returns number of elements in common (spatial
         * distance) if these two vector broadcast alias scalar loads are on
         * the same vector register
         */
        int ConvolutionsCompilerForAICore::GetSpatialDistanceAmongVectorBroadcasts(
                const Load *a, const Load *b) {
          if(a->name == b->name) {
            Expr diff = a->index - b->index;
            diff = simplify(diff);
            const auto *diff_int = diff.as<IntImm>();
            if (diff_int) {
              int64_t offset = 0;
              if(diff_int->value > 0)
                offset = diff_int->value;
              else
                offset = -1 * diff_int->value;
              int64_t spatial_distance = offset;
              debug(2) << "Spatial distance b/w"
                       << Expr(a) << " and " << Expr(b) << " are " << spatial_distance
                       << " , " <<offset << "\n";
              auto a_remainder = a->alignment.remainder;
              auto b_remainder = b->alignment.remainder;
              if(spatial_distance >= 0
                 && a_remainder/num_simd_lanes_
                    == b_remainder/num_simd_lanes_)
                return (int) spatial_distance;
              else
                return -1;
            } else {
                return -1;
            }
          } else {
              return -1;
          }
        }

      /** This function builds logical groups where each group consists of
       * vector load operations, and each group carries either a full or
       * partial reuse of vector registers used.
       * The goal is to create a single large vector register and use
       * permute network to help with all these load operations.
       */
        ReuseGraph ConvolutionsCompilerForAICore::
            BuildTemporalReuseGraphAndLogicalMemoryGroups(
              vector<TripletIRNode> &statements) {

          //Step-1: Collect all vector load operations
          vector<const Load *> loads;
          for(auto &statement:statements) {
            //loads.push_back(statement.initial_loads_[0]);
            for (pair<const Load *, const Load *> &mac
                    : statement.pair_wise_loads_) {
              if (mac.first->type.is_vector() &&
                  ExtractStrideLengthOfAMemoryOperation(mac.first->index) != -1)
                loads.push_back(mac.first);

              if (mac.second->type.is_vector() &&
                  ExtractStrideLengthOfAMemoryOperation(mac.second->index) != -1)
                loads.push_back(mac.second);
            }
          }

          debug(2) << debug_header_ <<"Total Num of loads responsible for "
                   << "temporal reuse: " << loads.size() << "\n";

          auto num_loads = (int) loads.size();
          if(num_loads == 0)
            return ReuseGraph(0, {});

          //Step-2: Iterate pairwise to form group using input (RAR) reuse distance
          ReuseGraph reuse_graph(num_loads, loads);
          for(int i = 0; i < num_loads; i++) {
            for(int j = i+1; j < num_loads; j++) {
              int reuse = GetReuseAmongVectorLoads(loads[i], loads[j]);
              if(reuse > 0) {
                reuse_graph.AddEdge(i, j);
              }
            }
          }

          //Step-3: Identify connected componeent, i.e., logical groups
          reuse_graph.ExtractConnectedComponents();

          //Step-4: Compute extent of each group to create a vector register for each group
          for (LogicalMemoryGroup  &group : reuse_graph.logical_memory_groups_) {
            group.extent = Interval::nothing();
            for(int &id : group.ids) {
              Interval interval;
              const Load *load = loads[id];
              interval.min = load->index.as<Ramp>()->base;
              interval.max = load->index.as<Ramp>()->base
                            + (load->index.type().lanes()-1) *
                            ExtractStrideLengthOfAMemoryOperation(load->index);
              group.extent.include(interval);
            }

            group.extent.min = simplify(group.extent.min);
            group.extent.max = simplify(group.extent.max);
            debug(2) << debug_header_ << "Group min: " << group.extent.min
                     << ", Max: " << group.extent.max << "\n";

            Expr extent_size = simplify(group.extent.max - group.extent.min+1);
            const auto *diff_int = extent_size.as<IntImm>();
            if (diff_int) {
              group.extent_size = (int) diff_int->value;
              debug(2) << debug_header_ <<"Group extent size: "
                       << group.extent_size << "\n";
              int max_size = 1024 / (data_type_size_in_bits_);
              if(group.extent_size > max_size){
                debug(0) << debug_header_ <<"[Reuse Analysis Error]: "
                        << "More than 1024 bits are required to "
                           "allocate a register for the logical "
                           "group:\n";
                assert(false);
              }
            } else {
                debug(0) << debug_header_ <<"Extent size of a group is not an"
                         << " integer\n";
                assert(false);
            }
          }

          //Step-5: Analze the alignment of a big vector register to satisfy all loads
          // in a group
          for (LogicalMemoryGroup  &group : reuse_graph.logical_memory_groups_) {
            Expr min = group.extent.min;
            ModulusRemainder alignment = modulus_remainder(min);
            debug(2) << debug_header_ <<"Align: Mod: "
                     << alignment.modulus << ", rem: " << alignment.remainder <<"\n";

            int requirement_alignment = 128 / data_type_size_in_bits_;

            //We are assuming that the extent is always well aligned.
            if(!(alignment.modulus == 1 && alignment.remainder == 0)) {
              if (alignment.modulus % requirement_alignment != 0) {
                debug(0) << debug_header_ << "Unaligned groups are "
                                             "not currently supported.\n";
                assert(false);
              }
            }
          }


          //Step-6: Mapping from original loads to corresponding large vector with permute details
          int count = 0;
          string logical_memory_group_name = "loads_temporal_group_";
          for (LogicalMemoryGroup  &group : reuse_graph.logical_memory_groups_) {
            group.name = logical_memory_group_name + to_string(count++);
            for(int &id : group.ids) {
              const Load *load = loads[id];
              Expr offset = simplify(load->index.as<Ramp>()->base - group.extent.min);
              const auto *diff_int = offset.as<IntImm>();
              if (diff_int) {
                  debug(2) << debug_header_ << "Offset from min of the group "
                       + group.name +" extent is: " << diff_int->value << "\n";
                  group.offsets.push_back((int) diff_int->value);
              } else {
                  debug(0) << debug_header_ << "Non-integral offset\n";
                  assert(false);
              }
            }
          }

          for (LogicalMemoryGroup  &group : reuse_graph.logical_memory_groups_) {
            debug(3) << debug_header_ << "Offsets in the group: ";
            for(auto &offset : group.offsets) {
              debug(3) << offset << "\t";
            }

            debug(3) << "\n";
          }

            return reuse_graph;
        }

      /** This function builds logical groups where each group consists of broadcast
       * load operations, and each group carries either a full reuse of
       * vector registers used.
       * The goal is to create a single large vector regiser and use permute
       * network to help with all these load operations.
       */
        ReuseGraph ConvolutionsCompilerForAICore::
            BuildSpatialReuseGraphAndLogicalMemoryGroups(
              vector<TripletIRNode> &statements) {

            //Step-1: Collect all broadcast load operations
            vector<const Load *> loads;
            for(auto &statement : statements) {
              for (pair<const Load *, const Load *> &mac
                      : statement.pair_wise_loads_) {
                if (mac.first->type.is_scalar())
                  loads.push_back(mac.first);

                if (mac.second->type.is_scalar())
                  loads.push_back(mac.second);
              }
            }

            debug(2) << debug_header_ <<"Total Num of loads responsible for "
                     << "spatial reuse: " << loads.size() << "\n";

            auto num_loads = (int) loads.size();
            if(!num_loads)
              return ReuseGraph(0, {});

            //Step-2: Iterate pairwise to form group using input (RAR) reuse distance
            ReuseGraph reuse_graph(num_loads, loads);
            for(int i = 0; i < num_loads; i++) {
              for(int j = i+1; j < num_loads; j++) {
                int spatial_distance = GetSpatialDistanceAmongVectorBroadcasts(
                        loads[i], loads[j]);
                if(spatial_distance >= 0) {
                  reuse_graph.AddEdge(i, j);
                }
              }
            }

            //Step-3: Identify connected components, i.e., logical groups
            reuse_graph.ExtractConnectedComponents();



            //Step-4: Compute extent of each group to create a vector register for each group
            for (LogicalMemoryGroup  &group : reuse_graph.logical_memory_groups_) {
              group.extent = Interval::nothing();
              for(int &id : group.ids) {
                Interval interval;
                const Load *load = loads[id];
                interval.min = load->index;
                int remainder = (int)load->alignment.remainder% num_simd_lanes_;
                interval.max = load->index + num_simd_lanes_-remainder-1;
                debug(3) << debug_header_ << "Interval: " << interval.min
                         << ", " << interval.max << "\n";
                group.extent.include(interval);
              }

              group.extent.min = simplify(group.extent.min);
              group.extent.max = simplify(group.extent.max);
              debug(2) << debug_header_ << "Group min: " << group.extent.min
                       << ", Max: " << group.extent.max << "\n";

              Expr extent_size = simplify(group.extent.max - group.extent.min+1);
              const auto *diff_int = extent_size.as<IntImm>();
              if (diff_int) {
                group.extent_size = (int) diff_int->value;
                debug(2) << debug_header_ <<"Group extent size: "
                         << group.extent_size << "\n";
                int max_size = 1024 / (data_type_size_in_bits_);
                if(group.extent_size > max_size){
                  debug(0) << debug_header_ <<"[Reuse Analysis Error]: "
                           << "More than 1024 bits are required to "
                              "allocate a register for the logical "
                              "group:\n";
                  assert(false);
                }
              } else {
                debug(0) << debug_header_ <<"Extent size of a group is not an"
                         << " integer\n";
                assert(false);
              }
            }

            //Step-5: Analyze the alignment of a big vector register to
            // satisfy all loads in a group
            for (LogicalMemoryGroup  &group : reuse_graph.logical_memory_groups_) {
              Expr min = group.extent.min;
              ModulusRemainder alignment = modulus_remainder(min);
              debug(2) << debug_header_ <<"Align: Mod: " << alignment.modulus
                       << ", rem: " << alignment.remainder <<"\n";

              //We are assuming that the extent is always well aligned.
              int requirement_alignment = 128 / data_type_size_in_bits_;

              //We are assuming that the extent is always well aligned.
              if(!(alignment.modulus == 1 && alignment.remainder == 0)) {
                if (alignment.modulus % requirement_alignment != 0) {
                  debug(0) << debug_header_ << "Unaligned groups are "
                                               "not currently supported.\n";
                  assert(false);
                }
              }
            }

            //Step-6: Mapping from original loads to corresponding large vector with permute details
            int count = 0;
            string logical_group_name = "loads_spatial_group_";
            for (LogicalMemoryGroup  &group : reuse_graph.logical_memory_groups_) {
              group.name = logical_group_name + to_string(count++);
              for(int &i : group.ids) {
                const Load *load = loads[i];
                Expr offset = simplify(load->index - group.extent.min);
                const auto *diff_int = offset.as<IntImm>();
                if (diff_int) {
                  debug(2) << debug_header_ << "Offset from min of the group "
                         + group.name +" extent is: " << diff_int->value << "\n";
                    group.offsets.push_back((int) diff_int->value);
                } else {
                    debug(0) << debug_header_ << "Non-integral offset\n";
                    assert(false);
                }
              }
            }

            for (LogicalMemoryGroup  &group : reuse_graph.logical_memory_groups_) {
              debug(3) << debug_header_ << "Offsets in the group: ";
              for(auto &offset : group.offsets) {
                debug(3) << offset << "\t";
              }

              debug(3) << "\n";
            }

            return reuse_graph;
        }


        void ConvolutionsCompilerForAICore::DeclareAndInitializeLoads(
                LogicalMemoryGroup *load_logical_memory_group,
                int load_offset_from_logical_memory_group_base_address,
                string load_name) {

            bool is_load_larger_than_vector_length
                    = load_logical_memory_group->extent_size > num_simd_lanes_;

            //Load range start and end are multiples of vector length
            int load_range_start =
                  (load_offset_from_logical_memory_group_base_address) / num_simd_lanes_;

            int load_range_end = 0;
            if(is_load_larger_than_vector_length)
              load_range_end =
                      (load_offset_from_logical_memory_group_base_address
                       + memory_operations_stride_length_*
                          (num_simd_lanes_-1)) / num_simd_lanes_;
            else
              load_range_end = load_range_start;

            vector<int> unallocated_load_ranges;
            for(int i = load_range_start; i <= load_range_end; i++) {
              if(find(load_logical_memory_group->allocations.begin(),
                      load_logical_memory_group->allocations.end(), i)
                      == load_logical_memory_group->allocations.end()) {
                unallocated_load_ranges.push_back(i);
              }
            }

            if(unallocated_load_ranges.empty())
              return;

            string operand_type_in_string = "v" + to_string(num_simd_lanes_)
                                  + "int" + to_string(data_type_size_in_bits_);

            string buffer_type_in_string = "v";
            if(load_logical_memory_group->extent_size
                    <=  num_simd_lanes_ ) {
              buffer_type_in_string += to_string(num_simd_lanes_);
            } else if (load_logical_memory_group->extent_size
                     <=  2 * num_simd_lanes_ ) {
              buffer_type_in_string += to_string(2 * num_simd_lanes_);
            } else if (load_logical_memory_group->extent_size
                     <=  4 * num_simd_lanes_ ) {
              buffer_type_in_string += to_string(4 * num_simd_lanes_);
            } else {
              debug(0) << debug_header_ << "Buffer cannot be allocated "
                                           "because of larger size\n";
              assert(false);
            }

            buffer_type_in_string += "int" + to_string(data_type_size_in_bits_);

            if(is_load_larger_than_vector_length
               && load_logical_memory_group->allocations.empty()) {
              do_indent();
              stream << buffer_type_in_string << " "
                     << load_logical_memory_group->name
                     << " = undef_" << buffer_type_in_string << "(); \n";
            }

            if(is_load_larger_than_vector_length) {
              for(auto &i : unallocated_load_ranges) {
                string pointer = "*(" + operand_type_in_string + "* ) ("
                                 + print_name(load_name + "_buffer")
                                 + "+" + print_expr(load_logical_memory_group->extent.min)
                                 + "+" + to_string(num_simd_lanes_*i)
                                 +")";

                do_indent();
                stream << load_logical_memory_group->name << " = "
                       << "upd_w(" << load_logical_memory_group->name
                       << ", " << i << ", "
                       << pointer << "); \n";
                load_logical_memory_group->allocations.push_back(i);

                total_loads_++;
              }
            } else {
                assert(unallocated_load_ranges.size() == 1);
                string pointer = "*(" + operand_type_in_string + "* ) ("
                                 + print_name(load_name + "_buffer")
                                 + "+"
                                 + print_expr(load_logical_memory_group->extent.min)
                                 +")";
                do_indent();
                stream << operand_type_in_string << " "
                       << load_logical_memory_group->name
                       << " = " << pointer <<"; \n";
                load_logical_memory_group->allocations.push_back(
                        unallocated_load_ranges[0]);

              total_loads_++;
            }
        }

        /**This function returns the logical memory group of a given target
         * load. The load can be in either of two groups -- temporal and
         * spatial reuse graphs.
         * Inputs: Target load, temporal reuse graph, spatial reuse graph
         * Outputs: Logical group corresponding to the load, whether the load
         * is in temporal or spatial reuse graph, how far is the load offset
         * from the logical group base address.
         */
        LogicalMemoryGroup * ConvolutionsCompilerForAICore::
          GetLogicalMemoryGroupInformationGivenALoad(
            const Load *target_load, ReuseGraph &temporal_reuse_graph,
            ReuseGraph &spatial_reuse_graph,
            bool &is_the_load_in_temporal_reuse_graph,
            int &target_load_offset_from_group_base_address) {

          int result_id = -1;
          LogicalMemoryGroup *logical_memory_group = nullptr;

          //Step-1: Get id of the load to index into a memory group
          int counter = 0;
          for(auto &load: temporal_reuse_graph.loads_in_the_reuse_graph_) {
            if(load == target_load) {
              result_id = counter;
              is_the_load_in_temporal_reuse_graph = true;
              break;
            }

            counter++;
          }

          //If the load is in temporal reuse graph, then get its group!
          if(result_id != -1) {
            for (auto &group: temporal_reuse_graph.logical_memory_groups_) {
              for(unsigned i = 0; i < group.ids.size(); i++) {
                if(result_id == group.ids[i]) {
                  target_load_offset_from_group_base_address = group.offsets[i];
                  logical_memory_group = &group;
                  return logical_memory_group;
                }
              }
            }
          }

          counter = 0;
          for(auto &load: spatial_reuse_graph.loads_in_the_reuse_graph_) {
            if(load == target_load) {
              result_id = counter;
              is_the_load_in_temporal_reuse_graph = false;
              break;
            }

            counter++;
          }

          //If the load is in spatial reuse graph, then get its group!
          if(result_id != -1) {
            for (auto &group: spatial_reuse_graph.logical_memory_groups_) {
              for(unsigned i = 0; i < group.ids.size(); i++) {
                if(result_id == group.ids[i]) {
                  target_load_offset_from_group_base_address = group.offsets[i];
                  logical_memory_group = &group;
                  return logical_memory_group;
                }
              }
            }
          }

          return logical_memory_group;
        }

        LogicalMemoryGroup * ConvolutionsCompilerForAICore::
        GetLogicalMemoryGroupInformationGivenTheNameOfGroup(
                string target_group_name, ReuseGraph &temporal_reuse_graph,
                ReuseGraph &spatial_reuse_graph,
                bool &is_the_load_in_temporal_reuse_graph) {

          for(auto &group : temporal_reuse_graph.logical_memory_groups_) {
            if(group.name == target_group_name) {
              is_the_load_in_temporal_reuse_graph = true;
              return &group;
            }
          }

          for(auto &group : spatial_reuse_graph.logical_memory_groups_) {
            if(group.name == target_group_name) {
              is_the_load_in_temporal_reuse_graph = false;
              return &group;
            }
          }

          return nullptr;
        }

        void ConvolutionsCompilerForAICore
            ::ValidateOffsetsAndReturnStepOfFusedMACOperations(
                AIVectorMacOperation* fused_vector_mac,
                int &mac_operand1_offset_step,
                int &mac_operand2_offset_step) {

          if (fused_vector_mac->operand_offsets.size()
              % num_simd_columns_per_lane_ != 0) {
            debug(0) << debug_header_ << "All columns of a SIMD lane "
                     << "are not used\n";
            assert(false);
          }

          auto mac_counter = 0;
          auto mac_operand1_offset_prev = -1, mac_operand2_offset_prev = -1;
          auto mac_operand1_offset_step_prev = -1;
          auto mac_operand2_offset_step_prev = -1;
          int mac_operand1_offset_step_cur;
          int mac_operand2_offset_step_cur;
          for(auto &vector_mac: fused_vector_mac->operand_offsets) {
            if(mac_counter % num_simd_columns_per_lane_ != 0) {
              if(mac_counter % num_simd_columns_per_lane_ == 1) {
                mac_operand1_offset_step_prev
                        = vector_mac.first - mac_operand1_offset_prev;
                mac_operand2_offset_step_prev
                        = vector_mac.second - mac_operand2_offset_prev;
              } else {
                mac_operand1_offset_step_cur
                        = vector_mac.first - mac_operand1_offset_prev;
                mac_operand2_offset_step_cur
                        = vector_mac.second - mac_operand2_offset_prev;
                if(mac_operand1_offset_step_cur
                   != mac_operand1_offset_step_prev) {
                  debug(0) << debug_header_ << "Non-uniform "
                           << "step size in first operand of a vector mac "
                              "operation for multiple SIMD columns\n";
                  assert(false);
                }

                if(mac_operand2_offset_step_cur
                   != mac_operand2_offset_step_prev) {
                  debug(0) << debug_header_ << "Non-uniform "
                           << "step size in second operand of a vector mac "
                              "operation for multiple SIMD columns\n";
                  assert(false);
                }
              }
            }

            mac_operand1_offset_prev = vector_mac.first;
            mac_operand2_offset_prev = vector_mac.second;
            mac_counter++;
          }

          mac_operand1_offset_step = mac_operand1_offset_step_prev;
          mac_operand2_offset_step = mac_operand2_offset_step_prev;

          debug(3) << debug_header_ << "MAC operand1 step: " <<
                   mac_operand1_offset_step;

          debug(3) << debug_header_ << "MAC operand2 step: " <<
                   mac_operand2_offset_step;

          debug(3) << debug_header_ << "SIMD columns: " <<
                   num_simd_columns_per_lane_;
        }

        void ConvolutionsCompilerForAICore
            ::GetPermuteCoefficients(
                    int offset_from_memory_group_base,
                    int offset_step,
                    bool is_operand_load_in_temporal_group,
                    string &start,
                    string &offset_lo,
                    string &offset_hi,
                    string &step,
                    string &mini_square) {
          if (num_simd_lanes_ == 8 && data_type_size_in_bits_ == 32) {
            start = to_string(offset_from_memory_group_base);
            offset_lo = is_operand_load_in_temporal_group ? "0x76543210" : "0";
            offset_hi = step = mini_square = "-1";
          } else if (num_simd_lanes_ == 16 && data_type_size_in_bits_ == 16) {
            if(is_operand_load_in_temporal_group) {
              int offset_from_start = offset_from_memory_group_base;
              int access_granularity = 32 / data_type_size_in_bits_;
              int start_in_number_format =
                      (offset_from_memory_group_base / access_granularity)
                      * access_granularity;

              start = to_string(start_in_number_format);
              step = to_string(offset_step);
              offset_from_start -= start_in_number_format;
              if (offset_from_start % 2 != 0) {
                debug(0) << debug_header_ << "Cannot identify suitable xstart "
                         << "since starting element address after xstart "
                         << "is not multiple of 2: " << offset_from_start
                         << "\n";
                assert(false);
              }

              int mini_square_lane_increment;
              if (memory_operations_stride_length_ == offset_step) {
                mini_square = "0x2110";
                mini_square_lane_increment = 0;
              } else if (memory_operations_stride_length_ % 2 == 0) {
                mini_square = "0x3210";
                mini_square_lane_increment =
                        (memory_operations_stride_length_ - 2
                         - 2 * offset_from_start) / 2;
              } else {
                debug(0) << debug_header_ << "Mini square lane increment "
                         << "should be in multiples of 2\n";
                assert(false);
              }

              for (int i = 0; i < 8; i += 2) {
                offset_lo += GetHexValueOfaDigit((offset_from_start
                        + i * memory_operations_stride_length_) / 2)
                        + GetHexValueOfaDigit(mini_square_lane_increment);
              }

              offset_lo += "x0";
              reverse(offset_lo.begin(), offset_lo.end());

              for (int i = 8; i < 16; i += 2) {
                offset_hi += GetHexValueOfaDigit((offset_from_start
                          + i * memory_operations_stride_length_) / 2)
                          + GetHexValueOfaDigit(mini_square_lane_increment);
              }

              offset_hi += "x0";
              reverse(offset_hi.begin(), offset_hi.end());
            } else {
              start = to_string(offset_from_memory_group_base);
              offset_lo = offset_hi = to_string(0);
              mini_square = "-1";
              if(offset_step == -1) {
                debug(0) << "Step increment cannot be -1\n";
                assert(false);
              }
              step = to_string(offset_step);
            }
          }
        }

        void ConvolutionsCompilerForAICore::GenerateCode(
                vector<TripletIRNode> &statements,
                ReuseGraph &temporal_reuse_graph,
                ReuseGraph &spatial_reuse_graph,
                bool eliminate_initial_loads) {

          if(statements.empty())
              return;

          string mac_intrinsic_name_in_string;
          string mul_intrinsic_name_in_string;
          string accumulator_type_in_string;
          string operand_type_in_string;
          BuildRequiredVectorDataTypes(mac_intrinsic_name_in_string,
                                       mul_intrinsic_name_in_string,
                                       accumulator_type_in_string,
                                       operand_type_in_string);

          int stmt_id = 0;
          for(auto &stmt: statements) {
            auto statement_output_name_in_string = "out_" + to_string(stmt_id);

            assert(stmt.initial_loads_.size() == 1);
            if(stmt.pair_wise_loads_.empty())
                continue;

            //Step-1: Accumulator initialization
            auto accumulator_name_in_string =
                    statement_output_name_in_string + "_acc";
            do_indent();
            stream << accumulator_type_in_string << " "
                   << accumulator_name_in_string <<";\n";

            bool use_mul_intrinsic = true;
            for(auto &fused_vector_mac : stmt.fused_vector_macs) {
              int mac_operand1_offset_step = 0;
              int mac_operand2_offset_step= 0;
              ValidateOffsetsAndReturnStepOfFusedMACOperations(
                      fused_vector_mac, mac_operand1_offset_step,
                      mac_operand2_offset_step);

              //Operand1 details:
              bool is_operand1_load_in_temporal_group;
              auto operand1_load = stmt.pair_wise_loads_[0].first;

              LogicalMemoryGroup *operand1_memory_group =
                      GetLogicalMemoryGroupInformationGivenTheNameOfGroup(
                      fused_vector_mac->operand1_group_name,
                      temporal_reuse_graph,
                      spatial_reuse_graph,
                      is_operand1_load_in_temporal_group);


              //Operand2 details:
              bool is_operand2_load_in_temporal_group;
              auto operand2_load = stmt.pair_wise_loads_[0].second;

              LogicalMemoryGroup *operand2_memory_group =
                      GetLogicalMemoryGroupInformationGivenTheNameOfGroup(
                              fused_vector_mac->operand2_group_name,
                              temporal_reuse_graph,
                              spatial_reuse_graph,
                              is_operand2_load_in_temporal_group);

              assert(is_operand1_load_in_temporal_group
                             ^ is_operand2_load_in_temporal_group);

              //Step-2: Perform MAC operations
              for(unsigned int vector_mac_counter = 0;
                  vector_mac_counter < fused_vector_mac->operand_offsets.size();
                  vector_mac_counter++) {
                if(vector_mac_counter % num_simd_columns_per_lane_ == 0) {
                  for(int col = 0; col < num_simd_columns_per_lane_; col++) {
                    auto &vector_mac_col_operands =
                            fused_vector_mac->operand_offsets[vector_mac_counter + col];

                    //Declare and initialize loads on demand
                    DeclareAndInitializeLoads(
                            operand1_memory_group,
                            vector_mac_col_operands.first,
                            operand1_load->name);

                    DeclareAndInitializeLoads(
                            operand2_memory_group,
                            vector_mac_col_operands.second,
                            operand2_load->name);
                  }

                  //Initialize the accumulator
                  do_indent();
                  if (use_mul_intrinsic) {
                    stream << accumulator_name_in_string
                           << " = " << mul_intrinsic_name_in_string << "(";
                    use_mul_intrinsic = false;
                  } else {
                    stream << accumulator_name_in_string << " = "
                           << mac_intrinsic_name_in_string << "("
                           << accumulator_name_in_string << ",";
                  }


                  //Generate operand1 permute coefficients
                  string operand1_start;
                  string operand1_offset_lo;
                  string operand1_offset_hi;
                  string operand1_offset_step;
                  string operand1_mini_square;
                  debug(3) << debug_header_ << "Invoking get permute coeff: "
                           << " offset from group: " <<  fused_vector_mac
                          ->operand_offsets[vector_mac_counter].first
                           << " ,step: " << mac_operand1_offset_step
                           << " ,Temporal load? "
                           << is_operand1_load_in_temporal_group << "\n";
                  GetPermuteCoefficients(
                          fused_vector_mac
                                   ->operand_offsets[vector_mac_counter].first,
                          mac_operand1_offset_step,
                          is_operand1_load_in_temporal_group,
                          operand1_start, operand1_offset_lo,
                          operand1_offset_hi, operand1_offset_step,
                          operand1_mini_square);

                  stream << operand1_memory_group->name << ", ";
                  stream << operand1_start << ", " << operand1_offset_lo;
                  if(!operand1_offset_hi.empty()
                      && operand1_offset_hi != "-1")
                    stream << ", " << operand1_offset_hi;

                  if(!operand1_mini_square.empty()
                      && operand1_mini_square != "-1")
                    stream << ", " << operand1_mini_square;

                  stream << ", ";

                  //Generate operand2 permute coefficients
                  string operand2_start;
                  string operand2_offset_lo;
                  string operand2_offset_hi;
                  string operand2_offset_step;
                  string operand2_mini_square;
                  debug(3) << debug_header_ << "Invoking get permute coeff: "
                           << " offset from group: " <<  fused_vector_mac
                                   ->operand_offsets[vector_mac_counter].second
                           << " ,step: " << mac_operand2_offset_step
                           << " ,Temporal load? "
                           << is_operand2_load_in_temporal_group << "\n";
                  GetPermuteCoefficients(
                          fused_vector_mac
                                  ->operand_offsets[vector_mac_counter].second,
                          mac_operand2_offset_step,
                          is_operand2_load_in_temporal_group,
                          operand2_start, operand2_offset_lo,
                          operand2_offset_hi, operand2_offset_step,
                          operand2_mini_square);

                  stream << operand2_memory_group->name << ", ";
                  stream << operand2_start << ", " << operand2_offset_lo;
                  if(!operand2_offset_hi.empty()
                      && operand2_offset_hi != "-1")
                    stream << ", " << operand2_offset_hi;

                  if(!operand2_offset_step.empty()
                     && operand2_offset_step != "-1")
                    stream << ", " << operand2_offset_step;

                  stream << ");\n";

                  total_mac_operations_++;
                }
              }
            }

            //Perform store operation
            //Initial load is same as store
            const Load* stmt_store = stmt.initial_loads_[0];
            string stmt_store_in_string = "*(" + operand_type_in_string + "*)"
                             +  "("  + print_name(stmt_store->name + "_buffer");
            if(stmt_store->type.is_vector()) {
                stmt_store_in_string += "+" +
                        print_expr(stmt_store->index.as<Ramp>()->base) + ")";
            } else {
              debug(0) << debug_header_ << "A store cannot be a scalar\n";
              assert(false);
            }

            if(!eliminate_initial_loads) {
              do_indent();
              stream << operand_type_in_string << " "
                     << statement_output_name_in_string << " = " <<
                     stmt_store_in_string << "; \n";

              //Add with initial value
              do_indent();
              stream << accumulator_name_in_string << " = operator+("
                     << accumulator_name_in_string
                     << ", " << statement_output_name_in_string << ");\n";
            }

            //Writing back the data... again assuming that they are aligned stores
            do_indent();
            stream  << stmt_store_in_string << " = srs("
                    << accumulator_name_in_string << ", XHALIDE_SRS_SHIFT);\n";

            total_stores_++;
            stmt_id++;
          }
        }

        void ConvolutionsCompilerForAICore::BuildRequiredVectorDataTypes(
          string &mac_intrinsic_name_in_string,
          string &mul_intrinsic_name_in_string,
          string &accumulator_type_in_string,
          string &operand_type_in_string) {
          accumulator_type_in_string= "v" + to_string(num_simd_lanes_) + "acc";
          operand_type_in_string = "v" + to_string(num_simd_lanes_)
                                  + "int" + to_string(data_type_size_in_bits_);
          string buffer_type_in_string = "v" + to_string(2 * num_simd_lanes_)
                              + "int" + to_string(data_type_size_in_bits_);
          if(num_simd_lanes_ == 8 && data_type_size_in_bits_ == 32) {
              accumulator_type_in_string += "80";
              mac_intrinsic_name_in_string = "lmac8";
              mul_intrinsic_name_in_string = "lmul8";
          } else if(num_simd_lanes_ == 16 && data_type_size_in_bits_ == 16) {
              accumulator_type_in_string += "48";
              mac_intrinsic_name_in_string = "mac16";
              mul_intrinsic_name_in_string = "mul16";
          } else if(num_simd_lanes_ == 8 && data_type_size_in_bits_ == 16) {
              accumulator_type_in_string += "48";
              mac_intrinsic_name_in_string = "mac8";
              mul_intrinsic_name_in_string = "mul8";
          } else {
              debug(0) << debug_header_ << "SIMD length " << num_simd_lanes_
                       << " Columns " << num_simd_columns_per_lane_
                       << "are not handled\n";
              assert(false);
          }
        }

        ConvolutionsCompilerForAICore::ConvolutionsCompilerForAICore(
                std::ostream &dest, Target target, OutputKind output_kind)
                : CodeGen_C(dest, target, output_kind)  {
            signal(SIGSEGV, handler);
            debug_header_ = "[AI-CodeGen]: ";
            stream << "// " << debug_header_
                   << " Code generated by XHalide compiler for AI architecture with vector intrinsics.\n";
            stream <<"#include<stdint.h>\n";
            stream <<"#define XHALIDE_SRS_SHIFT 0\n";
        }

        void ConvolutionsCompilerForAICore::visit(const AssertStmt *op) {
            //Do nothing
        }

        void ConvolutionsCompilerForAICore::visit(const For *op) {
            debug(2) << "For-loop type: " << op->for_type << "\n";
            if(op->for_type != ForType::PrepareForSoftwarePipelining) {
                CodeGen_C::visit(op);
            } else {
                string id_min = print_expr(op->min);
                string id_extent = print_expr(op->extent);


                do_indent();
                stream << "for (int "
                       << print_name(op->name)
                       << " = " << id_min
                       << "; "
                       << print_name(op->name)
                       << " < " << id_min
                       << " + " << id_extent
                       << "; "
                       << print_name(op->name)
                       << "++)\n";
                do_indent();

                Expr extent_simplified = substitute_in_all_lets(op->extent);
                const auto *int_extent = extent_simplified.as<IntImm>();
                if(int_extent) {
                    debug(3) << debug_header_ << "Hey, I got the range: " << int_extent->value << "\n";
                    stream << "chess_prepare_for_pipelining chess_loop_range("
                           << to_string(int_extent->value) << ", "
                           << to_string(int_extent->value) << ")\n";
                } else {
                    debug(3) << debug_header_ << "I didn't get the range for pipeline:\n";
                    stream << "chess_prepare_for_pipelining\n";
                }

                open_scope();
                op->body.accept(this);
                close_scope("for " + print_name(op->name));
            }
        }

        void ConvolutionsCompilerForAICore::visit(const Call *op) {
            debug(2) << "Encountered call method: " << op->name << "\n";

            string call_name = "_";
            if(op->name == Call::buffer_get_min) {
                call_name += "min";
            } else if (op->name == Call::buffer_get_extent) {
                call_name += "extent";
            } else if (op->name == Call::buffer_get_stride) {
                call_name += "stride";
            } else if (op->name == Call::buffer_get_host) {
                call_name += "host";
            }


            if(call_name == "_host") {
                print_assignment(op->type, print_expr(op->args[0]));
            } else if(call_name != "_") {
                assert(op->args.size() == 2);
                string buffer_name = print_expr(op->args[0]);
                string buffer_dim = print_expr(op->args[1]);
                string variable = buffer_name + "_dim" + buffer_dim + call_name;
                debug(2) << debug_header_ << "Converted name: " << variable << "\n";
                print_assignment(op->type, variable);
            } else {
                CodeGen_C::visit(op);
            }
        }

        void ConvolutionsCompilerForAICore::visit(const Ramp *op) {

            string id_stride = print_expr(op->stride);
            if(id_stride == "1")
                return;
            else
                assert(false);
        }

      /** This method explores a set of statements inside a block by invoking
       * TripletIRParser. Ff it is suitable for vectorization, then it invokes
       * analyzer, transformer, and code generator to generate vectorized code
       * for a single AI core
       */
        void ConvolutionsCompilerForAICore::visit(const Block *op) {
          bool does_block_has_straight_line_of_code;
          vector<Stmt> statements;

        // Step-1: Find if the block is a straight line of code
          ExtractStatementsFromBlock(op, does_block_has_straight_line_of_code,
                                   statements);

          //If the block is not a straight line of code, invoke
          // C code generator to generate sequential code
          if(!does_block_has_straight_line_of_code) {
            CodeGen_C::visit(op);
            return;
          }

          debug(1) << debug_header_ << "Num. of statements inside the "
                   << "block are: " << statements.size() << "\n";


          //Step-2: Invoke parser to extract TripletIR if the block is
          //suitable for vectorization
          vector<TripletIRNode> statements_in_triplet_form;
          ParseStatements(statements, statements_in_triplet_form);


          //Step-3: Fuse statements writing into same output locations
          //This is to avoid redundant initial loads and stores to memory
          vector<TripletIRNode> statements_after_fusion;
          FuseStatementsWritingToSameOutput(
                  statements_in_triplet_form, statements_after_fusion);

          //Step-4: Perform reuse analysis for both temporal and spatial reuses
          auto temporal_reuse_graph =
                  BuildTemporalReuseGraphAndLogicalMemoryGroups(
                          statements_after_fusion);
          auto spatial_reuse_graph =
                  BuildSpatialReuseGraphAndLogicalMemoryGroups(
                          statements_after_fusion);


          //Step-5: Perform logical fusion of MAC operations to exploit
          //multiple columns in the hardware
          //Two vector MAC operations can be fused as long as they contribute
          //to same output and also use same vector registers as operands
          FuseVectorMacOperations(statements_after_fusion,
                                  temporal_reuse_graph, spatial_reuse_graph);

          //Step-6: Perform code generation
          GenerateCode(statements_after_fusion, temporal_reuse_graph,
                       spatial_reuse_graph);

          debug(0) << debug_header_ << "Code generation is finished\n";

          double arithemtic_intensity = total_mac_operations_
                              / (double) (total_loads_ + total_stores_);

          debug(0) << debug_header_ << "Arithmetic intensity (MAC/Operand): "
                                   << arithemtic_intensity << "\n";

          arithemtic_intensity /= (data_type_size_in_bits_/8.0);

          debug(0) << debug_header_ << "Arithmetic intensity (MAC/Byte): "
                                    << arithemtic_intensity << "\n";
        }

        /** This method tries to extract a set of statements in a straight
         * line of code without any conditionals.
         */
        void ConvolutionsCompilerForAICore::
          ExtractStatementsFromBlock(const Block *op,
                                   bool &does_block_has_straight_line_of_code,
                                   vector<Stmt> &statements) {
          does_block_has_straight_line_of_code= true;
          statements = convert_statements_in_block_to_vector_of_statements(op);

          // Check if statements inside the block are just store operations,
          // i.e., straight line of code without conditionals.
          for (auto &statement : statements) {
            if(!statement.as<Store>()) {
              does_block_has_straight_line_of_code = false;
              break;
            }
          }

          does_block_has_straight_line_of_code &= (!statements.empty());
        }

        /** This method parses statements in a block having single line of
         * code without conditionals, and returns a vector of nodes in
         * Triplet form.
         */
        void ConvolutionsCompilerForAICore::ParseStatements(
                vector<Stmt> &statements,
                vector<TripletIRNode> &statements_in_triplet_form) {

          //Simplify statements before parsing
          for (auto &statement : statements) {
            Stmt stmt = substitute_in_all_lets(statement);
            statement = simplify(stmt);
          }

          //Step-2: Invoke parser to extract TripletIR if the block is
          //suitable for vectorization
          for (const auto &statement : statements) {
            TripletIRParser parser;
            parser.debug_header_ = debug_header_;

            Stmt stmt = substitute_in_all_lets(statement);
            stmt = simplify(stmt);
            stmt.accept(&parser);

            data_type_size_in_bits_ = parser.data_type_size_in_bits_;
            num_simd_lanes_ = parser.num_simd_lanes_;
            num_simd_columns_per_lane_ = parser.GetNumSIMDColumns();
            memory_operations_stride_length_ = parser
                    .memory_operations_stride_length_;
            statements_in_triplet_form.push_back(parser.node);
          }

          debug(1) << debug_header_ << "Num. of statements before fusion:  "
                   << statements_in_triplet_form.size() << "\n";

          PrintStatementsInTripletForm(statements_in_triplet_form);
        }

        /** This method fuses statements in triplet form if they are writing
         * to same output memory locations, and generates one single initial
         * load and one single store corresponding to the statement.
         * This optimization is to reduce number of writes and initial loads
         * the scratchpad buffer.
         */
        void ConvolutionsCompilerForAICore::FuseStatementsWritingToSameOutput(
                vector<TripletIRNode> &statements_in_triplet_form,
                vector<TripletIRNode> &statements_after_fusion) {

          PrintStatementsInTripletForm(statements_in_triplet_form);

          for (auto &statement : statements_in_triplet_form) {
            bool is_this_statement_found = false;
            int index = -1;
            auto current_load = statement.initial_loads_[0];
            for(unsigned int j = 0; j < statements_after_fusion.size(); j++) {
              auto fused_statement_load = statements_after_fusion[j].initial_loads_[0];
                if(current_load->name == fused_statement_load->name
                  && equal(current_load->index, fused_statement_load->index)) {
                    is_this_statement_found = true;
                    index = j;
                }
            }

            if(is_this_statement_found) {
              statements_after_fusion[index].pair_wise_loads_.insert(
                      statements_after_fusion[index].pair_wise_loads_.end(),
                      std::begin(statement.pair_wise_loads_),
                      std::end(statement.pair_wise_loads_));
            } else {
                statements_after_fusion.push_back(statement);
            }
          }

          debug(1) << debug_header_ << "Num. of statements after fusion:  "
                   << statements_after_fusion.size() << "\n";

          PrintStatementsInTripletForm(statements_after_fusion);
        }

        /** This method prints statements in triplet form. */
        void ConvolutionsCompilerForAICore::PrintStatementsInTripletForm(
                vector<TripletIRNode> &statements) {
            for (const auto &statement : statements) {
                const Load *load = statement.initial_loads_[0];
                debug(1) << debug_header_ << "Initial load: " << load->name
                         << "," << print_expr(load->index) << "\n";
                for (const auto &mac : statement.pair_wise_loads_) {
                    const Load *load1 = mac.first;
                    if(load1 != nullptr) {
                      debug(1) << debug_header_ << "Operand1: " << load1->name
                               << "," << load1->index << "\n";
                    }

                    const Load *load2 = mac.second;
                    if(load2 != nullptr) {
                      debug(1) << debug_header_ << "Operand2: " << load2->name
                               << "," << load2->index << "\n";
                    }
                }
            }
        }

        /** This function fuses MAC operations inside a statement to exploit
         * multiple MAC operations inside a SIMD lane, i.e., to leveraging
         * post-add support for 16-bit and 8-bit support
         */
        void ConvolutionsCompilerForAICore::FuseVectorMacOperations(
                vector<TripletIRNode> &statements,
                ReuseGraph &temporal_reuse_graph,
                ReuseGraph &spatial_reuse_graph) {

          for (auto &statement : statements) {
            vector<AIVectorMacOperation *> fused_vector_macs;
            for (auto &vector_mac : statement.pair_wise_loads_) {

              //Operand1 details:
              bool is_operand1_load_in_temporal_group;
              int operand1_offset_from_its_logical_memory_group_base;

              LogicalMemoryGroup *operand1_memory_group =
                  GetLogicalMemoryGroupInformationGivenALoad(
                  vector_mac.first, temporal_reuse_graph,
                  spatial_reuse_graph,
                  is_operand1_load_in_temporal_group,
                  operand1_offset_from_its_logical_memory_group_base);

              //Operand2 details:
              bool is_operand2_load_in_temporal_group;
              int operand2_offset_from_its_logical_memory_group_base;

              LogicalMemoryGroup *operand2_memory_group =
                      GetLogicalMemoryGroupInformationGivenALoad(
                      vector_mac.second, temporal_reuse_graph,
                      spatial_reuse_graph,
                      is_operand2_load_in_temporal_group,
                      operand2_offset_from_its_logical_memory_group_base);

              if(operand1_memory_group == nullptr
                  || operand2_memory_group == nullptr) {
                  debug(0) << debug_header_ << "[Fusion of Vector "
                           << "MAC error: Not able to retrieve "
                           << "memory groups of vector operands\n";
                  assert(false);
              }

              if(fused_vector_macs.empty()) {
                  auto fused_vector_mac = new AIVectorMacOperation(
                          operand1_memory_group->name,
                          operand2_memory_group->name,
                          1, num_simd_lanes_);

                  fused_vector_mac->operand_offsets.emplace_back
                      (operand1_offset_from_its_logical_memory_group_base,
                       operand2_offset_from_its_logical_memory_group_base);
                  fused_vector_macs.push_back(fused_vector_mac);
                  continue;
              }

              bool is_fusion_successful = false;
              for(auto &fused_mac : fused_vector_macs) {
                if(fused_mac->operand1_group_name == operand1_memory_group->name
                   && fused_mac->operand2_group_name == operand2_memory_group->name) {
                  fused_mac->simd_columns++;
                  fused_mac->operand_offsets.emplace_back
                      (operand1_offset_from_its_logical_memory_group_base,
                       operand2_offset_from_its_logical_memory_group_base);
                  is_fusion_successful = true;
                }
              }

              if(!is_fusion_successful) {
                auto fused_vector_mac = new AIVectorMacOperation(
                        operand1_memory_group->name,
                        operand2_memory_group->name, 1,
                        num_simd_lanes_);
                fused_vector_mac->operand_offsets.emplace_back
                      (operand1_offset_from_its_logical_memory_group_base,
                       operand2_offset_from_its_logical_memory_group_base);
                fused_vector_macs.push_back(fused_vector_mac);
              }
            }

            statement.fused_vector_macs = fused_vector_macs;
            debug(3) << debug_header_ << "Before fusing, number of vector MACs: "
                     << statement.pair_wise_loads_.size() << "\n";
            debug(3) << debug_header_ << "After fusing, number of vector MACs: "
                     << statement.fused_vector_macs.size() << "\n";

            for(auto &vector_mac : statement.fused_vector_macs) {
              debug(3) << debug_header_ << "Fused MAC operands\n";
              for (auto &operand_offset : vector_mac->operand_offsets) {
                debug(3) << operand_offset.first << ", "
                         << operand_offset.second << "\n";
              }
            }
          }
        }
    }
}
