# Copyright (C) 2011, 2012, 2013 Apple Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.

# First come the common protocols that both interpreters use. Note that each
# of these must have an ASSERT() in LLIntData.cpp

# Work-around for the fact that the toolchain's awareness of armv7s results in
# a separate slab in the fat binary, yet the offlineasm doesn't know to expect
# it.
if ARMv7s
end

# These declarations must match interpreter/JSStack.h.

if JSVALUE64
const PtrSize = 8
const CallFrameHeaderSlots = 6
else
const PtrSize = 4
const CallFrameHeaderSlots = 5
end
const SlotSize = 8

const CallerFrame = 0
const ReturnPC = CallerFrame + PtrSize
const CodeBlock = ReturnPC + PtrSize
const ScopeChain = CodeBlock + SlotSize
const Callee = ScopeChain + SlotSize
const ArgumentCount = Callee + SlotSize
const ThisArgumentOffset = ArgumentCount + SlotSize
const CallFrameHeaderSize = ThisArgumentOffset

# Some value representation constants.
if JSVALUE64
const TagBitTypeOther = 0x2
const TagBitBool      = 0x4
const TagBitUndefined = 0x8
const ValueEmpty      = 0x0
const ValueFalse      = TagBitTypeOther | TagBitBool
const ValueTrue       = TagBitTypeOther | TagBitBool | 1
const ValueUndefined  = TagBitTypeOther | TagBitUndefined
const ValueNull       = TagBitTypeOther
else
const Int32Tag = -1
const BooleanTag = -2
const NullTag = -3
const UndefinedTag = -4
const CellTag = -5
const EmptyValueTag = -6
const DeletedValueTag = -7
const LowestTag = DeletedValueTag
end

# Watchpoint states
const ClearWatchpoint = 0
const IsWatched = 1
const IsInvalidated = 2

# Some register conventions.
if JSVALUE64
    # - Use a pair of registers to represent the PC: one register for the
    #   base of the bytecodes, and one register for the index.
    # - The PC base (or PB for short) should be stored in the csr. It will
    #   get clobbered on calls to other JS code, but will get saved on calls
    #   to C functions.
    # - C calls are still given the Instruction* rather than the PC index.
    #   This requires an add before the call, and a sub after.
    const PC = t4
    const PB = t6
    const tagTypeNumber = csr1
    const tagMask = csr2
    
    macro loadisFromInstruction(offset, dest)
        loadis offset * 8[PB, PC, 8], dest
    end
    
    macro loadpFromInstruction(offset, dest)
        loadp offset * 8[PB, PC, 8], dest
    end
    
    macro storepToInstruction(value, offset)
        storep value, offset * 8[PB, PC, 8]
    end

else
    const PC = t4
    macro loadisFromInstruction(offset, dest)
        loadis offset * 4[PC], dest
    end
    
    macro loadpFromInstruction(offset, dest)
        loadp offset * 4[PC], dest
    end
end

# Constants for reasoning about value representation.
if BIG_ENDIAN
    const TagOffset = 0
    const PayloadOffset = 4
else
    const TagOffset = 4
    const PayloadOffset = 0
end

# Constant for reasoning about butterflies.
const IsArray                  = 1
const IndexingShapeMask        = 30
const NoIndexingShape          = 0
const Int32Shape               = 20
const DoubleShape              = 22
const ContiguousShape          = 26
const ArrayStorageShape        = 28
const SlowPutArrayStorageShape = 30

# Type constants.
const StringType = 5
const ObjectType = 17
const FinalObjectType = 18

# Type flags constants.
const MasqueradesAsUndefined = 1
const ImplementsHasInstance = 2
const ImplementsDefaultHasInstance = 8

# Bytecode operand constants.
const FirstConstantRegisterIndex = 0x40000000

# Code type constants.
const GlobalCode = 0
const EvalCode = 1
const FunctionCode = 2

# The interpreter steals the tag word of the argument count.
const LLIntReturnPC = ArgumentCount + TagOffset

# String flags.
const HashFlags8BitBuffer = 32

# Copied from PropertyOffset.h
const firstOutOfLineOffset = 100

# ResolveType
const GlobalProperty = 0
const GlobalVar = 1
const ClosureVar = 2
const GlobalPropertyWithVarInjectionChecks = 3
const GlobalVarWithVarInjectionChecks = 4
const ClosureVarWithVarInjectionChecks = 5
const Dynamic = 6

const ResolveModeMask = 0xffff

