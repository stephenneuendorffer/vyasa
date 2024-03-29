#ifndef HALIDE_MODULE_H
#define HALIDE_MODULE_H

/** \file
 *
 * Defines Module, an IR container that fully describes a Halide program.
 */

#include <functional>

#include "Argument.h"
#include "ExternalCode.h"
#include "IR.h"
#include "ModulusRemainder.h"
#include "Outputs.h"
#include "Target.h"

namespace Halide {

/** Type of linkage a function in a lowered Halide module can have.
    Also controls whether auxiliary functions and metadata are generated. */
enum class LinkageType {
    External, ///< Visible externally.
    ExternalPlusMetadata, ///< Visible externally. Argument metadata and an argv wrapper are also generated.
    Internal, ///< Not visible externally, similar to 'static' linkage in C.
};

namespace Internal {

/** Definition of an argument to a LoweredFunc. This is similar to
 * Argument, except it enables passing extra information useful to
 * some targets to LoweredFunc. */
struct LoweredArgument : public Argument {
    /** For scalar arguments, the modulus and remainder of this
     * argument. */
    ModulusRemainder alignment;

    LoweredArgument() = default;
    explicit LoweredArgument(const Argument &arg) : Argument(arg) {}
    LoweredArgument(const std::string &_name, Kind _kind, const Type &_type, uint8_t _dimensions, const ArgumentEstimates &argument_estimates)
        : Argument(_name, _kind, _type, _dimensions, argument_estimates) {}
};

/** Definition of a lowered function. This object provides a concrete
 * mapping between parameters used in the function body and their
 * declarations in the argument list. */
struct LoweredFunc {
    std::string name;

    /** Arguments referred to in the body of this function. */
    std::vector<LoweredArgument> args;

    /** Body of this function. */
    Stmt body;

    /** The linkage of this function. */
    LinkageType linkage;

    /** The name-mangling choice for the function. Defaults to using
     * the Target. */
    NameMangling name_mangling;

    LoweredFunc(const std::string &name,
                const std::vector<LoweredArgument> &args,
                Stmt body,
                LinkageType linkage,
                NameMangling mangling = NameMangling::Default);
    LoweredFunc(const std::string &name,
                const std::vector<Argument> &args,
                Stmt body,
                LinkageType linkage,
                NameMangling mangling = NameMangling::Default);
};

}  // namespace Internal

namespace Internal {
struct ModuleContents;
}

/** A halide module. This represents IR containing lowered function
 * definitions and buffers. */
class Module {
    Internal::IntrusivePtr<Internal::ModuleContents> contents;

public:
    Module(const std::string &name, const Target &target);

    /** Get the target this module has been lowered for. */
    const Target &target() const;

    /** The name of this module. This is used as the default filename
     * for output operations. */
    const std::string &name() const;

    /** If this Module had an auto-generated schedule, this is the C++ source
     * for that schedule. */
    const std::string &auto_schedule() const;

    /** If this Module had featurization data from autoscheduling, this is it. */
    const std::vector<uint8_t> &featurization() const;

    /** Return whether this module uses strict floating-point anywhere. */
    bool any_strict_float() const;

    /** The declarations contained in this module. */
    // @{
    const std::vector<Buffer<>> &buffers() const;
    const std::vector<Internal::LoweredFunc> &functions() const;
    std::vector<Internal::LoweredFunc> &functions();
    const std::vector<Module> &submodules() const;
    const std::vector<ExternalCode> &external_code() const;
    // @}

    /** Return the function with the given name. If no such function
    * exists in this module, assert. */
    Internal::LoweredFunc get_function_by_name(const std::string &name) const;

    /** Add a declaration to this module. */
    // @{
    void append(const Buffer<> &buffer);
    void append(const Internal::LoweredFunc &function);
    void append(const Module &module);
    void append(const ExternalCode &external_code);
    // @}

    /** Compile a halide Module to variety of outputs, depending on
     * the fields set in output_files. */
    void compile(const Outputs &output_files_arg) const;

    /** Compile a halide Module to in-memory object code. Currently
     * only supports LLVM based compilation, but should be extended to
     * handle source code backends. */
    Buffer<uint8_t> compile_to_buffer() const;

    /** Return a new module with all submodules compiled to buffers on
     * on the result Module. */
    Module resolve_submodules() const;

    /** When generating metadata from this module, remap any occurrences
     * of 'from' into 'to'. */
    void remap_metadata_name(const std::string &from, const std::string &to) const;

    /** Retrieve the metadata name map. */
    std::map<std::string, std::string> get_metadata_name_map() const;

    /** Set the auto_schedule text for the Module. It is an error to call this
     * multiple times for a given Module. */
    void set_auto_schedule(const std::string &auto_schedule);

    /** Set the featurization data for the Module. It is an error to call this
     * multiple times for a given Module. */
    void set_featurization(const std::vector<uint8_t> &featurization);

    /** Set whether this module uses strict floating-point directives anywhere. */
    void set_any_strict_float(bool any_strict_float);
};

/** Link a set of modules together into one module. */
Module link_modules(const std::string &name, const std::vector<Module> &modules);

/** Create an object file containing the Halide runtime for a given target. For
 * use with Target::NoRuntime. Standalone runtimes are only compatible with
 * pipelines compiled by the same build of Halide used to call this function. */
void compile_standalone_runtime(const std::string &object_filename, Target t);

/** Create an object and/or static library file containing the Halide runtime
 * for a given target. For use with Target::NoRuntime. Standalone runtimes are
 * only compatible with pipelines compiled by the same build of Halide used to
 * call this function. Return an Outputs with just the actual outputs filled in
 * (typically, object_name and/or static_library_name).
 */
Outputs compile_standalone_runtime(const Outputs &output_files, Target t);

typedef std::function<Module(const std::string &, const Target &)> ModuleProducer;

void compile_multitarget(const std::string &fn_name,
                         const Outputs &output_files,
                         const std::vector<Target> &targets,
                         ModuleProducer module_producer,
                         const std::map<std::string, std::string> &suffixes = {});

}  // namespace Halide

#endif
