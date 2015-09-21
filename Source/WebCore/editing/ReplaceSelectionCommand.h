/*
 * Copyright (C) 2005, 2006, 2008 Apple Inc. All rights reserved.
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

#ifndef ReplaceSelectionCommand_h
#define ReplaceSelectionCommand_h

#include "CompositeEditCommand.h"
#include "NodeTraversal.h"

namespace WebCore {

class DocumentFragment;
class ReplacementFragment;

class ReplaceSelectionCommand : public CompositeEditCommand {
public:
    enum CommandOption {
        SelectReplacement = 1 << 0,
        SmartReplace = 1 << 1,
        MatchStyle = 1 << 2,
        PreventNesting = 1 << 3,
        MovingParagraph = 1 << 4,
        SanitizeFragment = 1 << 5,
        IgnoreMailBlockquote = 1 << 6,
    };

    typedef unsigned CommandOptions;

    static Ref<ReplaceSelectionCommand> create(Document& document, RefPtr<DocumentFragment>&& fragment, CommandOptions options, EditAction editingAction = EditActionInsert)
    {
        return adoptRef(*new ReplaceSelectionCommand(document, WTF::move(fragment), options, editingAction));
    }

private:
    ReplaceSelectionCommand(Document&, RefPtr<DocumentFragment>&&, CommandOptions, EditAction);

    virtual void doApply();

    class InsertedNodes {
    public:
        void respondToNodeInsertion(Node*);
        void willRemoveNodePreservingChildren(Node*);
        void willRemoveNode(Node*);
        void didReplaceNode(Node*, Node* newNode);

        Node* firstNodeInserted() const { return m_firstNodeInserted.get(); }
        Node* lastLeafInserted() const { return m_lastNodeInserted->lastDescendant(); }
        Node* pastLastLeaf() const
        {
            if (m_lastNodeInserted) {
                ASSERT(lastLeafInserted());
                return NodeTraversal::next(*lastLeafInserted());
            }
            return nullptr;
        }

    private:
        RefPtr<Node> m_firstNodeInserted;
        RefPtr<Node> m_lastNodeInserted;
    };

    Node* insertAsListItems(PassRefPtr<HTMLElement> listElement, Node* insertionNode, const Position&, InsertedNodes&);

    void updateNodesInserted(Node*);
    bool shouldRemoveEndBR(Node*, const VisiblePosition&);
    
    bool shouldMergeStart(bool, bool, bool);
    bool shouldMergeEnd(bool selectionEndWasEndOfParagraph);
    bool shouldMerge(const VisiblePosition&, const VisiblePosition&);
    
    void mergeEndIfNeeded();
    
    void removeUnrenderedTextNodesAtEnds(InsertedNodes&);
    
    void removeRedundantStylesAndKeepStyleSpanInline(InsertedNodes&);
    void makeInsertedContentRoundTrippableWithHTMLTreeBuilder(InsertedNodes&);
    void moveNodeOutOfAncestor(PassRefPtr<Node>, PassRefPtr<Node> ancestor, InsertedNodes&);
    void handleStyleSpans(InsertedNodes&);
    void handlePasteAsQuotationNode();
    
    VisiblePosition positionAtStartOfInsertedContent() const;
    VisiblePosition positionAtEndOfInsertedContent() const;

    bool shouldPerformSmartReplace() const;
    void addSpacesForSmartReplace();
    void completeHTMLReplacement(const Position& lastPositionToSelect);
    void mergeTextNodesAroundPosition(Position&, Position& positionOnlyToBeUpdated);

    bool performTrivialReplace(const ReplacementFragment&);

    Position m_startOfInsertedContent;
    Position m_endOfInsertedContent;
    RefPtr<EditingStyle> m_insertionStyle;
    bool m_selectReplacement;
    bool m_smartReplace;
    bool m_matchStyle;
    RefPtr<DocumentFragment> m_documentFragment;
    bool m_preventNesting;
    bool m_movingParagraph;
    bool m_sanitizeFragment;
    bool m_shouldMergeEnd;
    bool m_ignoreMailBlockquote;
};

} // namespace WebCore

#endif // ReplaceSelectionCommand_h