const MarkedBlockSize = 64 * 1024
const MarkedBlockMask = ~(MarkedBlockSize - 1)
# Constants for checking mark bits.
const AtomNumberShift = 3
const BitMapWordShift = 4

# Allocation constants
if JSVALUE64
    const JSFinalObjectSizeClassIndex = 1
else
    const JSFinalObjectSizeClassIndex = 3
end

# This must match wtf/Vector.h
const VectorBufferOffset = 0
if JSVALUE64
    const VectorSizeOffset = 12
else
    const VectorSizeOffset = 8
end

# Some common utilities.
macro crash()
    if C_LOOP
        cloopCrash
    else
        storei t0, 0xbbadbeef[]
        move 0, t0
        call t0
    end
end

macro assert(assertion)
    if ASSERT_ENABLED
        assertion(.ok)
        crash()
    .ok:
    end
end

macro preserveReturnAddressAfterCall(destinationRegister)
    if C_LOOP or ARM or ARMv7 or ARMv7_TRADITIONAL or ARM64 or MIPS or SH4
        # In C_LOOP case, we're only preserving the bytecode vPC.
        move lr, destinationRegister
    elsif X86 or X86_64
        pop destinationRegister
    else
        error
    end
end

macro restoreReturnAddressBeforeReturn(sourceRegister)
    if C_LOOP or ARM or ARMv7 or ARMv7_TRADITIONAL or ARM64 or MIPS or SH4
        # In C_LOOP case, we're only restoring the bytecode vPC.
        move sourceRegister, lr
    elsif X86 or X86_64
        push sourceRegister
    else
        error
    end
end

macro traceExecution()
    if EXECUTION_TRACING
        callSlowPath(_llint_trace)
    end
end

macro callTargetFunction(callLinkInfo)
    if C_LOOP
        cloopCallJSFunction LLIntCallLinkInfo::machineCodeTarget[callLinkInfo]
    else
        call LLIntCallLinkInfo::machineCodeTarget[callLinkInfo]
        dispatchAfterCall()
    end
end

macro slowPathForCall(slowPath)
    callCallSlowPath(
        slowPath,
        macro (callee)
            if C_LOOP
                cloopCallJSFunction callee
            else
                call callee
                dispatchAfterCall()
            end
        end)
end

macro arrayProfile(structureAndIndexingType, profile, scratch)
    const structure = structureAndIndexingType
    const indexingType = structureAndIndexingType
    storep structure, ArrayProfile::m_lastSeenStructure[profile]
    loadb Structure::m_indexingType[structure], indexingType
end

macro checkMarkByte(cell, scratch1, scratch2, continuation)
    move cell, scratch1
    move cell, scratch2

    andp MarkedBlockMask, scratch1
    andp ~MarkedBlockMask, scratch2

    rshiftp AtomNumberShift + BitMapWordShift, scratch2
    loadb MarkedBlock::m_marks[scratch1, scratch2, 1], scratch1
    continuation(scratch1)
end

macro checkSwitchToJIT(increment, action)
    loadp CodeBlock[cfr], t0
    baddis increment, CodeBlock::m_llintExecuteCounter + ExecutionCounter::m_counter[t0], .continue
    action()
    .continue:
end

macro checkSwitchToJITForEpilogue()
    checkSwitchToJIT(
        10,
        macro ()
            callSlowPath(_llint_replace)
        end)
end

macro assertNotConstant(index)
    assert(macro (ok) bilt index, FirstConstantRegisterIndex, ok end)
end

macro functionForCallCodeBlockGetter(targetRegister)
    loadp Callee[cfr], targetRegister
    loadp JSFunction::m_executable[targetRegister], targetRegister
    loadp FunctionExecutable::m_codeBlockForCall[targetRegister], targetRegister
end

macro functionForConstructCodeBlockGetter(targetRegister)
    loadp Callee[cfr], targetRegister
    loadp JSFunction::m_executable[targetRegister], targetRegister
    loadp FunctionExecutable::m_codeBlockForConstruct[targetRegister], targetRegister
end

macro notFunctionCodeBlockGetter(targetRegister)
    loadp CodeBlock[cfr], targetRegister
end

macro functionCodeBlockSetter(sourceRegister)
    storep sourceRegister, CodeBlock[cfr]
end

macro notFunctionCodeBlockSetter(sourceRegister)
    # Nothing to do!
end

