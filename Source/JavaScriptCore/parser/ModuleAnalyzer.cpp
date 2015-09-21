/*
 * Copyright (C) 2015 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ModuleAnalyzer.h"

#include "IdentifierInlines.h"
#include "ModuleRecord.h"

namespace JSC {


ModuleAnalyzer::ModuleAnalyzer(VM& vm, const VariableEnvironment& declaredVariables, const VariableEnvironment& lexicalVariables)
    : m_vm(&vm)
    , m_moduleRecord(ModuleRecord::create())
    , m_declaredVariables(declaredVariables)
    , m_lexicalVariables(lexicalVariables)
{
}

Identifier ModuleAnalyzer::exportedBinding(const RefPtr<UniquedStringImpl>& ident)
{
    const auto iterator = m_aliasMap.find(ident);
    if (iterator != m_aliasMap.end())
        return iterator->value;
    return Identifier::fromUid(&vm(), ident.get());
}

void ModuleAnalyzer::declareExportAlias(const Identifier& localName, const Identifier& exportName)
{
    m_aliasMap.add(localName.impl(), exportName);
}

void ModuleAnalyzer::exportVariable(const RefPtr<UniquedStringImpl>& localName, const VariableEnvironmentEntry& variable)
{
    // In the parser, we already marked the variables as Exported and Imported.
    // By leveraging this information, we collect the information that is needed
    // to construct the module environment.
    //
    // I E
    //   * = exported module local variable
    // *   = imported binding
    //     = non-exported module local variable
    // * * = indirect exported binding
    //
    // One exception is namespace binding (like import * as ns from "mod").
    // This is annotated as an imported, but the actual binding is locate in the
    // current module.

    if (!variable.isExported())
        return;

    const Identifier exportName = exportedBinding(localName);

    // Exported module local variable.
    if (!variable.isImported()) {
        moduleRecord().addExportEntry(ModuleRecord::ExportEntry::createLocal(exportName, Identifier::fromUid(m_vm, localName.get()), variable));
        return;
    }

    const auto& importEntry = moduleRecord().lookUpImportEntry(localName);
    if (importEntry.isNamespace(vm())) {
        // Exported namespace binding.
        // import * as namespace from "mod"
        // export { namespace }
        moduleRecord().addExportEntry(ModuleRecord::ExportEntry::createNamespace(exportName, importEntry.moduleRequest));
        return;
    }

    // Indirectly exported binding.
    // import a from "mod"
    // export { a }
    moduleRecord().addExportEntry(ModuleRecord::ExportEntry::createIndirect(exportName, importEntry.importName, importEntry.moduleRequest));
}



Ref<ModuleRecord> ModuleAnalyzer::analyze(ModuleProgramNode& moduleProgramNode)
{
    // Traverse the module AST and collect
    // * Import entries
    // * Export entries that have FromClause (e.g. export { a } from "mod")
    // * Export entries that have star (e.g. export * from "mod")
    // * Aliased export names (e.g. export { a as b })
    moduleProgramNode.analyzeModule(*this);

    // Based on the collected information, categorize export entries into 3 types.
    // 1. Local export entries
    //     This references the local variable in the current module.
    //     This variable should be allocated in the current module environment as a heap variable.
    //
    //     const variable = 20
    //     export { variable }
    //
    // 2. Namespace export entries
    //     This references the namespace object imported by some import entries.
    //     This variable itself should be allocated in the current module environment as a heap variable.
    //     But when the other modules attempt to resolve this export name in this module, this module
    //     should tell the link to the original module.
    //
    //     import * as namespace from "mod"
    //     export { namespace as mod }
    //
    // 3. Indirect export entries
    //     This references the imported binding name from the other module.
    //     This module environment itself should hold the pointer to (1) the original module and
    //     (2) the binding in the original module. The variable itself is allocated in the original
    //     module. This indirect binding is resolved when the CodeBlock resolves the references.
    //
    //     import mod from "mod"
    //     export { mod }
    //
    //     export { a } from "mod"
    //
    // And separeted from the above 3 types, we also collect the star export entries.
    //
    // 4. Star export entries
    //     This exports all the names from the specified external module as the current module's name.
    //
    //     export * from "mod"
    for (const auto& pair : m_declaredVariables)
        exportVariable(pair.key, pair.value);

    for (const auto& pair : m_lexicalVariables)
        exportVariable(pair.key, pair.value);

    if (Options::dumpModuleRecord())
        m_moduleRecord->dump();

    return *m_moduleRecord;
}

} // namespace JSC
