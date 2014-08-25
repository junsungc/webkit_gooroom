/*
 * Copyright (C) 2013, 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DFGDesiredWeakReferences_h
#define DFGDesiredWeakReferences_h

#include <wtf/Vector.h>

#if ENABLE(DFG_JIT)

namespace JSC { 

class CodeBlock;
class JSCell;
class SlotVisitor;
class VM;

namespace DFG {

class CommonData;

class DesiredWeakReferences {
public:
    DesiredWeakReferences();
    DesiredWeakReferences(CodeBlock*);
    ~DesiredWeakReferences();

    void addLazily(JSCell*);
    void reallyAdd(VM&, CommonData*);
    
    void visitChildren(SlotVisitor&);

private:
    CodeBlock* m_codeBlock;
    Vector<JSCell*> m_references;
};

} } // namespace JSC::DFG

#endif // ENABLE(DFG_JIT)

#endif // DFGDesiredWeakReferences_h