# Do the bare minimum required to execute code. Sets up the PC, leave the CodeBlock*
# in t1. May also trigger prologue entry OSR.
macro prologue(codeBlockGetter, codeBlockSetter, osrSlowPath, traceSlowPath)
    preserveReturnAddressAfterCall(t2)
    
    # Set up the call frame and check if we should OSR.
    storep t2, ReturnPC[cfr]
    if EXECUTION_TRACING
        callSlowPath(traceSlowPath)
    end
    codeBlockGetter(t1)
    baddis 5, CodeBlock::m_llintExecuteCounter + ExecutionCounter::m_counter[t1], .continue
    cCall2(osrSlowPath, cfr, PC)
    move t1, cfr
    btpz t0, .recover
    loadp ReturnPC[cfr], t2
    restoreReturnAddressBeforeReturn(t2)
    jmp t0
.recover:
    codeBlockGetter(t1)
.continue:
    codeBlockSetter(t1)
    
    # Set up the PC.
    if JSVALUE64
        loadp CodeBlock::m_instructions[t1], PB
        move 0, PC
    else
        loadp CodeBlock::m_instructions[t1], PC
    end
end

# Expects that CodeBlock is in t1, which is what prologue() leaves behind.
# Must call dispatch(0) after calling this.
macro functionInitialization(profileArgSkip)
    # Profile the arguments. Unfortunately, we have no choice but to do this. This
    # code is pretty horrendous because of the difference in ordering between
    # arguments and value profiles, the desire to have a simple loop-down-to-zero
    # loop, and the desire to use only three registers so as to preserve the PC and
    # the code block. It is likely that this code should be rewritten in a more
    # optimal way for architectures that have more than five registers available
    # for arbitrary use in the interpreter.
    loadi CodeBlock::m_numParameters[t1], t0
    addp -profileArgSkip, t0 # Use addi because that's what has the peephole
    assert(macro (ok) bpgteq t0, 0, ok end)
    btpz t0, .argumentProfileDone
    loadp CodeBlock::m_argumentValueProfiles + VectorBufferOffset[t1], t3
    mulp sizeof ValueProfile, t0, t2 # Aaaaahhhh! Need strength reduction!
    lshiftp 3, t0
    addp t2, t3
.argumentProfileLoop:
    if JSVALUE64
        loadq ThisArgumentOffset - 8 + profileArgSkip * 8[cfr, t0], t2
        subp sizeof ValueProfile, t3
        storeq t2, profileArgSkip * sizeof ValueProfile + ValueProfile::m_buckets[t3]
    else
        loadi ThisArgumentOffset + TagOffset - 8 + profileArgSkip * 8[cfr, t0], t2
        subp sizeof ValueProfile, t3
        storei t2, profileArgSkip * sizeof ValueProfile + ValueProfile::m_buckets + TagOffset[t3]
        loadi ThisArgumentOffset + PayloadOffset - 8 + profileArgSkip * 8[cfr, t0], t2
        storei t2, profileArgSkip * sizeof ValueProfile + ValueProfile::m_buckets + PayloadOffset[t3]
    end
    baddpnz -8, t0, .argumentProfileLoop
.argumentProfileDone:
        
    # Check stack height.
    loadi CodeBlock::m_numCalleeRegisters[t1], t0
    addi 1, t0 # Account that local0 goes at slot -1
    loadp CodeBlock::m_vm[t1], t2
    lshiftp 3, t0
    subp cfr, t0, t0
    bpbeq VM::m_jsStackLimit[t2], t0, .stackHeightOK

    # Stack height check failed - need to call a slow_path.
    callSlowPath(_llint_stack_check)
.stackHeightOK:
end

macro allocateJSObject(allocator, structure, result, scratch1, slowCase)
    if ALWAYS_ALLOCATE_SLOW
        jmp slowCase
    else
        const offsetOfFirstFreeCell = 
            MarkedAllocator::m_freeList + 
            MarkedBlock::FreeList::head

        # Get the object from the free list.   
        loadp offsetOfFirstFreeCell[allocator], result
        btpz result, slowCase
        
        # Remove the object from the free list.
        loadp [result], scratch1
        storep scratch1, offsetOfFirstFreeCell[allocator]
    
        # Initialize the object.
        storep structure, JSCell::m_structure[result]
        storep 0, JSObject::m_butterfly[result]
    end
end

macro doReturn()
    loadp ReturnPC[cfr], t2
    loadp CallerFrame[cfr], cfr
    restoreReturnAddressBeforeReturn(t2)
    ret
end

