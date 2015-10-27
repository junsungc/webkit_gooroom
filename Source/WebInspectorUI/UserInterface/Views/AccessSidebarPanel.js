/*
 * Copyright (C) 2015 Company 100, Inc. All rights reserved.
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

WebInspector.AccessSidebarPanel = class AccessSidebarPanel extends WebInspector.NavigationSidebarPanel
{
    constructor(contentBrowser)
    {
        super("access", WebInspector.UIString("Access"));

        this.contentBrowser = contentBrowser;

        this.filterBar.placeholder = WebInspector.UIString("Filter Process List");

        this._navigationBar = new WebInspector.NavigationBar;
        this.element.appendChild(this._navigationBar.element);

        this.contentTreeOutline.onselect = this._treeElementSelected.bind(this);

        this._refreshScopeBar();
        this._refreshSideBar();
    }

    // Protected

    hasCustomFilters()
    {
        console.assert(this._scopeBar.selectedItems.length === 1);
        var selectedScopeBarItem = this._scopeBar.selectedItems[0];
        return selectedScopeBarItem && !selectedScopeBarItem.exclusive;
    }

    matchTreeElementAgainstCustomFilters(treeElement, flags)
    {
        console.assert(this._scopeBar.selectedItems.length === 1);
        var selectedScopeBarItem = this._scopeBar.selectedItems[0];

        // Show everything if there is no selection or "All Storage" is selected (the exclusive item).
        if (!selectedScopeBarItem || selectedScopeBarItem.exclusive)
            return true;

        if (treeElement instanceof WebInspector.WebProcessTreeElement) {
            return true;
        }

        function match()
        {
            if (selectedScopeBarItem.id === treeElement.mainTitle)
                return true;
            return false;
        }

        var matched = match();
        if (matched)
            flags.expandTreeElement = true;
        return matched;
    }

    // Private

    _compareTreeElements(a, b)
    {
        console.assert(a.mainTitle);
        console.assert(b.mainTitle);

        return (a.mainTitle || "").localeCompare(b.mainTitle || "");
    }

    _refreshScopeBar()
    {
        this._navigationBar.removeNavigationItem(0);
        var scopeItemPrefix = "accesss-sidebar-";
        var scopeBarItems = [];
        scopeBarItems.push(new WebInspector.ScopeBarItem(scopeItemPrefix + "type-all", WebInspector.UIString("All PID"), true));

        var processes = [];

        for (var process of WebInspector.accessManager.uiProcesses) {
            var processItem = {identifier: process.pid.toString(), title: process.pid.toString(), classes: [WebInspector.ProcessTreeElement]};
            processes.push(processItem);
        }

        processes.sort(function(a, b) { return a.title.localeCompare(b.title);});

        for (var info of processes) {
            var scopeBarItem = new WebInspector.ScopeBarItem(info.identifier.toString(), info.title);
            scopeBarItem.__storageTypeInfo = info; scopeBarItems.push(scopeBarItem);
        }

        this._scopeBar = new WebInspector.ScopeBar("access-sidebar-scope-bar", scopeBarItems, scopeBarItems[0], true);

        this._navigationBar.addNavigationItem(this._scopeBar);
        this._scopeBar.addEventListener(WebInspector.ScopeBar.Event.SelectionChanged, this._scopeBarSelectionDidChange, this);
    }

    _refreshSideBar()
    {
        for (var process of WebInspector.accessManager.uiProcesses) {
            var processTreeElement = new WebInspector.UIProcessTreeElement(process.pid.toString(), "process-icon", process);
            this.contentTreeOutline.insertChild(processTreeElement, insertionIndexForObjectInListSortedByFunction(processTreeElement, this.contentTreeOutline.children, this._compareTreeElements));
            for (var webProcess of process.webProcesses) {
                var webProcessTreeElement = new WebInspector.WebProcessTreeElement(webProcess.pid.toString(), "process-icon", webProcess);
                processTreeElement.appendChild(webProcessTreeElement);
            }
        }
    }

    _treeElementSelected(treeElement, selectedByUser)
    {
        if (treeElement instanceof WebInspector.UIProcessTreeElement || treeElement instanceof WebInspector.WebProcessTreeElement) {
            WebInspector.showRepresentedObject(treeElement.representedObject);
            return;
        }

        return;
    }

    _scopeBarSelectionDidChange(event)
    {
        this.updateFilter();
    }
};