if C_LOOP
else
# stub to call into JavaScript or Native functions
# EncodedJSValue callToJavaScript(void* code, ExecState** vm, ProtoCallFrame* protoFrame, Register* topOfStack)
# EncodedJSValue callToNativeFunction(void* code, ExecState** vm, ProtoCallFrame* protoFrame, Register* topOfStack)
# Note, if these stubs or one of their related macros are changed, make the
# equivalent changes in jit/JITStubsX86.h and/or jit/JITStubsMSVC64.asm
_callToJavaScript:
    doCallToJavaScript(makeJavaScriptCall, doReturnFromJavaScript)

_callToNativeFunction:
    doCallToJavaScript(makeHostFunctionCall, doReturnFromHostFunction)
end

# Indicate the beginning of LLInt.
_llint_begin:
    crash()


_llint_program_prologue:
    prologue(notFunctionCodeBlockGetter, notFunctionCodeBlockSetter, _llint_entry_osr, _llint_trace_prologue)
    dispatch(0)


_llint_eval_prologue:
    prologue(notFunctionCodeBlockGetter, notFunctionCodeBlockSetter, _llint_entry_osr, _llint_trace_prologue)
    dispatch(0)


_llint_function_for_call_prologue:
    prologue(functionForCallCodeBlockGetter, functionCodeBlockSetter, _llint_entry_osr_function_for_call, _llint_trace_prologue_function_for_call)
.functionForCallBegin:
    functionInitialization(0)
    dispatch(0)
    

_llint_function_for_construct_prologue:
    prologue(functionForConstructCodeBlockGetter, functionCodeBlockSetter, _llint_entry_osr_function_for_construct, _llint_trace_prologue_function_for_construct)
.functionForConstructBegin:
    functionInitialization(1)
    dispatch(0)
    

_llint_function_for_call_arity_check:
    prologue(functionForCallCodeBlockGetter, functionCodeBlockSetter, _llint_entry_osr_function_for_call_arityCheck, _llint_trace_arityCheck_for_call)
    functionArityCheck(.functionForCallBegin, _slow_path_call_arityCheck)


_llint_function_for_construct_arity_check:
    prologue(functionForConstructCodeBlockGetter, functionCodeBlockSetter, _llint_entry_osr_function_for_construct_arityCheck, _llint_trace_arityCheck_for_construct)
    functionArityCheck(.functionForConstructBegin, _slow_path_construct_arityCheck)


# Value-representation-specific code.
if JSVALUE64
    include LowLevelInterpreter64
else
    include LowLevelInterpreter32_64
end


# Value-representation-agnostic code.
_llint_op_touch_entry:
    traceExecution()
    callSlowPath(_slow_path_touch_entry)
    dispatch(1)


_llint_op_new_array:
    traceExecution()
    callSlowPath(_llint_slow_path_new_array)
    dispatch(5)


_llint_op_new_array_with_size:
    traceExecution()
    callSlowPath(_llint_slow_path_new_array_with_size)
    dispatch(4)


_llint_op_new_array_buffer:
    traceExecution()
    callSlowPath(_llint_slow_path_new_array_buffer)
    dispatch(5)


_llint_op_new_regexp:
    traceExecution()
    callSlowPath(_llint_slow_path_new_regexp)
    dispatch(3)


_llint_op_less:
    traceExecution()
    callSlowPath(_slow_path_less)
    dispatch(4)


_llint_op_lesseq:
    traceExecution()
    callSlowPath(_slow_path_lesseq)
    dispatch(4)


_llint_op_greater:
    traceExecution()
    callSlowPath(_slow_path_greater)
    dispatch(4)


_llint_op_greatereq:
    traceExecution()
    callSlowPath(_slow_path_greatereq)
    dispatch(4)


_llint_op_mod:
    traceExecution()
    callSlowPath(_slow_path_mod)
    dispatch(4)


_llint_op_typeof:
    traceExecution()
    callSlowPath(_slow_path_typeof)
    dispatch(3)


_llint_op_is_object:
    traceExecution()
    callSlowPath(_slow_path_is_object)
    dispatch(3)


_llint_op_is_function:
    traceExecution()
    callSlowPath(_slow_path_is_function)
    dispatch(3)


_llint_op_in:
    traceExecution()
    callSlowPath(_slow_path_in)
    dispatch(4)

macro withInlineStorage(object, propertyStorage, continuation)
    # Indicate that the object is the property storage, and that the
    # property storage register is unused.
    continuation(object, propertyStorage)
end

macro withOutOfLineStorage(object, propertyStorage, continuation)
    loadp JSObject::m_butterfly[object], propertyStorage
    # Indicate that the propertyStorage register now points to the
    # property storage, and that the object register may be reused
    # if the object pointer is not needed anymore.
    continuation(propertyStorage, object)
end


_llint_op_del_by_id:
    traceExecution()
    callSlowPath(_llint_slow_path_del_by_id)
    dispatch(4)


_llint_op_del_by_val:
    traceExecution()
    callSlowPath(_llint_slow_path_del_by_val)
    dispatch(4)


_llint_op_put_by_index:
    traceExecution()
    callSlowPath(_llint_slow_path_put_by_index)
    dispatch(4)


_llint_op_put_getter_setter:
    traceExecution()
    callSlowPath(_llint_slow_path_put_getter_setter)
    dispatch(5)


_llint_op_jtrue:
    traceExecution()
    jumpTrueOrFalse(
        macro (value, target) btinz value, target end,
        _llint_slow_path_jtrue)


_llint_op_jfalse:
    traceExecution()
    jumpTrueOrFalse(
        macro (value, target) btiz value, target end,
        _llint_slow_path_jfalse)


_llint_op_jless:
    traceExecution()
    compare(
        macro (left, right, target) bilt left, right, target end,
        macro (left, right, target) bdlt left, right, target end,
        _llint_slow_path_jless)


_llint_op_jnless:
    traceExecution()
    compare(
        macro (left, right, target) bigteq left, right, target end,
        macro (left, right, target) bdgtequn left, right, target end,
        _llint_slow_path_jnless)


_llint_op_jgreater:
    traceExecution()
    compare(
        macro (left, right, target) bigt left, right, target end,
        macro (left, right, target) bdgt left, right, target end,
        _llint_slow_path_jgreater)


_llint_op_jngreater:
    traceExecution()
    compare(
        macro (left, right, target) bilteq left, right, target end,
        macro (left, right, target) bdltequn left, right, target end,
        _llint_slow_path_jngreater)


_llint_op_jlesseq:
    traceExecution()
    compare(
        macro (left, right, target) bilteq left, right, target end,
        macro (left, right, target) bdlteq left, right, target end,
        _llint_slow_path_jlesseq)


_llint_op_jnlesseq:
    traceExecution()
    compare(
        macro (left, right, target) bigt left, right, target end,
        macro (left, right, target) bdgtun left, right, target end,
        _llint_slow_path_jnlesseq)


_llint_op_jgreatereq:
    traceExecution()
    compare(
        macro (left, right, target) bigteq left, right, target end,
        macro (left, right, target) bdgteq left, right, target end,
        _llint_slow_path_jgreatereq)


_llint_op_jngreatereq:
    traceExecution()
    compare(
        macro (left, right, target) bilt left, right, target end,
        macro (left, right, target) bdltun left, right, target end,
        _llint_slow_path_jngreatereq)


_llint_op_loop_hint:
    traceExecution()
    loadp CodeBlock[cfr], t1
    loadp CodeBlock::m_vm[t1], t1
    loadb VM::watchdog+Watchdog::m_timerDidFire[t1], t0
    btbnz t0, .handleWatchdogTimer
.afterWatchdogTimerCheck:
    checkSwitchToJITForLoop()
    dispatch(1)
.handleWatchdogTimer:
    callWatchdogTimerHandler(.throwHandler)
    jmp .afterWatchdogTimerCheck
.throwHandler:
    jmp _llint_throw_from_slow_path_trampoline

_llint_op_switch_string:
    traceExecution()
    callSlowPath(_llint_slow_path_switch_string)
    dispatch(0)


_llint_op_new_func_exp:
    traceExecution()
    callSlowPath(_llint_slow_path_new_func_exp)
    dispatch(3)


_llint_op_call:
    traceExecution()
    arrayProfileForCall()
    doCall(_llint_slow_path_call)


_llint_op_construct:
    traceExecution()
    doCall(_llint_slow_path_construct)


_llint_op_call_varargs:
    traceExecution()
    callSlowPath(_llint_slow_path_size_and_alloc_frame_for_varargs)
    branchIfException(_llint_throw_from_slow_path_trampoline)
    slowPathForCall(_llint_slow_path_call_varargs)


_llint_op_call_eval:
    traceExecution()
    
    # Eval is executed in one of two modes:
    #
    # 1) We find that we're really invoking eval() in which case the
    #    execution is perfomed entirely inside the slow_path, and it
    #    returns the PC of a function that just returns the return value
    #    that the eval returned.
    #
    # 2) We find that we're invoking something called eval() that is not
    #    the real eval. Then the slow_path returns the PC of the thing to
    #    call, and we call it.
    #
    # This allows us to handle two cases, which would require a total of
    # up to four pieces of state that cannot be easily packed into two
    # registers (C functions can return up to two registers, easily):
    #
    # - The call frame register. This may or may not have been modified
    #   by the slow_path, but the convention is that it returns it. It's not
    #   totally clear if that's necessary, since the cfr is callee save.
    #   But that's our style in this here interpreter so we stick with it.
    #
    # - A bit to say if the slow_path successfully executed the eval and has
    #   the return value, or did not execute the eval but has a PC for us
    #   to call.
    #
    # - Either:
    #   - The JS return value (two registers), or
    #
    #   - The PC to call.
    #
    # It turns out to be easier to just always have this return the cfr
    # and a PC to call, and that PC may be a dummy thunk that just
    # returns the JS value that the eval returned.
    
    slowPathForCall(_llint_slow_path_call_eval)


_llint_generic_return_point:
    dispatchAfterCall()


_llint_op_strcat:
    traceExecution()
    callSlowPath(_slow_path_strcat)
    dispatch(4)


_llint_op_get_pnames:
    traceExecution()
    callSlowPath(_llint_slow_path_get_pnames)
    dispatch(0) # The slow_path either advances the PC or jumps us to somewhere else.


_llint_op_push_with_scope:
    traceExecution()
    callSlowPath(_llint_slow_path_push_with_scope)
    dispatch(2)


_llint_op_pop_scope:
    traceExecution()
    callSlowPath(_llint_slow_path_pop_scope)
    dispatch(1)


_llint_op_push_name_scope:
    traceExecution()
    callSlowPath(_llint_slow_path_push_name_scope)
    dispatch(4)


_llint_op_throw:
    traceExecution()
    callSlowPath(_llint_slow_path_throw)
    dispatch(2)


_llint_op_throw_static_error:
    traceExecution()
    callSlowPath(_llint_slow_path_throw_static_error)
    dispatch(3)


_llint_op_profile_will_call:
    traceExecution()
    callSlowPath(_llint_slow_path_profile_will_call)
    dispatch(2)


_llint_op_profile_did_call:
    traceExecution()
    callSlowPath(_llint_slow_path_profile_did_call)
    dispatch(2)


_llint_op_debug:
    traceExecution()
    loadp CodeBlock[cfr], t0
    loadp CodeBlock::m_globalObject[t0], t0
    loadp JSGlobalObject::m_debugger[t0], t0
    btiz t0, .opDebugDone
    loadb Debugger::m_needsOpDebugCallbacks[t0], t0
    btbz t0, .opDebugDone

    callSlowPath(_llint_slow_path_debug)
.opDebugDone:                    
    dispatch(2)


_llint_native_call_trampoline:
    nativeCallTrampoline(NativeExecutable::m_function)


_llint_native_construct_trampoline:
    nativeCallTrampoline(NativeExecutable::m_constructor)


# Lastly, make sure that we can link even though we don't support all opcodes.
# These opcodes should never arise when using LLInt or either JIT. We assert
# as much.

macro notSupported()
    if ASSERT_ENABLED
        crash()
    else
        # We should use whatever the smallest possible instruction is, just to
        # ensure that there is a gap between instruction labels. If multiple
        # smallest instructions exist, we should pick the one that is most
        # likely result in execution being halted. Currently that is the break
        # instruction on all architectures we're interested in. (Break is int3
        # on Intel, which is 1 byte, and bkpt on ARMv7, which is 2 bytes.)
        break
    end
end

_llint_op_get_by_id_chain:
    notSupported()

_llint_op_get_by_id_custom_chain:
    notSupported()

_llint_op_get_by_id_custom_proto:
    notSupported()

_llint_op_get_by_id_custom_self:
    notSupported()

_llint_op_get_by_id_generic:
    notSupported()

_llint_op_get_by_id_getter_chain:
    notSupported()

_llint_op_get_by_id_getter_proto:
    notSupported()

_llint_op_get_by_id_getter_self:
    notSupported()

_llint_op_get_by_id_proto:
    notSupported()

_llint_op_get_by_id_self:
    notSupported()

_llint_op_get_string_length:
    notSupported()

_llint_op_put_by_id_generic:
    notSupported()

_llint_op_put_by_id_replace:
    notSupported()

_llint_op_put_by_id_transition:
    notSupported()

_llint_op_init_global_const_nop:
    dispatch(5)

# Indicate the end of LLInt.
_llint_end:
    crash()

